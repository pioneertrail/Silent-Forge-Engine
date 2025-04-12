// src/main.cpp
#include "core/WindowManager.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Mesh.hpp"
#include <vector>
#include <iostream> // For potential error output if needed

int main() {
    Engine::WindowManager windowManager(800, 600, "NewEngine - Step 3: Triangle");

    if (!windowManager.initialize()) {
        std::cerr << "Failed to initialize WindowManager." << std::endl;
        return -1;
    }

    // Create shader program
    // Make sure the shader files are accessible (e.g., copied to build dir or use absolute paths)
    // The CMakeLists.txt now includes a command to copy shaders to the build/bin/shaders directory.
    // We should use relative paths from the executable.
    Engine::Shader shader("shaders/simple.vert", "shaders/simple.frag");

    // Create triangle mesh data
    std::vector<float> vertices = {
        // positions
        -0.5f, -0.5f, 0.0f,  // bottom left
         0.5f, -0.5f, 0.0f,  // bottom right
         0.0f,  0.5f, 0.0f   // top center
    };
    // No indices needed for a single triangle drawn with glDrawArrays
    Engine::Mesh mesh(vertices, 3); // 3 vertices

    // Main loop
    while (!windowManager.shouldClose()) {
        // Input processing (TODO)

        // Rendering
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Teal background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw the triangle
        shader.use();
        mesh.draw();

        // Swap buffers and poll events
        windowManager.swapBuffers();
        windowManager.pollEvents();
    }

    // Cleanup is handled by destructors (WindowManager, Shader, Mesh)

    return 0;
} 