// src/rendering/Mesh.cpp
#include "rendering/Mesh.hpp"
#include <iostream>

namespace SFE { // Changed namespace to SFE

Mesh::Mesh() : m_VAO(0), m_VBO(0), m_EBO(0), m_indexCount(0), m_texture(nullptr) {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
}

Mesh::~Mesh() {
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
}

void Mesh::setVertices(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    glBindVertexArray(m_VAO);

    // Vertex data
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Index data
    m_indexCount = static_cast<GLsizei>(indices.size());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Mesh::setTexture(Texture* texture) {
    m_texture = texture;
}

void Mesh::render() const {
    if (m_texture) {
        m_texture->bind();
    }

    glBindVertexArray(m_VAO);
    if (m_indexCount > 0) {
        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);

    if (m_texture) {
        m_texture->unbind();
    }
}

} // End namespace SFE 