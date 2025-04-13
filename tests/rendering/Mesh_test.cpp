#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "rendering/Mesh.hpp"
#include "rendering/Texture.hpp"
#include "helpers/GLContextHelper.hpp"
#include <vector>
#include <string>

using namespace SFE::Rendering;
using namespace Catch::Matchers;

TEST_CASE("Mesh Class", "[rendering][mesh]") {
    // Initialize OpenGL context for testing
    REQUIRE(SFE::Testing::GLContextHelper::init());

    SECTION("Mesh Construction and Destruction") {
        Mesh mesh;
        REQUIRE(mesh.getVAO() == 0);
        REQUIRE(mesh.getVBO() == 0);
        REQUIRE(mesh.getEBO() == 0);
        REQUIRE(mesh.getVertexCount() == 0);
        REQUIRE(mesh.getIndexCount() == 0);
    }

    SECTION("Mesh Data Loading") {
        std::vector<std::string> errorMessages;
        auto logCallback = [&errorMessages](const std::string& msg) {
            errorMessages.push_back(msg);
        };

        SECTION("Load Empty Data") {
            Mesh mesh;
            std::vector<float> vertices;
            std::vector<unsigned int> indices;
            REQUIRE_FALSE(mesh.loadData(vertices, indices, logCallback));
            REQUIRE_FALSE(errorMessages.empty());
        }

        SECTION("Load Valid Data") {
            Mesh mesh;
            std::vector<float> vertices = {
                // Position        // Normal          // TexCoord
                -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
                 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
                -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f
            };
            std::vector<unsigned int> indices = {
                0, 1, 2,
                2, 3, 0
            };
            REQUIRE(mesh.loadData(vertices, indices, logCallback));
            REQUIRE(errorMessages.empty());
            REQUIRE(mesh.getVAO() != 0);
            REQUIRE(mesh.getVBO() != 0);
            REQUIRE(mesh.getEBO() != 0);
            REQUIRE(mesh.getVertexCount() == 4);
            REQUIRE(mesh.getIndexCount() == 6);
        }
    }

    SECTION("Mesh Rendering") {
        std::vector<std::string> errorMessages;
        auto logCallback = [&errorMessages](const std::string& msg) {
            errorMessages.push_back(msg);
        };

        Mesh mesh;
        std::vector<float> vertices = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f
        };
        std::vector<unsigned int> indices = {
            0, 1, 2,
            2, 3, 0
        };
        REQUIRE(mesh.loadData(vertices, indices, logCallback));

        SECTION("Draw Without Texture") {
            REQUIRE(mesh.draw(logCallback));
            REQUIRE(errorMessages.empty());
        }

        SECTION("Render With Texture") {
            Texture texture;
            REQUIRE(texture.loadFromFile("resources/textures/test.png", logCallback));
            mesh.setTexture(&texture);
            REQUIRE(mesh.render(logCallback));
            REQUIRE(errorMessages.empty());
        }
    }

    SECTION("Mesh State Validation") {
        std::vector<std::string> errorMessages;
        auto logCallback = [&errorMessages](const std::string& msg) {
            errorMessages.push_back(msg);
        };

        Mesh mesh;
        std::vector<float> vertices = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f
        };
        std::vector<unsigned int> indices = {
            0, 1, 2,
            2, 3, 0
        };
        REQUIRE(mesh.loadData(vertices, indices, logCallback));

        SECTION("Buffer State") {
            GLint vaoBinding, vboBinding, eboBinding;
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vaoBinding);
            glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vboBinding);
            glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &eboBinding);

            REQUIRE(vaoBinding == mesh.getVAO());
            REQUIRE(vboBinding == mesh.getVBO());
            REQUIRE(eboBinding == mesh.getEBO());
        }

        SECTION("Vertex Attributes") {
            GLint enabled;
            glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
            REQUIRE(enabled == GL_TRUE);

            GLint size;
            glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size);
            REQUIRE(size == 3); // Position

            glGetVertexAttribiv(1, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size);
            REQUIRE(size == 3); // Normal

            glGetVertexAttribiv(2, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size);
            REQUIRE(size == 2); // TexCoord
        }
    }

    // Cleanup
    SFE::Testing::GLContextHelper::cleanup();
} 