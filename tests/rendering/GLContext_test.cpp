#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "rendering/GLContext.hpp"
#include "helpers/GLContextHelper.hpp"
#include <vector>
#include <string>

using namespace SFE::Rendering;
using namespace Catch::Matchers;

TEST_CASE("GLContext Validation", "[rendering][glcontext]") {
    // Initialize OpenGL context for testing
    REQUIRE(SFE::Testing::GLContextHelper::init());

    SECTION("Context Validation") {
        std::vector<std::string> errorMessages;
        auto logCallback = [&errorMessages](const std::string& msg) {
            errorMessages.push_back(msg);
        };

        // Test with valid context
        REQUIRE(GLContext::ValidateContext(logCallback));
        REQUIRE(errorMessages.empty());

        // Test error handling
        GLContext::CheckGLError("test operation", logCallback);
        REQUIRE(errorMessages.empty()); // Should be no errors in valid context
    }

    SECTION("Error String Conversion") {
        REQUIRE_THAT(GLContext::GetErrorString(GL_NO_ERROR), Equals("No error"));
        REQUIRE_THAT(GLContext::GetErrorString(GL_INVALID_ENUM), Equals("Invalid enum"));
        REQUIRE_THAT(GLContext::GetErrorString(GL_INVALID_VALUE), Equals("Invalid value"));
        REQUIRE_THAT(GLContext::GetErrorString(GL_INVALID_OPERATION), Equals("Invalid operation"));
        REQUIRE_THAT(GLContext::GetErrorString(GL_INVALID_FRAMEBUFFER_OPERATION), Equals("Invalid framebuffer operation"));
        REQUIRE_THAT(GLContext::GetErrorString(GL_OUT_OF_MEMORY), Equals("Out of memory"));
        REQUIRE_THAT(GLContext::GetErrorString(0xFFFFFFFF), Equals("Unknown error"));
    }

    SECTION("Buffer Validation") {
        std::vector<std::string> errorMessages;
        auto logCallback = [&errorMessages](const std::string& msg) {
            errorMessages.push_back(msg);
        };

        // Test invalid buffer
        REQUIRE_FALSE(GLContext::ValidateBuffer(0, logCallback));
        REQUIRE_FALSE(errorMessages.empty());
        errorMessages.clear();

        // Test valid buffer
        GLuint buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, 1024, nullptr, GL_STATIC_DRAW);
        REQUIRE(GLContext::ValidateBuffer(buffer, logCallback));
        REQUIRE(errorMessages.empty());

        glDeleteBuffers(1, &buffer);
    }

    SECTION("Texture Validation") {
        std::vector<std::string> errorMessages;
        auto logCallback = [&errorMessages](const std::string& msg) {
            errorMessages.push_back(msg);
        };

        // Test invalid texture
        REQUIRE_FALSE(GLContext::ValidateTexture(0, logCallback));
        REQUIRE_FALSE(errorMessages.empty());
        errorMessages.clear();

        // Test valid texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        REQUIRE(GLContext::ValidateTexture(texture, logCallback));
        REQUIRE(errorMessages.empty());

        glDeleteTextures(1, &texture);
    }

    SECTION("Framebuffer Validation") {
        std::vector<std::string> errorMessages;
        auto logCallback = [&errorMessages](const std::string& msg) {
            errorMessages.push_back(msg);
        };

        // Test invalid framebuffer
        REQUIRE_FALSE(GLContext::ValidateFramebuffer(0, logCallback));
        REQUIRE_FALSE(errorMessages.empty());
        errorMessages.clear();

        // Test valid framebuffer
        GLuint fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        
        // Create and attach a texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
        
        REQUIRE(GLContext::ValidateFramebuffer(fbo, logCallback));
        REQUIRE(errorMessages.empty());

        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &texture);
    }

    SECTION("Vertex Array Validation") {
        std::vector<std::string> errorMessages;
        auto logCallback = [&errorMessages](const std::string& msg) {
            errorMessages.push_back(msg);
        };

        // Test invalid VAO
        REQUIRE_FALSE(GLContext::ValidateVertexArray(0, logCallback));
        REQUIRE_FALSE(errorMessages.empty());
        errorMessages.clear();

        // Test valid VAO
        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        REQUIRE(GLContext::ValidateVertexArray(vao, logCallback));
        REQUIRE(errorMessages.empty());

        glDeleteVertexArrays(1, &vao);
    }

    // Cleanup
    SFE::Testing::GLContextHelper::cleanup();
} 