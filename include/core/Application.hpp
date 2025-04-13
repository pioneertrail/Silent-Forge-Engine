#pragma once
#include "core/WindowManager.hpp"
#include "core/Camera.hpp"
#include "core/InputManager.hpp"
#include <string_view>
#include <memory>
#include <functional>
#include <chrono>
#include <array>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace SFE {

// Forward declarations
namespace Rendering {
    class ShaderManager;
    class TextRenderer;
}

/**
 * @brief Configuration for the application
 */
struct ApplicationConfig {
    int width{800};
    int height{600};
    std::string title{"Silent Forge Engine"};
    bool enableVSync{true};
    bool enableMSAA{true};
    int msaaSamples{4};
    std::string shaderPath{"shaders"};
    std::string assetPath{"assets"};
    bool showFps{true};
    bool showStats{false};
};

/**
 * @brief The main application class
 * 
 * This class manages the main application loop, window, and rendering.
 * It also provides a simple API for creating and managing objects.
 */
class Application {
public:
    using UpdateCallback = std::function<void(float)>;
    using RenderCallback = std::function<void()>;

    /**
     * @brief Initialize the application with the given configuration
     * @param config Application configuration
     * @return true if initialization was successful
     */
    bool initialize(const ApplicationConfig& config = {});

    /**
     * @brief Set the update callback
     * @param callback Function to call every update
     */
    void setUpdateCallback(UpdateCallback callback);

    /**
     * @brief Set the render callback
     * @param callback Function to call every render
     */
    void setRenderCallback(RenderCallback callback);

    /**
     * @brief Run the main application loop
     * @return Exit code
     */
    int run();

    /**
     * @brief Get the camera
     * @return Reference to the camera
     */
    Camera& getCamera() { return m_camera; }
    
    /**
     * @brief Get the input manager
     * @return Reference to the input manager
     */
    InputManager& getInput() { return m_input; }
    
    /**
     * @brief Get the window manager
     * @return Reference to the window manager
     */
    WindowManager& getWindow() { return m_window; }
    
    /**
     * @brief Get the shader manager
     * @return Reference to the shader manager
     */
    Rendering::ShaderManager& getShaderManager();
    
    /**
     * @brief Get the delta time (time since last frame)
     * @return Delta time in seconds
     */
    float getDeltaTime() const { return m_deltaTime; }
    
    /**
     * @brief Get the current FPS (frames per second)
     * @return Current FPS
     */
    int getFps() const { return m_fps; }
    
    /**
     * @brief Exit the application
     */
    void exit() { m_shouldExit = true; }

private:
    // Update frame timing
    void updateTiming();
    
    // Render frame statistics
    void renderStats();

    // Core components
    WindowManager m_window;
    Camera m_camera{glm::vec3(0.0f, 1.0f, 5.0f)};
    InputManager m_input;
    
    // Removed direct TextRenderer dependency
    // Rendering::TextRenderer m_textRenderer;

    // Timing
    float m_currentTime{0.0f};
    float m_deltaTime{0.0f};
    float m_lastFrameTime{0.0f};
    std::array<float, 60> m_frameTimes{};
    int m_frameIndex{0};
    float m_frameTimeSum{0.0f};
    int m_fps{0};
    
    // Configuration
    ApplicationConfig m_config;
    
    // State
    bool m_initialized{false};
    bool m_shouldExit{false};
    
    // Callbacks
    UpdateCallback m_updateCallback;
    RenderCallback m_renderCallback;
};

} // namespace SFE 