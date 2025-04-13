#include "core/SceneNode.hpp"
#include <iostream>

namespace SFE {

SceneNode::SceneNode(Rendering::Mesh* mesh) : mesh(mesh) {
    // Calculate orbit radius if this is not the center node
    if (position.x != 0.0f || position.y != 0.0f || position.z != 0.0f) {
        orbitRadius = glm::length(position);
    }
}

void SceneNode::update(float deltaTime, float currentTime) {
    // For center cube: rotate around multiple axes
    if (orbitRadius < 0.1f) {
        // Use deltaTime to ensure smooth rotation regardless of frame rate
        rotation.x = 30.0f * currentTime;
        rotation.y = 45.0f * currentTime;
    } 
    // For orbiting cubes: orbit around center and rotate on Y axis
    else {
        // Use deltaTime to ensure smooth orbit regardless of frame rate
        float orbitAngle = currentTime * orbitSpeed;
        position.x = orbitRadius * cos(glm::radians(orbitAngle));
        position.z = orbitRadius * sin(glm::radians(orbitAngle));
        rotation.y = orbitAngle;
    }
}

void SceneNode::draw(Rendering::Shader& shader, const Camera& camera) const {
    if (!mesh) return;
    
    if (texture) {
        mesh->setTexture(texture);
    }
    
    // Calculate model matrix based on position, rotation, scale
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    
    // Apply rotations
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    
    // Apply scale
    model = glm::scale(model, scale);
    
    // Set uniforms and draw
    shader.setMat4("model", model);
    shader.setMat4("view", camera.getViewMatrix());
    shader.setMat4("projection", camera.getProjectionMatrix(800.0f / 600.0f));
    
    mesh->draw();
}

} 