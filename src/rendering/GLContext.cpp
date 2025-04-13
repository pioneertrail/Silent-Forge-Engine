#include "rendering/GLContext.hpp"
#include <iostream>
#include <sstream>

namespace SFE {
namespace Rendering {

void GLContext::LogError(const std::string& message, LogCallback logCallback) {
    if (logCallback) {
        logCallback(message);
    } else {
        std::cerr << "OpenGL Error: " << message << std::endl;
    }
}

bool GLContext::ValidateContext(LogCallback logCallback) {
    if (!glfwGetCurrentContext()) {
        LogError("No active OpenGL context", logCallback);
        return false;
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LogError("Failed to initialize GLAD", logCallback);
        return false;
    }

    return true;
}

bool GLContext::ValidateShader(GLuint shader, LogCallback logCallback) {
    if (!shader) {
        LogError("Invalid shader ID", logCallback);
        return false;
    }

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> log(logLength);
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());
        LogError("Shader compilation failed: " + std::string(log.data()), logCallback);
        return false;
    }

    return true;
}

bool GLContext::ValidateProgram(GLuint program, LogCallback logCallback) {
    if (!program) {
        LogError("Invalid program ID", logCallback);
        return false;
    }

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> log(logLength);
        glGetProgramInfoLog(program, logLength, nullptr, log.data());
        LogError("Program linking failed: " + std::string(log.data()), logCallback);
        return false;
    }

    return true;
}

bool GLContext::ValidateBuffer(GLuint buffer, LogCallback logCallback) {
    if (!buffer) {
        LogError("Invalid buffer ID", logCallback);
        return false;
    }

    GLint size;
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    if (glGetError() != GL_NO_ERROR) {
        LogError("Invalid buffer object", logCallback);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return false;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

bool GLContext::ValidateTexture(GLuint texture, LogCallback logCallback) {
    if (!texture) {
        LogError("Invalid texture ID", logCallback);
        return false;
    }

    GLint width, height;
    glBindTexture(GL_TEXTURE_2D, texture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    if (glGetError() != GL_NO_ERROR) {
        LogError("Invalid texture object", logCallback);
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

bool GLContext::ValidateFramebuffer(GLuint framebuffer, LogCallback logCallback) {
    if (!framebuffer) {
        LogError("Invalid framebuffer ID", logCallback);
        return false;
    }

    GLint status;
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        LogError("Framebuffer is not complete: " + std::to_string(status), logCallback);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

bool GLContext::ValidateVertexArray(GLuint vao, LogCallback logCallback) {
    if (!vao) {
        LogError("Invalid vertex array ID", logCallback);
        return false;
    }

    GLint currentVAO;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
    
    glBindVertexArray(vao);
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        LogError("Invalid vertex array object", logCallback);
        glBindVertexArray(currentVAO);
        return false;
    }
    
    glBindVertexArray(currentVAO);
    return true;
}

std::string GLContext::GetErrorString(GLenum error) {
    switch (error) {
        case GL_NO_ERROR: return "No error";
        case GL_INVALID_ENUM: return "Invalid enum";
        case GL_INVALID_VALUE: return "Invalid value";
        case GL_INVALID_OPERATION: return "Invalid operation";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "Invalid framebuffer operation";
        case GL_OUT_OF_MEMORY: return "Out of memory";
        default: return "Unknown error";
    }
}

void GLContext::CheckGLError(const std::string& operation, LogCallback logCallback) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::stringstream ss;
        ss << "OpenGL error during " << operation << ": " << GetErrorString(error);
        LogError(ss.str(), logCallback);
    }
}

} // namespace Rendering
} // namespace SFE 