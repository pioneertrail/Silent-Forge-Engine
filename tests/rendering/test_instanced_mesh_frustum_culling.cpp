#include <gtest/gtest.h>
#include "rendering/InstancedMesh.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Frustum.hpp"
#include "core/Logger.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

using namespace SFE;

class InstancedMeshFrustumCullingTest : public ::testing::Test {
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

GLFWwindow* InstancedMeshFrustumCullingTest::window = nullptr;

TEST_F(InstancedMeshFrustumCullingTest, FrustumCulling) {
    const int numInstances = 100;
    std::vector<glm::mat4> modelMatrices(numInstances);
    std::vector<InstanceBounds> bounds(numInstances);
    
    // Create instances in a grid pattern
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            int index = i * 10 + j;
            glm::vec3 position(i * 2.0f - 10.0f, 0.0f, j * 2.0f - 10.0f);
            modelMatrices[index] = glm::translate(glm::mat4(1.0f), position);
            
            // Set bounds for each instance
            bounds[index].center = position;
            bounds[index].radius = 1.0f;
            bounds[index].min = position - glm::vec3(1.0f);
            bounds[index].max = position + glm::vec3(1.0f);
        }
    }
    
    // Update instance data and bounds
    instancedMesh->updateInstanceData(modelMatrices);
    instancedMesh->updateInstanceBounds(bounds);
    
    // Create a view-projection matrix for testing
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 10.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    glm::mat4 proj = glm::perspective(
        glm::radians(45.0f),
        800.0f / 600.0f,
        0.1f,
        100.0f
    );
    glm::mat4 viewProj = proj * view;
    
    // Create and update frustum
    Frustum frustum;
    frustum.update(viewProj);
    
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
    
    // Draw with frustum culling
    instancedMesh->drawInstanced(frustum, numInstances);
}

TEST_F(InstancedMeshFrustumCullingTest, BoundsMismatch) {
    const int numInstances = 10;
    std::vector<glm::mat4> modelMatrices(numInstances);
    std::vector<InstanceBounds> bounds(numInstances - 1); // One less bound than instances
    
    instancedMesh->updateInstanceData(modelMatrices);
    instancedMesh->updateInstanceBounds(bounds);
    
    Frustum frustum;
    frustum.update(glm::mat4(1.0f));
    
    // This should log a warning but not crash
    instancedMesh->drawInstanced(frustum, numInstances);
}

TEST_F(InstancedMeshFrustumCullingTest, EmptyFrustum) {
    const int numInstances = 10;
    std::vector<glm::mat4> modelMatrices(numInstances);
    std::vector<InstanceBounds> bounds(numInstances);
    
    // Place instances far away from the frustum
    for (int i = 0; i < numInstances; ++i) {
        modelMatrices[i] = glm::translate(glm::mat4(1.0f), 
            glm::vec3(1000.0f + i * 10.0f, 0.0f, 0.0f));
        bounds[i].center = glm::vec3(1000.0f + i * 10.0f, 0.0f, 0.0f);
        bounds[i].radius = 1.0f;
    }
    
    instancedMesh->updateInstanceData(modelMatrices);
    instancedMesh->updateInstanceBounds(bounds);
    
    Frustum frustum;
    frustum.update(glm::mat4(1.0f));
    
    // This should not draw anything but not crash
    instancedMesh->drawInstanced(frustum, numInstances);
} 