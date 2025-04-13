#include "input/gamepad.h"
#include <algorithm>
#include <stdexcept>

namespace SilentForge {

Gamepad::Gamepad() : m_initialized(false) {
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0) {
        throw std::runtime_error("Failed to initialize SDL gamepad subsystem");
    }
    m_initialized = true;

    // Pre-allocate space for up to 4 gamepads
    m_gamepads.reserve(4);
}

Gamepad::~Gamepad() {
    for (auto& state : m_gamepads) {
        if (state.controller) {
            SDL_GameControllerClose(state.controller);
        }
    }
    if (m_initialized) {
        SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
    }
}

bool Gamepad::pollEvents() {
    if (!m_initialized) return false;

    // Update SDL's internal gamepad state
    SDL_GameControllerUpdate();

    // Process all connected gamepads
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        // Ensure we have enough space in our vector
        if (i >= static_cast<int>(m_gamepads.size())) {
            GamepadState newState;
            newState.controller = SDL_GameControllerOpen(i);
            if (newState.controller) {
                // Pre-allocate button and axis state vectors
                newState.buttonStates.resize(SDL_CONTROLLER_BUTTON_MAX);
                newState.axisValues.resize(SDL_CONTROLLER_AXIS_MAX);
                m_gamepads.push_back(std::move(newState));
            }
        }

        // Update button states
        auto& state = m_gamepads[i];
        if (state.controller) {
            // Use SIMD-optimized memory operations where possible
            std::fill(state.buttonStates.begin(), state.buttonStates.end(), false);
            for (int button = 0; button < SDL_CONTROLLER_BUTTON_MAX; ++button) {
                state.buttonStates[button] = SDL_GameControllerGetButton(
                    state.controller, static_cast<SDL_GameControllerButton>(button)) != 0;
            }

            // Update axis values with normalization
            for (int axis = 0; axis < SDL_CONTROLLER_AXIS_MAX; ++axis) {
                Sint16 rawValue = SDL_GameControllerGetAxis(
                    state.controller, static_cast<SDL_GameControllerAxis>(axis));
                state.axisValues[axis] = static_cast<float>(rawValue) / 32767.0f;
            }
        }
    }

    return true;
}

int Gamepad::getConnectedCount() const {
    return static_cast<int>(m_gamepads.size());
}

bool Gamepad::isConnected(int index) const {
    return index >= 0 && index < static_cast<int>(m_gamepads.size()) &&
           m_gamepads[index].controller != nullptr;
}

bool Gamepad::getButtonState(int index, SDL_GameControllerButton button) const {
    if (!isConnected(index) || button >= SDL_CONTROLLER_BUTTON_MAX) {
        return false;
    }
    return m_gamepads[index].buttonStates[button];
}

float Gamepad::getAxisValue(int index, SDL_GameControllerAxis axis) const {
    if (!isConnected(index) || axis >= SDL_CONTROLLER_AXIS_MAX) {
        return 0.0f;
    }
    return m_gamepads[index].axisValues[axis];
}

} // namespace SilentForge 