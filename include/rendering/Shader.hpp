#pragma once
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

namespace SFE {
class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    void use() const;
    GLuint getID() const { return programID; }

    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setVec3(const std::string& name, const glm::vec3& vec) const;
    void setInt(const std::string& name, int value) const;

private:
    GLuint programID;
    std::string readFile(const std::string& path);
    GLuint compileShader(GLenum type, const std::string& source);
};
} 