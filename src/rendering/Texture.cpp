#include "rendering/Texture.hpp"
#include "SFEGlobalIncludes.hpp"
#include <stb/stb_image.h>
#include <iostream>
#include <chrono>

namespace SFE {
namespace Rendering {

Texture::Texture() : m_textureID(0), m_width(0), m_height(0), m_channels(0) {
    if (glfwGetCurrentContext()) {
        glGenTextures(1, &m_textureID);
    } else {
        std::cerr << "Warning: Creating Texture without active OpenGL context" << std::endl;
    }
}

Texture::~Texture() {
    if (m_textureID && glfwGetCurrentContext()) {
        glDeleteTextures(1, &m_textureID);
    }
}

bool Texture::loadFromFile(const std::string& filename) {
    if (!glfwGetCurrentContext()) {
        std::cerr << "Error: Cannot load texture without active OpenGL context" << std::endl;
        return false;
    }

    auto start = std::chrono::high_resolution_clock::now();
    
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename.c_str(), &m_width, &m_height, &m_channels, 0);
    
    if (!data) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return false;
    }

    if (!bind()) {
        stbi_image_free(data);
        return false;
    }

    GLenum format = GL_RGB;  // Default to RGB
    if (m_channels == 1) {
        format = GL_RED;
    } else if (m_channels == 3) {
        format = GL_RGB;
    } else if (m_channels == 4) {
        format = GL_RGBA;
    } else {
        std::cerr << "Unsupported number of channels: " << m_channels << std::endl;
        stbi_image_free(data);
        return false;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    unbind();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    if (duration > 50) {  // Warn if loading takes more than 50ms
        std::cerr << "Warning: Texture loading took " << duration << "ms for " << filename << std::endl;
    }

    return true;
}

bool Texture::bind(GLenum textureUnit) const {
    if (!glfwGetCurrentContext()) {
        std::cerr << "Error: Cannot bind texture without active OpenGL context" << std::endl;
        return false;
    }
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    return true;
}

bool Texture::unbind() const {
    if (!glfwGetCurrentContext()) {
        std::cerr << "Error: Cannot unbind texture without active OpenGL context" << std::endl;
        return false;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

int Texture::getWidth() const {
    return m_width;
}

int Texture::getHeight() const {
    return m_height;
}

int Texture::getChannels() const {
    return m_channels;
}

GLuint Texture::getTextureID() const {
    return m_textureID;
}

void Texture::setId(GLuint id) {
    // Delete existing texture if we have one
    if (m_textureID && glfwGetCurrentContext()) {
        glDeleteTextures(1, &m_textureID);
    }
    
    m_textureID = id;
    // Default dimensions for manually created textures
    // These can be updated if needed by the caller
    m_width = 2;  
    m_height = 2;
    m_channels = 4;  // Assume RGBA
}

} // namespace Rendering
} // namespace SFE 