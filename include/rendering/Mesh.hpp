#pragma once
#include <glad/glad.h>
#include <vector>
#include <cstddef> // For size_t
#include <memory>
#include "rendering/Texture.hpp"
#include <string>
#include <glm/glm.hpp>
#include "rendering/GLContext.hpp"
#include <functional>

namespace SFE { // Changed namespace to SFE
namespace Rendering {

class Mesh {
public:
    // Vertex structure for mesh creation
    struct Vertex {
        glm::vec3 position;
        glm::vec2 texCoord;
        glm::vec3 normal;
        
        Vertex(const glm::vec3& pos = glm::vec3(0.0f), 
               const glm::vec2& tex = glm::vec2(0.0f),
               const glm::vec3& norm = glm::vec3(0.0f)) 
            : position(pos), texCoord(tex), normal(norm) {}
    };

    using LogCallback = std::function<void(const std::string&)>;

    Mesh(LogCallback logCallback = nullptr);
    // Constructor that accepts vertex and index arrays
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, 
         LogCallback logCallback = nullptr);
    ~Mesh();

    // Delete copy operations
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Allow move operations
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void setLogCallback(LogCallback callback);

    // Load mesh data
    bool loadFromFile(const std::string& filepath);
    bool loadFromData(const std::vector<glm::vec3>& vertices,
                     const std::vector<glm::vec3>& normals,
                     const std::vector<glm::vec2>& texCoords,
                     const std::vector<unsigned int>& indices);
                     
    // Load from Vertex array
    bool loadFromVertexData(const std::vector<Vertex>& vertices,
                           const std::vector<unsigned int>& indices);

    // Bind/unbind mesh for rendering
    void bind() const;
    void unbind() const;

    // Get mesh data
    GLuint getVAO() const;
    GLuint getVBO() const;
    GLuint getEBO() const;
    size_t getVertexCount() const;
    size_t getIndexCount() const;
    const std::vector<glm::vec3>& getVertices() const;
    const std::vector<glm::vec3>& getNormals() const;
    const std::vector<glm::vec2>& getTexCoords() const;
    const std::vector<unsigned int>& getIndices() const;

    void setTexture(std::shared_ptr<Texture> texture);
    void render() const;

    // Draw the mesh
    void draw() const;

private:
    void setupBuffers();
    void cleanupBuffers();
    void logWarning(const std::string& message) const;
    bool validateState() const;

    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
    size_t m_vertexCount;
    size_t m_indexCount;
    LogCallback m_logCallback;
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_texCoords;
    std::vector<unsigned int> m_indices;
    std::shared_ptr<Texture> m_texture;
    bool useEBO;          // Whether to use indexed drawing
};

} // namespace Rendering
} // namespace SFE 