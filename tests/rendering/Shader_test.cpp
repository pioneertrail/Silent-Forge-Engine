#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <rendering/Shader.hpp>
#include <rendering/GLContext.hpp>
#include <glm/glm.hpp>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <crtdbg.h>
#include <gtest/gtest.h>
#include <helpers/GLContextHelper.hpp>
#include <sstream>
#include "TestFixture.hpp"
#include <memory>
#include <string>

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

// Test fixture for Shader tests
class ShaderTestFixture {
public:
    ShaderTestFixture() : memoryDetector() {
        // Create test shader files
        createTestShaderFiles();
        context = std::make_unique<GLContext>();
    }

    ~ShaderTestFixture() {
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

    const std::string vertexShaderPath = "test_vertex_shader.glsl";
    const std::string fragmentShaderPath = "test_fragment_shader.glsl";
    std::unique_ptr<GLContext> context;

private:
    MemoryLeakDetector memoryDetector;
};

TEST_CASE_METHOD(ShaderTestFixture, "Shader Construction and Destruction", "[Shader]") {
    SECTION("Valid shader creation") {
        REQUIRE_NOTHROW(Shader shader(vertexShaderPath, fragmentShaderPath));
    }

    SECTION("Invalid shader file paths") {
        REQUIRE_THROWS_AS(Shader("nonexistent.vert", "nonexistent.frag"), std::runtime_error);
    }

    SECTION("Invalid shader source") {
        // Create invalid shader files
        std::ofstream invalidVertex(vertexShaderPath);
        invalidVertex << "invalid glsl code";
        invalidVertex.close();

        REQUIRE_THROWS_AS(Shader(vertexShaderPath, fragmentShaderPath), std::runtime_error);
    }
}

TEST_CASE_METHOD(ShaderTestFixture, "Shader Uniform Functions", "[Shader]") {
    Shader shader(vertexShaderPath, fragmentShaderPath);

    SECTION("Boolean uniforms") {
        REQUIRE_NOTHROW(shader.setBool("someBool", true));
        REQUIRE_NOTHROW(shader.setBool("someBool", false));
    }

    SECTION("Integer uniforms") {
        REQUIRE_NOTHROW(shader.setInt("someInt", 42));
        REQUIRE_NOTHROW(shader.setInt("someInt", -1));
    }

    SECTION("Float uniforms") {
        REQUIRE_NOTHROW(shader.setFloat("someFloat", 3.14f));
        REQUIRE_NOTHROW(shader.setFloat("someFloat", -1.0f));
    }

    SECTION("Vector uniforms") {
        REQUIRE_NOTHROW(shader.setVec2("someVec2", glm::vec2(1.0f, 2.0f)));
        REQUIRE_NOTHROW(shader.setVec2("someVec2", 1.0f, 2.0f));
        
        REQUIRE_NOTHROW(shader.setVec3("someVec3", glm::vec3(1.0f, 2.0f, 3.0f)));
        REQUIRE_NOTHROW(shader.setVec3("someVec3", 1.0f, 2.0f, 3.0f));
        
        REQUIRE_NOTHROW(shader.setVec4("someVec4", glm::vec4(1.0f, 2.0f, 3.0f, 4.0f)));
        REQUIRE_NOTHROW(shader.setVec4("someVec4", 1.0f, 2.0f, 3.0f, 4.0f));
    }

    SECTION("Matrix uniforms") {
        REQUIRE_NOTHROW(shader.setMat2("someMat2", glm::mat2(1.0f)));
        REQUIRE_NOTHROW(shader.setMat3("someMat3", glm::mat3(1.0f)));
        REQUIRE_NOTHROW(shader.setMat4("someMat4", glm::mat4(1.0f)));
    }
}

TEST_CASE_METHOD(ShaderTestFixture, "Shader State Management", "[Shader]") {
    SECTION("Shader program ID") {
        Shader shader(vertexShaderPath, fragmentShaderPath);
        REQUIRE(shader.getID() != 0);
    }

    SECTION("Shader use function") {
        Shader shader(vertexShaderPath, fragmentShaderPath);
        REQUIRE_NOTHROW(shader.use());
    }
}

TEST_CASE_METHOD(ShaderTestFixture, "Shader Error Handling", "[Shader]") {
    SECTION("Invalid uniform names") {
        Shader shader(vertexShaderPath, fragmentShaderPath);
        // These should not throw but should log errors
        REQUIRE_NOTHROW(shader.setBool("nonexistentUniform", true));
        REQUIRE_NOTHROW(shader.setInt("nonexistentUniform", 42));
        REQUIRE_NOTHROW(shader.setFloat("nonexistentUniform", 3.14f));
    }
}

TEST_CASE_METHOD(ShaderTestFixture, "Shader Performance Benchmarks", "[Shader][Benchmark]") {
    Shader shader(vertexShaderPath, fragmentShaderPath);

    BENCHMARK("Shader::use()") {
        shader.use();
    };

    BENCHMARK("Shader::setMat4 (1000 iterations)") {
        for (int i = 0; i < 1000; ++i) {
            shader.setMat4("model", glm::mat4(1.0f));
        }
    };

    BENCHMARK("Shader::setVec3 (1000 iterations)") {
        for (int i = 0; i < 1000; ++i) {
            shader.setVec3("color", 1.0f, 0.0f, 0.0f);
        }
    };

    BENCHMARK("Shader::setFloat (1000 iterations)") {
        for (int i = 0; i < 1000; ++i) {
            shader.setFloat("someFloat", 3.14f);
        }
    };
}

class ShaderTest : public TestFixture {
protected:
    void SetUp() override {
        TestFixture::SetUp();
        shader = std::make_unique<Shader>();
    }

    void TearDown() override {
        shader.reset();
        TestFixture::TearDown();
    }

    std::unique_ptr<Shader> shader;
};

// Basic Shader Operations
TEST_F(ShaderTest, ShaderCreation) {
    EXPECT_FALSE(shader->isCompiled());
    EXPECT_FALSE(shader->isLinked());
}

// Shader Compilation Tests
TEST_F(ShaderTest, VertexShaderCompilation) {
    const std::string vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos, 1.0);
        }
    )";
    
    EXPECT_TRUE(shader->compileVertexShader(vertexSource));
    EXPECT_TRUE(shader->isCompiled());
}

TEST_F(ShaderTest, FragmentShaderCompilation) {
    const std::string fragmentSource = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        }
    )";
    
    EXPECT_TRUE(shader->compileFragmentShader(fragmentSource));
    EXPECT_TRUE(shader->isCompiled());
}

// Shader Linking Tests
TEST_F(ShaderTest, ShaderLinking) {
    const std::string vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos, 1.0);
        }
    )";
    
    const std::string fragmentSource = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        }
    )";
    
    EXPECT_TRUE(shader->compileVertexShader(vertexSource));
    EXPECT_TRUE(shader->compileFragmentShader(fragmentSource));
    EXPECT_TRUE(shader->link());
    EXPECT_TRUE(shader->isLinked());
}

// Uniform Tests
TEST_F(ShaderTest, UniformManagement) {
    const std::string vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        uniform mat4 model;
        void main() {
            gl_Position = model * vec4(aPos, 1.0);
        }
    )";
    
    const std::string fragmentSource = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec3 color;
        void main() {
            FragColor = vec4(color, 1.0);
        }
    )";
    
    EXPECT_TRUE(shader->compileVertexShader(vertexSource));
    EXPECT_TRUE(shader->compileFragmentShader(fragmentSource));
    EXPECT_TRUE(shader->link());
    
    EXPECT_TRUE(shader->hasUniform("model"));
    EXPECT_TRUE(shader->hasUniform("color"));
    EXPECT_FALSE(shader->hasUniform("nonexistent"));
}

// Performance Tests
TEST_F(ShaderTest, Performance_ShaderCompilation) {
    const std::string vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos, 1.0);
        }
    )";
    
    TestPerformance([&]() {
        auto tempShader = std::make_unique<Shader>();
        tempShader->compileVertexShader(vertexSource);
    });
}

TEST_F(ShaderTest, Performance_UniformUpdates) {
    const std::string vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        uniform mat4 model;
        void main() {
            gl_Position = model * vec4(aPos, 1.0);
        }
    )";
    
    EXPECT_TRUE(shader->compileVertexShader(vertexSource));
    EXPECT_TRUE(shader->link());
    
    glm::mat4 model(1.0f);
    TestPerformance([&]() {
        shader->setUniform("model", model);
    });
}

// Thread Safety Tests
TEST_F(ShaderTest, ThreadSafety_UniformUpdates) {
    const std::string vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        uniform mat4 model;
        void main() {
            gl_Position = model * vec4(aPos, 1.0);
        }
    )";
    
    EXPECT_TRUE(shader->compileVertexShader(vertexSource));
    EXPECT_TRUE(shader->link());
    
    TestThreadSafety(*shader, [](Shader& s) {
        glm::mat4 model(1.0f);
        s.setUniform("model", model);
    });
}

// Memory Tests
TEST_F(ShaderTest, MemoryLeak_ShaderCreation) {
    TestMemoryLeak([&]() {
        auto tempShader = std::make_unique<Shader>();
        const std::string vertexSource = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            void main() {
                gl_Position = vec4(aPos, 1.0);
            }
        )";
        tempShader->compileVertexShader(vertexSource);
    });
}

// Edge Cases
TEST_F(ShaderTest, InvalidShaderSource) {
    EXPECT_FALSE(shader->compileVertexShader("invalid source"));
    EXPECT_FALSE(shader->compileFragmentShader("invalid source"));
}

TEST_F(ShaderTest, MissingShaderStage) {
    const std::string vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos, 1.0);
        }
    )";
    
    EXPECT_TRUE(shader->compileVertexShader(vertexSource));
    EXPECT_FALSE(shader->link());
}

TEST_F(ShaderTest, InvalidUniformType) {
    const std::string vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        uniform mat4 model;
        void main() {
            gl_Position = model * vec4(aPos, 1.0);
        }
    )";
    
    EXPECT_TRUE(shader->compileVertexShader(vertexSource));
    EXPECT_TRUE(shader->link());
    
    EXPECT_FALSE(shader->setUniform("model", glm::vec3(1.0f)));
}

} // namespace Test
} // namespace SFE 