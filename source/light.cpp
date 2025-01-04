#include "light.h"

#include <glm/gtc/matrix_transform.hpp>

bool light::m_gl_isInit = false;

GLuint light::m_gl_VAO = 0;
GLuint light::m_gl_VBO = 0;
GLuint light::m_gl_EBO = 0;

// Light quat lies on XY plane
// ===========================

/* static */ const glm::vec3 light::m_gl_vertices[4] = {
    {-1.0f, -1.0f, 0.0f},   // 0 - bottom left 
    {-1.0f,  1.0f, 0.0f},   // 1 - top left 
    { 1.0f,  1.0f, 0.0f},   // 2 - top right
    { 1.0f, -1.0f, 0.0f}    // 3 - bottom right
};

/* static */ const glm::uvec3 light::m_gl_indices[2] = {
    {0, 1, 2}, 
    {3, 2, 0}
};

light::light()
{}

light::~light()
{}

void light::InitGL()
{
    if (m_gl_isInit)
        return;

    glCreateVertexArrays(1, &m_gl_VAO);
    glCreateBuffers(1, &m_gl_VBO);
    glCreateBuffers(1, &m_gl_EBO);

    glBindVertexArray(m_gl_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_gl_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 4, m_gl_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uvec3) * 2, m_gl_indices, GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_gl_isInit = true;
}

void light::Draw()
{
    glBindVertexArray(m_gl_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void light::DeInitGL()
{
    if (!m_gl_isInit)
        return;
    
    glDeleteVertexArrays(1, &m_gl_VAO);
    glDeleteBuffers(1, &m_gl_VBO);
    glDeleteBuffers(1, &m_gl_EBO);
    m_gl_isInit = false;
}
