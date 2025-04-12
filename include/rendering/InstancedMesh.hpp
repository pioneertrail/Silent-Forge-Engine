#pragma once
#include "Mesh.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace SFE {
class InstancedMesh : public Mesh {
public:
    InstancedMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~InstancedMesh();

    void updateInstanceData(const std::vector<glm::mat4>& modelMatrices);
    void drawInstanced(unsigned int instanceCount) const;

private:
    unsigned int instanceVBO;
    void setupInstanceVBO();
};
} 