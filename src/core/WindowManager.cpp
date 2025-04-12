// src/core/WindowManager.cpp
#include "core/WindowManager.hpp"
#include <iostream>

namespace Engine {

// Static callback function needs C linkage if used directly with C API like GLFW
// Or make it a static member function as done here.
void WindowManager::framebufferSizeCallback(GLFWwindow* /*window*/, int width, int height) {
    // Make sure the viewport matches the new window dimensions;
    glViewport(0, 0, width, height);
}

WindowManager::WindowManager(int width, int height, const std::string& title)
    : width(width), height(height), title(title), window(nullptr) {}

WindowManager::~WindowManager() {
    // Ensure shutdown is called, although explicit call is preferred
    shutdown();
}

bool WindowManager::initialize() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on MacOS
#endif

    // Create the window
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Set the framebuffer size callback
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Initialize GLAD: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    // Corrected: Use gladLoadGLLoader with glfwGetProcAddress
    // if (!gladLoadGL()) { // Original incorrect gladLoadGL call
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }

    // Set initial viewport size
    glViewport(0, 0, width, height);

    std::cout << "WindowManager initialized successfully." << std::endl;
    return true;
}

void WindowManager::shutdown() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr; // Prevent double deletion
    }
    // Terminate GLFW only once, potentially at the end of the application
    // This assumes WindowManager is the primary owner of the GLFW lifecycle
    glfwTerminate();
    std::cout << "WindowManager shut down." << std::endl;
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