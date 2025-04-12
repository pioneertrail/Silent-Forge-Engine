#pragma once
#include <glad/glad.h>
#include <string>

namespace SFE {
class Texture {
public:
    Texture();
    ~Texture();

    bool loadFromFile(const std::string& path);
    void bind(GLuint unit = 0) const;
    GLuint getID() const { return id; }
    GLuint& getID() { return id; }

private:
    GLuint id;
};
} 