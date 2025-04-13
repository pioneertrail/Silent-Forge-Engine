#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <memory>
#include <functional>

namespace SFE {
namespace Testing {

class GLContextHelper {
public:
    static void init() {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Create invisible window

        m_window = glfwCreateWindow(1, 1, "Test Window", nullptr, nullptr);
        if (!m_window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(m_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            glfwDestroyWindow(m_window);
            glfwTerminate();
            throw std::runtime_error("Failed to initialize GLAD");
        }
    }

    static void cleanup() {
        if (m_window) {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }
        glfwTerminate();
    }

    static bool isInitialized() {
        return m_window != nullptr;
    }

private:
    static GLFWwindow* m_window;
};

} // namespace Testing
} // namespace SFE 