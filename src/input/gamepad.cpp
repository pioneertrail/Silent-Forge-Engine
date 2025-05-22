#include "input/gamepad.h"
#include "utils/log.h"
#include <algorithm>
#include <stdexcept>

namespace sfe {

Gamepad::Gamepad() : m_initialized(false) {
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0) {
        LOG_ERROR("Failed to initialize SDL gamepad subsystem: " + std::string(SDL_GetError()));
        throw std::runtime_error("SDL gamepad init failed");
    }
    LOG_INFO("Initialized gamepad subsystem");
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
        LOG_INFO("Shut down gamepad subsystem");
    }
}

bool Gamepad::pollEvents() {
    if (!m_initialized) {
        LOG_ERROR("Gamepad subsystem not initialized");
        return false;
    }

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
                LOG_INFO("Connected gamepad " + std::to_string(i));
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

bool Gamepad::isButtonSupported(SDL_GameControllerButton button) const {
    return button < SDL_CONTROLLER_BUTTON_MAX;
}

bool Gamepad::isAxisSupported(SDL_GameControllerAxis axis) const {
    return axis < SDL_CONTROLLER_AXIS_MAX;
}

bool Gamepad::getButtonState(int index, SDL_GameControllerButton button) const {
    if (!isConnected(index) || !isButtonSupported(button)) {
        return false;
    }
    return m_gamepads[index].buttonStates[button];
}

float Gamepad::getAxisValue(int index, SDL_GameControllerAxis axis) const {
    if (!isConnected(index) || !isAxisSupported(axis)) {
        return 0.0f;
    }
    return m_gamepads[index].axisValues[axis];
}

} // namespace sfe 