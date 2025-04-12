#pragma once
#include <glad/glad.h>
#include <vector>
#include <cstddef> // For size_t
#include <memory>
#include "rendering/Texture.hpp"
#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>

namespace SFE { // Changed namespace to SFE

class Mesh {
public:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    Mesh();
    ~Mesh();

    bool loadFromFile(const std::string& filename);
    void setVertices(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void setTexture(Texture* texture);
    void render() const;

    // Rule of five: Prevent copying/moving for simple resource management
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(Mesh&&) = delete;

    // Draw the mesh
    void draw() const;
    void setTexture(std::shared_ptr<Texture> tex);

private:
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;
    GLsizei m_indexCount;
    Texture* m_texture;
    bool useEBO;          // Whether to use indexed drawing
};
} // End namespace SFE 