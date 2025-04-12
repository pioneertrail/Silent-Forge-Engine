#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <functional>
#include "core/Camera.hpp"

namespace SFE {

class InputManager {
public:
    InputManager() = default;

    void processInput(GLFWwindow* window, Camera& camera, float deltaTime);

private:
    float movementSpeed = 2.5f;
};

} 