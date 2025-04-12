#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SFE {

// Defines several possible camera movement options
enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

class Camera {
public:
    // Camera attributes
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    
    // Euler angles
    float yaw;
    float pitch;
    
    // Camera options
    float movementSpeed;
    float mouseSensitivity;
    float zoom;

    // Constructor with vectors
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f)
    );
    
    // Constructor with scalar values
    Camera(
        float posX, float posY, float posZ,
        float upX, float upY, float upZ,
        float yaw, float pitch
    );

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix() const;

    // Returns the projection matrix
    glm::mat4 getProjectionMatrix(float aspectRatio, float fov = 45.0f, float near = 0.1f, float far = 100.0f) const;

    // Processes input received from keyboard
    void processKeyboard(CameraMovement direction, float deltaTime);

    // Processes input received from a mouse input system
    void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

    // Processes input received from a mouse scroll-wheel event
    void processMouseScroll(float yOffset);

    // Simple setters/getters for basic control
    void setPosition(const glm::vec3& pos) { position = pos; }
    glm::vec3 getPosition() const { return position; }

private:
    // Calculates the front vector from the Camera's Euler Angles
    void updateCameraVectors();
};

} 