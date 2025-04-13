#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>
#include <functional>

namespace SFE {
namespace Rendering {

class GLContext {
public:
    using LogCallback = std::function<void(const std::string&)>;

    static bool ValidateContext(LogCallback logCallback = nullptr);
    static bool ValidateShader(GLuint shader, LogCallback logCallback = nullptr);
    static bool ValidateProgram(GLuint program, LogCallback logCallback = nullptr);
    static bool ValidateBuffer(GLuint buffer, LogCallback logCallback = nullptr);
    static bool ValidateTexture(GLuint texture, LogCallback logCallback = nullptr);
    static bool ValidateFramebuffer(GLuint framebuffer, LogCallback logCallback = nullptr);
    static bool ValidateVertexArray(GLuint vao, LogCallback logCallback = nullptr);

    static std::string GetErrorString(GLenum error);
    static void CheckGLError(const std::string& operation, LogCallback logCallback = nullptr);

private:
    static void LogError(const std::string& message, LogCallback logCallback);
};

} // namespace Rendering
} // namespace SFE 