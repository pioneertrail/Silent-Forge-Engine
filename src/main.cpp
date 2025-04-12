// src/main.cpp
#include "core/WindowManager.hpp"
#include "core/Camera.hpp"
#include "core/InputManager.hpp"
#include "core/SceneNode.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/InstancedMesh.hpp"
#include "rendering/TextRenderer.hpp"
#include "rendering/Texture.hpp"
#include "rendering/ShaderManager.hpp"
#include "core/Logger.hpp"
#include "core/ScreenshotManager.hpp"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream> // Include for potential debug output
#include <sstream>
#include <iomanip>
#include <memory>
#include <filesystem>

// Timing variables (moved outside main for clarity)
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Add performance tracking
struct PerformanceMetrics {
    float textRenderTime = 0.0f;
    float sceneRenderTime = 0.0f;
    int textDrawCalls = 0;
    int totalCharacters = 0;
};

struct AutomatedTestConfig {
    bool enabled = false;
    float duration = 10.0f;  // Test duration in seconds
    float screenshotInterval = 1.0f;  // Screenshot interval in seconds
    std::string outputDir = "test_results";
    float lastScreenshotTime = 0.0f;
    float testStartTime = 0.0f;
};

int main() {
    // Create output directory for test results
    std::filesystem::create_directories("test_results");
    std::filesystem::create_directories("test_results/screenshots");

    // Initialize logger
    auto& logger = SFE::Logger::getInstance();
    logger.initialize("test_results/performance_log.csv");

    // Use SFE namespace for engine classes
    SFE::WindowManager window(800, 600, "Silent Forge Engine");
    if (!window.initialize()) {
        std::cerr << "Failed to initialize WindowManager." << std::endl;
        return -1;
    }

    // Create and load shader using ShaderManager
    auto& shaderManager = SFE::ShaderManager::getInstance();
    auto shader = shaderManager.loadShader("simple", "shaders/simple.vert", "shaders/simple.frag");

    // Create cube vertices with proper normals
    // Using 8 vertices with averaged normals for smoother lighting
    std::vector<SFE::Mesh::Vertex> vertices = {
        // Positions          // Texture coords // Normals (normalized to length 1)
        {{ -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }, { -0.577f, -0.577f, -0.577f }}, // 0: Back-bottom-left
        {{  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f }, {  0.577f, -0.577f, -0.577f }}, // 1: Back-bottom-right
        {{  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f }, {  0.577f,  0.577f, -0.577f }}, // 2: Back-top-right
        {{ -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f }, { -0.577f,  0.577f, -0.577f }}, // 3: Back-top-left
        {{ -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f }, { -0.577f, -0.577f,  0.577f }}, // 4: Front-bottom-left
        {{  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f }, {  0.577f, -0.577f,  0.577f }}, // 5: Front-bottom-right
        {{  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f }, {  0.577f,  0.577f,  0.577f }}, // 6: Front-top-right
        {{ -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f }, { -0.577f,  0.577f,  0.577f }}  // 7: Front-top-left
    };

    // Cube indices (6 faces, 2 triangles each, 36 indices)
    std::vector<unsigned int> indices = {
        // Back face
        0, 1, 2,  2, 3, 0,
        // Front face
        4, 5, 6,  6, 7, 4,
        // Left face
        4, 0, 3,  3, 7, 4,
        // Right face
        1, 5, 6,  6, 2, 1,
        // Bottom face
        0, 1, 5,  5, 4, 0,
        // Top face
        3, 2, 6,  6, 7, 3
    };

    // Create instanced mesh instead of regular mesh
    auto cubeMesh = std::make_shared<SFE::InstancedMesh>(vertices, indices);

    // Load texture
    auto texture = std::make_shared<SFE::Texture>();
    if (!texture->loadFromFile("assets/textures/colortest.png")) {
        std::cerr << "Failed to load texture from PNG, creating a simple color pattern" << std::endl;
        
        // Create a simple 2x2 texture with red, green, blue, and yellow pixels
        unsigned char data[] = {
            255, 0, 0, 255,     0, 255, 0, 255,
            0, 0, 255, 255,     255, 255, 0, 255
        };
        
        glGenTextures(1, &texture->getID());
        glBindTexture(GL_TEXTURE_2D, texture->getID());
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Store transformation matrices for all cubes
    std::vector<glm::mat4> cubeTransforms;
    
    // Center cube
    glm::mat4 centerTransform = glm::mat4(1.0f);
    cubeTransforms.push_back(centerTransform);
    
    // Left orbiting cube
    glm::mat4 leftTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
    leftTransform = glm::scale(leftTransform, glm::vec3(0.5f, 0.5f, 0.5f));
    cubeTransforms.push_back(leftTransform);
    
    // Right orbiting cube
    glm::mat4 rightTransform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
    rightTransform = glm::scale(rightTransform, glm::vec3(0.5f, 0.5f, 0.5f));
    cubeTransforms.push_back(rightTransform);

    // Initialize instance data
    cubeMesh->updateInstanceData(cubeTransforms);

    // Position camera farther back to see all cubes
    SFE::Camera camera(glm::vec3(0.0f, 1.0f, 7.0f));
    SFE::InputManager input;
    // No initialize method needed, InputManager is ready to use after construction

    SFE::TextRenderer textRenderer;
    if (!textRenderer.initialize("assets/fonts/consolas.png", "assets/fonts/consolas.fnt")) {
        std::cerr << "Failed to initialize text renderer" << std::endl;
    }

    float frameTimes[60] = {0.0f};
    int frameIndex = 0;
    float frameTimeSum = 0.0f;

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Add performance tracking
    PerformanceMetrics metrics;
    bool showStressTest = false;
    static bool wasRPressed = false;
    static bool wasTPressed = false;

    // Add automated test configuration
    AutomatedTestConfig testConfig;
    testConfig.enabled = true;  // Enable automated testing
    testConfig.testStartTime = static_cast<float>(glfwGetTime());

    // Log test start
    logger.logMessage("Starting automated test");
    logger.logMessage("Window size: " + std::to_string(window.getWidth()) + "x" + 
                     std::to_string(window.getHeight()));

    // Main loop
    while (!window.shouldClose()) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check if automated test should end
        if (testConfig.enabled) {
            float testElapsedTime = currentFrame - testConfig.testStartTime;
            if (testElapsedTime >= testConfig.duration) {
                logger.logMessage("Automated test completed");
                break;
            }

            // Toggle stress test every 5 seconds
            if (static_cast<int>(testElapsedTime) % 5 == 0 && 
                static_cast<int>(testElapsedTime) != static_cast<int>(testElapsedTime - deltaTime)) {
                showStressTest = !showStressTest;
                logger.logMessage("Stress test " + std::string(showStressTest ? "enabled" : "disabled"));
            }

            // Capture screenshot at specified intervals
            if (currentFrame - testConfig.lastScreenshotTime >= testConfig.screenshotInterval) {
                std::string screenshotPath = "test_results/screenshots/frame_" + 
                                           std::to_string(static_cast<int>(testElapsedTime)) + ".png";
                if (SFE::ScreenshotManager::getInstance().captureScreenshot(
                        screenshotPath, window.getWidth(), window.getHeight())) {
                    logger.logMessage("Screenshot saved: " + screenshotPath);
                } else {
                    logger.logMessage("Failed to save screenshot: " + screenshotPath);
                }
                testConfig.lastScreenshotTime = currentFrame;
            }
        }

        // Handle input for shader reload and stress test toggle
        bool isRPressed = glfwGetKey(window.getWindow(), GLFW_KEY_R) == GLFW_PRESS;
        if (isRPressed && !wasRPressed) {
            std::cout << "Reloading shaders..." << std::endl;
            shaderManager.reloadAllShaders();
            shader = shaderManager.getShader("simple");
        }
        wasRPressed = isRPressed;

        bool isTPressed = glfwGetKey(window.getWindow(), GLFW_KEY_T) == GLFW_PRESS;
        if (isTPressed && !wasTPressed) {
            showStressTest = !showStressTest;
            std::cout << "Stress test " << (showStressTest ? "enabled" : "disabled") << std::endl;
        }
        wasTPressed = isTPressed;

        // Update FPS counter
        frameTimeSum -= frameTimes[frameIndex];
        frameTimes[frameIndex] = deltaTime;
        frameTimeSum += deltaTime;
        frameIndex = (frameIndex + 1) % 60;
        float avgFrameTime = frameTimeSum / 60.0f;
        int fps = static_cast<int>(1.0f / avgFrameTime);
        std::stringstream ss;
        ss << "FPS: " << fps;
        std::string fpsText = ss.str();

        // Process input
        input.processInput(window.getWindow(), camera, deltaTime);

        // Update cube transforms
        float angle = currentFrame * 0.5f;
        cubeTransforms[1] = glm::translate(glm::mat4(1.0f), 
            glm::vec3(-2.0f * cos(angle), 0.0f, -2.0f * sin(angle)));
        cubeTransforms[1] = glm::scale(cubeTransforms[1], glm::vec3(0.5f));
        
        cubeTransforms[2] = glm::translate(glm::mat4(1.0f), 
            glm::vec3(2.0f * cos(angle), 0.0f, 2.0f * sin(angle)));
        cubeTransforms[2] = glm::scale(cubeTransforms[2], glm::vec3(0.5f));
        
        // Update instance data
        cubeMesh->updateInstanceData(cubeTransforms);

        // Start scene rendering timer
        float sceneStartTime = static_cast<float>(glfwGetTime());

        // Rendering
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render 3D scene
        shader->use();
        shader->setInt("texture0", 0);
        shader->setVec3("lightDir", glm::vec3(1.0f, 1.0f, -1.0f));
        shader->setVec3("viewPos", camera.getPosition());
        shader->setMat4("view", camera.getViewMatrix());
        shader->setMat4("projection", glm::perspective(glm::radians(45.0f),
            static_cast<float>(window.getWidth()) / window.getHeight(), 0.1f, 100.0f));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->getID());
        cubeMesh->drawInstanced(static_cast<unsigned int>(cubeTransforms.size()));

        metrics.sceneRenderTime = static_cast<float>(glfwGetTime()) - sceneStartTime;

        // Start text rendering timer
        float textStartTime = static_cast<float>(glfwGetTime());
        metrics.textDrawCalls = 0;
        metrics.totalCharacters = 0;

        // Setup text projection
        glm::mat4 ortho = glm::ortho(0.0f, static_cast<float>(window.getWidth()), 
                                    static_cast<float>(window.getHeight()), 0.0f);

        // 1. Title and FPS (cached)
        textRenderer.renderText("Silent Forge Engine", 10.0f, 30.0f, 1.5f, 
                              glm::vec3(1.0f, 0.0f, 0.0f), ortho);
        metrics.totalCharacters += 18; // "Silent Forge Engine"

        textRenderer.renderText(fpsText, 10.0f, 60.0f, 1.2f, 
                              glm::vec3(0.0f, 1.0f, 0.0f), ortho);
        metrics.totalCharacters += fpsText.length();

        // 2. Performance metrics
        std::vector<std::pair<std::string, glm::vec3>> perfInfo = {
            {"Performance Metrics:", glm::vec3(1.0f, 1.0f, 0.0f)},
            {"Scene Render Time: " + std::to_string(metrics.sceneRenderTime * 1000.0f) + " ms", 
             glm::vec3(0.8f, 0.8f, 0.8f)},
            {"Text Draw Calls: " + std::to_string(metrics.textDrawCalls), 
             glm::vec3(0.8f, 0.8f, 0.8f)},
            {"Total Characters: " + std::to_string(metrics.totalCharacters), 
             glm::vec3(0.8f, 0.8f, 0.8f)}
        };

        float yPos = 90.0f;
        for (const auto& [text, color] : perfInfo) {
            textRenderer.addToBatch(text, 10.0f, yPos, 1.0f, color);
            metrics.totalCharacters += text.length();
            yPos += 25.0f;
        }
        textRenderer.renderBatch(ortho);
        metrics.textDrawCalls++;

        // 3. Camera debug info
        std::vector<std::pair<std::string, glm::vec3>> debugInfo = {
            {"Camera Position:", glm::vec3(0.8f, 0.8f, 0.8f)},
            {"X: " + std::to_string(camera.getPosition().x), glm::vec3(1.0f, 0.8f, 0.8f)},
            {"Y: " + std::to_string(camera.getPosition().y), glm::vec3(0.8f, 1.0f, 0.8f)},
            {"Z: " + std::to_string(camera.getPosition().z), glm::vec3(0.8f, 0.8f, 1.0f)}
        };

        yPos = 190.0f;
        for (const auto& [text, color] : debugInfo) {
            textRenderer.addToBatch(text, 10.0f, yPos, 1.0f, color);
            metrics.totalCharacters += text.length();
            yPos += 25.0f;
        }
        textRenderer.renderBatch(ortho);
        metrics.textDrawCalls++;

        // 4. Animated wave text
        std::string animatedText = "Press R to reload shaders | Press T to toggle stress test";
        float baseY = window.getHeight() - 40.0f;
        float charX = window.getWidth() - 500.0f;
        for (char c : animatedText) {
            float offsetY = sin(currentFrame * 2.0f + charX * 0.05f) * 10.0f;
            std::string charStr(1, c);
            textRenderer.addToBatch(charStr, charX, baseY + offsetY, 1.0f, 
                                  glm::vec3(0.7f + sin(currentFrame) * 0.3f,
                                          0.7f + cos(currentFrame) * 0.3f,
                                          0.7f));
            charX += 15.0f;
        }
        textRenderer.renderBatch(ortho);
        metrics.textDrawCalls++;
        metrics.totalCharacters += animatedText.length();

        // 5. Stress test (optional)
        if (showStressTest) {
            // Generate a grid of text with different colors and scales
            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 10; j++) {
                    float x = 100.0f + i * 80.0f;
                    float y = 300.0f + j * 30.0f;
                    float scale = 0.8f + sin(currentFrame + i * 0.1f + j * 0.1f) * 0.2f;
                    
                    std::string text = "Test" + std::to_string(i) + std::to_string(j);
                    glm::vec3 color(
                        0.5f + sin(currentFrame + i * 0.2f) * 0.5f,
                        0.5f + sin(currentFrame + j * 0.2f) * 0.5f,
                        0.5f + sin(currentFrame + (i + j) * 0.2f) * 0.5f
                    );
                    
                    textRenderer.addToBatch(text, x, y, scale, color);
                    metrics.totalCharacters += text.length();
                }
            }
            textRenderer.renderBatch(ortho);
            metrics.textDrawCalls++;
        }

        metrics.textRenderTime = static_cast<float>(glfwGetTime()) - textStartTime;

        // After performance metrics are updated
        logger.logPerformanceMetrics(fps, metrics.sceneRenderTime, metrics.textRenderTime,
                                   metrics.textDrawCalls, metrics.totalCharacters, showStressTest);

        window.swapBuffers();
        window.pollEvents();
    }

    // Log final message
    logger.logMessage("Application shutting down");

    // WindowManager destructor handles GLFW termination

    return 0;
} 