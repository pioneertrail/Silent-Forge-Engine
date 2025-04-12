// src/core/WindowManager.cpp
#include "core/WindowManager.hpp"
#include <iostream>

namespace SFE {

// Static callback function needs C linkage if used directly with C API like GLFW
// Or make it a static member function as done here.
void WindowManager::framebufferSizeCallback(GLFWwindow* /*window*/, int width, int height) {
    // Make sure the viewport matches the new window dimensions;
    glViewport(0, 0, width, height);
}

WindowManager::WindowManager(int width, int height, const std::string& title)
    : width(width), height(height), title(title), window(nullptr) {}

WindowManager::~WindowManager() {
    // Only destroy the window here, don't terminate GLFW
    if (window) {
        glfwDestroyWindow(window);
        std::cout << "GLFW window destroyed." << std::endl;
    }
    // shutdown(); // Don't call shutdown which terminates GLFW
}

bool WindowManager::initialize() {
    // Initialize GLFW only if it hasn't been initialized yet.
    // This requires careful handling if multiple WindowManagers could exist.
    // For a single window, initializing once is fine.
    if (!glfwInit()) { // Check if glfwInit() needs to be called
        // It's okay if it's already initialized, glfwInit() returns GL_TRUE.
        // We might need a static flag to ensure glfwInit is called only once. 
        // For now, assume it's okay to call it.
        if (!glfwInit()) { // Try again just in case? Unlikely needed.
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        // Don't terminate GLFW here if window creation fails, 
        // let the caller handle potential cleanup or retry.
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window); // Clean up the created window
        return false;
    }

    glViewport(0, 0, width, height);
    std::cout << "WindowManager initialized successfully." << std::endl;
    return true;
}

// Remove the shutdown method or change its behavior
// Option 1: Remove shutdown entirely
/*
void WindowManager::shutdown() {
    // Obsolete if destructor handles window destruction 
    // and main handles glfwTerminate
}
*/

// Option 2: Keep shutdown but ONLY destroy window
void WindowManager::shutdown() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
        std::cout << "GLFW window destroyed via shutdown()." << std::endl;
    }
    // DO NOT call glfwTerminate() here
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

} 