#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include "rendering/Shader.hpp"
#include "rendering/Texture.hpp"

namespace SFE {

class Material {
public:
    using UniformValue = std::variant<
        int,
        float,
        glm::vec2,
        glm::vec3,
        glm::vec4,
        glm::mat3,
        glm::mat4,
        std::shared_ptr<Texture>
    >;

    Material(std::shared_ptr<Shader> shader);
    ~Material() = default;

    // Set uniform values
    void setUniform(const std::string& name, const UniformValue& value);
    
    // Get uniform value
    UniformValue getUniform(const std::string& name) const;

    // Bind the material (binds shader and all uniforms)
    void bind();

    // Get the shader
    std::shared_ptr<Shader> getShader() const { return shader; }

    // Set blending mode
    void setBlending(bool enabled);
    bool getBlending() const { return blendingEnabled; }

    // Set depth testing
    void setDepthTest(bool enabled);
    bool getDepthTest() const { return depthTestEnabled; }

    // Set culling mode
    void setCulling(bool enabled);
    bool getCulling() const { return cullingEnabled; }

private:
    std::shared_ptr<Shader> shader;
    std::unordered_map<std::string, UniformValue> uniforms;
    
    bool blendingEnabled = false;
    bool depthTestEnabled = true;
    bool cullingEnabled = true;
};

} // namespace SFE 