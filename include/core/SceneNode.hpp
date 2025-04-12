#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "rendering/Mesh.hpp"
#include "rendering/Shader.hpp"
#include "core/Camera.hpp"
#include <memory>

namespace SFE {
class SceneNode {
public:
    SceneNode(Mesh* mesh = nullptr);
    ~SceneNode() = default;

    void setPosition(const glm::vec3& pos) { position = pos; }
    void setRotation(const glm::vec3& rot) { rotation = rot; }
    void setScale(const glm::vec3& scaleValue) { scale = scaleValue; }
    void setTexture(std::shared_ptr<Texture> tex) { texture = tex; }
    
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getRotation() const { return rotation; }
    glm::vec3 getScale() const { return scale; }
    
    void update(float deltaTime, float currentTime);
    void draw(Shader& shader, const Camera& camera) const;

private:
    Mesh* mesh;
    std::shared_ptr<Texture> texture;
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation{0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
    float orbitSpeed{30.0f}; // Degrees per second
    float orbitRadius{0.0f};
};
} 