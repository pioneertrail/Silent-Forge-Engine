#pragma once
#include <glad/glad.h>
#include <vector>
#include <cstddef> // For size_t

namespace Engine {

struct Vertex {
    // Position, Normal, TexCoords, etc. - Define as needed
    float Position[3];
    // Example: Add other attributes here
    // float Normal[3];
    // float TexCoords[2];
};

class Mesh {
public:
    // Constructor taking vertex data and optionally index data
    Mesh(const std::vector<float>& vertices, GLsizei vertexCount, const std::vector<unsigned int>& indices = {});
    // Alternative constructor using a Vertex struct (more structured)
    // Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices = {});

    ~Mesh();

    // Rule of five: Prevent copying/moving for simple resource management
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(Mesh&&) = delete;

    void draw() const;

private:
    void setupMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    // void setupMeshFromStruct(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    GLuint vao, vbo, ebo; // Vertex Array Object, Vertex Buffer Object, Element Buffer Object
    GLsizei elementCount; // Number of indices to draw (if using EBO)
    GLsizei vertexDrawCount; // Number of vertices to draw (if not using EBO)
    bool useEbo;
};
} 