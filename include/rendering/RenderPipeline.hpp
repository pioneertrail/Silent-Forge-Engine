#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "rendering/Renderable.hpp"
#include "rendering/Shader.hpp"

namespace SFE {

class RenderPipeline {
public:
    RenderPipeline();
    ~RenderPipeline();

    // Initialize the pipeline
    bool initialize();

    // Add a renderable to the pipeline
    void addRenderable(std::shared_ptr<Renderable> renderable);

    // Remove a renderable from the pipeline
    void removeRenderable(std::shared_ptr<Renderable> renderable);

    // Set the view matrix
    void setViewMatrix(const glm::mat4& view);

    // Set the projection matrix
    void setProjectionMatrix(const glm::mat4& projection);

    // Render all objects in the pipeline
    void render();

    // Clear all renderables
    void clear();

private:
    // Sort renderables by shader and material for efficient rendering
    void sortRenderables();

    // Render a batch of objects with the same shader
    void renderBatch(const std::vector<std::shared_ptr<Renderable>>& batch);

    std::vector<std::shared_ptr<Renderable>> renderables;
    std::unordered_map<std::shared_ptr<Shader>, std::vector<std::shared_ptr<Renderable>>> renderBatches;
    
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    
    bool needsSorting;
};

} // namespace SFE 