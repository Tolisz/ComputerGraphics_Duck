#include "duck.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <iostream>
#include <fstream>
#include <vector>

duck::duck()
{}

duck::~duck()
{}

void duck::InitGLFromFile(std::string duckFile)
{
    glGenVertexArrays(1, &m_gl_VAO);
    glGenBuffers(1, &m_gl_VerticesBO);
    glGenBuffers(1, &m_gl_NormalsBO);
    glGenBuffers(1, &m_gl_TextCoordsBO);
    glGenBuffers(1, &m_gl_EBO);

    std::ifstream fs(duckFile);
    if (!fs.is_open()) {
        std::cout << "Duck GL Init failed because file stream was not opened"
        << "\n\t[file]: " << duckFile << std::endl;
        return;
    }

    unsigned int V; fs >> V;
    
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    vertices.reserve(V);
    normals.reserve(V);
    texCoords.reserve(V);

    glm::vec3 v, n;
    glm::vec2 t;
    for (unsigned int i = 0; i < V; i++) {
        fs >> v.x; fs >> v.y; fs >> v.z;
        vertices.push_back(v);
        fs >> n.x; fs >> n.y; fs >> n.z;
        normals.push_back(n);
        fs >> t.x; fs >> t.y;
        texCoords.push_back(t);
    }

    unsigned int T; fs >> T;

    std::vector<glm::uvec3> indices;
    indices.reserve(T);

    glm::uvec3 ind;
    for (unsigned int i = 0; i < T; i++) {
        fs >> ind.x; fs >> ind.y; fs >> ind.z;
        indices.push_back(ind);
    }

    m_gl_indicesNum = T * 3;

    glBindVertexArray(m_gl_VAO);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, T * sizeof(glm::uvec3), indices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_gl_VerticesBO);
    glBufferData(GL_ARRAY_BUFFER, V * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_gl_NormalsBO);
    glBufferData(GL_ARRAY_BUFFER, V * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, m_gl_TextCoordsBO);
    glBufferData(GL_ARRAY_BUFFER, V * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void duck::DeInitGL()
{
    glDeleteVertexArrays(1, &m_gl_VAO);
    glGenBuffers(1, &m_gl_VerticesBO);
    glGenBuffers(1, &m_gl_NormalsBO);
    glGenBuffers(1, &m_gl_TextCoordsBO);
    glDeleteBuffers(1, &m_gl_EBO);
}

void duck::Draw()
{
    glBindVertexArray(m_gl_VAO);
    glDrawElements(GL_TRIANGLES, m_gl_indicesNum, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
