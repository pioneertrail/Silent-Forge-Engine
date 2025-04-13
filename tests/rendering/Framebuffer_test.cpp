#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <rendering/Framebuffer.hpp>
#include <rendering/Texture.hpp>
#include <rendering/GLContext.hpp>
#include <memory>
#include <crtdbg.h>

namespace SFE {
namespace Test {

// Memory leak detection helper
class MemoryLeakDetector {
public:
    MemoryLeakDetector() {
        _CrtMemCheckpoint(&startState);
    }

    ~MemoryLeakDetector() {
        _CrtMemState endState;
        _CrtMemState diffState;
        _CrtMemCheckpoint(&endState);
        
        if (_CrtMemDifference(&diffState, &startState, &endState)) {
            _CrtMemDumpStatistics(&diffState);
            REQUIRE_FALSE("Memory leak detected!");
        }
    }

private:
    _CrtMemState startState;
};

// Test fixture for Framebuffer tests
class FramebufferTestFixture {
public:
    FramebufferTestFixture() : memoryDetector() {
        context = std::make_unique<GLContext>();
        framebuffer = std::make_unique<Framebuffer>(800, 600);
        
        // Create test textures
        colorTexture = std::make_shared<Texture>(800, 600, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
        depthTexture = std::make_shared<Texture>(800, 600, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
        stencilTexture = std::make_shared<Texture>(800, 600, GL_STENCIL_INDEX, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE);
        depthStencilTexture = std::make_shared<Texture>(800, 600, GL_DEPTH_STENCIL, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
    }

private:
    MemoryLeakDetector memoryDetector;
    std::unique_ptr<GLContext> context;
    std::unique_ptr<Framebuffer> framebuffer;
    std::shared_ptr<Texture> colorTexture;
    std::shared_ptr<Texture> depthTexture;
    std::shared_ptr<Texture> stencilTexture;
    std::shared_ptr<Texture> depthStencilTexture;
};

TEST_CASE_METHOD(FramebufferTestFixture, "Framebuffer Construction and Destruction", "[Framebuffer]") {
    SECTION("Valid framebuffer creation") {
        REQUIRE_NOTHROW(Framebuffer fb(800, 600));
    }

    SECTION("Invalid dimensions") {
        REQUIRE_THROWS_AS(Framebuffer(0, 600), std::runtime_error);
        REQUIRE_THROWS_AS(Framebuffer(800, 0), std::runtime_error);
        REQUIRE_THROWS_AS(Framebuffer(-1, 600), std::runtime_error);
    }
}

TEST_CASE_METHOD(FramebufferTestFixture, "Framebuffer Attachment Management", "[Framebuffer]") {
    SECTION("Color attachment") {
        REQUIRE_NOTHROW(framebuffer->attachColorTexture(colorTexture));
        REQUIRE(framebuffer->isComplete());
    }

    SECTION("Depth attachment") {
        REQUIRE_NOTHROW(framebuffer->attachDepthTexture(depthTexture));
        REQUIRE(framebuffer->isComplete());
    }

    SECTION("Stencil attachment") {
        REQUIRE_NOTHROW(framebuffer->attachStencilTexture(stencilTexture));
        REQUIRE(framebuffer->isComplete());
    }

    SECTION("Depth-Stencil attachment") {
        REQUIRE_NOTHROW(framebuffer->attachDepthStencilTexture(depthStencilTexture));
        REQUIRE(framebuffer->isComplete());
    }

    SECTION("Multiple color attachments") {
        REQUIRE_NOTHROW(framebuffer->attachColorTexture(colorTexture, GL_COLOR_ATTACHMENT0));
        REQUIRE_NOTHROW(framebuffer->attachColorTexture(colorTexture, GL_COLOR_ATTACHMENT1));
        REQUIRE(framebuffer->isComplete());
    }

    SECTION("Invalid texture attachment") {
        REQUIRE_THROWS_AS(framebuffer->attachColorTexture(nullptr), std::runtime_error);
    }
}

TEST_CASE_METHOD(FramebufferTestFixture, "Framebuffer State Management", "[Framebuffer]") {
    SECTION("Bind/Unbind") {
        REQUIRE_NOTHROW(framebuffer->bind());
        REQUIRE_NOTHROW(framebuffer->unbind());
    }

    SECTION("Viewport setting") {
        REQUIRE_NOTHROW(framebuffer->setViewport());
    }

    SECTION("Clear operations") {
        REQUIRE_NOTHROW(framebuffer->clear(GL_COLOR_BUFFER_BIT));
        REQUIRE_NOTHROW(framebuffer->clear(GL_DEPTH_BUFFER_BIT));
        REQUIRE_NOTHROW(framebuffer->clear(GL_STENCIL_BUFFER_BIT));
        REQUIRE_NOTHROW(framebuffer->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
    }
}

TEST_CASE_METHOD(FramebufferTestFixture, "Framebuffer Performance Benchmarks", "[Framebuffer][Benchmark]") {
    SECTION("Bind/Unbind operations") {
        BENCHMARK("Framebuffer::bind()") {
            framebuffer->bind();
        };

        BENCHMARK("Framebuffer::unbind()") {
            framebuffer->unbind();
        };
    }

    SECTION("Clear operations") {
        BENCHMARK("Framebuffer::clear() - Color only") {
            framebuffer->clear(GL_COLOR_BUFFER_BIT);
        };

        BENCHMARK("Framebuffer::clear() - Depth only") {
            framebuffer->clear(GL_DEPTH_BUFFER_BIT);
        };

        BENCHMARK("Framebuffer::clear() - All buffers") {
            framebuffer->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        };
    }

    SECTION("Texture attachment") {
        BENCHMARK("Framebuffer::attachColorTexture()") {
            framebuffer->attachColorTexture(colorTexture);
        };

        BENCHMARK("Framebuffer::attachDepthTexture()") {
            framebuffer->attachDepthTexture(depthTexture);
        };
    }
}

} // namespace Test
} // namespace SFE 