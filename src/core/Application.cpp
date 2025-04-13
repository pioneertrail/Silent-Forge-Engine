#include "core/Application.hpp"
#include "rendering/GLStateCache.hpp"
#include "rendering/ShaderManager.hpp"
#include "rendering/TextRenderer.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace SFE {

bool Application::initialize(const ApplicationConfig& config) {
    if (m_initialized) {
        std::cerr << "Application already initialized" << std::endl;
        return false;
    }
    
    m_config = config;
    
    // Initialize window
    if (!m_window.initialize(config.width, config.height, config.title)) {
        std::cerr << "Failed to initialize window" << std::endl;
        return false;
    }
    
    // Configure OpenGL
    if (config.enableVSync) {
        glfwSwapInterval(1);
    } else {
        glfwSwapInterval(0);
    }
    
    if (config.enableMSAA) {
        glEnable(GL_MULTISAMPLE);
    }
    
    /* Temporarily comment out text renderer initialization
    // Initialize text renderer
    std::string fontPath = config.assetPath + "/fonts/consolas.png";
    std::string fontMeta = config.assetPath + "/fonts/consolas.fnt";
    if (!m_textRenderer.initialize(fontPath, fontMeta)) {
        std::cerr << "Failed to initialize text renderer" << std::endl;
        return false;
    }
    */
    
    // Configure standard GL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Reset the state cache
    Rendering::GLStateCache::getInstance().reset();
    
    // Initialize timing
    m_currentTime = static_cast<float>(glfwGetTime());
    m_lastFrameTime = m_currentTime;
    m_deltaTime = 0.0f;
    
    m_initialized = true;
    return true;
}

void Application::setUpdateCallback(UpdateCallback callback) {
    m_updateCallback = std::move(callback);
}

void Application::setRenderCallback(RenderCallback callback) {
    m_renderCallback = std::move(callback);
}

Rendering::ShaderManager& Application::getShaderManager() {
    return Rendering::ShaderManager::getInstance();
}

int Application::run() {
    if (!m_initialized) {
        std::cerr << "Application not initialized" << std::endl;
        return -1;
    }
    
    while (!m_window.shouldClose() && !m_shouldExit) {
        // Update timing
        updateTiming();
        
        // Process input
        m_input.processInput(m_window.getWindow(), m_camera, m_deltaTime);
        
        // Clear the screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Call update callback
        if (m_updateCallback) {
            m_updateCallback(m_deltaTime);
        }
        
        // Call render callback
        if (m_renderCallback) {
            m_renderCallback();
        }
        
        // Render stats
        if (m_config.showFps || m_config.showStats) {
            renderStats();
        }
        
        // Swap buffers
        m_window.swapBuffers();
        
        // Poll events
        glfwPollEvents();
    }
    
    return 0;
}

void Application::updateTiming() {
    m_currentTime = static_cast<float>(glfwGetTime());
    m_deltaTime = m_currentTime - m_lastFrameTime;
    m_lastFrameTime = m_currentTime;
    
    // Update frame time history for FPS calculation
    m_frameTimeSum -= m_frameTimes[m_frameIndex];
    m_frameTimes[m_frameIndex] = m_deltaTime;
    m_frameTimeSum += m_deltaTime;
    m_frameIndex = (m_frameIndex + 1) % m_frameTimes.size();
    
    // Calculate FPS
    float avgFrameTime = m_frameTimeSum / static_cast<float>(m_frameTimes.size());
    m_fps = static_cast<int>(1.0f / avgFrameTime);
}

void Application::renderStats() {
    std::stringstream ss;
    
    if (m_config.showFps) {
        ss << "FPS: " << m_fps << " | ";
        ss << "Frame time: " << std::fixed << std::setprecision(2) << (m_deltaTime * 1000.0f) << " ms";
    }
    
    if (m_config.showStats) {
        if (m_config.showFps) {
            ss << " | ";
        }
        
        ss << "Draw calls: " << 0 << " | "; // TODO: Track draw calls
        ss << "Vertices: " << 0; // TODO: Track vertices
    }
    
    // Instead of rendering text, just log to console for now
    // m_textRenderer.renderText(ss.str(), 10.0f, 10.0f, 0.5f, glm::vec3(1.0f, 1.0f, 0.0f));
    static float lastLogTime = 0.0f;
    if (m_currentTime - lastLogTime > 1.0f) {  // Log once per second
        std::cout << ss.str() << std::endl;
        lastLogTime = m_currentTime;
    }
}

} // namespace SFE 