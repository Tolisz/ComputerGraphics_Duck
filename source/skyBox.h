#pragma once

#include <glad/glad.h>
#include <glm/vec3.hpp>

class skyBox {

private:

    GLuint m_gl_VAO;
    GLuint m_gl_VBO;
    GLuint m_gl_EBO;

    static glm::vec3 m_skyBoxVertices[8];
    static glm::uvec3 m_skyBoxIndices[12];

public: 

    // *=*=*=*=*=*=*=*=*=*=
    //   Object Creation
    // *=*=*=*=*=*=*=*=*=*=

    skyBox();
    ~skyBox();

    skyBox(const skyBox&) = delete;
    skyBox(skyBox&&) = delete;
    skyBox& operator=(const skyBox&) = delete;
    skyBox& operator=(skyBox&&) = delete;

    // *=*=*=*=*=*=*=*=*=*=
    //      Methods
    // *=*=*=*=*=*=*=*=*=*=

    void InitGL();
    void Draw();
    void DeInitGL();

private:


};

