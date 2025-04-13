// src/rendering/Mesh.cpp
#include "rendering/Mesh.hpp"
#include "SFEGlobalIncludes.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

namespace SFE {
namespace Rendering {

Mesh::Mesh(LogCallback logCallback)
    : m_vao(0)
    , m_vbo(0)
    , m_ebo(0)
    , m_vertexCount(0)
    , m_indexCount(0)
    , m_logCallback(logCallback) {
    if (!GLContext::ValidateContext(m_logCallback)) {
        logWarning("Cannot create Mesh without active OpenGL context");
        return;
    }

    setupBuffers();
}

// New constructor for vertices and indices
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, LogCallback logCallback)
    : m_vao(0)
    , m_vbo(0)
    , m_ebo(0)
    , m_vertexCount(0)
    , m_indexCount(0)
    , m_logCallback(logCallback) {
    if (!GLContext::ValidateContext(m_logCallback)) {
        logWarning("Cannot create Mesh without active OpenGL context");
        return;
    }

    setupBuffers();
    loadFromVertexData(vertices, indices);
}

Mesh::~Mesh() {
    cleanupBuffers();
}

Mesh::Mesh(Mesh&& other) noexcept
    : m_vao(other.m_vao)
    , m_vbo(other.m_vbo)
    , m_ebo(other.m_ebo)
    , m_vertexCount(other.m_vertexCount)
    , m_indexCount(other.m_indexCount)
    , m_logCallback(std::move(other.m_logCallback))
    , m_vertices(std::move(other.m_vertices))
    , m_normals(std::move(other.m_normals))
    , m_texCoords(std::move(other.m_texCoords))
    , m_indices(std::move(other.m_indices)) {
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
    other.m_vertexCount = 0;
    other.m_indexCount = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        cleanupBuffers();

        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_ebo = other.m_ebo;
        m_vertexCount = other.m_vertexCount;
        m_indexCount = other.m_indexCount;
        m_logCallback = std::move(other.m_logCallback);
        m_vertices = std::move(other.m_vertices);
        m_normals = std::move(other.m_normals);
        m_texCoords = std::move(other.m_texCoords);
        m_indices = std::move(other.m_indices);

        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_ebo = 0;
        other.m_vertexCount = 0;
        other.m_indexCount = 0;
    }
    return *this;
}

void Mesh::setLogCallback(LogCallback callback) {
    m_logCallback = callback;
}

bool Mesh::loadFromFile(const std::string& filepath) {
    if (!validateState()) {
        return false;
    }

    // TODO: Implement mesh loading from file
    logWarning("Mesh loading from file not implemented yet");
    return false;
}

bool Mesh::loadFromData(const std::vector<glm::vec3>& vertices,
                       const std::vector<glm::vec3>& normals,
                       const std::vector<glm::vec2>& texCoords,
                       const std::vector<unsigned int>& indices) {
    if (!validateState()) {
        return false;
    }

    if (vertices.empty()) {
        logWarning("Attempted to load mesh with no vertices");
        return false;
    }

    m_vertices = vertices;
    m_normals = normals;
    m_texCoords = texCoords;
    m_indices = indices;
    m_vertexCount = vertices.size();
    m_indexCount = indices.size();

    // Bind VAO
    glBindVertexArray(m_vao);
    GLContext::CheckGLError("Binding VAO for data update", m_logCallback);

    // Update vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    GLContext::CheckGLError("Binding VBO for data update", m_logCallback);

    size_t vertexDataSize = vertices.size() * sizeof(glm::vec3);
    size_t normalDataSize = normals.size() * sizeof(glm::vec3);
    size_t texCoordDataSize = texCoords.size() * sizeof(glm::vec2);
    size_t totalSize = vertexDataSize + normalDataSize + texCoordDataSize;

    glBufferData(GL_ARRAY_BUFFER, totalSize, nullptr, GL_STATIC_DRAW);
    GLContext::CheckGLError("Allocating vertex buffer", m_logCallback);

    // Copy vertex data
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexDataSize, vertices.data());
    GLContext::CheckGLError("Copying vertex data", m_logCallback);

    // Copy normal data
    if (!normals.empty()) {
        glBufferSubData(GL_ARRAY_BUFFER, vertexDataSize, normalDataSize, normals.data());
        GLContext::CheckGLError("Copying normal data", m_logCallback);
    }

    // Copy texture coordinate data
    if (!texCoords.empty()) {
        glBufferSubData(GL_ARRAY_BUFFER, vertexDataSize + normalDataSize, 
                       texCoordDataSize, texCoords.data());
        GLContext::CheckGLError("Copying texture coordinate data", m_logCallback);
    }

    // Set up vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    GLContext::CheckGLError("Setting up position attribute", m_logCallback);

    if (!normals.empty()) {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 
                            (void*)vertexDataSize);
        GLContext::CheckGLError("Setting up normal attribute", m_logCallback);
    }

    if (!texCoords.empty()) {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 
                            (void*)(vertexDataSize + normalDataSize));
        GLContext::CheckGLError("Setting up texture coordinate attribute", m_logCallback);
    }

    // Update index buffer if needed
    if (!indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        GLContext::CheckGLError("Binding EBO for data update", m_logCallback);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
                    indices.data(), GL_STATIC_DRAW);
        GLContext::CheckGLError("Updating index buffer", m_logCallback);
    }

    // Clean up
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    GLContext::CheckGLError("Unbinding after data update", m_logCallback);

    return true;
}

bool Mesh::loadFromVertexData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    if (!validateState()) {
        return false;
    }

    if (vertices.empty()) {
        logWarning("Attempted to load mesh with no vertices");
        return false;
    }

    // Update counts
    m_vertexCount = vertices.size();
    m_indexCount = indices.size();

    // Extract position, normal, and texture coordinate data
    m_vertices.resize(vertices.size());
    m_normals.resize(vertices.size());
    m_texCoords.resize(vertices.size());

    for (size_t i = 0; i < vertices.size(); ++i) {
        m_vertices[i] = vertices[i].position;
        m_normals[i] = vertices[i].normal;
        m_texCoords[i] = vertices[i].texCoord;
    }

    m_indices = indices;

    // Bind VAO
    glBindVertexArray(m_vao);
    GLContext::CheckGLError("Binding VAO for vertex data update", m_logCallback);

    // Update vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    GLContext::CheckGLError("Binding VBO for vertex data update", m_logCallback);

    // Allocate and upload buffer data
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    GLContext::CheckGLError("Uploading vertex data", m_logCallback);

    // Set up vertex attributes - position, texcoord, normal
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    GLContext::CheckGLError("Setting up position attribute", m_logCallback);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    GLContext::CheckGLError("Setting up texture coordinate attribute", m_logCallback);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    GLContext::CheckGLError("Setting up normal attribute", m_logCallback);

    // Update index buffer if needed
    if (!indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        GLContext::CheckGLError("Binding EBO for index data update", m_logCallback);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
                    indices.data(), GL_STATIC_DRAW);
        GLContext::CheckGLError("Uploading index data", m_logCallback);
    }

    // Clean up
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    GLContext::CheckGLError("Unbinding after vertex data update", m_logCallback);

    return true;
}

void Mesh::bind() const {
    if (!validateState()) {
        return;
    }

    glBindVertexArray(m_vao);
    GLContext::CheckGLError("Binding mesh VAO", m_logCallback);
}

void Mesh::unbind() const {
    if (!validateState()) {
        return;
    }

    glBindVertexArray(0);
    GLContext::CheckGLError("Unbinding mesh VAO", m_logCallback);
}

GLuint Mesh::getVAO() const {
    return m_vao;
}

GLuint Mesh::getVBO() const {
    return m_vbo;
}

GLuint Mesh::getEBO() const {
    return m_ebo;
}

size_t Mesh::getVertexCount() const {
    return m_vertexCount;
}

size_t Mesh::getIndexCount() const {
    return m_indexCount;
}

const std::vector<glm::vec3>& Mesh::getVertices() const {
    return m_vertices;
}

const std::vector<glm::vec3>& Mesh::getNormals() const {
    return m_normals;
}

const std::vector<glm::vec2>& Mesh::getTexCoords() const {
    return m_texCoords;
}

const std::vector<unsigned int>& Mesh::getIndices() const {
    return m_indices;
}

void Mesh::setupBuffers() {
    if (!GLContext::ValidateContext(m_logCallback)) {
        logWarning("Cannot set up mesh buffers without active OpenGL context");
        return;
    }

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    if (!GLContext::ValidateVertexArray(m_vao, m_logCallback) ||
        !GLContext::ValidateBuffer(m_vbo, m_logCallback) ||
        !GLContext::ValidateBuffer(m_ebo, m_logCallback)) {
        logWarning("Failed to create mesh buffers");
        cleanupBuffers();
        return;
    }

    GLContext::CheckGLError("Creating mesh buffers", m_logCallback);
}

void Mesh::cleanupBuffers() {
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if (m_ebo != 0) {
        glDeleteBuffers(1, &m_ebo);
        m_ebo = 0;
    }
    GLContext::CheckGLError("Cleaning up mesh buffers", m_logCallback);
}

void Mesh::logWarning(const std::string& message) const {
    if (m_logCallback) {
        m_logCallback(message);
    } else {
        std::cerr << "Mesh Warning: " << message << std::endl;
    }
}

bool Mesh::validateState() const {
    if (!GLContext::ValidateContext(m_logCallback)) {
        logWarning("Invalid OpenGL context");
        return false;
    }

    if (!GLContext::ValidateVertexArray(m_vao, m_logCallback)) {
        logWarning("Invalid vertex array object");
        return false;
    }

    if (!GLContext::ValidateBuffer(m_vbo, m_logCallback)) {
        logWarning("Invalid vertex buffer object");
        return false;
    }

    if (m_ebo != 0 && !GLContext::ValidateBuffer(m_ebo, m_logCallback)) {
        logWarning("Invalid element buffer object");
        return false;
    }

    return true;
}

void Mesh::setTexture(std::shared_ptr<Texture> texture) {
    m_texture = texture;
}

void Mesh::render() const {
    if (!validateState()) {
        return;
    }

    if (m_texture) {
        m_texture->bind();
    }

    bind();

    if (m_indexCount > 0) {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, 0);
        GLContext::CheckGLError("Drawing indexed mesh", m_logCallback);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertexCount));
        GLContext::CheckGLError("Drawing non-indexed mesh", m_logCallback);
    }

    unbind();

    if (m_texture) {
        m_texture->unbind();
    }
}

void Mesh::draw() const {
    if (!validateState()) {
        return;
    }

    bind();

    if (m_indexCount > 0) {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, 0);
        GLContext::CheckGLError("Drawing indexed mesh", m_logCallback);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertexCount));
        GLContext::CheckGLError("Drawing non-indexed mesh", m_logCallback);
    }

    unbind();
}

} // namespace Rendering
} // namespace SFE 