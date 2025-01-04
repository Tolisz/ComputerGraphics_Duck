#include "waterGrid.h"

#include <vector>
#include <glm/vec3.hpp>

#include <iostream>

#include <stb_image.h>

waterGrid::waterGrid()
    : waterGrid(GRID_SIZE, 2.0f, 1.0f)
{ }

waterGrid::waterGrid(int N, float a, float c)
    : m_N{N}, m_a{a}, m_c{c}
{
    m_h = a / (N - 1);
    m_dt = 1.0f / N;

    m_gl_WorkGroupNum = std::ceil(m_N / GRID_WORK_GROUP_XY_SIZE);
}

waterGrid::~waterGrid()
{}

void waterGrid::InitGL(const std::string& shaderPath)
{
    glGenVertexArrays(1, &m_gl_VAO);
    glGenBuffers(1, &m_gl_PreviousPosBuffer);
    glGenBuffers(1, &m_gl_CurrentPosBuffer);
    glGenBuffers(1, &m_gl_EBO);
    glGenBuffers(1, &m_gl_DampingBuffer);

    PopulateBuffers();
    PrepareShaders(shaderPath);
    PrepareTextures();
}

void waterGrid::Draw()
{
    glBindVertexArray(m_gl_VAO);
    glDrawElements(GL_TRIANGLES, m_gl_triangleCount * 3, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void waterGrid::SimulateWater(float dt)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_gl_PreviousPosBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_gl_CurrentPosBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_gl_DampingBuffer);

    glBindImageTexture(0, m_gl_normalTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    // Compute new height of water elements
    m_sh_waterSimulation.Use();
    if (m_bShouldDisturb) {
        m_sh_waterSimulation.set1i("i_disturb[0]", m_iDisturb[0]);
        m_sh_waterSimulation.set1i("j_disturb[0]", m_jDisturb[0]);
        m_sh_waterSimulation.set1f("disturbHeight[0]", m_disturbHeight[0]);
        
        m_sh_waterSimulation.set1i("i_disturb[1]", m_iDisturb[1]);
        m_sh_waterSimulation.set1i("j_disturb[1]", m_jDisturb[1]);
        m_sh_waterSimulation.set1f("disturbHeight[1]", m_disturbHeight[1]);
    }

    glDispatchCompute(m_gl_WorkGroupNum, m_gl_WorkGroupNum, 1);
    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

    if (m_bShouldDisturb) {
        m_sh_waterSimulation.set1i("i_disturb[0]", -1);
        m_sh_waterSimulation.set1i("j_disturb[0]", -1);

        m_sh_waterSimulation.set1i("i_disturb[1]", -1);
        m_sh_waterSimulation.set1i("j_disturb[1]", -1);
        
        m_bShouldDisturb = false;
    }

    // For computed height update normals;
    m_sh_computeNormals.Use();
    glDispatchCompute(m_gl_WorkGroupNum, m_gl_WorkGroupNum, 1);
    glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);

    // swap buffers
    GLuint temp = m_gl_PreviousPosBuffer;
    m_gl_PreviousPosBuffer = m_gl_CurrentPosBuffer;
    m_gl_CurrentPosBuffer = temp;
}

void waterGrid::PopulateBuffers()
{
    // water lies on XZ plane
    // *~*~*~*~*~*~*~*~*~*~*~*~*
    //  
    //  ^  ^  ^  ^  ^  ^  ^  ^  (N*N)
    //  |  |  |  |  |  |  |  |
    //  |  |  |  |  |  |  |  |
    //  |  |  |  |  |  |  |  | 
    //  |  |  |  |  |  |  |  | 
    //  |  |  |  |  |  |  |  |
    //  |  |  |  |  |  |  |  |
    //  0  N  2N 3N 4N ...   N(N-1)
    // 
    // *~*~*~*~*~*~*~*~*~*~*~*~*
    std::vector<glm::vec4> points;
    points.reserve(m_N * m_N);
    
    std::vector<float> dampings;
    dampings.reserve(m_N * m_N);

    float a_half = m_a / 2.0f;
    glm::vec2 border(a_half);
    float PosX, PosZ;
    for (int x = 0; x < m_N; x++) {
        for (int z = 0; z < m_N; z++) {
            PosX = -a_half + m_h * x;
            PosZ = -a_half + m_h * z;
            points.push_back(glm::vec4(PosX, 0.0f, PosZ, 1.0f));

            // compute damping
            glm::vec2 pos(PosX, PosZ);
            glm::vec2 t1 = glm::abs(border - pos);
            glm::vec2 t2 = glm::abs(-border - pos);
            glm::vec2 t = glm::min(t1, t2);
            float l = glm::min(t.x, t.y);
            dampings.push_back(0.95 /* * glm::min(1.0f, l / 0.2f) */);
        }
    }

    std::vector<glm::uvec3> indices;
    m_gl_triangleCount = (m_N - 1) * (m_N - 1) * 2;
    indices.reserve(m_gl_triangleCount);

    for (int x = 0; x < m_N - 1; x++) {
        for (int z = 0; z < m_N - 1; z++) {
            indices.push_back(glm::uvec3(x * m_N + z, x * m_N + (z + 1), (x + 1) * m_N + (z + 1)));
            indices.push_back(glm::uvec3((x + 1) * m_N + z, x * m_N + z, (x + 1) * m_N + (z + 1)));
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_gl_PreviousPosBuffer);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec4), points.data(), GL_DYNAMIC_COPY);

    glBindBuffer(GL_ARRAY_BUFFER, m_gl_CurrentPosBuffer);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec4), points.data(), GL_DYNAMIC_COPY);

    glBindBuffer(GL_ARRAY_BUFFER, m_gl_DampingBuffer);
    glBufferData(GL_ARRAY_BUFFER, dampings.size() * sizeof(GLfloat), dampings.data(), GL_STATIC_READ);

    glBindBuffer(GL_ARRAY_BUFFER, m_gl_EBO);
    glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(glm::uvec3), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(m_gl_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_gl_PreviousPosBuffer);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_EBO);
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void waterGrid::PrepareShaders(const std::string& shaderPath)
{
    m_sh_waterSimulation.Init();
    m_sh_waterSimulation.AttachShader(shaderPath + "waterSimulation.comp", GL_COMPUTE_SHADER);
    m_sh_waterSimulation.Link();

    m_sh_waterSimulation.Use();
    m_sh_waterSimulation.set1i("N", m_N);
    m_sh_waterSimulation.set1f("dt", m_dt);
    m_sh_waterSimulation.set1f("c", m_c);
    m_sh_waterSimulation.set1f("h", m_h);

    m_sh_computeNormals.Init();
    m_sh_computeNormals.AttachShader(shaderPath + "waterNormals.comp", GL_COMPUTE_SHADER);
    m_sh_computeNormals.Link();

    m_sh_computeNormals.Use();
    m_sh_computeNormals.set1i("N", m_N);
    m_sh_computeNormals.set1f("h", m_h);
    m_sh_computeNormals.set1f("a", m_a);
}

void waterGrid::PrepareTextures() 
{
    glGenTextures(1, &m_gl_normalTex);
    glBindTexture(GL_TEXTURE_2D, m_gl_normalTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_N, m_N, 0, GL_RGBA, GL_FLOAT, nullptr);
}


void waterGrid::DeInitGL()
{
    glDeleteVertexArrays(1, &m_gl_VAO);
    glDeleteBuffers(1, &m_gl_PreviousPosBuffer);
    glDeleteBuffers(1, &m_gl_CurrentPosBuffer);
    glDeleteBuffers(1, &m_gl_DampingBuffer);
    glDeleteBuffers(1, &m_gl_EBO);
}

GLuint waterGrid::GetNormalTex()
{
    return m_gl_normalTex;
}

float waterGrid::GetA()
{
    return m_a;
}

void waterGrid::DisturbWaterAt(glm::vec2 coords, float newHeight, int i)
{
    coords += m_a / 2.0f;
    glm::vec2 indices = glm::floor(((coords / m_a) * (float)m_N));
    
    m_bShouldDisturb = true;
    m_iDisturb[i] = (int)indices.x;
    m_jDisturb[i] = (int)indices.y;
    m_disturbHeight[i] = newHeight;
}
