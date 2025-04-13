#pragma once
#include "SFEGlobalIncludes.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <memory>

namespace SFE {
namespace Rendering {

class GLContextHelper {
public:
    static GLContextHelper& getInstance();
    
    bool initialize(int majorVersion = 4, int minorVersion = 5, int width = 800, int height = 600);
    void cleanup();
    bool isInitialized() const { return m_initialized; }
    
    GLFWwindow* getWindow() const { return m_window; }
    void makeCurrent() const;
    void swapBuffers() const;
    void pollEvents() const;
    bool shouldClose() const;

private:
    GLContextHelper() = default;
    ~GLContextHelper(); // Implemented in .cpp file
    GLContextHelper(const GLContextHelper&) = delete;
    GLContextHelper& operator=(const GLContextHelper&) = delete;

    GLFWwindow* m_window = nullptr;
    bool m_initialized = false;
    int m_majorVersion = 4;
    int m_minorVersion = 5;
};

} // namespace Rendering
} // namespace SFE 