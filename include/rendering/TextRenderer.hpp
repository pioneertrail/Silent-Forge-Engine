#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>
#include "rendering/Shader.hpp"

namespace SFE {
class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();

    bool initialize(const std::string& fontAtlasPath, const std::string& fontDescPath);
    void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color, const glm::mat4& projection);

private:
    struct Character {
        glm::vec2 uvBottomLeft; // Texture coords (bottom-left)
        glm::vec2 uvTopRight;   // Texture coords (top-right)
        glm::vec2 size;         // Glyph dimensions (width, height)
        glm::vec2 offset;       // Offset from cursor pos to top-left (xoffset, yoffset)
        float advance;          // How far to advance cursor (xadvance)
    };

    GLuint vao, vbo;
    Shader shader;
    std::map<char, Character> characters; // Store characters by ID
    GLuint textureID; // Font atlas texture
    float atlasWidth, atlasHeight; // Store atlas dimensions

    void setupQuad();
    bool loadFontAtlas(const std::string& atlasPath);
    bool loadFontDescriptor(const std::string& descPath);
};
}
