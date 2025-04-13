#include <catch2/catch_test_macros.hpp>
#include "core/Gamepad.hpp"
#include "core/config.h"
#include <SDL2/SDL.h>

// Mock SDL for testing
struct SDLMock {
    static bool initialized;
    static bool controllerOpen;
    static std::vector<bool> buttons;
    static std::vector<Sint16> axes;
    static std::string controllerName;

    static int SDL_InitSubSystem(uint32_t) {
        initialized = true;
        return 0;
    }
    static void SDL_QuitSubSystem(uint32_t) { initialized = false; }
    static int SDL_NumJoysticks() { return 1; }
    static SDL_bool SDL_IsGameController(int) { return SDL_TRUE; }
    static SDL_GameController* SDL_GameControllerOpen(int) {
        controllerOpen = true;
        return reinterpret_cast<SDL_GameController*>(1);
    }
    static void SDL_GameControllerClose(SDL_GameController*) { controllerOpen = false; }
    static void SDL_GameControllerUpdate() {}
    static int SDL_GameControllerGetButton(SDL_GameController*, SDL_GameControllerButton b) {
        return buttons[b] ? 1 : 0;
    }
    static Sint16 SDL_GameControllerGetAxis(SDL_GameController*, SDL_GameControllerAxis a) {
        return axes[a];
    }
    static const char* SDL_GameControllerName(SDL_GameController*) { return controllerName.c_str(); }
    static SDL_bool SDL_GameControllerHasButton(SDL_GameController*, SDL_GameControllerButton b) {
        return b < SDL_CONTROLLER_BUTTON_MAX ? SDL_TRUE : SDL_FALSE;
    }
    static SDL_bool SDL_GameControllerHasAxis(SDL_GameController*, SDL_GameControllerAxis a) {
        return a < SDL_CONTROLLER_AXIS_MAX ? SDL_TRUE : SDL_FALSE;
    }
};

bool SDLMock::initialized = false;
bool SDLMock::controllerOpen = false;
std::vector<bool> SDLMock::buttons(SDL_CONTROLLER_BUTTON_MAX, false);
std::vector<Sint16> SDLMock::axes(SDL_CONTROLLER_AXIS_MAX, 0);
std::string SDLMock::controllerName = "TestPad";

// Override SDL functions with mocks
#define SDL_InitSubSystem SDLMock::SDL_InitSubSystem
#define SDL_QuitSubSystem SDLMock::SDL_QuitSubSystem
#define SDL_NumJoysticks SDLMock::SDL_NumJoysticks
#define SDL_IsGameController SDLMock::SDL_IsGameController
#define SDL_GameControllerOpen SDLMock::SDL_GameControllerOpen
#define SDL_GameControllerClose SDLMock::SDL_GameControllerClose
#define SDL_GameControllerUpdate SDLMock::SDL_GameControllerUpdate
#define SDL_GameControllerGetButton SDLMock::SDL_GameControllerGetButton
#define SDL_GameControllerGetAxis SDLMock::SDL_GameControllerGetAxis
#define SDL_GameControllerName SDLMock::SDL_GameControllerName
#define SDL_GameControllerHasButton SDLMock::SDL_GameControllerHasButton
#define SDL_GameControllerHasAxis SDLMock::SDL_GameControllerHasAxis

TEST_CASE("Gamepad initialization and shutdown", "[Gamepad]") {
    SECTION("Initialize and shutdown") {
        sfe::Gamepad gamepad;
        REQUIRE(gamepad.initialize());
        REQUIRE(gamepad.isConnected());
        REQUIRE(gamepad.getName() == "TestPad");
        gamepad.shutdown();
        REQUIRE_FALSE(gamepad.isConnected());
    }

    SECTION("Initialize failure") {
        SDLMock::initialized = false;
        sfe::Gamepad gamepad;
        REQUIRE_FALSE(gamepad.initialize());
        REQUIRE_FALSE(gamepad.isConnected());
    }
}

TEST_CASE("Gamepad button states", "[Gamepad]") {
    sfe::Gamepad gamepad;
    REQUIRE(gamepad.initialize());

    SECTION("Button support validation") {
        REQUIRE(gamepad.isButtonSupported(SDL_CONTROLLER_BUTTON_A));
        REQUIRE_FALSE(gamepad.isButtonSupported(static_cast<SDL_GameControllerButton>(SDL_CONTROLLER_BUTTON_MAX)));
    }

    SECTION("Button state polling") {
        SDLMock::buttons[SDL_CONTROLLER_BUTTON_A] = true;
        REQUIRE(gamepad.pollEvents());
        REQUIRE(gamepad.getButtonState(SDL_CONTROLLER_BUTTON_A));
        REQUIRE_FALSE(gamepad.getButtonState(SDL_CONTROLLER_BUTTON_B));
    }

    SECTION("Button state without polling") {
        SDLMock::buttons[SDL_CONTROLLER_BUTTON_A] = true;
        REQUIRE_FALSE(gamepad.getButtonState(SDL_CONTROLLER_BUTTON_A));
    }
}

TEST_CASE("Gamepad axis states", "[Gamepad]") {
    sfe::Gamepad gamepad;
    REQUIRE(gamepad.initialize());

    SECTION("Axis support validation") {
        REQUIRE(gamepad.isAxisSupported(SDL_CONTROLLER_AXIS_LEFTX));
        REQUIRE_FALSE(gamepad.isAxisSupported(static_cast<SDL_GameControllerAxis>(SDL_CONTROLLER_AXIS_MAX)));
    }

    SECTION("Axis value polling") {
        SDLMock::axes[SDL_CONTROLLER_AXIS_LEFTX] = 16384; // ~0.5
        REQUIRE(gamepad.pollEvents());
        REQUIRE(gamepad.getAxisValue(SDL_CONTROLLER_AXIS_LEFTX) == Approx(0.5f));
        REQUIRE(gamepad.getAxisValue(SDL_CONTROLLER_AXIS_LEFTY) == 0.0f);
    }

    SECTION("Axis value without polling") {
        SDLMock::axes[SDL_CONTROLLER_AXIS_LEFTX] = 16384;
        REQUIRE(gamepad.getAxisValue(SDL_CONTROLLER_AXIS_LEFTX) == 0.0f);
    }
}

TEST_CASE("Gamepad configuration integration", "[Gamepad]") {
    sfe::Gamepad gamepad;
    sfe::Config config;
    REQUIRE(gamepad.initialize());

    SECTION("Button action mapping") {
        SDLMock::buttons[SDL_CONTROLLER_BUTTON_A] = true;
        REQUIRE(gamepad.pollEvents());
        REQUIRE(gamepad.getMappedAction(SDL_CONTROLLER_BUTTON_A, config).empty());
    }

    SECTION("Unsupported button mapping") {
        REQUIRE(gamepad.getMappedAction(static_cast<SDL_GameControllerButton>(SDL_CONTROLLER_BUTTON_MAX), config).empty());
    }
}

// Restore original SDL functions
#undef SDL_InitSubSystem
#undef SDL_QuitSubSystem
#undef SDL_NumJoysticks
#undef SDL_IsGameController
#undef SDL_GameControllerOpen
#undef SDL_GameControllerClose
#undef SDL_GameControllerUpdate
#undef SDL_GameControllerGetButton
#undef SDL_GameControllerGetAxis
#undef SDL_GameControllerName
#undef SDL_GameControllerHasButton
#undef SDL_GameControllerHasAxis 