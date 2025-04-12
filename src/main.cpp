#include "core/WindowManager.hpp"
#include "core/Camera.hpp"
#include "core/InputManager.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/TextRenderer.hpp"
#include "rendering/Texture.hpp"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <memory>

int main() {
    SFE::WindowManager window(800, 600, "Silent Forge Engine");
    if (!window.initialize()) {
        return -1;
    }

    SFE::Shader shader("shaders/simple.vert", "shaders/simple.frag");

    std::vector<SFE::Mesh::Vertex> vertices = {
        {{ -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f }},
        {{  0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f }},
        {{  0.0f,  0.5f, 0.0f }, { 0.5f, 1.0f }}
    };
    SFE::Mesh mesh(vertices);

    auto texture = std::make_shared<SFE::Texture>();
    // Using hardcoded color data instead of loading from file for simplicity
    if (!texture->loadFromFile("assets/textures/test.ppm")) {
        std::cerr << "Failed to load texture from PPM, creating a simple color pattern" << std::endl;
        
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
    mesh.setTexture(texture);

    SFE::Camera camera;
    SFE::InputManager input;
    input.initialize(window.getWindow());

    SFE::TextRenderer textRenderer;
    if (!textRenderer.initialize("assets/fonts/consolas.png", "assets/fonts/consolas.fnt")) {
        std::cerr << "Failed to initialize text renderer" << std::endl;
    }

    float lastFrame = 0.0f;
    float frameTimes[60] = {0.0f};
    int frameIndex = 0;
    float frameTimeSum = 0.0f;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!window.shouldClose()) {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        frameTimeSum -= frameTimes[frameIndex];
        frameTimes[frameIndex] = deltaTime;
        frameTimeSum += deltaTime;
        frameIndex = (frameIndex + 1) % 60;
        float avgFrameTime = frameTimeSum / 60.0f;
        int fps = static_cast<int>(1.0f / avgFrameTime);
        std::stringstream ss;
        ss << "FPS: " << fps;
        std::string fpsText = ss.str();

        input.processInput(window.getWindow(), camera, deltaTime);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setInt("texture0", 0);
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        shader.setMat4("view", camera.getViewMatrix());
        shader.setMat4("projection", camera.getProjectionMatrix(800.0f / 600.0f));
        mesh.draw();

        glm::mat4 ortho = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
        textRenderer.renderText("Silent Forge Engine", 10.0f, 560.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f), ortho);
        textRenderer.renderText(fpsText, 10.0f, 540.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f), ortho);

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
} 