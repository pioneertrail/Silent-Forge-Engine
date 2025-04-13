#include "rendering/Framebuffer.hpp"
#include "rendering/Texture.hpp"
#include "rendering/GLContext.hpp"
#include <stdexcept>
#include <iostream>

namespace SFE {

Framebuffer::Framebuffer(int width, int height) 
    : width(width), height(height), fboID(0) {
    glGenFramebuffers(1, &fboID);
    bind();
}

Framebuffer::~Framebuffer() {
    if (fboID != 0) {
        glDeleteFramebuffers(1, &fboID);
    }
}

void Framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
}

void Framebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attachColorTexture(std::shared_ptr<Texture> texture, GLenum attachment) {
    if (!texture) {
        throw std::runtime_error("Cannot attach null texture to framebuffer");
    }

    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->getID(), 0);
    validateAttachment(attachment);
    colorAttachments.push_back(texture);
}

void Framebuffer::attachDepthTexture(std::shared_ptr<Texture> texture) {
    if (!texture) {
        throw std::runtime_error("Cannot attach null texture to framebuffer");
    }

    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->getID(), 0);
    validateAttachment(GL_DEPTH_ATTACHMENT);
    depthAttachment = texture;
}

void Framebuffer::attachStencilTexture(std::shared_ptr<Texture> texture) {
    if (!texture) {
        throw std::runtime_error("Cannot attach null texture to framebuffer");
    }

    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->getID(), 0);
    validateAttachment(GL_STENCIL_ATTACHMENT);
    stencilAttachment = texture;
}

void Framebuffer::attachDepthStencilTexture(std::shared_ptr<Texture> texture) {
    if (!texture) {
        throw std::runtime_error("Cannot attach null texture to framebuffer");
    }

    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->getID(), 0);
    validateAttachment(GL_DEPTH_STENCIL_ATTACHMENT);
    depthStencilAttachment = texture;
}

bool Framebuffer::isComplete() const {
    bind();
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    return status == GL_FRAMEBUFFER_COMPLETE;
}

void Framebuffer::clear(GLbitfield mask) {
    bind();
    glClear(mask);
}

void Framebuffer::setViewport() const {
    glViewport(0, 0, width, height);
}

void Framebuffer::checkStatus() const {
    bind();
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::string error;
        switch (status) {
            case GL_FRAMEBUFFER_UNDEFINED:
                error = "GL_FRAMEBUFFER_UNDEFINED";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                error = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                error = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                error = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                error = "GL_FRAMEBUFFER_UNSUPPORTED";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                error = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                error = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
                break;
            default:
                error = "Unknown framebuffer error";
        }
        throw std::runtime_error("Framebuffer is not complete: " + error);
    }
}

void Framebuffer::validateAttachment(GLenum attachment) const {
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::string error = "Failed to attach texture to framebuffer attachment " + std::to_string(attachment);
        throw std::runtime_error(error);
    }
}

} // namespace SFE 