#include "rendering/RenderPipeline.hpp"
#include <algorithm>

namespace SFE {

RenderPipeline::RenderPipeline()
    : needsSorting(true) {
}

RenderPipeline::~RenderPipeline() {
    clear();
}

bool RenderPipeline::initialize() {
    // Initialize any required OpenGL states
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    return true;
}

void RenderPipeline::addRenderable(std::shared_ptr<Renderable> renderable) {
    if (renderable) {
        renderables.push_back(renderable);
        needsSorting = true;
    }
}

void RenderPipeline::removeRenderable(std::shared_ptr<Renderable> renderable) {
    auto it = std::find(renderables.begin(), renderables.end(), renderable);
    if (it != renderables.end()) {
        renderables.erase(it);
        needsSorting = true;
    }
}

void RenderPipeline::setViewMatrix(const glm::mat4& view) {
    viewMatrix = view;
}

void RenderPipeline::setProjectionMatrix(const glm::mat4& projection) {
    projectionMatrix = projection;
}

void RenderPipeline::render() {
    if (needsSorting) {
        sortRenderables();
        needsSorting = false;
    }

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render each batch
    for (const auto& [shader, batch] : renderBatches) {
        renderBatch(batch);
    }
}

void RenderPipeline::clear() {
    renderables.clear();
    renderBatches.clear();
    needsSorting = true;
}

void RenderPipeline::sortRenderables() {
    renderBatches.clear();

    // Group renderables by shader
    for (const auto& renderable : renderables) {
        auto material = renderable->getMaterial();
        if (material) {
            auto shader = material->getShader();
            renderBatches[shader].push_back(renderable);
        }
    }
}

void RenderPipeline::renderBatch(const std::vector<std::shared_ptr<Renderable>>& batch) {
    if (batch.empty()) return;

    // Get the shader from the first renderable's material
    auto material = batch[0]->getMaterial();
    if (!material) return;

    auto shader = material->getShader();
    if (!shader) return;

    // Bind the shader and set common uniforms
    shader->use();
    shader->setMat4("view", viewMatrix);
    shader->setMat4("projection", projectionMatrix);

    // Render each object in the batch
    for (const auto& renderable : batch) {
        renderable->prepare();
        renderable->bind();
        renderable->draw();
    }
}

} // namespace SFE 