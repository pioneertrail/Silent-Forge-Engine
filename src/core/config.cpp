#include "core/config.h"
#include "core/Gamepad.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>

namespace sfe {

Config::Config(Gamepad* gamepad) : gamepad_(gamepad) {
    // Initialize default axis ranges
    for (int axis = 0; axis < SDL_CONTROLLER_AXIS_MAX; ++axis) {
        axisRanges_[static_cast<SDL_GameControllerAxis>(axis)] = std::make_pair(-1.0f, 1.0f);
    }
}

bool Config::loadGamepadBindings(const std::string& filename) {
    if (!std::filesystem::exists(filename)) {
        return false;
    }

    try {
        std::ifstream file(filename);
        nlohmann::json json;
        file >> json;

        // Load button actions
        if (json.contains("buttons")) {
            for (const auto& [key, value] : json["buttons"].items()) {
                int button = std::stoi(key);
                if (button >= 0 && button < SDL_CONTROLLER_BUTTON_MAX) {
                    SDL_GameControllerButton sdlButton = static_cast<SDL_GameControllerButton>(button);
                    if (!gamepad_ || gamepad_->isButtonSupported(sdlButton)) {
                        buttonActions_[sdlButton] = value.get<std::string>();
                    }
                }
            }
        }

        // Load axis ranges
        if (json.contains("axes")) {
            for (const auto& [key, value] : json["axes"].items()) {
                int axis = std::stoi(key);
                if (axis >= 0 && axis < SDL_CONTROLLER_AXIS_MAX) {
                    SDL_GameControllerAxis sdlAxis = static_cast<SDL_GameControllerAxis>(axis);
                    if (!gamepad_ || gamepad_->isAxisSupported(sdlAxis)) {
                        float min = value["min"].get<float>();
                        float max = value["max"].get<float>();
                        if (min <= max) {
                            axisRanges_[sdlAxis] = std::make_pair(min, max);
                        }
                    }
                }
            }
        }

        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveGamepadBindings(const std::string& filename) const {
    try {
        nlohmann::json json;

        // Save button actions
        for (const auto& [button, action] : buttonActions_) {
            json["buttons"][std::to_string(button)] = action;
        }

        // Save axis ranges
        for (const auto& [axis, range] : axisRanges_) {
            json["axes"][std::to_string(axis)] = {
                {"min", range.first},
                {"max", range.second}
            };
        }

        std::ofstream file(filename);
        file << json.dump(4);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string Config::getButtonAction(SDL_GameControllerButton button) const {
    auto it = buttonActions_.find(button);
    return it != buttonActions_.end() ? it->second : "";
}

std::pair<float, float> Config::getAxisRange(SDL_GameControllerAxis axis) const {
    auto it = axisRanges_.find(axis);
    return it != axisRanges_.end() ? it->second : std::make_pair(-1.0f, 1.0f);
}

} // namespace sfe 