#include "rendering/InstancedMesh.hpp"
#include <glad/glad.h>

namespace SFE {

InstancedMesh::InstancedMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    : Mesh(vertices, indices) {
    setupInstanceVBO();
}

InstancedMesh::~InstancedMesh() {
    glDeleteBuffers(1, &instanceVBO);
}

void InstancedMesh::setupInstanceVBO() {
    glGenBuffers(1, &instanceVBO);
    
    // Bind VAO first
    glBindVertexArray(VAO);
    
    // Set up instance data buffer
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    // We'll update the buffer data later with actual instances
    
    // Set up matrix attribute pointers (a mat4 uses 4 vertex attributes)
    std::size_t vec4Size = sizeof(glm::vec4);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(3 + i); // Start after existing attributes
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                             (void*)(vec4Size * i));
        glVertexAttribDivisor(3 + i, 1); // Tell OpenGL this is instanced
    }
    
    glBindVertexArray(0);
}

void InstancedMesh::updateInstanceData(const std::vector<glm::mat4>& modelMatrices) {
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4),
                 modelMatrices.data(), GL_DYNAMIC_DRAW);
}

void InstancedMesh::drawInstanced(unsigned int instanceCount) const {
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
                          GL_UNSIGNED_INT, 0, instanceCount);
    glBindVertexArray(0);
}

} 