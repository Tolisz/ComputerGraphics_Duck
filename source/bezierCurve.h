#pragma once

#include <glad/glad.h>
#include <glm/vec3.hpp>

class bezierCurve 
{
private:

    GLuint m_gl_VAO;
    GLuint m_gl_VBO;

public:

    // *=*=*=*=*=*=*=*=*=*=
    //   Object Creation
    // *=*=*=*=*=*=*=*=*=*=

    bezierCurve();
    ~bezierCurve();

    bezierCurve(const bezierCurve&) = delete;
    bezierCurve(bezierCurve&&) = delete;
    bezierCurve& operator=(const bezierCurve&) = delete;
    bezierCurve& operator=(bezierCurve&&) = delete;

    // *=*=*=*=*=*=*=*=*=*=
    //      Methods
    // *=*=*=*=*=*=*=*=*=*=

    void InitGL();
    void Draw();
    void DeInitGL();

    void UpdatePoints(glm::vec3 points[4]);
};