#include "rendering/GLContextHelper.hpp"
#include <glad/glad.h>
#include <stdexcept>

namespace SFE {
namespace Rendering {

GLContextHelper& GLContextHelper::getInstance() {
    static GLContextHelper instance;
    return instance;
}

bool GLContextHelper::initialize(int majorVersion, int minorVersion, int width, int height) {
    if (m_initialized) {
        return true;
    }

    m_majorVersion = majorVersion;
    m_minorVersion = minorVersion;

    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_majorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_minorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);  // Hidden window for tests

    m_window = glfwCreateWindow(width, height, "Test Window", nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        return false;
    }

    makeCurrent();

    if (!gladLoadGL()) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
        glfwTerminate();
        return false;
    }

    m_initialized = true;
    return true;
}

void GLContextHelper::cleanup() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    if (glfwGetCurrentContext()) {  // Check if GLFW is initialized
        glfwTerminate();
    }
    m_initialized = false;
}

void GLContextHelper::makeCurrent() const {
    if (m_window) {
        glfwMakeContextCurrent(m_window);
    }
}

void GLContextHelper::swapBuffers() const {
    if (m_window) {
        glfwSwapBuffers(m_window);
    }
}

void GLContextHelper::pollEvents() const {
    glfwPollEvents();
}

bool GLContextHelper::shouldClose() const {
    return m_window && glfwWindowShouldClose(m_window);
}

GLContextHelper::~GLContextHelper() {
    cleanup();
}

} // namespace Rendering
} // namespace SFE 