#pragma once

#include "rendering/GLIncludes.hpp"
#include <string>
#include "SFEGlobalIncludes.hpp"

namespace SFE {
namespace Rendering {

class Texture {
public:
    Texture();
    ~Texture();

    bool loadFromFile(const std::string& filename);
    bool bind(GLenum textureUnit = GL_TEXTURE0) const;
    bool unbind() const;
    int getWidth() const;
    int getHeight() const;
    int getChannels() const;
    GLuint getTextureID() const;
    
    // Added method to set texture ID manually
    void setId(GLuint id);

private:
    GLuint m_textureID;
    int m_width, m_height, m_channels;
};

} // namespace Rendering
} // namespace SFE 