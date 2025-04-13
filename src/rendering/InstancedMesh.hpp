#pragma once

#include "Mesh.hpp"
#include "GLContext.hpp"
#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <functional>
#include <string>

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
};

/**
 * @class InstancedMesh
 * @brief A class that extends Mesh to support instanced rendering.
 * 
 * This class allows rendering multiple instances of the same mesh with different
 * model matrices efficiently using OpenGL instanced rendering.
 * 
 * @note The shader must include a mat4 instance attribute at locations 3-6
 * (e.g., layout(location = 3) in mat4 instanceMatrix).
 */
class InstancedMesh {
public:
    /// Type definition for logging callback function
    using LogCallback = std::function<void(const std::string&)>;

    /**
     * @brief Constructs an InstancedMesh from a base mesh.
     * @param mesh The base mesh to be instanced.
     * @param logCallback Optional callback function for logging messages.
     */
    InstancedMesh(const Mesh& mesh, LogCallback logCallback = nullptr);
    ~InstancedMesh();

    // Move operations
    InstancedMesh(InstancedMesh&& other) noexcept;
    InstancedMesh& operator=(InstancedMesh&& other) noexcept;

    // Delete copy operations
    InstancedMesh(const InstancedMesh&) = delete;
    InstancedMesh& operator=(const InstancedMesh&) = delete;

    /**
     * @brief Updates the instance data with new model matrices.
     * @param modelMatrices Vector of model matrices for each instance.
     * @note If the vector is empty, the update will be skipped and a warning logged.
     */
    void updateInstanceData(const std::vector<glm::mat4>& modelMatrices);

    /**
     * @brief Draws the mesh with the specified number of instances.
     * @param instanceCount Number of instances to render.
     * @note The draw call will be skipped if instanceCount is invalid or if the mesh
     *       has no indices.
     */
    void drawInstanced(GLsizei instanceCount) const;

    /**
     * @brief Sets a new logging callback function.
     * @param callback The new logging callback function.
     */
    void setLogCallback(LogCallback callback);

    void setupInstanceVBO();
    void setupInstanceAttributes(const std::vector<InstanceAttribute>& attributes);
    void updateInstanceData(const std::vector<InstanceAttribute>& attributes, 
                          const void* data, size_t dataSize, size_t instanceCount);
    void updateInstanceBounds(const std::vector<InstanceBounds>& bounds);
    void drawInstanced(const Frustum& frustum, GLsizei instanceCount) const;

private:
    /**
     * @brief Sets up the instance VBO for storing model matrices.
     */
    void setupInstanceVBO();

    /**
     * @brief Logs a warning message using the configured callback or std::cerr.
     * @param message The warning message to log.
     */
    void logWarning(const std::string& message) const;

    const Mesh& m_mesh;        ///< Reference to the base mesh
    GLuint m_instanceVBO;      ///< VBO for storing instance data (model matrices)
    size_t m_maxInstances;     ///< Maximum number of instances supported
    LogCallback m_logCallback;  ///< Optional callback for logging messages
    std::vector<InstanceAttribute> m_currentAttributes;
    std::vector<InstanceBounds> m_instanceBounds;
    mutable std::vector<GLsizei> m_visibleInstances;
    std::vector<glm::mat4> m_modelMatrices;

    bool validateState() const;
    void resizeBuffer(size_t newSize);
};

} // namespace Rendering
} // namespace SFE 