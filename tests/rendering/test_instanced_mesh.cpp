#include <gtest/gtest.h>
#include "rendering/InstancedMesh.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/Frustum.hpp"
#include "SFEGlobalIncludes.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <functional>

namespace SFE {
namespace Rendering {

// Custom logger for testing
void TestLogger(const std::string& message) {
    std::cout << "[TEST LOG] " << message << std::endl;
}

class InstancedMeshTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize GLFW
        if (!glfwInit()) {
            FAIL() << "Failed to initialize GLFW";
        }

        // Create a window and OpenGL context
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Make window invisible

        window = glfwCreateWindow(800, 600, "Test Window", nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            FAIL() << "Failed to create GLFW window";
        }

        glfwMakeContextCurrent(window);

        // Initialize GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            glfwTerminate();
            FAIL() << "Failed to initialize GLAD";
        }

        // Create a simple mesh for testing
        std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            {{0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 1.0f}}
        };
        std::vector<unsigned int> indices = {0, 1, 2};
        testMesh = std::make_unique<Mesh>(vertices, indices);
    }

    void TearDown() override {
        testMesh.reset();
        glfwTerminate();
    }

    GLFWwindow* window;
    std::unique_ptr<Mesh> testMesh;
};

TEST_F(InstancedMeshTest, ConstructorInitialization) {
    InstancedMesh instancedMesh(*testMesh, TestLogger);
    EXPECT_NE(instancedMesh.getInstanceVBO(), 0);
    EXPECT_EQ(instancedMesh.getInstanceCount(), 0);
}

TEST_F(InstancedMeshTest, UpdateInstanceData) {
    InstancedMesh instancedMesh(*testMesh, TestLogger);
    
    // Test empty instance data
    std::vector<glm::mat4> emptyMatrices;
    instancedMesh.updateInstanceData(emptyMatrices);
    EXPECT_EQ(instancedMesh.getInstanceCount(), 0);

    // Test valid instance data
    std::vector<glm::mat4> matrices = {
        glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f))
    };
    instancedMesh.updateInstanceData(matrices);
    EXPECT_EQ(instancedMesh.getInstanceCount(), matrices.size());
}

TEST_F(InstancedMeshTest, DrawInstances) {
    InstancedMesh instancedMesh(*testMesh, TestLogger);
    
    // Set up some instance data
    std::vector<glm::mat4> matrices = {
        glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f))
    };
    instancedMesh.updateInstanceData(matrices);

    // Test drawing with valid instance count
    EXPECT_NO_THROW(instancedMesh.drawInstanced(2));

    // Test drawing with zero instances
    EXPECT_NO_THROW(instancedMesh.drawInstanced(0));
}

TEST_F(InstancedMeshTest, FrustumCulling) {
    InstancedMesh instancedMesh(*testMesh, TestLogger);
    
    // Create a frustum
    Frustum frustum;
    frustum.update(glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f) * 
                  glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), 
                             glm::vec3(0.0f, 0.0f, 0.0f), 
                             glm::vec3(0.0f, 1.0f, 0.0f)));

    // Set up instance data with bounds
    std::vector<glm::mat4> matrices = {
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)),  // Inside frustum
        glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 100.0f))  // Outside frustum
    };
    std::vector<InstanceBounds> bounds = {
        {{0.0f, 0.0f, 0.0f}, 1.0f},  // Inside frustum
        {{100.0f, 100.0f, 100.0f}, 1.0f}  // Outside frustum
    };
    
    instancedMesh.updateInstanceData(matrices);
    instancedMesh.updateInstanceBounds(bounds);

    // Test frustum culling
    EXPECT_NO_THROW(instancedMesh.drawInstanced(frustum, 2));
}

TEST_F(InstancedMeshTest, DynamicInstanceCount) {
    InstancedMesh instancedMesh(*testMesh, TestLogger);
    
    // Test adding instances
    for (int i = 0; i < 5; ++i) {
        glm::mat4 matrix = glm::translate(glm::mat4(1.0f), glm::vec3(i, 0.0f, 0.0f));
        InstanceBounds bounds{{i, 0.0f, 0.0f}, 1.0f};
        instancedMesh.addInstance(matrix, bounds);
    }
    EXPECT_EQ(instancedMesh.getInstanceCount(), 5);

    // Test removing instances
    instancedMesh.removeInstance(2);
    EXPECT_EQ(instancedMesh.getInstanceCount(), 4);

    // Test clearing instances
    instancedMesh.clearInstances();
    EXPECT_EQ(instancedMesh.getInstanceCount(), 0);
}

TEST_F(InstancedMeshTest, ErrorHandling) {
    InstancedMesh instancedMesh(*testMesh, TestLogger);
    
    // Test invalid instance count
    EXPECT_NO_THROW(instancedMesh.drawInstanced(-1));

    // Test invalid index removal
    EXPECT_NO_THROW(instancedMesh.removeInstance(100));

    // Test invalid bounds count
    std::vector<InstanceBounds> invalidBounds = {{glm::vec3(0.0f), 1.0f}};
    instancedMesh.updateInstanceBounds(invalidBounds);
    EXPECT_NO_THROW(instancedMesh.drawInstanced(2));
}

} // namespace Rendering
} // namespace SFE

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 