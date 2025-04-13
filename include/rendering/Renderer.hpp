#pragma once

#include <glad/glad.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "rendering/Shader.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/Texture.hpp"
#include "rendering/Framebuffer.hpp"
#include "rendering/GLContext.hpp"

namespace SFE {

class Renderer {
public:
    Renderer();
    ~Renderer();

    // Rule of five: Prevent copying/moving
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    // Initialization and cleanup
    void initialize();
    void shutdown();

    // State management
    void setClearColor(const glm::vec4& color);
    void setViewport(int x, int y, int width, int height);
    void clear(GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rendering commands
    void beginFrame();
    void endFrame();
    void submit(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader, const glm::mat4& transform = glm::mat4(1.0f));
    void submit(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader, const std::vector<glm::mat4>& transforms);
    void flush();

    // Framebuffer management
    void setTargetFramebuffer(std::shared_ptr<Framebuffer> framebuffer);
    void resetTargetFramebuffer();

    // Statistics
    struct Statistics {
        uint32_t drawCalls = 0;
        uint32_t vertices = 0;
        uint32_t indices = 0;
        uint32_t batches = 0;
    };

    const Statistics& getStatistics() const { return statistics; }
    void resetStatistics();

private:
    struct RenderCommand {
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Shader> shader;
        std::vector<glm::mat4> transforms;
    };

    struct Batch {
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Shader> shader;
        std::vector<glm::mat4> transforms;
    };

    void processRenderCommands();
    void renderBatch(const Batch& batch);
    void createBatch(const RenderCommand& command);
    void flushBatch(const Batch& batch);

    std::unique_ptr<GLContext> context;
    std::shared_ptr<Framebuffer> currentFramebuffer;
    std::vector<RenderCommand> renderCommands;
    std::vector<Batch> batches;
    Statistics statistics;
    glm::vec4 clearColor;
    bool initialized;
};

} // namespace SFE 