// src/rendering/Mesh.cpp
#include "rendering/Mesh.hpp"
#include <iostream>

namespace SFE { // Changed namespace to SFE

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    : vertexCount(static_cast<GLsizei>(vertices.size())), 
      indexCount(static_cast<GLsizei>(indices.size())), 
      texture(nullptr),
      useEBO(!indices.empty())
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    if (useEBO) {
        glGenBuffers(1, &ebo);
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    if (useEBO) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }

    // Position attribute (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    
    // Texture coordinate attribute (u, v)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(1);
    
    // Normal attribute (nx, ny, nz)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Note: Don't unbind EBO while VAO is active as VAO stores the EBO binding
    
    std::cout << "Mesh created (VAO: " << vao << ") - Vertices: " << vertexCount 
              << (useEBO ? ", Indices: " + std::to_string(indexCount) : "") << std::endl;
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    if (useEBO) {
        glDeleteBuffers(1, &ebo);
    }
    std::cout << "Mesh deleted (VAO: " << vao << ")" << std::endl;
}

void Mesh::draw() const {
    if (texture) {
        texture->bind(0);
    }
    
    glBindVertexArray(vao);
    if (useEBO) {
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }
    glBindVertexArray(0);
    
    if (texture) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Mesh::setTexture(std::shared_ptr<Texture> tex) {
    texture = tex;
}

} // End namespace SFE 