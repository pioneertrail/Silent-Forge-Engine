#include <catch2/catch.hpp>
#include "rendering/GLContextHelper.hpp"
#include <thread>
#include <chrono>

namespace SFE {
namespace Rendering {

class GLContextHelperTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto& context = GLContextHelper::getInstance();
        context.cleanup();  // Ensure clean state before each test
    }

    void TearDown() override {
        auto& context = GLContextHelper::getInstance();
        context.cleanup();  // Ensure cleanup after each test
    }
};

TEST_CASE_METHOD(GLContextHelperTest, "GLContextHelper Initialization", "[GLContextHelper]") {
    auto& context = GLContextHelper::getInstance();
    
    SECTION("Successful Initialization") {
        REQUIRE(context.initialize(4, 5, 800, 600));
        REQUIRE(context.isInitialized());
        REQUIRE(context.getWindow() != nullptr);
        
        // Verify OpenGL version
        context.makeCurrent();
        GLint major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        REQUIRE(major == 4);
        REQUIRE(minor == 5);
    }
    
    SECTION("Double Initialization") {
        REQUIRE(context.initialize());
        REQUIRE(context.isInitialized());
        REQUIRE(context.initialize()); // Should return true without reinitializing
    }
    
    SECTION("Cleanup") {
        REQUIRE(context.initialize());
        context.cleanup();
        REQUIRE_FALSE(context.isInitialized());
        REQUIRE(context.getWindow() == nullptr);
    }
}

TEST_CASE_METHOD(GLContextHelperTest, "GLContextHelper Context Management", "[GLContextHelper]") {
    auto& context = GLContextHelper::getInstance();
    REQUIRE(context.initialize());
    
    SECTION("Context Switching") {
        context.makeCurrent();
        REQUIRE(glfwGetCurrentContext() == context.getWindow());
        
        // Test context switching in another thread
        std::thread t([&context]() {
            context.makeCurrent();
            REQUIRE(glfwGetCurrentContext() == nullptr); // Context should not be current in this thread
        });
        t.join();
        
        // Verify context is still current in main thread
        REQUIRE(glfwGetCurrentContext() == context.getWindow());
    }
    
    SECTION("Buffer Operations") {
        context.makeCurrent();
        glClear(GL_COLOR_BUFFER_BIT);
        REQUIRE_NOTHROW(context.swapBuffers());
    }
    
    SECTION("Event Polling") {
        REQUIRE_NOTHROW(context.pollEvents());
    }
}

TEST_CASE_METHOD(GLContextHelperTest, "GLContextHelper Error Handling", "[GLContextHelper]") {
    auto& context = GLContextHelper::getInstance();
    
    SECTION("Invalid Version") {
        REQUIRE_FALSE(context.initialize(99, 99)); // Invalid OpenGL version
        REQUIRE_FALSE(context.isInitialized());
    }
    
    SECTION("Cleanup During Operation") {
        REQUIRE(context.initialize());
        
        std::thread t([&context]() {
            context.makeCurrent();
            glClear(GL_COLOR_BUFFER_BIT);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        });
        
        // Destroy context while thread is running
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        context.cleanup();
        t.join();
        
        // Verify cleanup was successful
        REQUIRE_FALSE(context.isInitialized());
    }
}

TEST_CASE_METHOD(GLContextHelperTest, "GLContextHelper Context Sharing", "[GLContextHelper]") {
    auto& context1 = GLContextHelper::getInstance();
    REQUIRE(context1.initialize());

    SECTION("Resource Sharing") {
        // Create a texture in context1
        context1.makeCurrent();
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Create a shared window
        GLFWwindow* sharedWindow = glfwCreateWindow(800, 600, "Shared Window", nullptr, context1.getWindow());
        REQUIRE(sharedWindow != nullptr);

        // Verify the texture is accessible in shared context
        glfwMakeContextCurrent(sharedWindow);
        glBindTexture(GL_TEXTURE_2D, texture);
        GLint boundTexture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);
        REQUIRE(boundTexture == texture);

        glDeleteTextures(1, &texture);
        glfwDestroyWindow(sharedWindow);
    }
}

TEST_CASE_METHOD(GLContextHelperTest, "GLContextHelper Context Destruction During Rendering", "[GLContextHelper]") {
    auto& context = GLContextHelper::getInstance();
    REQUIRE(context.initialize());

    std::thread t([&context]() {
        context.makeCurrent();
        for (int i = 0; i < 100; ++i) {
            glClear(GL_COLOR_BUFFER_BIT);
            context.swapBuffers();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    context.cleanup();
    t.join();

    REQUIRE_FALSE(context.isInitialized());
}

TEST_CASE_METHOD(GLContextHelperTest, "GLContextHelper Singleton Pattern", "[GLContextHelper]") {
    auto& context1 = GLContextHelper::getInstance();
    auto& context2 = GLContextHelper::getInstance();
    
    REQUIRE(&context1 == &context2); // Should be the same instance
    
    SECTION("Copy Prevention") {
        // These should not compile, but we can verify the behavior
        // GLContextHelper copy(context1); // Should not compile
        // GLContextHelper copy = context1; // Should not compile
    }
}

} // namespace Rendering
} // namespace SFE 