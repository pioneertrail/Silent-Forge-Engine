#include "rendering/Material.hpp"
#include <glad/glad.h>
#include <stdexcept>

namespace SFE {

Material::Material(std::shared_ptr<Shader> shader)
    : shader(shader) {
    if (!shader) {
        throw std::runtime_error("Material: Shader cannot be null");
    }
}

void Material::setUniform(const std::string& name, const UniformValue& value) {
    uniforms[name] = value;
}

Material::UniformValue Material::getUniform(const std::string& name) const {
    auto it = uniforms.find(name);
    if (it == uniforms.end()) {
        throw std::runtime_error("Material: Uniform '" + name + "' not found");
    }
    return it->second;
}

void Material::bind() {
    shader->use();

    // Apply render states
    if (blendingEnabled) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
    }

    if (depthTestEnabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    if (cullingEnabled) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }

    // Bind uniforms
    for (const auto& [name, value] : uniforms) {
        std::visit([this, &name](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, int>) {
                shader->setInt(name, arg);
            }
            else if constexpr (std::is_same_v<T, float>) {
                shader->setFloat(name, arg);
            }
            else if constexpr (std::is_same_v<T, glm::vec2>) {
                shader->setVec2(name, arg);
            }
            else if constexpr (std::is_same_v<T, glm::vec3>) {
                shader->setVec3(name, arg);
            }
            else if constexpr (std::is_same_v<T, glm::vec4>) {
                shader->setVec4(name, arg);
            }
            else if constexpr (std::is_same_v<T, glm::mat3>) {
                shader->setMat3(name, arg);
            }
            else if constexpr (std::is_same_v<T, glm::mat4>) {
                shader->setMat4(name, arg);
            }
            else if constexpr (std::is_same_v<T, std::shared_ptr<Texture>>) {
                if (arg) {
                    arg->bind();
                    shader->setInt(name, 0); // Assuming texture unit 0
                }
            }
        }, value);
    }
}

void Material::setBlending(bool enabled) {
    blendingEnabled = enabled;
}

void Material::setDepthTest(bool enabled) {
    depthTestEnabled = enabled;
}

void Material::setCulling(bool enabled) {
    cullingEnabled = enabled;
}

} // namespace SFE 