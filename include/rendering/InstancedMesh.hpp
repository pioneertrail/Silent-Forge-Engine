#pragma once
#include "rendering/Mesh.hpp"
#include "rendering/GLContext.hpp"
#include "rendering/Frustum.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <functional>
#include <string>
#include <string_view>
#include <memory>
#include <optional>

namespace SFE {
namespace Rendering {

struct InstanceAttribute {
    GLuint index;
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    const void* offset;
    GLuint divisor;
};

struct InstanceBounds {
    glm::vec3 center;
    float radius;
    glm::vec3 min;
    glm::vec3 max;
};

class InstancedMesh {
public:
    using LogCallback = std::function<void(std::string_view)>;

    // Constructor taking shared_ptr to Mesh instead of reference for better ownership
    explicit InstancedMesh(std::shared_ptr<Mesh> mesh, LogCallback logCallback = nullptr);
    ~InstancedMesh();

    // Delete copy operations
    InstancedMesh(const InstancedMesh&) = delete;
    InstancedMesh& operator=(const InstancedMesh&) = delete;

    // Move operations with inline implementation
    InstancedMesh(InstancedMesh&& other) noexcept;
    InstancedMesh& operator=(InstancedMesh&& other) noexcept;

    void setLogCallback(LogCallback callback);

    // Update instance data with model matrices
    void updateInstanceData(const std::vector<glm::mat4>& modelMatrices);

    // Update instance data with custom attributes
    void updateInstanceData(const std::vector<InstanceAttribute>& attributes, 
                          const void* data, size_t dataSize, size_t instanceCount);

    // Update instance bounds for culling
    void updateInstanceBounds(const std::vector<InstanceBounds>& bounds);

    // Add a single instance - return index of added instance
    size_t addInstance(const glm::mat4& modelMatrix, const InstanceBounds& bounds);

    // Remove an instance by index - return success status
    bool removeInstance(size_t index);

    // Clear all instances
    void clearInstances();

    // Getters
    size_t getInstanceCount() const noexcept { return m_modelMatrices.size(); }
    std::shared_ptr<Mesh> getMesh() const noexcept { return m_pMesh; }
    const std::vector<glm::mat4>& getModelMatrices() const noexcept { return m_modelMatrices; }

    // Draw instances with frustum culling
    void drawInstanced(const Frustum& frustum, GLsizei instanceCount) const;

    // Draw instances without culling
    void drawInstanced(GLsizei instanceCount) const;

private:
    void setupInstanceVBO();
    void setupInstanceAttributes(const std::vector<InstanceAttribute>& attributes);
    void resizeBuffer(size_t newSize);
    void logWarning(std::string_view message) const;
    bool validateState() const noexcept;

    // Use shared_ptr for better ownership semantics
    std::shared_ptr<Mesh> m_pMesh;
    GLuint m_instanceVBO{0};
    GLsizei m_maxInstances{0};
    LogCallback m_logCallback;
    std::vector<InstanceAttribute> m_currentAttributes;
    std::vector<InstanceBounds> m_instanceBounds;
    mutable std::vector<GLsizei> m_visibleInstances;
    std::vector<glm::mat4> m_modelMatrices;
};

// Inline implementations for simple methods (move semantics)
inline InstancedMesh::InstancedMesh(InstancedMesh&& other) noexcept
    : m_pMesh(std::move(other.m_pMesh))
    , m_instanceVBO(other.m_instanceVBO)
    , m_maxInstances(other.m_maxInstances)
    , m_logCallback(std::move(other.m_logCallback))
    , m_currentAttributes(std::move(other.m_currentAttributes))
    , m_instanceBounds(std::move(other.m_instanceBounds))
    , m_visibleInstances(std::move(other.m_visibleInstances))
    , m_modelMatrices(std::move(other.m_modelMatrices)) {
    other.m_instanceVBO = 0;
    other.m_maxInstances = 0;
}

inline InstancedMesh& InstancedMesh::operator=(InstancedMesh&& other) noexcept {
    if (this != &other) {
        if (m_instanceVBO != 0) {
            glDeleteBuffers(1, &m_instanceVBO);
            GLContext::CheckGLError("Deleting instance VBO in move assignment", m_logCallback);
        }

        m_pMesh = std::move(other.m_pMesh);
        m_instanceVBO = other.m_instanceVBO;
        m_maxInstances = other.m_maxInstances;
        m_logCallback = std::move(other.m_logCallback);
        m_currentAttributes = std::move(other.m_currentAttributes);
        m_instanceBounds = std::move(other.m_instanceBounds);
        m_visibleInstances = std::move(other.m_visibleInstances);
        m_modelMatrices = std::move(other.m_modelMatrices);

        other.m_instanceVBO = 0;
        other.m_maxInstances = 0;
    }
    return *this;
}

} // namespace Rendering
} // namespace SFE 