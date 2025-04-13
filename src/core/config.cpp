#include "config.h"
#include "utils/log.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>

namespace sfe {

bool Config::loadGamepadBindings(const std::string& path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        Log::error("Failed to open gamepad config: " + path);
        return false;
    }

    try {
        nlohmann::json j;
        file >> j;

        // Create temporary maps for validation
        std::map<SDL_GameControllerButton, std::string> newButtonMap;
        std::map<SDL_GameControllerAxis, std::pair<float, float>> newAxisMap;

        // Parse buttons
        if (j.contains("buttons")) {
            for (const auto& btn : j["buttons"].items()) {
                try {
                    int key = std::stoi(btn.key());
                    if (key < 0 || key >= SDL_CONTROLLER_BUTTON_MAX) {
                        Log::error("Invalid button key: " + btn.key());
                        return false;
                    }
                    auto sdlBtn = static_cast<SDL_GameControllerButton>(key);
                    newButtonMap[sdlBtn] = btn.value();
                } catch (const std::invalid_argument& e) {
                    Log::error("Invalid button key format: " + btn.key());
                    return false;
                }
            }
        }

        // Parse axes
        if (j.contains("axes")) {
            for (const auto& axis : j["axes"].items()) {
                try {
                    int key = std::stoi(axis.key());
                    if (key < 0 || key >= SDL_CONTROLLER_AXIS_MAX) {
                        Log::error("Invalid axis key: " + axis.key());
                        return false;
                    }
                    auto sdlAxis = static_cast<SDL_GameControllerAxis>(key);
                    newAxisMap[sdlAxis] = {axis.value()["min"], axis.value()["max"]};
                } catch (const std::invalid_argument& e) {
                    Log::error("Invalid axis key format: " + axis.key());
                    return false;
                }
            }
        }

        // macOS MFi validation
        #ifdef __APPLE__
        if (!j.contains("mfi_compliant")) {
            Log::warn("Config lacks MFi compliance flag");
        }
        #endif

        // TODO: Validate against Gamepad if available (future PR)
        if (gamepad_ && !newButtonMap.empty()) {
            // Future: Add gamepad_->validateButtons(newButtonMap) and similar for axes
        }

        // Update maps only on success
        buttonMap_ = std::move(newButtonMap);
        axisMap_ = std::move(newAxisMap);

        Log::info("Loaded gamepad bindings from " + path + 
                 " (buttons: " + std::to_string(buttonMap_.size()) + 
                 ", axes: " + std::to_string(axisMap_.size()) + ")");
        return true;
    } catch (const nlohmann::json::exception& e) {
        Log::error("JSON parse error: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveGamepadBindings(const std::string& path) const {
    nlohmann::json j;
    try {
        // Serialize buttons
        for (const auto& [btn, action] : buttonMap_) {
            j["buttons"][std::to_string(static_cast<int>(btn))] = action;
        }

        // Serialize axes
        for (const auto& [axis, range] : axisMap_) {
            j["axes"][std::to_string(static_cast<int>(axis))]["min"] = range.first;
            j["axes"][std::to_string(static_cast<int>(axis))]["max"] = range.second;
        }

        // Add macOS MFi compliance flag
        #ifdef __APPLE__
        j["mfi_compliant"] = true;
        #endif

        // Create directory if it doesn't exist
        std::filesystem::path configPath(path);
        std::filesystem::create_directories(configPath.parent_path());

        // Write to file
        std::ofstream file(path);
        if (!file.is_open()) {
            Log::error("Failed to open gamepad config for writing: " + path);
            return false;
        }
        file << j.dump(4); // Pretty-print with 4-space indent
        Log::info("Saved gamepad bindings to " + path + 
                 " (buttons: " + std::to_string(buttonMap_.size()) + 
                 ", axes: " + std::to_string(axisMap_.size()) + ")");
        return true;
    } catch (const nlohmann::json::exception& e) {
        Log::error("JSON serialization error: " + std::string(e.what()));
        return false;
    }
}

std::string Config::getButtonAction(SDL_GameControllerButton button) const {
    auto it = buttonMap_.find(button);
    return it != buttonMap_.end() ? it->second : "";
}

std::pair<float, float> Config::getAxisRange(SDL_GameControllerAxis axis) const {
    auto it = axisMap_.find(axis);
    return it != axisMap_.end() ? it->second : std::make_pair(-1.0f, 1.0f);
}

} // namespace sfe 