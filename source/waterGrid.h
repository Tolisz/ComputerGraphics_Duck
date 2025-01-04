#pragma once

#include <glad/glad.h>

#include "shader.h"

/* Changes water grid's size. Default value is 256.
 * This value was used to make optimalizations in the compute shaders
 */
#define GRID_SIZE 256
#define GRID_WORK_GROUP_XY_SIZE 32

class waterGrid
{
private: 

    int m_N;
    float m_a;
    float m_h;
    float m_c;
    float m_dt;

private: 

    GLuint m_gl_WorkGroupNum;
    GLuint m_gl_VAO;
    GLuint m_gl_EBO;

public:

    GLuint m_gl_PreviousPosBuffer;
    GLuint m_gl_CurrentPosBuffer;
    GLuint m_gl_DampingBuffer;
    GLuint m_gl_normalTex;

private:

    GLsizei m_gl_triangleCount;
    shader m_sh_waterSimulation;
    shader m_sh_computeNormals;

private:

    bool m_bShouldDisturb;
    int m_iDisturb[2];
    int m_jDisturb[2];
    float m_disturbHeight[2];

public:

    // *=*=*=*=*=*=*=*=*=*=
    //   Object Creation
    // *=*=*=*=*=*=*=*=*=*=

    waterGrid();
    waterGrid(int N, float a, float c);
    ~waterGrid();

    waterGrid(const waterGrid&) = delete;
    waterGrid(waterGrid&&) = delete;
    waterGrid& operator=(const waterGrid&) = delete;
    waterGrid& operator=(waterGrid&&) = delete;

    // *=*=*=*=*=*=*=*=*=*=
    //      Methods
    // *=*=*=*=*=*=*=*=*=*=

    void InitGL(const std::string& shaderPath);
    void DeInitGL();
    
    void SimulateWater(float dt);
    void DisturbWaterAt(glm::vec2 coords, float newHeight, int i);
    void Draw();

    GLuint GetNormalTex();
    float GetA();
    
private:

    void PopulateBuffers();
    void PrepareShaders(const std::string& shaderPath);
    void PrepareTextures();
};

