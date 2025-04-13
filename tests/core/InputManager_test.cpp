#include <gtest/gtest.h>
#include <core/InputManager.hpp>
#include "TestFixture.hpp"
#include "core/WindowManager.hpp"
#include <memory>
#include <thread>

using namespace SFE;
using namespace SFE::Test;

class InputManagerTest : public TestFixture {
protected:
    void SetUp() override {
        TestFixture::SetUp();
        windowManager = std::make_unique<WindowManager>();
        inputManager = std::make_unique<InputManager>(windowManager.get());
    }

    void TearDown() override {
        inputManager.reset();
        windowManager.reset();
        TestFixture::TearDown();
    }

    std::unique_ptr<WindowManager> windowManager;
    std::unique_ptr<InputManager> inputManager;
};

// Basic Input Tests
TEST_F(InputManagerTest, KeyStateManagement) {
    // Test key press
    inputManager->setKeyState(GLFW_KEY_W, GLFW_PRESS);
    EXPECT_TRUE(inputManager->isKeyPressed(GLFW_KEY_W));
    
    // Test key release
    inputManager->setKeyState(GLFW_KEY_W, GLFW_RELEASE);
    EXPECT_FALSE(inputManager->isKeyPressed(GLFW_KEY_W));
}

TEST_F(InputManagerTest, MouseStateManagement) {
    // Test mouse button press
    inputManager->setMouseButtonState(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS);
    EXPECT_TRUE(inputManager->isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT));
    
    // Test mouse button release
    inputManager->setMouseButtonState(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE);
    EXPECT_FALSE(inputManager->isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT));
}

// Mouse Position Tests
TEST_F(InputManagerTest, MousePositionTracking) {
    glm::vec2 position(100.0f, 200.0f);
    inputManager->setMousePosition(position);
    
    EXPECT_EQ(inputManager->getMousePosition(), position);
    EXPECT_EQ(inputManager->getMouseDelta(), glm::vec2(0.0f));
    
    // Test mouse movement
    glm::vec2 newPosition(150.0f, 250.0f);
    inputManager->setMousePosition(newPosition);
    
    EXPECT_EQ(inputManager->getMousePosition(), newPosition);
    EXPECT_EQ(inputManager->getMouseDelta(), newPosition - position);
}

// Gamepad Tests
TEST_F(InputManagerTest, GamepadConnection) {
    // Test gamepad connection
    inputManager->setGamepadConnected(0, true);
    EXPECT_TRUE(inputManager->isGamepadConnected(0));
    
    // Test gamepad disconnection
    inputManager->setGamepadConnected(0, false);
    EXPECT_FALSE(inputManager->isGamepadConnected(0));
}

TEST_F(InputManagerTest, GamepadButtonState) {
    inputManager->setGamepadConnected(0, true);
    
    // Test button press
    inputManager->setGamepadButtonState(0, GLFW_GAMEPAD_BUTTON_A, GLFW_PRESS);
    EXPECT_TRUE(inputManager->isGamepadButtonPressed(0, GLFW_GAMEPAD_BUTTON_A));
    
    // Test button release
    inputManager->setGamepadButtonState(0, GLFW_GAMEPAD_BUTTON_A, GLFW_RELEASE);
    EXPECT_FALSE(inputManager->isGamepadButtonPressed(0, GLFW_GAMEPAD_BUTTON_A));
}

// Performance Tests
TEST_F(InputManagerTest, Performance_InputProcessing) {
    TestPerformance([&]() {
        for (int i = 0; i < 1000; ++i) {
            inputManager->setKeyState(GLFW_KEY_W, GLFW_PRESS);
            inputManager->setMousePosition(glm::vec2(i, i));
            inputManager->setGamepadButtonState(0, GLFW_GAMEPAD_BUTTON_A, GLFW_PRESS);
        }
    });
}

// Thread Safety Tests
TEST_F(InputManagerTest, ThreadSafety_InputUpdates) {
    TestThreadSafety(*inputManager, [](InputManager& input) {
        input.setKeyState(GLFW_KEY_W, GLFW_PRESS);
        input.setMousePosition(glm::vec2(100.0f, 100.0f));
        input.setGamepadButtonState(0, GLFW_GAMEPAD_BUTTON_A, GLFW_PRESS);
    });
}

// Memory Tests
TEST_F(InputManagerTest, MemoryLeak_InputProcessing) {
    TestMemoryLeak([&]() {
        auto tempInput = std::make_unique<InputManager>(windowManager.get());
        tempInput->setKeyState(GLFW_KEY_W, GLFW_PRESS);
        tempInput->setMousePosition(glm::vec2(100.0f, 100.0f));
    });
}

// Edge Cases
TEST_F(InputManagerTest, InvalidKeyCode) {
    EXPECT_FALSE(inputManager->isKeyPressed(-1));
    EXPECT_FALSE(inputManager->isKeyPressed(GLFW_KEY_LAST + 1));
}

TEST_F(InputManagerTest, InvalidGamepadIndex) {
    EXPECT_FALSE(inputManager->isGamepadConnected(-1));
    EXPECT_FALSE(inputManager->isGamepadConnected(GLFW_JOYSTICK_LAST + 1));
}

// Input Callback Tests
TEST_F(InputManagerTest, KeyCallback) {
    bool callbackCalled = false;
    inputManager->setKeyCallback([&](int key, int action) {
        callbackCalled = true;
        EXPECT_EQ(key, GLFW_KEY_W);
        EXPECT_EQ(action, GLFW_PRESS);
    });
    
    inputManager->setKeyState(GLFW_KEY_W, GLFW_PRESS);
    EXPECT_TRUE(callbackCalled);
}

TEST_F(InputManagerTest, MouseCallback) {
    bool callbackCalled = false;
    inputManager->setMouseButtonCallback([&](int button, int action) {
        callbackCalled = true;
        EXPECT_EQ(button, GLFW_MOUSE_BUTTON_LEFT);
        EXPECT_EQ(action, GLFW_PRESS);
    });
    
    inputManager->setMouseButtonState(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS);
    EXPECT_TRUE(callbackCalled);
} 