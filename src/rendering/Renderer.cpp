#include "rendering/Renderer.hpp"
#include <stdexcept>
#include <iostream>

namespace SFE {

Renderer::Renderer() 
    : clearColor(0.0f, 0.0f, 0.0f, 1.0f), initialized(false) {
    context = std::make_unique<GLContext>();
}

Renderer::~Renderer() {
    if (initialized) {
        shutdown();
    }
}

void Renderer::initialize() {
    if (initialized) {
        return;
    }

    // Set initial OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    initialized = true;
}

void Renderer::shutdown() {
    if (!initialized) {
        return;
    }

    // Clean up resources
    renderCommands.clear();
    batches.clear();
    currentFramebuffer.reset();

    initialized = false;
}

void Renderer::setClearColor(const glm::vec4& color) {
    clearColor = color;
    glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer::setViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void Renderer::clear(GLbitfield mask) {
    if (currentFramebuffer) {
        currentFramebuffer->clear(mask);
    } else {
        glClear(mask);
    }
}

void Renderer::beginFrame() {
    if (!initialized) {
        throw std::runtime_error("Renderer not initialized");
    }

    resetStatistics();
    renderCommands.clear();
    batches.clear();
}

void Renderer::endFrame() {
    if (!initialized) {
        throw std::runtime_error("Renderer not initialized");
    }

    processRenderCommands();
    flush();
}

void Renderer::submit(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader, const glm::mat4& transform) {
    if (!mesh || !shader) {
        throw std::runtime_error("Invalid mesh or shader");
    }

    RenderCommand command;
    command.mesh = mesh;
    command.shader = shader;
    command.transforms.push_back(transform);
    renderCommands.push_back(command);
}

void Renderer::submit(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader, const std::vector<glm::mat4>& transforms) {
    if (!mesh || !shader || transforms.empty()) {
        throw std::runtime_error("Invalid mesh, shader, or transforms");
    }

    RenderCommand command;
    command.mesh = mesh;
    command.shader = shader;
    command.transforms = transforms;
    renderCommands.push_back(command);
}

void Renderer::flush() {
    for (const auto& batch : batches) {
        flushBatch(batch);
    }
    batches.clear();
}

void Renderer::setTargetFramebuffer(std::shared_ptr<Framebuffer> framebuffer) {
    currentFramebuffer = framebuffer;
    if (framebuffer) {
        framebuffer->bind();
        framebuffer->setViewport();
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void Renderer::resetTargetFramebuffer() {
    setTargetFramebuffer(nullptr);
}

void Renderer::processRenderCommands() {
    for (const auto& command : renderCommands) {
        createBatch(command);
    }
}

void Renderer::createBatch(const RenderCommand& command) {
    // Find existing batch with same mesh and shader
    auto it = std::find_if(batches.begin(), batches.end(),
        [&](const Batch& batch) {
            return batch.mesh == command.mesh && batch.shader == command.shader;
        });

    if (it != batches.end()) {
        // Append transforms to existing batch
        it->transforms.insert(it->transforms.end(), command.transforms.begin(), command.transforms.end());
    } else {
        // Create new batch
        Batch batch;
        batch.mesh = command.mesh;
        batch.shader = command.shader;
        batch.transforms = command.transforms;
        batches.push_back(batch);
        statistics.batches++;
    }
}

void Renderer::flushBatch(const Batch& batch) {
    batch.shader->use();
    
    // Set common uniforms
    batch.shader->setMat4("view", glm::mat4(1.0f));
    batch.shader->setMat4("projection", glm::mat4(1.0f));

    // Render each instance
    for (const auto& transform : batch.transforms) {
        batch.shader->setMat4("model", transform);
        batch.mesh->draw();
        
        statistics.drawCalls++;
        statistics.vertices += batch.mesh->getVertexCount();
        statistics.indices += batch.mesh->getIndexCount();
    }
}

void Renderer::resetStatistics() {
    statistics = Statistics();
}

} // namespace SFE 