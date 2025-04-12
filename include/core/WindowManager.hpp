#pragma once
#include <glad/glad.h> // Include GLAD before GLFW
#include <GLFW/glfw3.h>
#include <string>

namespace SFE {
class WindowManager {
public:
    WindowManager(int width, int height, const std::string& title);
    ~WindowManager();

    // Rule of five: Prevent copying/moving for simplicity for now
    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;
    WindowManager(WindowManager&&) = delete;
    WindowManager& operator=(WindowManager&&) = delete;

    bool initialize();
    void shutdown();
    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();

    GLFWwindow* getWindow() const { return window; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow* window;
    int width, height;
    std::string title;
};
} 