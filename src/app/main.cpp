// src/main.cpp
#include "rendering/GLIncludes.hpp"
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
#include "core/Application.hpp"

using namespace SFE;

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

// Demo scene variables
struct DemoScene {
    std::shared_ptr<Rendering::Shader> shader;
    std::shared_ptr<Rendering::InstancedMesh> cubeMesh;
    std::shared_ptr<Rendering::Texture> texture;
    std::vector<glm::mat4> cubeTransforms;
    float rotationSpeed{30.0f};
};

// Create a simple cube mesh
std::shared_ptr<Rendering::Mesh> createCubeMesh() {
    // Define cube vertices with positions, texture coordinates and normals
    std::vector<Rendering::Mesh::Vertex> vertices = {
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

    return std::make_shared<Rendering::Mesh>(vertices, indices);
}

// Create or load a texture
std::shared_ptr<Rendering::Texture> createTexture() {
    auto texture = std::make_shared<Rendering::Texture>();
    
    // Try to load from file first
    if (std::filesystem::exists("assets/textures/colortest.png") && 
        texture->loadFromFile("assets/textures/colortest.png")) {
        return texture;
    }
    
    // If file loading fails, create a simple texture
    std::cout << "Failed to load texture from file, creating a simple color pattern" << std::endl;
        
        // Create a simple 2x2 texture with red, green, blue, and yellow pixels
        unsigned char data[] = {
            255, 0, 0, 255,     0, 255, 0, 255,
            0, 0, 255, 255,     255, 255, 0, 255
        };
        
    // Create texture manually using OpenGL
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    
    texture->setId(textureId);  // Assuming Texture has this method
    return texture;
}

// Initialize the demo scene
bool initializeDemoScene(Application& app, DemoScene& scene) {
    // Create and load shader
    auto& shaderManager = app.getShaderManager();
    scene.shader = shaderManager.loadShader("simple", "shaders/simple.vert", "shaders/simple.frag");
    if (!scene.shader) {
        std::cerr << "Failed to load shader" << std::endl;
        return false;
    }
    
    // Create cube mesh
    auto cubeMesh = createCubeMesh();
    
    // Create instanced mesh
    scene.cubeMesh = std::make_shared<Rendering::InstancedMesh>(cubeMesh);
    
    // Load texture
    scene.texture = createTexture();
    
    // Setup initial cube transforms
    // Center cube
    glm::mat4 centerTransform = glm::mat4(1.0f);
    scene.cubeTransforms.push_back(centerTransform);
    
    // Left orbiting cube
    glm::mat4 leftTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
    leftTransform = glm::scale(leftTransform, glm::vec3(0.5f, 0.5f, 0.5f));
    scene.cubeTransforms.push_back(leftTransform);
    
    // Right orbiting cube
    glm::mat4 rightTransform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
    rightTransform = glm::scale(rightTransform, glm::vec3(0.5f, 0.5f, 0.5f));
    scene.cubeTransforms.push_back(rightTransform);

    // Initialize instance data
    scene.cubeMesh->updateInstanceData(scene.cubeTransforms);
    
    return true;
}

// Update callback for the demo scene
void updateDemoScene(Application& app, DemoScene& scene, float deltaTime) {
    // Update cube transforms based on time
    float time = static_cast<float>(glfwGetTime());
    float angle = time * scene.rotationSpeed;
    
    // Update orbiting cubes
    scene.cubeTransforms[1] = glm::translate(glm::mat4(1.0f), 
        glm::vec3(-2.0f * cos(glm::radians(angle)), 0.0f, -2.0f * sin(glm::radians(angle))));
    scene.cubeTransforms[1] = glm::scale(scene.cubeTransforms[1], glm::vec3(0.5f));
    
    scene.cubeTransforms[2] = glm::translate(glm::mat4(1.0f), 
        glm::vec3(2.0f * cos(glm::radians(angle)), 0.0f, 2.0f * sin(glm::radians(angle))));
    scene.cubeTransforms[2] = glm::scale(scene.cubeTransforms[2], glm::vec3(0.5f));
    
    // Update center cube rotation
    scene.cubeTransforms[0] = glm::rotate(glm::mat4(1.0f), 
        glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0f));
    
    // Update instance data
    scene.cubeMesh->updateInstanceData(scene.cubeTransforms);
}

// Render callback for the demo scene
void renderDemoScene(Application& app, DemoScene& scene) {
    scene.shader->use();
    
    // Set camera uniforms
    scene.shader->setMat4("view", app.getCamera().getViewMatrix());
    scene.shader->setMat4("projection", 
        app.getCamera().getProjectionMatrix(
            static_cast<float>(app.getWindow().getWidth()) / app.getWindow().getHeight()
        )
    );
    
    // Bind texture
    scene.texture->bind(0);
    scene.shader->setInt("textureSampler", 0);
    
    // Draw instanced cubes
    scene.cubeMesh->drawInstanced(static_cast<GLsizei>(scene.cubeTransforms.size()));
}

int main() {
    // Create application
    Application app;
    
    // Configure the application
    ApplicationConfig config;
    config.width = 1280;
    config.height = 720;
    config.title = "Silent Forge Engine - Demo";
    config.showStats = true;
    
    // Initialize application
    if (!app.initialize(config)) {
        std::cerr << "Failed to initialize application" << std::endl;
        return -1;
    }
    
    // Position camera for better view
    app.getCamera().setPosition(glm::vec3(0.0f, 1.0f, 7.0f));
    
    // Create and initialize demo scene
    DemoScene scene;
    if (!initializeDemoScene(app, scene)) {
        std::cerr << "Failed to initialize demo scene" << std::endl;
        return -1;
    }
    
    // Set update callback
    app.setUpdateCallback([&](float deltaTime) {
        updateDemoScene(app, scene, deltaTime);
    });
    
    // Set render callback
    app.setRenderCallback([&]() {
        renderDemoScene(app, scene);
    });
    
    // Run the application
    return app.run();
} 