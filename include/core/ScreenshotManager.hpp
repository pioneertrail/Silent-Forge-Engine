#pragma once
#include <string>
#include <vector>
#include <glad/glad.h>
#include "stb/stb_image_write.h"

namespace SFE {

class ScreenshotManager {
public:
    static ScreenshotManager& getInstance() {
        static ScreenshotManager instance;
        return instance;
    }

    bool captureScreenshot(const std::string& filename, int width, int height) {
        std::vector<unsigned char> pixels(width * height * 4);
        
        // Read pixels from framebuffer
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        
        // Flip image vertically (OpenGL has bottom-left origin)
        std::vector<unsigned char> flipped(width * height * 4);
        for (int y = 0; y < height; y++) {
            memcpy(&flipped[(height - 1 - y) * width * 4],
                   &pixels[y * width * 4],
                   width * 4);
        }
        
        // Save to PNG file
        return stbi_write_png(filename.c_str(), width, height, 4, flipped.data(), width * 4) != 0;
    }

private:
    ScreenshotManager() = default;
    ScreenshotManager(const ScreenshotManager&) = delete;
    ScreenshotManager& operator=(const ScreenshotManager&) = delete;
};

} // namespace SFE 