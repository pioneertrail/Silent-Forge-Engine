#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "rendering/Shader.hpp"

namespace SFE {
namespace Rendering {

class ShaderManager {
public:
    static ShaderManager& getInstance();
    
    std::shared_ptr<Shader> loadShader(const std::string& name, 
                                      const std::string& vertexPath, 
                                      const std::string& fragmentPath);
    std::shared_ptr<Shader> getShader(const std::string& name);
    void reloadAllShaders();
    void clear();

    // Delete copy constructor and assignment operator
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

private:
    ShaderManager() = default;
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaderCache;
    std::unordered_map<std::string, std::pair<std::string, std::string>> shaderPaths;
};

} // namespace Rendering
} // namespace SFE 