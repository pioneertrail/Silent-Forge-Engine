#include "core/Camera.hpp"

namespace SFE {

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up)
    : position(position), target(target), up(up) {}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio, float fov, float near, float far) const {
    return glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

} 