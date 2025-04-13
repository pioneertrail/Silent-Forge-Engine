#include "rendering/WindowManager.hpp"
#include "rendering/GLIncludes.hpp"
#include <iostream>

WindowManager::WindowManager() : window(nullptr) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "SilentForge", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return;
    }

    // Enable VSync
    glfwSwapInterval(1);
}

WindowManager::~WindowManager() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

bool WindowManager::shouldClose() const {
    return window ? glfwWindowShouldClose(window) : true;
}

void WindowManager::swapBuffers() {
    if (window) {
        glfwSwapBuffers(window);
    }
}

void WindowManager::pollEvents() {
    glfwPollEvents();
}

GLFWwindow* WindowManager::getWindow() const {
    return window;
} 