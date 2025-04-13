#include <gtest/gtest.h>
#include <chrono>
#include <random>
#include "rendering/InstancedMesh.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Texture.hpp"
#include "core/Logger.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

using namespace SFE;
using namespace std::chrono;

class InstancedMeshPerformanceTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        window = glfwCreateWindow(800, 600, "Test Window", nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            glfwTerminate();
            throw std::runtime_error("Failed to initialize GLAD");
        }
    }

    static void TearDownTestSuite() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void SetUp() override {
        // Create a simple mesh for testing
        std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            {{0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 1.0f}}
        };
        std::vector<unsigned int> indices = {0, 1, 2};
        
        mesh = std::make_unique<Mesh>(vertices, indices);
        instancedMesh = std::make_unique<InstancedMesh>(*mesh, [](const std::string& msg) {
            Logger::getInstance().log(LogLevel::WARNING, msg);
        });
    }

    void TearDown() override {
        instancedMesh.reset();
        mesh.reset();
    }

    static GLFWwindow* window;
    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<InstancedMesh> instancedMesh;
};

GLFWwindow* InstancedMeshPerformanceTest::window = nullptr;

TEST_F(InstancedMeshPerformanceTest, UpdatePerformance) {
    const int numInstances = 10000;
    std::vector<glm::mat4> modelMatrices(numInstances);
    
    // Generate random transformations
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-10.0f, 10.0f);
    
    for (auto& matrix : modelMatrices) {
        matrix = glm::translate(glm::mat4(1.0f), 
            glm::vec3(dis(gen), dis(gen), dis(gen)));
    }

    // Measure update performance
    auto start = high_resolution_clock::now();
    instancedMesh->updateInstanceData(modelMatrices);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    Logger::getInstance().log(LogLevel::INFO, 
        "Update " + std::to_string(numInstances) + 
        " instances took: " + std::to_string(duration.count()) + " microseconds");
    
    // Expect the update to take less than 1ms for 10,000 instances
    EXPECT_LT(duration.count(), 1000);
}

TEST_F(InstancedMeshPerformanceTest, DrawPerformance) {
    const int numInstances = 10000;
    std::vector<glm::mat4> modelMatrices(numInstances);
    instancedMesh->updateInstanceData(modelMatrices);

    // Create a simple shader for testing
    const char* vertexShaderSource = R"(
        #version 450 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoord;
        layout (location = 3) in mat4 instanceMatrix;
        
        void main() {
            gl_Position = instanceMatrix * vec4(aPos, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 450 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0);
        }
    )";

    Shader shader;
    shader.compile(vertexShaderSource, fragmentShaderSource);
    shader.use();

    // Measure draw performance
    auto start = high_resolution_clock::now();
    instancedMesh->drawInstanced(numInstances);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    Logger::getInstance().log(LogLevel::INFO, 
        "Drawing " + std::to_string(numInstances) + 
        " instances took: " + std::to_string(duration.count()) + " microseconds");
    
    // Expect the draw call to take less than 2ms for 10,000 instances
    EXPECT_LT(duration.count(), 2000);
}

TEST_F(InstancedMeshPerformanceTest, MemoryUsage) {
    const int numInstances = 10000;
    std::vector<glm::mat4> modelMatrices(numInstances);
    
    // Measure memory before update
    GLint beforeSize;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &beforeSize);
    
    instancedMesh->updateInstanceData(modelMatrices);
    
    // Measure memory after update
    GLint afterSize;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &afterSize);
    
    size_t memoryUsed = afterSize - beforeSize;
    size_t expectedMemory = numInstances * sizeof(glm::mat4);
    
    Logger::getInstance().log(LogLevel::INFO, 
        "Memory used for " + std::to_string(numInstances) + 
        " instances: " + std::to_string(memoryUsed) + " bytes");
    
    // Expect memory usage to be close to expected
    EXPECT_NEAR(memoryUsed, expectedMemory, expectedMemory * 0.1); // Allow 10% margin
} 