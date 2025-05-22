#include <catch2/catch_test_macros.hpp>
#include "core/config.h"
#include "core/Gamepad.hpp"
#include "input/gamepad.h"
#include <filesystem>
#include <fstream>
#include "utils/log.h"
#include <gtest/gtest.h>

// Mock Gamepad for testing
class MockGamepad : public sfe::Gamepad {
public:
    bool isButtonSupported(SDL_GameControllerButton button) const override {
        return button == SDL_CONTROLLER_BUTTON_A; // Only A supported
    }
    bool isAxisSupported(SDL_GameControllerAxis axis) const override {
        return axis == SDL_CONTROLLER_AXIS_LEFTX; // Only left X supported
    }
};

namespace sfe {

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directories
        std::filesystem::create_directories("tests/fixtures");
    }

    void TearDown() override {
        // Clean up test files
        std::filesystem::remove_all("tests/fixtures");
    }
};

TEST_CASE("Config loads and saves gamepad bindings", "[config]") {
    const std::string testDir = "tests/fixtures";
    const std::string validConfigPath = testDir + "/valid_gamepad.json";
    const std::string malformedConfigPath = testDir + "/malformed_gamepad.json";
    const std::string savePath = testDir + "/test_gamepad.json";

    auto createFixture = [&](const std::string& path, const std::string& content) {
        std::filesystem::create_directories(testDir);
        std::ofstream file(path);
        file << content;
        file.close();
    };

    auto cleanup = [&]() {
        for (const auto& file : {validConfigPath, malformedConfigPath, savePath}) {
            if (std::filesystem::exists(file)) {
                std::filesystem::remove(file);
            }
        }
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    };

    cleanup();

    SECTION("Load empty JSON configuration") {
        createFixture(validConfigPath, "{}");
        sfe::Config config; // No Gamepad needed for empty config
        REQUIRE(config.loadGamepadBindings(validConfigPath));
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_A).empty());
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_B).empty());
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTX).first == -1.0f);
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTX).second == 1.0f);
    }

    SECTION("Load full bindings") {
        createFixture(validConfigPath, R"({
            "buttons": {"0": "jump"},
            "axes": {"0": {"min": -1.0, "max": 1.0}}
        })");
        MockGamepad gamepad;
        sfe::Config config(&gamepad);
        REQUIRE(config.loadGamepadBindings(validConfigPath));
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_A) == "jump");
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_B).empty()); // Verify unsupported button is empty
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTX).first == -1.0f);
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTX).second == 1.0f);
    }

    SECTION("Load partial bindings") {
        createFixture(validConfigPath, R"({
            "buttons": {"0": "jump", "1": "attack", "2": "defend"},
            "axes": {"0": {"min": -1.0, "max": 1.0}}
        })");
        MockGamepad gamepad;
        sfe::Config config(&gamepad);
        REQUIRE(config.loadGamepadBindings(validConfigPath));
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_A) == "jump");
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_B).empty());
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTX).first == -1.0f);
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTX).second == 1.0f);
    }

    SECTION("Load all unsupported bindings") {
        createFixture(validConfigPath, R"({
            "buttons": {"1": "attack", "2": "defend"},
            "axes": {"1": {"min": -1.0, "max": 1.0}, "2": {"min": 0.0, "max": 1.0}}
        })");
        MockGamepad gamepad;
        sfe::Config config(&gamepad);
        REQUIRE(config.loadGamepadBindings(validConfigPath));
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_A).empty());
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_B).empty());
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_X).empty());
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTX).first == -1.0f);
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTY).first == -1.0f);
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_TRIGGERLEFT).first == -1.0f);
    }

    SECTION("Load valid bindings") {
        createFixture(validConfigPath, R"({
            "buttons": {
                "0": "jump",
                "1": "attack"
            },
            "axes": {
                "0": {
                    "min": -1.0,
                    "max": 1.0
                },
                "2": {
                    "min": 0.0,
                    "max": 1.0
                }
            }
        })");

        sfe::Config config;
        REQUIRE(config.loadGamepadBindings(validConfigPath));
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_A) == "jump");
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_B) == "attack");
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTX).first == -1.0f);
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTX).second == 1.0f);
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_TRIGGERLEFT).first == 0.0f);
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_TRIGGERLEFT).second == 1.0f);
    }

    SECTION("Load bindings with Gamepad validation") {
        createFixture(validConfigPath, R"({
            "buttons": {"0": "jump", "1": "attack"},
            "axes": {"0": {"min": -1.0, "max": 1.0}, "2": {"min": 0.0, "max": 1.0}}
        })");

        MockGamepad gamepad;
        sfe::Config config(&gamepad);
        REQUIRE(config.loadGamepadBindings(validConfigPath));
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_A) == "jump");
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_B).empty()); // Not supported
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTX).first == -1.0f);
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_TRIGGERLEFT).first == -1.0f); // Default
    }

    SECTION("Load bindings without Gamepad") {
        createFixture(validConfigPath, R"({
            "buttons": {"0": "jump", "1": "attack"},
            "axes": {"0": {"min": -1.0, "max": 1.0}}
        })");
        sfe::Config config; // No Gamepad
        REQUIRE(config.loadGamepadBindings(validConfigPath));
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_A) == "jump");
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_B) == "attack");
    }

    SECTION("Load large config without Gamepad") {
        createFixture(validConfigPath, R"({
            "buttons": {"0": "jump", "1": "attack", "2": "interact", "3": "menu"},
            "axes": {"0": {"min": -1.0, "max": 1.0}, "1": {"min": -1.0, "max": 1.0}}
        })");
        sfe::Config config;
        REQUIRE(config.loadGamepadBindings(validConfigPath));
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_A) == "jump");
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_B) == "attack");
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_X) == "interact");
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_Y) == "menu");
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTX).first == -1.0f);
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTX).second == 1.0f);
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTY).first == -1.0f);
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTY).second == 1.0f);
    }

    SECTION("Load invalid file") {
        sfe::Config config;
        REQUIRE_FALSE(config.loadGamepadBindings("nonexistent.json"));
    }

    SECTION("Load invalid JSON") {
        createFixture(validConfigPath, "invalid json content");
        sfe::Config config;
        REQUIRE_FALSE(config.loadGamepadBindings(validConfigPath));
    }

    SECTION("Load malformed JSON fields") {
        createFixture(malformedConfigPath, R"({
            "buttons": {"0": "jump"},
            "axes": {"0": {"min": "invalid", "max": 1.0}}
        })");
        sfe::Config config;
        REQUIRE_FALSE(config.loadGamepadBindings(malformedConfigPath));
    }

    SECTION("Load invalid button/axis keys") {
        createFixture(validConfigPath, R"({
            "buttons": {"999": "jump"},
            "axes": {"999": {"min": -1.0, "max": 1.0}}
        })");
        sfe::Config config;
        REQUIRE_FALSE(config.loadGamepadBindings(validConfigPath));
    }

    SECTION("Load file with permission error") {
        createFixture(validConfigPath, R"({"buttons": {"0": "jump"}})");
        #ifdef __linux__
        std::filesystem::permissions(validConfigPath, std::filesystem::perms::none);
        sfe::Config config;
        REQUIRE_FALSE(config.loadGamepadBindings(validConfigPath));
        std::filesystem::permissions(validConfigPath, std::filesystem::perms::all);
        #endif
    }

    SECTION("Load invalid axis range") {
        createFixture(validConfigPath, R"({
            "axes": {"0": {"min": 1.0, "max": -1.0}}
        })");
        sfe::Config config;
        REQUIRE_FALSE(config.loadGamepadBindings(validConfigPath));
    }

    SECTION("Save and reload bindings") {
        createFixture(validConfigPath, R"({
            "buttons": {"0": "jump", "1": "attack"},
            "axes": {"0": {"min": -1.0, "max": 1.0}}
        })");

        sfe::Config config;
        REQUIRE(config.loadGamepadBindings(validConfigPath));
        REQUIRE(config.saveGamepadBindings(savePath));
        
        sfe::Config newConfig;
        REQUIRE(newConfig.loadGamepadBindings(savePath));
        REQUIRE(newConfig.getButtonAction(SDL_CONTROLLER_BUTTON_A) == "jump");
        REQUIRE(newConfig.getButtonAction(SDL_CONTROLLER_BUTTON_B) == "attack");
        REQUIRE(newConfig.getAxisRange(SDL_CONTROLLER_AXIS_LEFTX).first == -1.0f);
        REQUIRE(newConfig.getAxisRange(SDL_CONTROLLER_AXIS_LEFTX).second == 1.0f);
    }

    SECTION("Default axis range") {
        sfe::Config config;
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTX).first == -1.0f);
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTX).second == 1.0f);
    }

    SECTION("Default button action") {
        sfe::Config config;
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_A).empty());
    }

    cleanup();
}

TEST_F(ConfigTest, LoadValidGamepadConfig) {
    Config config;
    EXPECT_TRUE(config.loadGamepadBindings("tests/fixtures/valid_gamepad.json"));
    
    // Verify button mappings
    EXPECT_EQ(config.getButtonAction(SDL_CONTROLLER_BUTTON_A), "jump");
    EXPECT_EQ(config.getButtonAction(SDL_CONTROLLER_BUTTON_B), "attack");
    
    // Verify axis ranges
    auto range = config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTX);
    EXPECT_FLOAT_EQ(range.first, -1.0f);
    EXPECT_FLOAT_EQ(range.second, 1.0f);
}

TEST_F(ConfigTest, LoadInvalidGamepadConfig) {
    Config config;
    EXPECT_FALSE(config.loadGamepadBindings("tests/fixtures/invalid_gamepad.json"));
}

TEST_F(ConfigTest, SaveGamepadConfig) {
    Config config;
    EXPECT_TRUE(config.loadGamepadBindings("tests/fixtures/valid_gamepad.json"));
    
    std::string testPath = "tests/fixtures/saved_gamepad.json";
    EXPECT_TRUE(config.saveGamepadBindings(testPath));
    EXPECT_TRUE(std::filesystem::exists(testPath));
    
    // Verify saved file can be loaded
    Config newConfig;
    EXPECT_TRUE(newConfig.loadGamepadBindings(testPath));
}

TEST_F(ConfigTest, GamepadValidation) {
    Config config;
    Gamepad gamepad;
    gamepad.initialize();
    
    // Test with valid gamepad
    if (gamepad.isConnected()) {
        EXPECT_TRUE(config.loadGamepadBindings("tests/fixtures/valid_gamepad.json"));
        
        // Verify button support
        EXPECT_TRUE(gamepad.isButtonSupported(SDL_CONTROLLER_BUTTON_A));
        EXPECT_TRUE(gamepad.isAxisSupported(SDL_CONTROLLER_AXIS_LEFTX));
    }
}

} // namespace sfe 