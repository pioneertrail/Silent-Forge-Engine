#pragma once
#include <glad/glad.h>
#include <vector>
#include <cstddef> // For size_t
#include <memory>
#include "rendering/Texture.hpp"

namespace SFE { // Changed namespace to SFE

class Mesh {
public:
    struct Vertex {
        float position[3];
        float texCoord[2];
        float normal[3];    // Added normal vector for lighting
    };

    // Constructor taking vertex data and optionally index data
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices = {});
    
    ~Mesh();

    // Rule of five: Prevent copying/moving for simple resource management
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(Mesh&&) = delete;

    // Draw the mesh
    void draw() const;
    void setTexture(std::shared_ptr<Texture> tex);

private:
    GLuint vao, vbo, ebo; // Vertex Array Object, Vertex Buffer Object, Element Buffer Object
    GLsizei vertexCount;  // Number of vertices
    GLsizei indexCount;   // Number of indices
    std::shared_ptr<Texture> texture;
    bool useEBO;          // Whether to use indexed drawing
};
} // End namespace SFE 