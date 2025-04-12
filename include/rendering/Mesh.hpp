#pragma once
#include <glad/glad.h>
#include <vector>
#include <memory>
#include "rendering/Texture.hpp"

namespace SFE {
class Mesh {
public:
    struct Vertex {
        float position[3];
        float texCoord[2];
    };

    Mesh(const std::vector<Vertex>& vertices);
    ~Mesh();

    void draw() const;
    void setTexture(std::shared_ptr<Texture> tex);

private:
    GLuint vao, vbo;
    GLsizei vertexCount;
    std::shared_ptr<Texture> texture;
};
} 