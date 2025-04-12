#pragma once

#include <string>
#include <GL/glew.h>

class Texture {
public:
    Texture();
    ~Texture();

    bool loadFromFile(const std::string& filename);
    void bind(GLenum textureUnit = GL_TEXTURE0) const;
    void unbind() const;

    GLuint getID() const { return m_textureID; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    GLuint m_textureID;
    int m_width;
    int m_height;
    int m_channels;
}; 