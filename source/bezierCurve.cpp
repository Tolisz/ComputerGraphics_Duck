#include "bezierCurve.h"

bezierCurve::bezierCurve()
{}

bezierCurve::~bezierCurve()
{}

void bezierCurve::InitGL()
{
    glGenVertexArrays(1, &m_gl_VAO);
    glGenBuffers(1, &m_gl_VBO);

    glBindVertexArray(m_gl_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_gl_VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void bezierCurve::Draw()
{
    glBindVertexArray(m_gl_VAO);
    glDrawArrays(GL_LINES_ADJACENCY, 0, 4);
    glBindVertexArray(0);
}

void bezierCurve::DeInitGL()
{
    glDeleteVertexArrays(1, &m_gl_VAO);
    glDeleteBuffers(1, &m_gl_VBO);
}

void bezierCurve::UpdatePoints(glm::vec3 points[4])
{
    glBindBuffer(GL_ARRAY_BUFFER, m_gl_VBO);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), points, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}