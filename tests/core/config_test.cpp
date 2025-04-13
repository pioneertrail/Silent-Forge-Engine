#include <catch2/catch_test_macros.hpp>
#include "core/config.h"
#include <filesystem>
#include <fstream>

TEST_CASE("Config loads and saves gamepad bindings", "[config]") {
    const std::string testDir = "tests/fixtures";
    const std::string testFile = "test_gamepad.json";
    const std::string validConfigPath = testDir + "/valid_gamepad.json";
    const std::string malformedConfigPath = testDir + "/malformed_gamepad.json";
    const std::string savePath = testDir + "/" + testFile;

    // Setup cleanup lambda
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

    // Initial cleanup
    cleanup();
    std::filesystem::create_directories(testDir);

    SECTION("Load valid bindings") {
        // Create a valid test config
        std::ofstream validConfig(validConfigPath);
        validConfig << R"({
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
        })";
        validConfig.close();

        sfe::Config config;
        REQUIRE(config.loadGamepadBindings(validConfigPath));
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_A) == "jump");
        REQUIRE(config.getButtonAction(SDL_CONTROLLER_BUTTON_B) == "attack");
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTX).first == -1.0f);
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_LEFTX).second == 1.0f);
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_TRIGGERLEFT).first == 0.0f);
        REQUIRE(config.getAxisRange(SDL_CONTROLLER_AXIS_TRIGGERLEFT).second == 1.0f);
    }

    SECTION("Load invalid file") {
        sfe::Config config;
        REQUIRE_FALSE(config.loadGamepadBindings("nonexistent.json"));
    }

    SECTION("Load invalid JSON") {
        std::ofstream invalidConfig(validConfigPath);
        invalidConfig << "invalid json content";
        invalidConfig.close();

        sfe::Config config;
        REQUIRE_FALSE(config.loadGamepadBindings(validConfigPath));
    }

    SECTION("Load malformed JSON fields") {
        std::ofstream malformedConfig(malformedConfigPath);
        malformedConfig << R"({
            "buttons": {"0": "jump"},
            "axes": {"0": {"min": "invalid", "max": 1.0}}
        })";
        malformedConfig.close();

        sfe::Config config;
        REQUIRE_FALSE(config.loadGamepadBindings(malformedConfigPath));
    }

    SECTION("Load invalid button/axis keys") {
        std::ofstream invalidConfig(validConfigPath);
        invalidConfig << R"({
            "buttons": {"999": "jump"},
            "axes": {"999": {"min": -1.0, "max": 1.0}}
        })";
        invalidConfig.close();

        sfe::Config config;
        REQUIRE_FALSE(config.loadGamepadBindings(validConfigPath));
    }

    SECTION("Load file with permission error") {
        std::ofstream validConfig(validConfigPath);
        validConfig << R"({"buttons": {"0": "jump"}})";
        validConfig.close();

        // Simulate read-only (platform-dependent)
        #ifdef __linux__
        std::filesystem::permissions(validConfigPath, std::filesystem::perms::none);
        sfe::Config config;
        REQUIRE_FALSE(config.loadGamepadBindings(validConfigPath));
        std::filesystem::permissions(validConfigPath, std::filesystem::perms::all);
        #endif
    }

    SECTION("Save and reload bindings") {
        // Create valid config
        std::ofstream validConfig(validConfigPath);
        validConfig << R"({
            "buttons": {"0": "jump", "1": "attack"},
            "axes": {"0": {"min": -1.0, "max": 1.0}}
        })";
        validConfig.close();

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

    // Final cleanup
    cleanup();
} 