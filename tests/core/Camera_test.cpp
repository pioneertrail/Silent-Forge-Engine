#include "TestFixture.hpp"
#include "core/Camera.hpp"
#include <glm/glm.hpp>
#include <memory>

using namespace SFE;
using namespace SFE::Test;

class CameraTest : public TestFixture {
protected:
    void SetUp() override {
        TestFixture::SetUp();
        camera = std::make_unique<Camera>();
    }

    void TearDown() override {
        camera.reset();
        TestFixture::TearDown();
    }

    std::unique_ptr<Camera> camera;
};

// Basic Camera Operations
TEST_F(CameraTest, CameraCreation) {
    EXPECT_EQ(camera->getPosition(), glm::vec3(0.0f));
    EXPECT_EQ(camera->getFront(), glm::vec3(0.0f, 0.0f, -1.0f));
    EXPECT_EQ(camera->getUp(), glm::vec3(0.0f, 1.0f, 0.0f));
}

// View Matrix Tests
TEST_F(CameraTest, ViewMatrixCalculation) {
    camera->setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    camera->setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    
    glm::mat4 viewMatrix = camera->getViewMatrix();
    glm::vec3 expectedPosition = glm::vec3(0.0f, 0.0f, -3.0f);
    EXPECT_EQ(glm::vec3(viewMatrix[3]), expectedPosition);
}

// Projection Matrix Tests
TEST_F(CameraTest, ProjectionMatrixCalculation) {
    camera->setPerspective(45.0f, 16.0f/9.0f, 0.1f, 100.0f);
    glm::mat4 projMatrix = camera->getProjectionMatrix();
    
    // Test near and far plane
    EXPECT_NEAR(projMatrix[2][2], -1.002002, 0.0001f);
    EXPECT_NEAR(projMatrix[3][2], -0.2002002, 0.0001f);
}

// Camera Movement Tests
TEST_F(CameraTest, CameraMovement) {
    camera->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    camera->moveForward(1.0f);
    EXPECT_EQ(camera->getPosition(), glm::vec3(0.0f, 0.0f, -1.0f));
    
    camera->moveRight(1.0f);
    EXPECT_EQ(camera->getPosition(), glm::vec3(1.0f, 0.0f, -1.0f));
    
    camera->moveUp(1.0f);
    EXPECT_EQ(camera->getPosition(), glm::vec3(1.0f, 1.0f, -1.0f));
}

// Camera Rotation Tests
TEST_F(CameraTest, CameraRotation) {
    camera->setYaw(90.0f);
    camera->setPitch(45.0f);
    
    glm::vec3 front = camera->getFront();
    EXPECT_NEAR(front.x, 0.7071f, 0.0001f);
    EXPECT_NEAR(front.y, 0.7071f, 0.0001f);
    EXPECT_NEAR(front.z, 0.0f, 0.0001f);
}

// Performance Tests
TEST_F(CameraTest, Performance_MatrixCalculations) {
    TestPerformance([&]() {
        camera->getViewMatrix();
        camera->getProjectionMatrix();
    });
}

// Thread Safety Tests
TEST_F(CameraTest, ThreadSafety_MatrixUpdates) {
    TestThreadSafety(*camera, [](Camera& cam) {
        cam.setPosition(glm::vec3(static_cast<float>(rand()), 0.0f, 0.0f));
        cam.getViewMatrix();
    });
}

// Memory Tests
TEST_F(CameraTest, MemoryLeak_MatrixCalculations) {
    TestMemoryLeak([&]() {
        auto tempCamera = std::make_unique<Camera>();
        tempCamera->getViewMatrix();
        tempCamera->getProjectionMatrix();
    });
}

// Edge Cases
TEST_F(CameraTest, InvalidProjectionParameters) {
    EXPECT_THROW(camera->setPerspective(0.0f, 16.0f/9.0f, 0.1f, 100.0f), std::invalid_argument);
    EXPECT_THROW(camera->setPerspective(45.0f, 0.0f, 0.1f, 100.0f), std::invalid_argument);
    EXPECT_THROW(camera->setPerspective(45.0f, 16.0f/9.0f, -0.1f, 100.0f), std::invalid_argument);
    EXPECT_THROW(camera->setPerspective(45.0f, 16.0f/9.0f, 0.1f, 0.0f), std::invalid_argument);
}

// View Frustum Tests
TEST_F(CameraTest, ViewFrustumCalculation) {
    camera->setPerspective(45.0f, 16.0f/9.0f, 0.1f, 100.0f);
    camera->setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    camera->setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    
    const auto& frustum = camera->getFrustum();
    EXPECT_TRUE(frustum.isPointInFrustum(glm::vec3(0.0f, 0.0f, 0.0f)));
    EXPECT_FALSE(frustum.isPointInFrustum(glm::vec3(0.0f, 0.0f, 101.0f)));
} 