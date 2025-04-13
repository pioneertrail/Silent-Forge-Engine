#pragma once

#include <GLFW/glfw3.h>
#include <string>

class Gamepad {
public:
    Gamepad(int id);
    ~Gamepad();

    bool isConnected() const;
    std::string getName() const;
    void update();
    float getAxis(int axis) const;
    bool isButtonPressed(int button) const;

private:
    int id;
    GLFWgamepadstate state;
    bool connected;
}; 