#pragma once

#include <SDL2/SDL_gamecontroller.h>
#include <string>
#include <vector>

namespace sfe {

class Config; // Forward declaration

class Gamepad {
public:
    Gamepad();
    ~Gamepad();

    bool initialize();
    void shutdown();
    bool pollEvents();
    bool isConnected() const;
    std::string getName() const;
    bool isButtonSupported(SDL_GameControllerButton button) const;
    bool isAxisSupported(SDL_GameControllerAxis axis) const;
    bool getButtonState(SDL_GameControllerButton button) const;
    float getAxisValue(SDL_GameControllerAxis axis) const;
    std::string getMappedAction(SDL_GameControllerButton button, const Config& config) const;

private:
    SDL_GameController* controller_ = nullptr;
    std::vector<bool> buttonStates_;
    std::vector<float> axisValues_;
    bool initialized_ = false;
};

} // namespace sfe 