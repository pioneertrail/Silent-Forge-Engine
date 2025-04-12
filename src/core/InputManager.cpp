#include "core/InputManager.hpp"
#include "core/Camera.hpp"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace SFE {

void InputManager::processInput(GLFWwindow* window, Camera& camera, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    glm::vec3 pos = camera.getPosition();

    // Basic WASD movement relative to world axes
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        pos.z -= velocity; // Move forward along world Z-
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        pos.z += velocity; // Move backward along world Z+
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        pos.x -= velocity; // Move left along world X-
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        pos.x += velocity; // Move right along world X+

    // Basic Up/Down movement relative to world axes
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        pos.y += velocity; // Move up along world Y+
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        pos.y -= velocity; // Move down along world Y-

    camera.setPosition(pos);

    // Close window on Escape
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

} 