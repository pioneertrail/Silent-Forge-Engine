#pragma once

#include <string>
#include <map>
#include <SDL2/SDL_gamecontroller.h>

namespace sfe {

class Gamepad; // Forward declaration

/// @brief Manages gamepad configuration and bindings
/// @details Handles loading and saving gamepad bindings from JSON files,
///          supporting cross-platform compatibility (Windows XInput, Linux SDL2, macOS MFi)
class Config {
public:
    /// @brief Constructs a Config instance with optional Gamepad reference
    /// @param gamepad Optional reference to Gamepad for validation
    explicit Config(Gamepad* gamepad = nullptr) : gamepad_(gamepad) {}

    /// @brief Loads gamepad bindings from a JSON file
    /// @param path File path (e.g., "resources/config/gamepad.json")
    /// @return True on success, false on error
    bool loadGamepadBindings(const std::string& path) const;

    /// @brief Saves current gamepad bindings to a JSON file
    /// @param path File path
    /// @return True on success, false on error
    bool saveGamepadBindings(const std::string& path) const;

    /// @brief Gets the action mapped to a gamepad button
    /// @param button SDL_GameControllerButton enum
    /// @return Action string (e.g., "jump")
    std::string getButtonAction(SDL_GameControllerButton button) const;

    /// @brief Gets the axis range for a gamepad axis
    /// @param axis SDL_GameControllerAxis enum
    /// @return Pair of min/max values
    std::pair<float, float> getAxisRange(SDL_GameControllerAxis axis) const;

    /// @brief Sets the Gamepad reference for validation
    /// @param gamepad Pointer to Gamepad instance
    void setGamepad(Gamepad* gamepad) { gamepad_ = gamepad; }

private:
    Gamepad* gamepad_ = nullptr;
    mutable std::map<SDL_GameControllerButton, std::string> buttonMap_;
    mutable std::map<SDL_GameControllerAxis, std::pair<float, float>> axisMap_;
};

} // namespace sfe 