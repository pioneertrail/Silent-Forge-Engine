#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "rendering/Mesh.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Texture.hpp"
#include "core/Camera.hpp"
#include <memory>
#include <vector>
#include <nlohmann/json.hpp>

namespace SFE {

// Forward declarations
namespace Rendering {
    class Mesh;
    class Shader;
    class Texture;
}

class SceneNode {
public:
    SceneNode(Rendering::Mesh* mesh = nullptr);
    ~SceneNode() = default;

    void setPosition(const glm::vec3& pos) { position = pos; }
    void setRotation(const glm::vec3& rot) { rotation = rot; }
    void setScale(const glm::vec3& scaleValue) { scale = scaleValue; }
    void setTexture(std::shared_ptr<Rendering::Texture> tex) { texture = tex; }
    void setParent(SceneNode* parent) { 
        if (this->parent) {
            auto& siblings = this->parent->children;
            siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
        }
        this->parent = parent;
        if (parent) {
            parent->children.push_back(this);
        }
    }
    
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getRotation() const { return rotation; }
    glm::vec3 getScale() const { return scale; }
    SceneNode* getParent() const { return parent; }
    const std::vector<SceneNode*>& getChildren() const { return children; }
    
    void update(float deltaTime, float currentTime);
    void draw(Rendering::Shader& shader, const Camera& camera) const;

    void serialize(nlohmann::json& json) const {
        json["position"] = {position.x, position.y, position.z};
        json["rotation"] = {rotation.x, rotation.y, rotation.z};
        json["scale"] = {scale.x, scale.y, scale.z};
        json["orbitSpeed"] = orbitSpeed;
        json["orbitRadius"] = orbitRadius;
        
        if (texture) {
            json["texture"] = "texture_path"; // Replace with actual texture path accessor
        }
        
        json["children"] = nlohmann::json::array();
        for (const auto* child : children) {
            nlohmann::json childJson;
            child->serialize(childJson);
            json["children"].push_back(childJson);
        }
    }

    void deserialize(const nlohmann::json& json) {
        position = glm::vec3(json["position"][0], json["position"][1], json["position"][2]);
        rotation = glm::vec3(json["rotation"][0], json["rotation"][1], json["rotation"][2]);
        scale = glm::vec3(json["scale"][0], json["scale"][1], json["scale"][2]);
        orbitSpeed = json["orbitSpeed"];
        orbitRadius = json["orbitRadius"];
        
        if (json.contains("texture")) {
            // TODO: Load texture from path
        }
        
        for (const auto& childJson : json["children"]) {
            auto child = std::make_unique<SceneNode>();
            child->deserialize(childJson);
            child->setParent(this);
        }
    }

private:
    Rendering::Mesh* mesh;
    std::shared_ptr<Rendering::Texture> texture;
    SceneNode* parent{nullptr};
    std::vector<SceneNode*> children;
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation{0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
    float orbitSpeed{30.0f}; // Degrees per second
    float orbitRadius{0.0f};
};
} 