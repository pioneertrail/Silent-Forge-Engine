#pragma once

#include <SDL2/SDL_gamecontroller.h>
#include <string>
#include <map>
#include <utility>

namespace sfe {

class Gamepad; // Forward declaration

/**
 * @brief Configuration class for managing game settings and bindings
 * 
 * This class handles loading and saving configuration files,
 * managing gamepad bindings, and providing access to settings.
 */
class Config {
public:
    Config(Gamepad* gamepad = nullptr);
    ~Config() = default;

    /**
     * @brief Load gamepad bindings from a JSON file
     * @param filename Path to the configuration file
     * @return true if loading was successful
     */
    bool loadGamepadBindings(const std::string& filename);

    /**
     * @brief Save gamepad bindings to a JSON file
     * @param filename Path to save the configuration file
     * @return true if saving was successful
     */
    bool saveGamepadBindings(const std::string& filename) const;

    /**
     * @brief Get the action mapped to a button
     * @param button The button to check
     * @return The mapped action or empty string if no mapping
     */
    std::string getButtonAction(SDL_GameControllerButton button) const;

    /**
     * @brief Get the range for an axis
     * @param axis The axis to check
     * @return Pair of min and max values for the axis
     */
    std::pair<float, float> getAxisRange(SDL_GameControllerAxis axis) const;

private:
    Gamepad* gamepad_;
    std::map<SDL_GameControllerButton, std::string> buttonActions_;
    std::map<SDL_GameControllerAxis, std::pair<float, float>> axisRanges_;
};

} // namespace sfe 