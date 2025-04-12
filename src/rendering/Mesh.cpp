// src/rendering/Mesh.cpp
#include "rendering/Mesh.hpp"
#include <iostream>
#include <string>

namespace Engine {

Mesh::Mesh(const std::vector<float>& vertices, GLsizei vertexCount, const std::vector<unsigned int>& indices)
    : vao(0), vbo(0), ebo(0), elementCount(0), vertexDrawCount(vertexCount), useEbo(!indices.empty())
{
    setupMesh(vertices, indices);
    std::cout << "Mesh created (VAO: " << vao << ") - Vertices: " << vertexDrawCount << (useEbo ? ", Indices: " + std::to_string(elementCount) : "") << std::endl;
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    if (useEbo) {
        glDeleteBuffers(1, &ebo);
    }
    std::cout << "Mesh deleted (VAO: " << vao << ")" << std::endl;
}

void Mesh::setupMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    if (useEbo) {
        glGenBuffers(1, &ebo);
        elementCount = static_cast<GLsizei>(indices.size());
    }

    glBindVertexArray(vao);

    // Load vertex data into VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Load index data into EBO (if applicable)
    if (useEbo) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }

    // Set vertex attribute pointers
    // Assuming vertex data is tightly packed: Pos(3 floats)
    // Modify stride and offset if vertex structure changes (e.g., adding normals, tex coords)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Add more glVertexAttribPointer calls here for other attributes (normals, tex coords, etc.)
    // Example: Normal attribute at location 1
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);

    // Unbind VAO to prevent accidental modification
    glBindVertexArray(0);
    // It's often safe practice to unbind the VBO and EBO after unbinding the VAO,
    // especially the EBO, as its binding is stored *within* the VAO state.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (useEbo) {
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void Mesh::draw() const {
    glBindVertexArray(vao);
    if (useEbo) {
        glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, vertexDrawCount);
    }
    glBindVertexArray(0); // Unbind VAO after drawing
}

} 