#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <rendering/Renderer.hpp>
#include <rendering/Shader.hpp>
#include <rendering/Mesh.hpp>
#include <rendering/Texture.hpp>
#include <rendering/Framebuffer.hpp>
#include <rendering/GLContext.hpp>
#include <memory>
#include <crtdbg.h>
#include <glm/gtc/matrix_transform.hpp>

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

// Test fixture for Renderer tests
class RendererTestFixture {
public:
    RendererTestFixture() : memoryDetector() {
        context = std::make_unique<GLContext>();
        renderer = std::make_unique<Renderer>();
        renderer->initialize();

        // Create test shader
        createTestShaderFiles();
        shader = std::make_shared<Shader>(vertexShaderPath, fragmentShaderPath);

        // Create test mesh
        std::vector<float> vertices = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };
        std::vector<unsigned int> indices = {0, 1, 2};
        mesh = std::make_shared<Mesh>(vertices, indices);

        // Create test framebuffer
        framebuffer = std::make_shared<Framebuffer>(800, 600);
        colorTexture = std::make_shared<Texture>(800, 600, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
        framebuffer->attachColorTexture(colorTexture);
    }

    ~RendererTestFixture() {
        // Clean up test shader files
        std::filesystem::remove(vertexShaderPath);
        std::filesystem::remove(fragmentShaderPath);
    }

    void createTestShaderFiles() {
        // Create a simple vertex shader
        std::ofstream vertexFile(vertexShaderPath);
        vertexFile << R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;
            void main() {
                gl_Position = projection * view * model * vec4(aPos, 1.0);
            }
        )";
        vertexFile.close();

        // Create a simple fragment shader
        std::ofstream fragmentFile(fragmentShaderPath);
        fragmentFile << R"(
            #version 330 core
            out vec4 FragColor;
            uniform vec3 color;
            void main() {
                FragColor = vec4(color, 1.0);
            }
        )";
        fragmentFile.close();
    }

private:
    MemoryLeakDetector memoryDetector;
    std::unique_ptr<GLContext> context;
    std::unique_ptr<Renderer> renderer;
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Framebuffer> framebuffer;
    std::shared_ptr<Texture> colorTexture;
    const std::string vertexShaderPath = "test_vertex_shader.glsl";
    const std::string fragmentShaderPath = "test_fragment_shader.glsl";
};

TEST_CASE_METHOD(RendererTestFixture, "Renderer Initialization", "[Renderer]") {
    SECTION("Initialization state") {
        REQUIRE_NOTHROW(renderer->initialize());
        REQUIRE(renderer->getStatistics().drawCalls == 0);
    }

    SECTION("Double initialization") {
        REQUIRE_NOTHROW(renderer->initialize());
        REQUIRE_NOTHROW(renderer->initialize()); // Should be safe to call multiple times
    }
}

TEST_CASE_METHOD(RendererTestFixture, "Renderer State Management", "[Renderer]") {
    SECTION("Clear color") {
        glm::vec4 color(1.0f, 0.0f, 0.0f, 1.0f);
        REQUIRE_NOTHROW(renderer->setClearColor(color));
    }

    SECTION("Viewport setting") {
        REQUIRE_NOTHROW(renderer->setViewport(0, 0, 800, 600));
    }

    SECTION("Clear operations") {
        REQUIRE_NOTHROW(renderer->clear(GL_COLOR_BUFFER_BIT));
        REQUIRE_NOTHROW(renderer->clear(GL_DEPTH_BUFFER_BIT));
        REQUIRE_NOTHROW(renderer->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }
}

TEST_CASE_METHOD(RendererTestFixture, "Renderer Frame Management", "[Renderer]") {
    SECTION("Begin/End frame") {
        REQUIRE_NOTHROW(renderer->beginFrame());
        REQUIRE_NOTHROW(renderer->endFrame());
    }

    SECTION("Submit single mesh") {
        renderer->beginFrame();
        glm::mat4 transform = glm::mat4(1.0f);
        REQUIRE_NOTHROW(renderer->submit(mesh, shader, transform));
        REQUIRE_NOTHROW(renderer->endFrame());
        
        auto stats = renderer->getStatistics();
        REQUIRE(stats.drawCalls == 1);
        REQUIRE(stats.vertices == 3);
        REQUIRE(stats.indices == 3);
    }

    SECTION("Submit multiple meshes") {
        renderer->beginFrame();
        glm::mat4 transform = glm::mat4(1.0f);
        REQUIRE_NOTHROW(renderer->submit(mesh, shader, transform));
        REQUIRE_NOTHROW(renderer->submit(mesh, shader, transform));
        REQUIRE_NOTHROW(renderer->endFrame());
        
        auto stats = renderer->getStatistics();
        REQUIRE(stats.drawCalls == 2);
        REQUIRE(stats.vertices == 6);
        REQUIRE(stats.indices == 6);
    }

    SECTION("Submit instanced meshes") {
        renderer->beginFrame();
        std::vector<glm::mat4> transforms = {
            glm::mat4(1.0f),
            glm::mat4(1.0f),
            glm::mat4(1.0f)
        };
        REQUIRE_NOTHROW(renderer->submit(mesh, shader, transforms));
        REQUIRE_NOTHROW(renderer->endFrame());
        
        auto stats = renderer->getStatistics();
        REQUIRE(stats.drawCalls == 3);
        REQUIRE(stats.vertices == 9);
        REQUIRE(stats.indices == 9);
    }
}

TEST_CASE_METHOD(RendererTestFixture, "Renderer Framebuffer Management", "[Renderer]") {
    SECTION("Set target framebuffer") {
        REQUIRE_NOTHROW(renderer->setTargetFramebuffer(framebuffer));
    }

    SECTION("Reset target framebuffer") {
        REQUIRE_NOTHROW(renderer->setTargetFramebuffer(framebuffer));
        REQUIRE_NOTHROW(renderer->resetTargetFramebuffer());
    }

    SECTION("Render to framebuffer") {
        renderer->setTargetFramebuffer(framebuffer);
        renderer->beginFrame();
        glm::mat4 transform = glm::mat4(1.0f);
        REQUIRE_NOTHROW(renderer->submit(mesh, shader, transform));
        REQUIRE_NOTHROW(renderer->endFrame());
    }
}

TEST_CASE_METHOD(RendererTestFixture, "Renderer Performance Benchmarks", "[Renderer][Benchmark]") {
    SECTION("Frame management") {
        BENCHMARK("Renderer::beginFrame()") {
            renderer->beginFrame();
        };

        BENCHMARK("Renderer::endFrame()") {
            renderer->endFrame();
        };
    }

    SECTION("Submit operations") {
        glm::mat4 transform = glm::mat4(1.0f);
        
        BENCHMARK("Renderer::submit() - Single mesh") {
            renderer->submit(mesh, shader, transform);
        };

        BENCHMARK("Renderer::submit() - Multiple meshes (100)") {
            for (int i = 0; i < 100; ++i) {
                renderer->submit(mesh, shader, transform);
            }
        };

        BENCHMARK("Renderer::submit() - Instanced meshes (100)") {
            std::vector<glm::mat4> transforms(100, transform);
            renderer->submit(mesh, shader, transforms);
        };
    }

    SECTION("Framebuffer operations") {
        BENCHMARK("Renderer::setTargetFramebuffer()") {
            renderer->setTargetFramebuffer(framebuffer);
        };

        BENCHMARK("Renderer::resetTargetFramebuffer()") {
            renderer->resetTargetFramebuffer();
        };
    }
}

} // namespace Test
} // namespace SFE 