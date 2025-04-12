#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include "rendering/Shader.hpp"

namespace SFE {

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();

    bool initialize(const std::string& fontAtlasPath, const std::string& fontDescPath);
    void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color, const glm::mat4& projection);
    void renderBatch(const glm::mat4& projection); // New method for batched rendering

private:
    struct Character {
        glm::vec2 uvBottomLeft; // Texture coords (bottom-left)
        glm::vec2 uvTopRight;   // Texture coords (top-right)
        glm::vec2 size;         // Glyph dimensions (width, height)
        glm::vec2 offset;       // Offset from cursor pos to top-left (xoffset, yoffset)
        float advance;          // How far to advance cursor (xadvance)
    };

    struct TextInstance {
        std::string text;
        float x, y, scale;
        glm::vec3 color;
        std::vector<float> vertices;
    };

    GLuint vao, vbo;
    Shader shader;
    std::unordered_map<char, Character> characters; // Store characters by ID
    GLuint textureID; // Font atlas texture
    float atlasWidth, atlasHeight; // Store atlas dimensions

    // New members for batched rendering
    std::vector<float> batchedVertices;
    std::unordered_map<std::string, TextInstance> cachedText;
    static constexpr size_t MAX_BATCH_VERTICES = 4096;

    void setupBuffers();
    bool loadFontAtlas(const std::string& atlasPath);
    bool loadFontDescriptor(const std::string& descPath);
    void generateVertices(const std::string& text, float x, float y, float scale, const glm::vec3& color, std::vector<float>& vertices);
    void flushBatch(const glm::mat4& projection);
    void addToBatch(const std::string& text, float x, float y, float scale, const glm::vec3& color);
};

} 