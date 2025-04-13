#pragma once

#include <glm/glm.hpp>
#include <array>

namespace SFE {

class Frustum {
public:
    struct Plane {
        glm::vec3 normal;
        float distance;
    };

    Frustum() = default;
    
    // Update frustum planes from view-projection matrix
    void update(const glm::mat4& viewProj);
    
    // Test if a point is inside the frustum
    bool isPointInside(const glm::vec3& point) const;
    
    // Test if a sphere is inside the frustum
    bool isSphereInside(const glm::vec3& center, float radius) const;
    
    // Test if a box is inside the frustum
    bool isBoxInside(const glm::vec3& min, const glm::vec3& max) const;

private:
    std::array<Plane, 6> planes;
    
    void normalizePlane(Plane& plane);
};

} 