#include "rendering/InstancedMesh.hpp"
#include "rendering/Mesh.hpp"
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <chrono>
#include <iomanip>

// Custom logging function that writes to a file with timestamps
class FileLogger {
public:
    FileLogger(const std::string& filename) : logFile(filename) {
        if (!logFile.is_open()) {
            std::cerr << "Failed to open log file: " << filename << std::endl;
        }
    }

    void operator()(const std::string& message) {
        if (logFile.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto now_c = std::chrono::system_clock::to_time_t(now);
            logFile << "[" << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S") 
                    << "] " << message << std::endl;
        }
    }

private:
    std::ofstream logFile;
};

int main() {
    // Create a simple quad mesh
    std::vector<SFE::Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0
    };

    // Create and initialize the base mesh
    SFE::Mesh baseMesh;
    baseMesh.setVertices(vertices, indices);

    // Create a file logger
    FileLogger fileLogger("instanced_mesh.log");

    // Create an InstancedMesh with the file logger
    SFE::InstancedMesh instancedMesh(baseMesh, fileLogger);

    // Create a 5x5 grid of instances, spaced 2 units apart, centered at the origin
    std::vector<glm::mat4> modelMatrices;
    const int gridSize = 5;
    const float spacing = 2.0f;

    for (int x = 0; x < gridSize; ++x) {
        for (int z = 0; z < gridSize; ++z) {
            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(
                (x - gridSize/2) * spacing,
                0.0f,
                (z - gridSize/2) * spacing
            ));
            modelMatrices.push_back(model);
        }
    }

    // Update instance data
    instancedMesh.updateInstanceData(modelMatrices);

    // Draw all instances
    instancedMesh.drawInstanced(static_cast<GLsizei>(modelMatrices.size()));

    // Test error cases
    std::vector<glm::mat4> emptyMatrices;
    instancedMesh.updateInstanceData(emptyMatrices);  // Should log a warning

    instancedMesh.drawInstanced(0);  // Should log a warning

    // Test buffer resizing with a large number of instances
    std::vector<glm::mat4> largeMatrices(200); // Exceeds initial capacity of 100
    for (size_t i = 0; i < largeMatrices.size(); ++i) {
        largeMatrices[i] = glm::mat4(1.0f);
    }
    instancedMesh.updateInstanceData(largeMatrices); // Should log a resize message

    return 0;
} 