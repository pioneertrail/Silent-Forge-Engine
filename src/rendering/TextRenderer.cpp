#include "rendering/TextRenderer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace SFE {
TextRenderer::TextRenderer() : vao(0), vbo(0), textureID(0), atlasWidth(0), atlasHeight(0), shader("shaders/text.vert", "shaders/text.frag") {}

TextRenderer::~TextRenderer() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
    }
}

bool TextRenderer::initialize(const std::string& fontAtlasPath, const std::string& fontDescPath) {
    if (!loadFontAtlas(fontAtlasPath)) {
        std::cerr << "Failed to load font atlas: " << fontAtlasPath << std::endl;
        return false;
    }
    if (!loadFontDescriptor(fontDescPath)) {
        std::cerr << "Failed to load font descriptor: " << fontDescPath << std::endl;
        if (textureID != 0) glDeleteTextures(1, &textureID);
        return false;
    }
    setupQuad();
    std::cout << "TextRenderer initialized successfully." << std::endl;
    return true;
}

bool TextRenderer::loadFontAtlas(const std::string& atlasPath) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(atlasPath.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Failed to load font atlas: " << atlasPath << " (" << stbi_failure_reason() << ")" << std::endl;
        // Use a placeholder white pixel texture for testing
        std::cout << "Creating placeholder texture..." << std::endl;
        atlasWidth = 256;
        atlasHeight = 256;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        unsigned char pixelData[] = {255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        return true;
    }

    atlasWidth = static_cast<float>(width);
    atlasHeight = static_cast<float>(height);
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    
    std::cout << "Loaded font atlas: " << atlasPath << " (" << width << "x" << height 
              << ", " << channels << " channels)" << std::endl;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    stbi_image_free(data);
    return true;
}

bool TextRenderer::loadFontDescriptor(const std::string& descPath) {
    std::ifstream file(descPath);
    if (!file.is_open()) {
        std::cerr << "Cannot open font descriptor file: " << descPath << std::endl;
        std::cout << "Setting up fallback character map..." << std::endl;
        // Create a simple ASCII table as fallback
        int cols = 16;
        int rows = 6;
        for (int i = 32; i < 128; ++i) {
            Character ch;
            int index = i - 32;
            float u = static_cast<float>(index % cols) / cols;
            float v = static_cast<float>(index / cols) / rows;
            ch.uvBottomLeft = glm::vec2(u, v);
            ch.uvTopRight = glm::vec2(u + 1.0f / cols, v + 1.0f / rows);
            ch.size = glm::vec2(16, 16);
            ch.offset = glm::vec2(0, 0);
            ch.advance = 16;
            characters[i] = ch;
        }
        return true;
    }

    std::string line;
    std::string token;
    float fileAtlasWidth = 0, fileAtlasHeight = 0;

    characters.clear();

    // Expect BMFont text format
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        ss >> token;

        if (token == "common") {
            while (ss >> token) {
                if (token.find("scaleW=") == 0) fileAtlasWidth = std::stof(token.substr(7));
                else if (token.find("scaleH=") == 0) fileAtlasHeight = std::stof(token.substr(7));
            }
            if (atlasWidth == 0 || atlasHeight == 0) {
                atlasWidth = fileAtlasWidth;
                atlasHeight = fileAtlasHeight;
            }
        }
        else if (token == "char") {
            int id = 0;
            Character ch;
            ch.size = glm::vec2(0, 0);
            ch.offset = glm::vec2(0, 0);
            ch.advance = 0;
            
            float x = 0, y = 0, width = 0, height = 0;
            
            while (ss >> token) {
                if (token.find("id=") == 0) id = std::stoi(token.substr(3));
                else if (token.find("x=") == 0) x = std::stof(token.substr(2));
                else if (token.find("y=") == 0) y = std::stof(token.substr(2));
                else if (token.find("width=") == 0) width = std::stof(token.substr(6));
                else if (token.find("height=") == 0) height = std::stof(token.substr(7));
                else if (token.find("xoffset=") == 0) ch.offset.x = std::stof(token.substr(8));
                else if (token.find("yoffset=") == 0) ch.offset.y = std::stof(token.substr(8));
                else if (token.find("xadvance=") == 0) ch.advance = std::stof(token.substr(9));
            }
            
            ch.size = glm::vec2(width, height);
            
            // Convert pixel coords to normalized texture coords
            ch.uvBottomLeft = glm::vec2(x / atlasWidth, (atlasHeight - (y + height)) / atlasHeight);
            ch.uvTopRight = glm::vec2((x + width) / atlasWidth, (atlasHeight - y) / atlasHeight);
            
            if (id >= 32 && id <= 126) { // Basic ASCII chars
                characters[id] = ch;
            }
        }
    }
    
    // Check if we loaded any characters
    if (characters.empty()) {
        std::cerr << "No character data found in font descriptor: " << descPath << std::endl;
        return false;
    }
    
    return true;
}

void TextRenderer::setupQuad() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Each vertex has pos (x,y) and texcoord (u,v) = 4 floats
    // 6 vertices per quad
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    // Position attribute (vec2)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture coordinate attribute (vec2)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color, const glm::mat4& projection) {
    if (textureID == 0 || characters.empty()) {
        std::cerr << "TextRenderer not properly initialized" << std::endl;
        return;
    }

    shader.use();
    shader.setMat4("projection", projection);
    shader.setVec3("textColor", color);
    shader.setInt("textTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBindVertexArray(vao);

    float currentX = x;
    for (char c : text) {
        if (characters.find(c) == characters.end()) {
            // Use space if character not found
            c = ' ';
            if (characters.find(c) == characters.end()) {
                // Skip if space not defined
                continue;
            }
        }

        Character& ch = characters[c];

        float xpos = currentX + ch.offset.x * scale;
        float ypos = y - (ch.size.y - ch.offset.y) * scale;
        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        float vertices[] = {
            xpos,     ypos + h, ch.uvBottomLeft.x, ch.uvTopRight.y,
            xpos + w, ypos + h, ch.uvTopRight.x,   ch.uvTopRight.y,
            xpos + w, ypos,     ch.uvTopRight.x,   ch.uvBottomLeft.y,
            xpos,     ypos + h, ch.uvBottomLeft.x, ch.uvTopRight.y,
            xpos + w, ypos,     ch.uvTopRight.x,   ch.uvBottomLeft.y,
            xpos,     ypos,     ch.uvBottomLeft.x, ch.uvBottomLeft.y
        };

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        currentX += ch.advance * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
}
