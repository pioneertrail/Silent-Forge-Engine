#include "core/Gamepad.hpp"
#include "core/config.h"
#include "utils/log.h"
#include <SDL2/SDL.h>

namespace sfe {

Gamepad::Gamepad() {
    buttonStates_.resize(SDL_CONTROLLER_BUTTON_MAX);
    axisValues_.resize(SDL_CONTROLLER_AXIS_MAX);
}

Gamepad::~Gamepad() {
    shutdown();
}

bool Gamepad::initialize() {
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0) {
        LOG_ERROR("Failed to initialize SDL gamepad: " + std::string(SDL_GetError()));
        return false;
    }

    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            controller_ = SDL_GameControllerOpen(i);
            if (controller_) {
                LOG_INFO("Gamepad connected: " + getName());
                initialized_ = true;
                return true;
            }
        }
    }

    LOG_WARNING("No gamepad found");
    return false;
}

void Gamepad::shutdown() {
    if (controller_) {
        SDL_GameControllerClose(controller_);
        controller_ = nullptr;
    }
    if (initialized_) {
        SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
        initialized_ = false;
    }
}

bool Gamepad::pollEvents() {
    if (!initialized_ || !controller_) {
        return false;
    }

    SDL_GameControllerUpdate();
    for (int button = 0; button < SDL_CONTROLLER_BUTTON_MAX; ++button) {
        buttonStates_[button] = SDL_GameControllerGetButton(
            controller_, static_cast<SDL_GameControllerButton>(button)) != 0;
    }

    for (int axis = 0; axis < SDL_CONTROLLER_AXIS_MAX; ++axis) {
        Sint16 rawValue = SDL_GameControllerGetAxis(
            controller_, static_cast<SDL_GameControllerAxis>(axis));
        axisValues_[axis] = static_cast<float>(rawValue) / 32767.0f;
    }

    return true;
}

bool Gamepad::isConnected() const {
    return controller_ != nullptr;
}

std::string Gamepad::getName() const {
    if (!controller_) {
        return "No Gamepad";
    }
    const char* name = SDL_GameControllerName(controller_);
    return name ? std::string(name) : "Unknown Gamepad";
}

bool Gamepad::isButtonSupported(SDL_GameControllerButton button) const {
    return controller_ && button >= 0 && button < SDL_CONTROLLER_BUTTON_MAX;
}

bool Gamepad::isAxisSupported(SDL_GameControllerAxis axis) const {
    return controller_ && axis >= 0 && axis < SDL_CONTROLLER_AXIS_MAX;
}

bool Gamepad::getButtonState(SDL_GameControllerButton button) const {
    if (!isButtonSupported(button)) {
        return false;
    }
    return buttonStates_[button];
}

float Gamepad::getAxisValue(SDL_GameControllerAxis axis) const {
    if (!isAxisSupported(axis)) {
        return 0.0f;
    }
    return axisValues_[axis];
}

std::string Gamepad::getMappedAction(SDL_GameControllerButton button, const Config& config) const {
    if (!getButtonState(button)) {
        return "";
    }
    return config.getButtonAction(button);
}

} // namespace sfe 