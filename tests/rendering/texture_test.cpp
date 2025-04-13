#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "rendering/Texture.hpp"
#include "helpers/GLContextHelper.hpp"
#include <vector>
#include <string>
#include "TestFixture.hpp"
#include <memory>
#include <filesystem>

using namespace SFE::Rendering;
using namespace Catch::Matchers;
using namespace SFE;
using namespace SFE::Test;

TEST_CASE("Texture Class", "[rendering][texture]") {
    // Initialize OpenGL context for testing
    REQUIRE(SFE::Testing::GLContextHelper::init());

    SECTION("Texture Construction and Destruction") {
        Texture texture;
        REQUIRE(texture.getTextureID() == 0);
        REQUIRE(texture.getWidth() == 0);
        REQUIRE(texture.getHeight() == 0);
        REQUIRE(texture.getChannels() == 0);
    }

    SECTION("Texture Loading") {
        std::vector<std::string> errorMessages;
        auto logCallback = [&errorMessages](const std::string& msg) {
            errorMessages.push_back(msg);
        };

        SECTION("Load Invalid File") {
            Texture texture;
            REQUIRE_FALSE(texture.loadFromFile("nonexistent.png", logCallback));
            REQUIRE_FALSE(errorMessages.empty());
        }

        SECTION("Load Valid File") {
            Texture texture;
            REQUIRE(texture.loadFromFile("resources/textures/test.png", logCallback));
            REQUIRE(errorMessages.empty());
            REQUIRE(texture.getTextureID() != 0);
            REQUIRE(texture.getWidth() > 0);
            REQUIRE(texture.getHeight() > 0);
            REQUIRE(texture.getChannels() > 0);
        }
    }

    SECTION("Texture Binding") {
        std::vector<std::string> errorMessages;
        auto logCallback = [&errorMessages](const std::string& msg) {
            errorMessages.push_back(msg);
        };

        Texture texture;
        REQUIRE(texture.loadFromFile("resources/textures/test.png", logCallback));

        SECTION("Bind and Unbind") {
            texture.bind();
            GLint boundTexture;
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);
            REQUIRE(boundTexture == texture.getTextureID());

            texture.unbind();
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);
            REQUIRE(boundTexture == 0);
        }

        SECTION("Multiple Bindings") {
            Texture texture2;
            REQUIRE(texture2.loadFromFile("resources/textures/test.png", logCallback));

            texture.bind();
            GLint boundTexture;
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);
            REQUIRE(boundTexture == texture.getTextureID());

            texture2.bind();
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);
            REQUIRE(boundTexture == texture2.getTextureID());

            texture.unbind();
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);
            REQUIRE(boundTexture == 0);
        }
    }

    SECTION("Texture State Management") {
        std::vector<std::string> errorMessages;
        auto logCallback = [&errorMessages](const std::string& msg) {
            errorMessages.push_back(msg);
        };

        Texture texture;
        REQUIRE(texture.loadFromFile("resources/textures/test.png", logCallback));

        SECTION("Texture Parameters") {
            texture.bind();
            GLint minFilter, magFilter, wrapS, wrapT;
            glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
            glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);
            glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapS);
            glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapT);

            REQUIRE(minFilter == GL_LINEAR);
            REQUIRE(magFilter == GL_LINEAR);
            REQUIRE(wrapS == GL_REPEAT);
            REQUIRE(wrapT == GL_REPEAT);
        }
    }

    // Cleanup
    SFE::Testing::GLContextHelper::cleanup();
}

class TextureTest : public TestFixture {
protected:
    void SetUp() override {
        TestFixture::SetUp();
        texture = std::make_unique<Texture>();
    }

    void TearDown() override {
        texture.reset();
        TestFixture::TearDown();
    }

    std::unique_ptr<Texture> texture;
};

// Basic Texture Operations
TEST_F(TextureTest, TextureCreation) {
    EXPECT_FALSE(texture->isLoaded());
    EXPECT_EQ(texture->getWidth(), 0);
    EXPECT_EQ(texture->getHeight(), 0);
}

// Texture Loading Tests
TEST_F(TextureTest, LoadFromFile) {
    // Create a test image file
    std::ofstream testImage("tests/fixtures/test.png", std::ios::binary);
    // Add minimal PNG header
    const unsigned char pngHeader[] = {
        0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A,
        0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
        0x08, 0x06, 0x00, 0x00, 0x00, 0x1F, 0x15, 0xC4,
        0x89, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x44, 0x41,
        0x54, 0x08, 0xD7, 0x63, 0x60, 0x00, 0x00, 0x00,
        0x02, 0x00, 0x01, 0xE2, 0x21, 0xBC, 0x33, 0x00,
        0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE,
        0x42, 0x60, 0x82
    };
    testImage.write(reinterpret_cast<const char*>(pngHeader), sizeof(pngHeader));
    testImage.close();

    EXPECT_TRUE(texture->loadFromFile("tests/fixtures/test.png"));
    EXPECT_TRUE(texture->isLoaded());
    EXPECT_EQ(texture->getWidth(), 1);
    EXPECT_EQ(texture->getHeight(), 1);

    // Clean up
    std::filesystem::remove("tests/fixtures/test.png");
}

// Texture Parameter Tests
TEST_F(TextureTest, TextureParameters) {
    // Create a test image
    std::vector<unsigned char> testData(4, 255); // 1x1 white pixel
    EXPECT_TRUE(texture->loadFromMemory(testData.data(), 1, 1, 4));
    
    // Test parameter setting
    EXPECT_TRUE(texture->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    EXPECT_TRUE(texture->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    EXPECT_TRUE(texture->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    EXPECT_TRUE(texture->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
}

// Performance Tests
TEST_F(TextureTest, Performance_TextureLoading) {
    std::vector<unsigned char> testData(1024 * 1024 * 4, 255); // 1MB test data
    TestPerformance([&]() {
        auto tempTexture = std::make_unique<Texture>();
        tempTexture->loadFromMemory(testData.data(), 1024, 1024, 4);
    });
}

TEST_F(TextureTest, Performance_TextureBinding) {
    std::vector<unsigned char> testData(4, 255);
    EXPECT_TRUE(texture->loadFromMemory(testData.data(), 1, 1, 4));
    
    TestPerformance([&]() {
        texture->bind();
        texture->unbind();
    });
}

// Thread Safety Tests
TEST_F(TextureTest, ThreadSafety_TextureBinding) {
    std::vector<unsigned char> testData(4, 255);
    EXPECT_TRUE(texture->loadFromMemory(testData.data(), 1, 1, 4));
    
    TestThreadSafety(*texture, [](Texture& t) {
        t.bind();
        t.unbind();
    });
}

// Memory Tests
TEST_F(TextureTest, MemoryLeak_TextureCreation) {
    TestMemoryLeak([&]() {
        auto tempTexture = std::make_unique<Texture>();
        std::vector<unsigned char> testData(4, 255);
        tempTexture->loadFromMemory(testData.data(), 1, 1, 4);
    });
}

// Edge Cases
TEST_F(TextureTest, InvalidFile) {
    EXPECT_FALSE(texture->loadFromFile("nonexistent.png"));
}

TEST_F(TextureTest, InvalidMemoryData) {
    EXPECT_FALSE(texture->loadFromMemory(nullptr, 1, 1, 4));
    EXPECT_FALSE(texture->loadFromMemory(new unsigned char[4], 0, 1, 4));
    EXPECT_FALSE(texture->loadFromMemory(new unsigned char[4], 1, 0, 4));
    EXPECT_FALSE(texture->loadFromMemory(new unsigned char[4], 1, 1, 0));
}

TEST_F(TextureTest, InvalidParameters) {
    EXPECT_FALSE(texture->setParameter(GL_TEXTURE_MIN_FILTER, 0xFFFFFFFF));
    EXPECT_FALSE(texture->setParameter(0xFFFFFFFF, GL_LINEAR));
}

// Resource Management Tests
TEST_F(TextureTest, ResourceCleanup) {
    {
        auto tempTexture = std::make_unique<Texture>();
        std::vector<unsigned char> testData(4, 255);
        EXPECT_TRUE(tempTexture->loadFromMemory(testData.data(), 1, 1, 4));
        EXPECT_TRUE(tempTexture->isLoaded());
    }
    // Texture should be properly cleaned up
    EXPECT_FALSE(texture->isLoaded());
}

// Copy and Move Tests
TEST_F(TextureTest, TextureCopy) {
    std::vector<unsigned char> testData(4, 255);
    EXPECT_TRUE(texture->loadFromMemory(testData.data(), 1, 1, 4));
    
    Texture copyTexture(*texture);
    EXPECT_TRUE(copyTexture.isLoaded());
    EXPECT_EQ(copyTexture.getWidth(), texture->getWidth());
    EXPECT_EQ(copyTexture.getHeight(), texture->getHeight());
}

TEST_F(TextureTest, TextureMove) {
    std::vector<unsigned char> testData(4, 255);
    EXPECT_TRUE(texture->loadFromMemory(testData.data(), 1, 1, 4));
    
    Texture moveTexture(std::move(*texture));
    EXPECT_TRUE(moveTexture.isLoaded());
    EXPECT_FALSE(texture->isLoaded());
} 