#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <gtest/gtest.h>

#include "core/Gamepad.hpp"
#include "core/config.h"
#include "core/Camera.hpp"
#include "utils/log.h"
#include <glm/glm.hpp>

namespace sfe {

class CoreTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize core systems
        camera = std::make_unique<Camera>(
            glm::vec3(0.0f, 0.0f, 3.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        gamepad = std::make_unique<Gamepad>();
        config = std::make_unique<Config>();
    }

    void TearDown() override {
        // Clean up resources
        if (gamepad) {
            gamepad->shutdown();
        }
    }

    std::unique_ptr<Camera> camera;
    std::unique_ptr<Gamepad> gamepad;
    std::unique_ptr<Config> config;
};

TEST_F(CoreTestFixture, GamepadInitialization) {
    bool result = gamepad->initialize();
    if (result) {
        EXPECT_TRUE(gamepad->isConnected());
        EXPECT_FALSE(gamepad->getName().empty());
    } else {
        std::cout << "No gamepad detected, skipping gamepad tests" << std::endl;
    }
}

TEST_F(CoreTestFixture, GamepadConfiguration) {
    if (gamepad->initialize() && gamepad->isConnected()) {
        EXPECT_TRUE(config->loadGamepadBindings("resources/config/gamepad.json"));
        
        // Test button mapping
        std::string action = gamepad->getMappedAction(SDL_CONTROLLER_BUTTON_A, *config);
        EXPECT_EQ(action, "jump");
        
        // Test axis range
        auto range = config->getAxisRange(SDL_CONTROLLER_AXIS_LEFTX);
        EXPECT_FLOAT_EQ(range.first, -1.0f);
        EXPECT_FLOAT_EQ(range.second, 1.0f);
    }
}

TEST_F(CoreTestFixture, CameraTransformation) {
    // Test camera view matrix
    glm::mat4 view = camera->getViewMatrix();
    EXPECT_FALSE(glm::all(glm::equal(view, glm::mat4(1.0f))));
    
    // Test camera projection matrix
    float aspectRatio = 800.0f / 600.0f;
    glm::mat4 proj = camera->getProjectionMatrix(aspectRatio);
    EXPECT_FALSE(glm::all(glm::equal(proj, glm::mat4(1.0f))));
}

TEST_F(CoreTestFixture, GamepadInput) {
    if (gamepad->initialize() && gamepad->isConnected()) {
        EXPECT_TRUE(gamepad->pollEvents());
        
        // Test axis values
        float leftX = gamepad->getAxisValue(SDL_CONTROLLER_AXIS_LEFTX);
        float leftY = gamepad->getAxisValue(SDL_CONTROLLER_AXIS_LEFTY);
        EXPECT_GE(leftX, -1.0f);
        EXPECT_LE(leftX, 1.0f);
        EXPECT_GE(leftY, -1.0f);
        EXPECT_LE(leftY, 1.0f);
    }
}

} // namespace sfe 