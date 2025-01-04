#pragma once 

#include <glad/glad.h>
#include <string>

class duck 
{
private:
    GLuint m_gl_VAO;
    GLuint m_gl_VerticesBO;
    GLuint m_gl_NormalsBO;
    GLuint m_gl_TextCoordsBO;
    GLuint m_gl_EBO;

    GLsizei m_gl_indicesNum;

public:
    // *=*=*=*=*=*=*=*=*=*=
    //   Object Creation
    // *=*=*=*=*=*=*=*=*=*=

    duck();
    ~duck();

    duck(const duck&) = delete;
    duck(duck&&) = delete;
    duck& operator=(const duck&) = delete;
    duck& operator=(duck&&) = delete;

    // *=*=*=*=*=*=*=*=*=*=
    //      Methods
    // *=*=*=*=*=*=*=*=*=*=

    void InitGLFromFile(std::string duckFile);
    void DeInitGL();

    void Draw();

};