#pragma once
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

namespace SFE {
namespace Rendering {

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    // Rule of five: Prevent copying/moving
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(Shader&&) = delete;

    void use() const;
    GLuint getID() const { return programID; }

    // Utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    GLuint programID;

    // Utility function for checking compile/link errors
    std::string readFile(const std::string& path);
    GLuint compileShader(GLenum type, const std::string& source);
    void checkCompileErrors(GLuint shader, std::string type);
    void checkLinkErrors(GLuint program);
};

} // namespace Rendering
} 