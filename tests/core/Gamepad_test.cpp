#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "core/Gamepad.hpp"
#include "core/config.h"
#include "utils/log.h"
#include <SDL2/SDL.h>

namespace sfe {

// Mock SDL for testing without hardware
struct SDLMock {
    static bool initSuccess;
    static bool controllerOpen;
    static bool buttonState;
    static float axisValue;
    static const char* controllerName;

    static int SDL_Init(uint32_t) { return initSuccess ? 0 : -1; }
    static SDL_GameController* SDL_GameControllerOpen(int) {
        return controllerOpen ? reinterpret_cast<SDL_GameController*>(1) : nullptr;
    }
    static bool SDL_GameControllerGetButton(SDL_GameController*, SDL_GameControllerButton) {
        return buttonState;
    }
    static Sint16 SDL_GameControllerGetAxis(SDL_GameController*, SDL_GameControllerAxis) {
        return static_cast<Sint16>(axisValue * 32767.0f);
    }
    static const char* SDL_GameControllerName(SDL_GameController*) {
        return controllerName;
    }
    static void SDL_GameControllerClose(SDL_GameController*) {}
    static void SDL_Quit() {}
};

bool SDLMock::initSuccess = true;
bool SDLMock::controllerOpen = true;
bool SDLMock::buttonState = false;
float SDLMock::axisValue = 0.0f;
const char* SDLMock::controllerName = "Test Controller";

// Override SDL functions for testing
#define SDL_Init SDLMock::SDL_Init
#define SDL_GameControllerOpen SDLMock::SDL_GameControllerOpen
#define SDL_GameControllerGetButton SDLMock::SDL_GameControllerGetButton
#define SDL_GameControllerGetAxis SDLMock::SDL_GameControllerGetAxis
#define SDL_GameControllerName SDLMock::SDL_GameControllerName
#define SDL_GameControllerClose SDLMock::SDL_GameControllerClose
#define SDL_Quit SDLMock::SDL_Quit

TEST_CASE("Gamepad initialization and connection", "[Gamepad]") {
    SECTION("Successful initialization") {
        SDLMock::initSuccess = true;
        SDLMock::controllerOpen = true;
        Gamepad gamepad;
        REQUIRE(gamepad.initialize());
        REQUIRE(gamepad.isConnected());
    }

    SECTION("SDL initialization failure") {
        SDLMock::initSuccess = false;
        Gamepad gamepad;
        REQUIRE_FALSE(gamepad.initialize());
        REQUIRE_FALSE(gamepad.isConnected());
    }

    SECTION("No controller available") {
        SDLMock::initSuccess = true;
        SDLMock::controllerOpen = false;
        Gamepad gamepad;
        REQUIRE(gamepad.initialize());
        REQUIRE_FALSE(gamepad.isConnected());
    }
}

TEST_CASE("Gamepad button and axis support", "[Gamepad]") {
    Gamepad gamepad;
    REQUIRE(gamepad.initialize());

    SECTION("Button support validation") {
        REQUIRE(gamepad.isButtonSupported(SDL_CONTROLLER_BUTTON_A));
        REQUIRE(gamepad.isButtonSupported(SDL_CONTROLLER_BUTTON_B));
        REQUIRE_FALSE(gamepad.isButtonSupported(static_cast<SDL_GameControllerButton>(-1)));
        REQUIRE_FALSE(gamepad.isButtonSupported(static_cast<SDL_GameControllerButton>(SDL_CONTROLLER_BUTTON_MAX)));
    }

    SECTION("Axis support validation") {
        REQUIRE(gamepad.isAxisSupported(SDL_CONTROLLER_AXIS_LEFTX));
        REQUIRE(gamepad.isAxisSupported(SDL_CONTROLLER_AXIS_LEFTY));
        REQUIRE_FALSE(gamepad.isAxisSupported(static_cast<SDL_GameControllerAxis>(-1)));
        REQUIRE_FALSE(gamepad.isAxisSupported(static_cast<SDL_GameControllerAxis>(SDL_CONTROLLER_AXIS_MAX)));
    }
}

TEST_CASE("Gamepad state and input", "[Gamepad]") {
    Gamepad gamepad;
    REQUIRE(gamepad.initialize());

    SECTION("Button state") {
        SDLMock::buttonState = true;
        REQUIRE(gamepad.pollEvents());
        REQUIRE(gamepad.getButtonState(SDL_CONTROLLER_BUTTON_A));

        SDLMock::buttonState = false;
        REQUIRE(gamepad.pollEvents());
        REQUIRE_FALSE(gamepad.getButtonState(SDL_CONTROLLER_BUTTON_A));
    }

    SECTION("Axis value") {
        SDLMock::axisValue = 0.5f;
        REQUIRE(gamepad.pollEvents());
        REQUIRE_THAT(gamepad.getAxisValue(SDL_CONTROLLER_AXIS_LEFTX), Catch::Matchers::WithinRel(0.5f));

        SDLMock::axisValue = -0.5f;
        REQUIRE(gamepad.pollEvents());
        REQUIRE_THAT(gamepad.getAxisValue(SDL_CONTROLLER_AXIS_LEFTX), Catch::Matchers::WithinRel(-0.5f));
    }
}

TEST_CASE("Gamepad configuration integration", "[Gamepad]") {
    Gamepad gamepad;
    REQUIRE(gamepad.initialize());

    SECTION("Mapped actions") {
        Config config;
        REQUIRE(config.loadGamepadBindings("tests/fixtures/valid_gamepad.json"));
        
        SDLMock::buttonState = true;
        REQUIRE(gamepad.pollEvents());
        REQUIRE(gamepad.getMappedAction(SDL_CONTROLLER_BUTTON_A, config) == "jump");
        REQUIRE(gamepad.getMappedAction(SDL_CONTROLLER_BUTTON_B, config) == "attack");
    }

    SECTION("Invalid configuration") {
        Config config;
        REQUIRE_FALSE(config.loadGamepadBindings("tests/fixtures/invalid_gamepad.json"));
    }
}

TEST_CASE("Gamepad name and identification", "[Gamepad]") {
    Gamepad gamepad;
    REQUIRE(gamepad.initialize());

    SECTION("Connected gamepad name") {
        SDLMock::controllerName = "Xbox Controller";
        REQUIRE(gamepad.getName() == "Xbox Controller");
    }

    SECTION("No gamepad name") {
        SDLMock::controllerOpen = false;
        REQUIRE(gamepad.getName() == "No Gamepad");
    }
}

// Restore SDL functions
#undef SDL_Init
#undef SDL_GameControllerOpen
#undef SDL_GameControllerGetButton
#undef SDL_GameControllerGetAxis
#undef SDL_GameControllerName
#undef SDL_GameControllerClose
#undef SDL_Quit

} // namespace sfe 