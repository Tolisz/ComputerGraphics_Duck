#include "skyBox.h"

/* static */ glm::vec3 skyBox::m_skyBoxVertices[8] = {
    // back (Z = -1.0f)
    // 1 -> 2 
    // |    |
    // 0    3
    {-1.0f, -1.0f, -1.0f},
    {-1.0f,  1.0f, -1.0f},
    { 1.0f,  1.0f, -1.0f},
    { 1.0f, -1.0f, -1.0f},

    // front (Z = 1.0f)
    // 5 -> 6 
    // |    |
    // 4    7
    {-1.0f, -1.0f, 1.0f},
    {-1.0f,  1.0f, 1.0f},
    { 1.0f,  1.0f, 1.0f},
    { 1.0f, -1.0f, 1.0f}
};

/* static */ glm::uvec3 skyBox::m_skyBoxIndices[12] = {
    /* back */
    {2, 1, 0},
    {3, 2, 0},
    /* front */ 
    {4, 5, 6},
    {4, 6, 7},
    
    /* right */  
    {6, 2, 3},
    {7, 6, 3},
    /* left */ 
    {0, 1, 5},
    {0, 5, 4},
    
    /* top */
    {5, 1, 2},
    {5, 2, 6},
    /* bottom */
    {3, 0, 4},
    {7, 3, 4}
};

skyBox::skyBox()
{
}

skyBox::~skyBox()
{
}

void skyBox::Draw()
{
    glBindVertexArray(m_gl_VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
}

void skyBox::InitGL()
{
    glGenVertexArrays(1, &m_gl_VAO);
    glGenBuffers(1, &m_gl_VBO);
    glGenBuffers(1, &m_gl_EBO);

    glBindVertexArray(m_gl_VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * sizeof(glm::uvec3), m_skyBoxIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_gl_VBO);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec3), m_skyBoxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void skyBox::DeInitGL()
{
    glDeleteVertexArrays(1, &m_gl_VAO);
    glDeleteBuffers(1, &m_gl_VBO);
    glDeleteBuffers(1, &m_gl_EBO);
}
