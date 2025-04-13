#include <gtest/gtest.h>
#include "rendering/InstancedMesh.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/Shader.hpp"
#include "core/Logger.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

using namespace SFE;

class InstancedMeshCustomAttributesTest : public ::testing::Test {
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

GLFWwindow* InstancedMeshCustomAttributesTest::window = nullptr;

TEST_F(InstancedMeshCustomAttributesTest, CustomColorAttributes) {
    const int numInstances = 100;
    struct InstanceData {
        glm::vec3 position;
        glm::vec3 color;
    };
    
    std::vector<InstanceData> instanceData(numInstances);
    for (int i = 0; i < numInstances; ++i) {
        instanceData[i].position = glm::vec3(i * 0.1f, 0.0f, 0.0f);
        instanceData[i].color = glm::vec3(i / float(numInstances), 0.0f, 0.0f);
    }
    
    std::vector<InstanceAttribute> attributes = {
        {3, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, position), 1},
        {4, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, color), 1}
    };
    
    instancedMesh->updateInstanceData(attributes, instanceData.data(), 
                                    sizeof(InstanceData), numInstances);
    
    // Create a shader that uses the custom attributes
    const char* vertexShaderSource = R"(
        #version 450 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoord;
        layout (location = 3) in vec3 instancePos;
        layout (location = 4) in vec3 instanceColor;
        
        out vec3 color;
        
        void main() {
            gl_Position = vec4(aPos + instancePos, 1.0);
            color = instanceColor;
        }
    )";
    
    const char* fragmentShaderSource = R"(
        #version 450 core
        in vec3 color;
        out vec4 FragColor;
        
        void main() {
            FragColor = vec4(color, 1.0);
        }
    )";
    
    Shader shader;
    shader.compile(vertexShaderSource, fragmentShaderSource);
    shader.use();
    
    // Draw the instances
    instancedMesh->drawInstanced(numInstances);
}

TEST_F(InstancedMeshCustomAttributesTest, MixedAttributes) {
    const int numInstances = 50;
    struct InstanceData {
        glm::mat4 transform;
        float scale;
        glm::vec3 color;
    };
    
    std::vector<InstanceData> instanceData(numInstances);
    for (int i = 0; i < numInstances; ++i) {
        instanceData[i].transform = glm::translate(glm::mat4(1.0f), 
            glm::vec3(i * 0.2f, 0.0f, 0.0f));
        instanceData[i].scale = 1.0f + (i * 0.1f);
        instanceData[i].color = glm::vec3(0.0f, i / float(numInstances), 0.0f);
    }
    
    std::vector<InstanceAttribute> attributes = {
        {3, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, transform), 1},
        {7, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(offsetof(InstanceData, transform) + sizeof(glm::vec4)), 1},
        {11, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(offsetof(InstanceData, transform) + 2 * sizeof(glm::vec4)), 1},
        {15, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(offsetof(InstanceData, transform) + 3 * sizeof(glm::vec4)), 1},
        {16, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, scale), 1},
        {17, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, color), 1}
    };
    
    instancedMesh->updateInstanceData(attributes, instanceData.data(), 
                                    sizeof(InstanceData), numInstances);
    
    // Create a shader that uses the mixed attributes
    const char* vertexShaderSource = R"(
        #version 450 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoord;
        layout (location = 3) in mat4 transform;
        layout (location = 16) in float scale;
        layout (location = 17) in vec3 color;
        
        out vec3 fragColor;
        
        void main() {
            gl_Position = transform * vec4(aPos * scale, 1.0);
            fragColor = color;
        }
    )";
    
    const char* fragmentShaderSource = R"(
        #version 450 core
        in vec3 fragColor;
        out vec4 FragColor;
        
        void main() {
            FragColor = vec4(fragColor, 1.0);
        }
    )";
    
    Shader shader;
    shader.compile(vertexShaderSource, fragmentShaderSource);
    shader.use();
    
    // Draw the instances
    instancedMesh->drawInstanced(numInstances);
} 