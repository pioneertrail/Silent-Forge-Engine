#include "rendering/ShaderManager.hpp"
#include "rendering/Shader.hpp"
#include <stdexcept>
#include <iostream>

namespace SFE {
namespace Rendering {

ShaderManager& ShaderManager::getInstance() {
    static ShaderManager instance;
    return instance;
}

std::shared_ptr<Shader> ShaderManager::loadShader(const std::string& name,
                                                 const std::string& vertexPath,
                                                 const std::string& fragmentPath) {
    // Check if shader already exists
    auto it = shaderCache.find(name);
    if (it != shaderCache.end()) {
        return it->second;
    }

    try {
        // Create new shader
        auto shader = std::make_shared<Shader>(vertexPath, fragmentPath);
        shaderCache[name] = shader;
        shaderPaths[name] = {vertexPath, fragmentPath};
        return shader;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to load shader '" << name << "': " << e.what() << std::endl;
        throw;
    }
}

std::shared_ptr<Shader> ShaderManager::getShader(const std::string& name) {
    auto it = shaderCache.find(name);
    if (it == shaderCache.end()) {
        throw std::runtime_error("Shader '" + name + "' not found");
    }
    return it->second;
}

void ShaderManager::reloadAllShaders() {
    std::unordered_map<std::string, std::shared_ptr<Shader>> newCache;
    
    for (const auto& [name, paths] : shaderPaths) {
        try {
            auto shader = std::make_shared<Shader>(paths.first, paths.second);
            newCache[name] = shader;
            std::cout << "Successfully reloaded shader '" << name << "'" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to reload shader '" << name << "': " << e.what() << std::endl;
            // Keep the old shader in case of reload failure
            if (auto it = shaderCache.find(name); it != shaderCache.end()) {
                newCache[name] = it->second;
            }
        }
    }
    
    shaderCache = std::move(newCache);
}

void ShaderManager::clear() {
    shaderCache.clear();
    shaderPaths.clear();
}

} // namespace Rendering
} // namespace SFE 