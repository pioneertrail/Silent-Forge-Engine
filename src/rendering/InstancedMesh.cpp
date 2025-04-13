#include "rendering/InstancedMesh.hpp"
#include "SFEGlobalIncludes.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

namespace SFE {
namespace Rendering {

InstancedMesh::InstancedMesh(std::shared_ptr<Mesh> mesh, LogCallback logCallback)
    : m_pMesh(std::move(mesh))
    , m_instanceVBO(0)
    , m_maxInstances(0)
    , m_logCallback(std::move(logCallback)) {
    
    if (!m_pMesh) {
        logWarning("Cannot create InstancedMesh with null mesh");
        return;
    }

    if (!GLContext::ValidateContext(m_logCallback)) {
        logWarning("Cannot create InstancedMesh without active OpenGL context");
        return;
    }

    setupInstanceVBO();
}

InstancedMesh::~InstancedMesh() {
    if (m_instanceVBO != 0) {
        glDeleteBuffers(1, &m_instanceVBO);
        GLContext::CheckGLError("Deleting instance VBO", m_logCallback);
        m_instanceVBO = 0;
    }
}

void InstancedMesh::setLogCallback(LogCallback callback) {
    m_logCallback = std::move(callback);
}

void InstancedMesh::setupInstanceVBO() {
    if (!validateState()) {
        return;
    }

    // Generate and bind the instance VBO
    glGenBuffers(1, &m_instanceVBO);
    GLContext::CheckGLError("Generating instance VBO", m_logCallback);

    // Bind the mesh's VAO
    m_pMesh->bind();
    GLContext::CheckGLError("Binding mesh VAO", m_logCallback);

    // Bind the instance VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    GLContext::CheckGLError("Binding instance VBO", m_logCallback);

    // Allocate initial buffer space
    m_maxInstances = 100; // Start with a reasonable size
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * m_maxInstances, nullptr, GL_DYNAMIC_DRAW);
    GLContext::CheckGLError("Allocating instance VBO memory", m_logCallback);

    // Set up vertex attributes for the model matrix (4 vec4s per matrix)
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), 
                            (void*)(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(3 + i, 1);
    }
    GLContext::CheckGLError("Setting up instance attributes", m_logCallback);

    // Clean up state
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_pMesh->unbind();
    GLContext::CheckGLError("Unbinding after setup", m_logCallback);
}

void InstancedMesh::setupInstanceAttributes(const std::vector<InstanceAttribute>& attributes) {
    if (!validateState()) {
        return;
    }

    m_currentAttributes = attributes;
    
    m_pMesh->bind();
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    
    for (const auto& attr : attributes) {
        glEnableVertexAttribArray(attr.index);
        glVertexAttribPointer(attr.index, attr.size, attr.type, 
                            attr.normalized, attr.stride, attr.offset);
        glVertexAttribDivisor(attr.index, attr.divisor);
    }
    GLContext::CheckGLError("Setting up instance attributes", m_logCallback);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_pMesh->unbind();
}

void InstancedMesh::updateInstanceData(const std::vector<glm::mat4>& modelMatrices) {
    if (!validateState() || modelMatrices.empty()) {
        return;
    }

    if (modelMatrices.size() > static_cast<size_t>(m_maxInstances)) {
        resizeBuffer(modelMatrices.size());
    }

    m_modelMatrices = modelMatrices;

    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    GLContext::CheckGLError("Binding instance VBO for update", m_logCallback);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * modelMatrices.size(), 
                   modelMatrices.data());
    GLContext::CheckGLError("Updating instance data", m_logCallback);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GLContext::CheckGLError("Unbinding instance VBO after update", m_logCallback);
}

void InstancedMesh::updateInstanceData(const std::vector<InstanceAttribute>& attributes, 
                                     const void* data, size_t dataSize, size_t instanceCount) {
    if (!validateState()) {
        return;
    }

    if (instanceCount == 0) {
        logWarning("Attempted to update instance data with zero instances");
        return;
    }
    
    if (instanceCount > static_cast<size_t>(m_maxInstances)) {
        resizeBuffer(instanceCount);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    GLContext::CheckGLError("Binding instance VBO for custom data update", m_logCallback);

    glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize * instanceCount, data);
    GLContext::CheckGLError("Updating custom instance data", m_logCallback);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GLContext::CheckGLError("Unbinding instance VBO after custom data update", m_logCallback);
    
    setupInstanceAttributes(attributes);
}

void InstancedMesh::updateInstanceBounds(const std::vector<InstanceBounds>& bounds) {
    m_instanceBounds = bounds;
}

size_t InstancedMesh::addInstance(const glm::mat4& modelMatrix, const InstanceBounds& bounds) {
    if (!validateState()) {
        return static_cast<size_t>(-1);
    }

    // Add the instance
    m_modelMatrices.push_back(modelMatrix);
    m_instanceBounds.push_back(bounds);
    
    // Resize buffer if needed
    if (m_modelMatrices.size() > static_cast<size_t>(m_maxInstances)) {
        resizeBuffer(m_modelMatrices.size() * 2); // Double the buffer size
    }
    
    // Update VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 
                   (m_modelMatrices.size() - 1) * sizeof(glm::mat4), 
                   sizeof(glm::mat4), 
                   &modelMatrix);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GLContext::CheckGLError("Adding instance", m_logCallback);
    
    return m_modelMatrices.size() - 1;
}

bool InstancedMesh::removeInstance(size_t index) {
    if (!validateState()) {
        return false;
    }

    if (index >= m_modelMatrices.size()) {
        logWarning("Attempted to remove out-of-range instance index");
        return false;
    }
    
    // Remove from vectors
    m_modelMatrices.erase(m_modelMatrices.begin() + index);
    
    if (index < m_instanceBounds.size()) {
        m_instanceBounds.erase(m_instanceBounds.begin() + index);
    }
    
    // Update entire VBO for simplicity
    // A more optimized approach would only update from the removed index onward
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    if (!m_modelMatrices.empty()) {
        glBufferSubData(GL_ARRAY_BUFFER, 0, 
                      m_modelMatrices.size() * sizeof(glm::mat4), 
                      m_modelMatrices.data());
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GLContext::CheckGLError("Removing instance", m_logCallback);
    
    return true;
}

void InstancedMesh::clearInstances() {
    m_modelMatrices.clear();
    m_instanceBounds.clear();
}

void InstancedMesh::drawInstanced(const Frustum& frustum, GLsizei instanceCount) const {
    if (!validateState()) {
        return;
    }

    const auto actualCount = instanceCount <= 0 ? 
        static_cast<GLsizei>(m_modelMatrices.size()) : 
        std::min(instanceCount, static_cast<GLsizei>(m_modelMatrices.size()));
    
    if (actualCount <= 0) {
        return;
    }
    
    if (m_instanceBounds.size() < static_cast<size_t>(actualCount)) {
        // If bounds aren't available, fall back to non-culling draw
        drawInstanced(actualCount);
        return;
    }
    
    // Determine visible instances
    m_visibleInstances.clear();
    m_visibleInstances.reserve(actualCount);
    
    for (GLsizei i = 0; i < actualCount; ++i) {
        const auto& bounds = m_instanceBounds[i];
        if (frustum.isSphereInside(bounds.center, bounds.radius)) {
            m_visibleInstances.push_back(i);
        }
    }
    
    if (m_visibleInstances.empty()) {
        return;
    }
    
    // Draw only visible instances
    m_pMesh->bind();
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    
    // Todo: Implement instance-specific draw for non-contiguous instances
    // For now, we just draw all instances which is less efficient but simpler
    glDrawElementsInstanced(GL_TRIANGLES, m_pMesh->getIndexCount(), 
                          GL_UNSIGNED_INT, nullptr, 
                          static_cast<GLsizei>(m_visibleInstances.size()));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_pMesh->unbind();
    GLContext::CheckGLError("Drawing culled instances", m_logCallback);
}

void InstancedMesh::drawInstanced(GLsizei instanceCount) const {
    if (!validateState()) {
        return;
    }

    const auto actualCount = instanceCount <= 0 ? 
        static_cast<GLsizei>(m_modelMatrices.size()) : 
        std::min(instanceCount, static_cast<GLsizei>(m_modelMatrices.size()));
    
    if (actualCount <= 0 || m_pMesh->getIndexCount() <= 0) {
        return;
    }

    // Bind the mesh's VAO
    m_pMesh->bind();
    GLContext::CheckGLError("Binding mesh VAO", m_logCallback);

    // Bind the instance VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    GLContext::CheckGLError("Binding instance VBO for drawing", m_logCallback);

    // Draw the mesh with instanced rendering
    glDrawElementsInstanced(GL_TRIANGLES, m_pMesh->getIndexCount(), 
                          GL_UNSIGNED_INT, nullptr, actualCount);
    GLContext::CheckGLError("Drawing instances", m_logCallback);

    // Clean up state
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_pMesh->unbind();
    GLContext::CheckGLError("Unbinding after drawing", m_logCallback);
}

void InstancedMesh::resizeBuffer(size_t newSize) {
    if (!validateState() || newSize <= 0) {
        return;
    }
    
    // Create a new buffer
    GLuint newBuffer = 0;
    glGenBuffers(1, &newBuffer);
    GLContext::CheckGLError("Generating new instance buffer", m_logCallback);
    
    glBindBuffer(GL_ARRAY_BUFFER, newBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * newSize, nullptr, GL_DYNAMIC_DRAW);
    GLContext::CheckGLError("Allocating new instance buffer", m_logCallback);
    
    // Copy existing data if there is any
    if (m_instanceVBO != 0 && !m_modelMatrices.empty()) {
        glBindBuffer(GL_COPY_READ_BUFFER, m_instanceVBO);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0,
                          m_modelMatrices.size() * sizeof(glm::mat4));
        glBindBuffer(GL_COPY_READ_BUFFER, 0);
        GLContext::CheckGLError("Copying instance data to new buffer", m_logCallback);
    }
    
    // Delete old buffer
    if (m_instanceVBO != 0) {
        glDeleteBuffers(1, &m_instanceVBO);
        GLContext::CheckGLError("Deleting old instance buffer", m_logCallback);
    }
    
    // Update VAO to use the new buffer
    m_pMesh->bind();
    
    // Update attribute pointers to use the new buffer
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), 
                            (void*)(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(3 + i, 1);
    }
    
    // Apply any custom attributes
    for (const auto& attr : m_currentAttributes) {
        glEnableVertexAttribArray(attr.index);
        glVertexAttribPointer(attr.index, attr.size, attr.type, 
                            attr.normalized, attr.stride, attr.offset);
        glVertexAttribDivisor(attr.index, attr.divisor);
    }
    
    m_pMesh->unbind();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Update instance variables
    m_instanceVBO = newBuffer;
    m_maxInstances = static_cast<GLsizei>(newSize);
    
    GLContext::CheckGLError("Completing buffer resize", m_logCallback);
}

void InstancedMesh::logWarning(std::string_view message) const {
    if (m_logCallback) {
        m_logCallback(message);
    } else {
        std::cerr << "InstancedMesh warning: " << message << std::endl;
    }
}

bool InstancedMesh::validateState() const noexcept {
    if (!m_pMesh) {
        std::cerr << "InstancedMesh error: No mesh available" << std::endl;
        return false;
    }
    
    if (!GLContext::ValidateContext(m_logCallback)) {
        std::cerr << "InstancedMesh error: No valid OpenGL context" << std::endl;
        return false;
    }
    
    return true;
}

} // namespace Rendering
} // namespace SFE 