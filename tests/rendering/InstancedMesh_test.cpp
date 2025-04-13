#include <gtest/gtest.h>
#include <rendering/InstancedMesh.hpp>
#include <rendering/Mesh.hpp>
#include <helpers/GLContextHelper.hpp>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <random>

using namespace SFE::Rendering;
using namespace SFE::Testing;

/**
 * @class InstancedMeshTest
 * @brief Test fixture for InstancedMesh functionality
 * 
 * This test suite verifies the core functionality of the InstancedMesh class,
 * including instance management, rendering, performance, and thread safety.
 */
class InstancedMeshTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize OpenGL context
        ASSERT_TRUE(GLContextHelper::init());
        
        // Create base mesh
        baseMesh = std::make_unique<Mesh>();
        std::vector<float> vertices = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f
        };
        std::vector<unsigned int> indices = {
            0, 1, 2,
            2, 3, 0
        };
        ASSERT_TRUE(baseMesh->loadData(vertices, indices));
    }

    void TearDown() override {
        baseMesh.reset();
        GLContextHelper::cleanup();
    }

    std::unique_ptr<Mesh> baseMesh;
};

// Core Functionality Tests
TEST_F(InstancedMeshTest, ConstructionAndDestruction) {
    InstancedMesh instancedMesh(*baseMesh);
    EXPECT_NE(instancedMesh.getVAO(), 0);
    EXPECT_NE(instancedMesh.getInstanceVBO(), 0);
    EXPECT_EQ(instancedMesh.getInstanceCount(), 0);
}

TEST_F(InstancedMeshTest, InstanceManagement) {
    InstancedMesh instancedMesh(*baseMesh);
    
    // Test adding instances
    std::vector<glm::mat4> transforms = {
        glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f))
    };
    
    EXPECT_TRUE(instancedMesh.setInstanceTransforms(transforms));
    EXPECT_EQ(instancedMesh.getInstanceCount(), transforms.size());
    
    // Test updating instances
    transforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f)));
    EXPECT_TRUE(instancedMesh.setInstanceTransforms(transforms));
    EXPECT_EQ(instancedMesh.getInstanceCount(), transforms.size());
    
    // Test clearing instances
    EXPECT_TRUE(instancedMesh.clearInstances());
    EXPECT_EQ(instancedMesh.getInstanceCount(), 0);
}

// Transform Operation Tests
TEST_F(InstancedMeshTest, TransformOperations) {
    InstancedMesh instancedMesh(*baseMesh);
    
    // Test translation
    std::vector<glm::mat4> transforms = {
        glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 3.0f))
    };
    EXPECT_TRUE(instancedMesh.setInstanceTransforms(transforms));
    
    // Test rotation
    transforms[0] = glm::rotate(transforms[0], glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    EXPECT_TRUE(instancedMesh.setInstanceTransforms(transforms));
    
    // Test scale
    transforms[0] = glm::scale(transforms[0], glm::vec3(2.0f, 2.0f, 2.0f));
    EXPECT_TRUE(instancedMesh.setInstanceTransforms(transforms));
    
    // Test combined transformations
    transforms[0] = glm::mat4(1.0f);
    transforms[0] = glm::translate(transforms[0], glm::vec3(1.0f, 2.0f, 3.0f));
    transforms[0] = glm::rotate(transforms[0], glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    transforms[0] = glm::scale(transforms[0], glm::vec3(2.0f, 2.0f, 2.0f));
    EXPECT_TRUE(instancedMesh.setInstanceTransforms(transforms));
}

// Frustum Culling Tests
TEST_F(InstancedMeshTest, FrustumCulling) {
    InstancedMesh instancedMesh(*baseMesh);
    
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

// Performance Tests
TEST_F(InstancedMeshTest, InstanceUpdatePerformance) {
    InstancedMesh instancedMesh(*baseMesh);
    
    // Create large number of instances
    const int numInstances = 10000;
    std::vector<glm::mat4> transforms(numInstances);
    
    // Generate random transformations
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-10.0f, 10.0f);
    
    for (auto& matrix : transforms) {
        matrix = glm::translate(glm::mat4(1.0f), 
            glm::vec3(dis(gen), dis(gen), dis(gen)));
    }
    
    // Measure update time
    auto start = std::chrono::high_resolution_clock::now();
    EXPECT_TRUE(instancedMesh.setInstanceTransforms(transforms));
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Expect update to complete within 50ms
    EXPECT_LT(duration.count(), 50);
}

// Thread Safety Tests
TEST_F(InstancedMeshTest, ConcurrentInstanceUpdates) {
    InstancedMesh instancedMesh(*baseMesh);
    const int numThreads = 4;
    const int updatesPerThread = 1000;
    std::vector<std::thread> threads;
    std::atomic<bool> success(true);
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&, i]() {
            std::vector<glm::mat4> transforms = {
                glm::translate(glm::mat4(1.0f), glm::vec3(static_cast<float>(i), 0.0f, 0.0f))
            };
            
            for (int j = 0; j < updatesPerThread; ++j) {
                if (!instancedMesh.setInstanceTransforms(transforms)) {
                    success = false;
                }
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_TRUE(success);
}

// Error Handling Tests
TEST_F(InstancedMeshTest, InvalidInstanceData) {
    InstancedMesh instancedMesh(*baseMesh);
    
    // Test empty transform array
    std::vector<glm::mat4> emptyTransforms;
    EXPECT_FALSE(instancedMesh.setInstanceTransforms(emptyTransforms));
    
    // Test invalid instance count
    std::vector<glm::mat4> tooManyTransforms(1000000); // Unreasonably large
    EXPECT_FALSE(instancedMesh.setInstanceTransforms(tooManyTransforms));
    
    // Test invalid transform data
    std::vector<glm::mat4> invalidTransforms = {
        glm::mat4(0.0f) // Zero matrix
    };
    EXPECT_FALSE(instancedMesh.setInstanceTransforms(invalidTransforms));
}

// Memory Tests
TEST_F(InstancedMeshTest, MemoryManagement) {
    {
        InstancedMesh instancedMesh(*baseMesh);
        std::vector<glm::mat4> transforms = {
            glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f))
        };
        EXPECT_TRUE(instancedMesh.setInstanceTransforms(transforms));
    }
    // MemoryLeakDetector will verify no leaks occurred
} 