#pragma once

#include <glad/glad.h>
#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace SFE {

class Texture;

class Framebuffer {
public:
    Framebuffer(int width, int height);
    ~Framebuffer();

    // Rule of five: Prevent copying/moving
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&&) = delete;
    Framebuffer& operator=(Framebuffer&&) = delete;

    // Bind/unbind the framebuffer
    void bind() const;
    void unbind() const;

    // Attach textures to the framebuffer
    void attachColorTexture(std::shared_ptr<Texture> texture, GLenum attachment = GL_COLOR_ATTACHMENT0);
    void attachDepthTexture(std::shared_ptr<Texture> texture);
    void attachStencilTexture(std::shared_ptr<Texture> texture);
    void attachDepthStencilTexture(std::shared_ptr<Texture> texture);

    // Check framebuffer status
    bool isComplete() const;

    // Get framebuffer dimensions
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Get framebuffer ID
    GLuint getID() const { return fboID; }

    // Clear the framebuffer
    void clear(GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set viewport to framebuffer dimensions
    void setViewport() const;

private:
    GLuint fboID;
    int width;
    int height;
    std::vector<std::shared_ptr<Texture>> colorAttachments;
    std::shared_ptr<Texture> depthAttachment;
    std::shared_ptr<Texture> stencilAttachment;
    std::shared_ptr<Texture> depthStencilAttachment;

    void checkStatus() const;
    void validateAttachment(GLenum attachment) const;
};

} // namespace SFE 