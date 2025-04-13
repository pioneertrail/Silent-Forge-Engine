#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include <vector>

namespace sfe {

/**
 * @class Gamepad
 * @brief Handles gamepad input with performance-optimized polling
 * 
 * This class provides low-level gamepad input handling with a focus on performance.
 * It integrates with the Config system for button mapping and supports multiple controllers.
 */
class Gamepad {
public:
    /**
     * @brief Construct a new Gamepad object
     * 
     * Initializes SDL gamepad subsystem if not already initialized.
     */
    Gamepad();

    /**
     * @brief Destroy the Gamepad object
     * 
     * Cleans up SDL gamepad resources.
     */
    ~Gamepad();

    /**
     * @brief Poll all connected gamepads
     * 
     * Updates the state of all connected gamepads with a target performance
     * of less than 0.1ms per call.
     * 
     * @return true if polling was successful
     * @return false if an error occurred
     */
    bool pollEvents();

    /**
     * @brief Get the number of connected gamepads
     * 
     * @return int Number of connected gamepads
     */
    int getConnectedCount() const;

    /**
     * @brief Check if a specific gamepad is connected
     * 
     * @param index Gamepad index to check
     * @return true if the gamepad is connected
     * @return false if the gamepad is not connected
     */
    bool isConnected(int index) const;

    /**
     * @brief Check if a button is supported by the gamepad
     * 
     * @param button Button to check
     * @return true if the button is supported
     * @return false if the button is not supported
     */
    bool isButtonSupported(SDL_GameControllerButton button) const;

    /**
     * @brief Check if an axis is supported by the gamepad
     * 
     * @param axis Axis to check
     * @return true if the axis is supported
     * @return false if the axis is not supported
     */
    bool isAxisSupported(SDL_GameControllerAxis axis) const;

    /**
     * @brief Get the state of a gamepad button
     * 
     * @param index Gamepad index
     * @param button Button to check (SDL_GameControllerButton)
     * @return true if the button is pressed
     * @return false if the button is released
     */
    bool getButtonState(int index, SDL_GameControllerButton button) const;

    /**
     * @brief Get the value of a gamepad axis
     * 
     * @param index Gamepad index
     * @param axis Axis to check (SDL_GameControllerAxis)
     * @return float Axis value in range [-1.0, 1.0]
     */
    float getAxisValue(int index, SDL_GameControllerAxis axis) const;

private:
    struct GamepadState {
        SDL_GameController* controller;
        std::vector<bool> buttonStates;
        std::vector<float> axisValues;
    };

    std::vector<GamepadState> m_gamepads;
    bool m_initialized;
}; 