#pragma once

#include <SDL2/SDL_gamecontroller.h>
#include <string>
#include <vector>

namespace sfe {

class Config; // Forward declaration

/**
 * @brief Gamepad class for handling game controller input
 * 
 * This class provides a high-level interface for gamepad input handling,
 * supporting button mapping and axis configuration through the Config class.
 * It ensures cross-platform compatibility using SDL2's game controller API.
 */
class Gamepad {
public:
    Gamepad();
    ~Gamepad();

    /**
     * @brief Initialize the gamepad system
     * @return true if initialization was successful
     */
    bool initialize();

    /**
     * @brief Shutdown the gamepad system
     */
    void shutdown();

    /**
     * @brief Poll for gamepad events and update state
     * @return true if events were processed successfully
     */
    bool pollEvents();

    /**
     * @brief Check if a gamepad is connected
     * @return true if a gamepad is connected
     */
    bool isConnected() const;

    /**
     * @brief Get the name of the connected gamepad
     * @return The gamepad name or "No Gamepad" if none connected
     */
    std::string getName() const;

    /**
     * @brief Check if a button is supported by the gamepad
     * @param button The button to check
     * @return true if the button is supported
     */
    bool isButtonSupported(SDL_GameControllerButton button) const;

    /**
     * @brief Check if an axis is supported by the gamepad
     * @param axis The axis to check
     * @return true if the axis is supported
     */
    bool isAxisSupported(SDL_GameControllerAxis axis) const;

    /**
     * @brief Get the current state of a button
     * @param button The button to check
     * @return true if the button is pressed
     */
    bool getButtonState(SDL_GameControllerButton button) const;

    /**
     * @brief Get the current value of an axis
     * @param axis The axis to check
     * @return The normalized axis value (-1.0 to 1.0)
     */
    float getAxisValue(SDL_GameControllerAxis axis) const;

    /**
     * @brief Get the mapped action for a button press
     * @param button The button to check
     * @param config The configuration to use for mapping
     * @return The mapped action or empty string if no mapping
     */
    std::string getMappedAction(SDL_GameControllerButton button, const Config& config) const;

private:
    SDL_GameController* controller_ = nullptr;
    std::vector<bool> buttonStates_;
    std::vector<float> axisValues_;
    bool initialized_ = false;
};

} // namespace sfe 