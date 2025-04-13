#pragma once
#include "rendering/GLIncludes.hpp"
#include <GLFW/glfw3.h>
#include <string>

namespace SFE {
class WindowManager {
public:
    // Default constructor
    WindowManager();
    
    WindowManager(int width, int height, const std::string& title);
    ~WindowManager();

    // Rule of five: Prevent copying/moving for simplicity for now
    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;
    WindowManager(WindowManager&&) = delete;
    WindowManager& operator=(WindowManager&&) = delete;

    bool initialize();
    bool initialize(int width, int height, const std::string& title);
    void shutdown();
    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();

    GLFWwindow* getWindow() const { return window; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow* window{nullptr};
    int width{800}, height{600};
    std::string title{"Window"};
    bool isInitialized{false};
};
} 