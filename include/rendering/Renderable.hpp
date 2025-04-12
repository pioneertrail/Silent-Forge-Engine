#pragma once
#include <glm/glm.hpp>
#include <memory>

namespace SFE {

class Material;
class Shader;

class Renderable {
public:
    virtual ~Renderable() = default;

    // Prepare the renderable for rendering (e.g., update buffers, compute transforms)
    virtual void prepare() = 0;

    // Bind the necessary resources (VAO, textures, etc.)
    virtual void bind() = 0;

    // Perform the actual draw call
    virtual void draw() = 0;

    // Set the material for this renderable
    virtual void setMaterial(std::shared_ptr<Material> material) = 0;

    // Get the current material
    virtual std::shared_ptr<Material> getMaterial() const = 0;

    // Update the model matrix
    virtual void setModelMatrix(const glm::mat4& model) = 0;

    // Get the current model matrix
    virtual glm::mat4 getModelMatrix() const = 0;
};

} // namespace SFE 