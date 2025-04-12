#include "rendering/TextRenderer.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb/stb_image.h"

namespace SFE {

TextRenderer::TextRenderer() 
    : vao(0), vbo(0), textureID(0), atlasWidth(0), atlasHeight(0),
      shader("shaders/text2d.vert", "shaders/text2d.frag") {
    batchedVertices.reserve(MAX_BATCH_VERTICES);
}

TextRenderer::~TextRenderer() {
    if (vao != 0) {
        glDeleteVertexArrays(1, &vao);
    }
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
    }
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
    }
}

bool TextRenderer::initialize(const std::string& fontAtlasPath, const std::string& fontDescPath) {
    // Load font atlas texture
    if (!loadFontAtlas(fontAtlasPath)) {
        std::cerr << "Failed to load font atlas: " << fontAtlasPath << std::endl;
        return false;
    }

    // Load font descriptor file
    if (!loadFontDescriptor(fontDescPath)) {
        std::cerr << "Failed to load font descriptor: " << fontDescPath << std::endl;
        return false;
    }

    setupBuffers();

    std::cout << "TextRenderer initialized successfully" << std::endl;
    return true;
}

void TextRenderer::setupBuffers() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    // Buffer size for batched rendering
    glBufferData(GL_ARRAY_BUFFER, MAX_BATCH_VERTICES * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    
    // Position (vec2), UV (vec2), Color (vec3)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

bool TextRenderer::loadFontAtlas(const std::string& atlasPath) {
    // Generate and bind texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Load texture data from file using stb_image
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(atlasPath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (!data) {
        std::cerr << "STB Image failed to load: " << atlasPath << " (" << stbi_failure_reason() << ")" << std::endl;
        // Fallback: create a 16x16 white square
        unsigned char fallbackData[16 * 16 * 4];
        for (int i = 0; i < 16 * 16 * 4; i++) {
            fallbackData[i] = 255;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, fallbackData);
        atlasWidth = 16.0f;
        atlasHeight = 16.0f;
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        atlasWidth = static_cast<float>(width);
        atlasHeight = static_cast<float>(height);
        stbi_image_free(data);
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

bool TextRenderer::loadFontDescriptor(const std::string& descPath) {
    std::ifstream file(descPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open font descriptor file: " << descPath << std::endl;
        return false;
    }
    
    std::string line;
    
    // Parse header line to get atlas size (optional)
    if (std::getline(file, line)) {
        // Some font format processing logic
    }
    
    // Parse character definitions
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        
        // Skip non-character lines
        if (line.substr(0, 5) != "char ") {
            continue;
        }
        
        // Parse character info
        int id = 0, x = 0, y = 0, width = 0, height = 0, xoffset = 0, yoffset = 0, xadvance = 0;
        
        std::string part;
        std::stringstream lineStream(line);
        
        // Parse each parameter in the line
        while (lineStream >> part) {
            if (part.substr(0, 3) == "id=") {
                id = std::stoi(part.substr(3));
            } else if (part.substr(0, 2) == "x=") {
                x = std::stoi(part.substr(2));
            } else if (part.substr(0, 2) == "y=") {
                y = std::stoi(part.substr(2));
            } else if (part.substr(0, 6) == "width=") {
                width = std::stoi(part.substr(6));
            } else if (part.substr(0, 7) == "height=") {
                height = std::stoi(part.substr(7));
            } else if (part.substr(0, 8) == "xoffset=") {
                xoffset = std::stoi(part.substr(8));
            } else if (part.substr(0, 8) == "yoffset=") {
                yoffset = std::stoi(part.substr(8));
            } else if (part.substr(0, 9) == "xadvance=") {
                xadvance = std::stoi(part.substr(9));
            }
        }
        
        // Only add if this is a valid ASCII character
        if (id >= 0 && id < 128) {
            Character character;
            
            // Calculate UV coordinates within atlas
            character.uvBottomLeft = glm::vec2(
                static_cast<float>(x) / atlasWidth,
                1.0f - static_cast<float>(y + height) / atlasHeight
            );
            character.uvTopRight = glm::vec2(
                static_cast<float>(x + width) / atlasWidth,
                1.0f - static_cast<float>(y) / atlasHeight
            );
            
            character.size = glm::vec2(static_cast<float>(width), static_cast<float>(height));
            character.offset = glm::vec2(static_cast<float>(xoffset), static_cast<float>(yoffset));
            character.advance = static_cast<float>(xadvance);
            
            characters.insert(std::pair<char, Character>(static_cast<char>(id), character));
        }
    }
    
    std::cout << "Loaded " << characters.size() << " characters from font descriptor" << std::endl;
    return !characters.empty();
}

void TextRenderer::generateVertices(const std::string& text, float x, float y, float scale, 
                                  const glm::vec3& color, std::vector<float>& vertices) {
    float cursorX = x;
    
    for (char c : text) {
        auto it = characters.find(c);
        if (it == characters.end()) continue;
        
        const Character& ch = it->second;
        float xpos = cursorX + ch.offset.x * scale;
        float ypos = y - (ch.size.y - ch.offset.y) * scale;
        float w = ch.size.x * scale;
        float h = ch.size.y * scale;
        
        // 6 vertices per character (2 triangles)
        float quad[] = {
            // pos.x, pos.y, uv.x, uv.y, color.r, color.g, color.b
            xpos,     ypos + h, ch.uvBottomLeft.x, ch.uvTopRight.y,   color.r, color.g, color.b,
            xpos,     ypos,     ch.uvBottomLeft.x, ch.uvBottomLeft.y, color.r, color.g, color.b,
            xpos + w, ypos,     ch.uvTopRight.x,   ch.uvBottomLeft.y, color.r, color.g, color.b,
            
            xpos,     ypos + h, ch.uvBottomLeft.x, ch.uvTopRight.y,   color.r, color.g, color.b,
            xpos + w, ypos,     ch.uvTopRight.x,   ch.uvBottomLeft.y, color.r, color.g, color.b,
            xpos + w, ypos + h, ch.uvTopRight.x,   ch.uvTopRight.y,   color.r, color.g, color.b
        };
        
        vertices.insert(vertices.end(), quad, quad + 42); // 6 vertices * 7 floats each
        cursorX += ch.advance * scale;
    }
}

void TextRenderer::addToBatch(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
    // Check if text is cached
    auto it = cachedText.find(text);
    if (it != cachedText.end() && 
        it->second.x == x && 
        it->second.y == y && 
        it->second.scale == scale &&
        it->second.color == color) {
        // Use cached vertices
        batchedVertices.insert(batchedVertices.end(), 
                             it->second.vertices.begin(), 
                             it->second.vertices.end());
    } else {
        // Generate new vertices
        std::vector<float> vertices;
        generateVertices(text, x, y, scale, color, vertices);
        
        // Cache the text if it's not too large
        if (vertices.size() <= MAX_BATCH_VERTICES / 4) { // Allow caching if less than 25% of max batch
            TextInstance instance{text, x, y, scale, color, vertices};
            cachedText[text] = instance;
        }
        
        batchedVertices.insert(batchedVertices.end(), vertices.begin(), vertices.end());
    }
    
    // Flush if batch is full
    if (batchedVertices.size() >= MAX_BATCH_VERTICES) {
        flushBatch(glm::mat4(1.0f)); // Use identity matrix as projection will be set later
    }
}

void TextRenderer::flushBatch(const glm::mat4& projection) {
    if (batchedVertices.empty()) return;
    
    shader.use();
    shader.setMat4("projection", projection);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, batchedVertices.size() * sizeof(float), batchedVertices.data());
    
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(batchedVertices.size() / 7));
    
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    batchedVertices.clear();
}

void TextRenderer::renderText(const std::string& text, float x, float y, float scale, 
                            const glm::vec3& color, const glm::mat4& projection) {
    addToBatch(text, x, y, scale, color);
    flushBatch(projection);
}

void TextRenderer::renderBatch(const glm::mat4& projection) {
    flushBatch(projection);
}

} 