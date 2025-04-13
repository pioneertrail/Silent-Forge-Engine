#include "rendering/Frustum.hpp"
#include <glm/gtc/matrix_access.hpp>

namespace SFE {

void Frustum::update(const glm::mat4& viewProj) {
    // Extract planes from the view-projection matrix
    // Left plane
    planes[0].normal.x = viewProj[0][3] + viewProj[0][0];
    planes[0].normal.y = viewProj[1][3] + viewProj[1][0];
    planes[0].normal.z = viewProj[2][3] + viewProj[2][0];
    planes[0].distance = viewProj[3][3] + viewProj[3][0];
    normalizePlane(planes[0]);

    // Right plane
    planes[1].normal.x = viewProj[0][3] - viewProj[0][0];
    planes[1].normal.y = viewProj[1][3] - viewProj[1][0];
    planes[1].normal.z = viewProj[2][3] - viewProj[2][0];
    planes[1].distance = viewProj[3][3] - viewProj[3][0];
    normalizePlane(planes[1]);

    // Bottom plane
    planes[2].normal.x = viewProj[0][3] + viewProj[0][1];
    planes[2].normal.y = viewProj[1][3] + viewProj[1][1];
    planes[2].normal.z = viewProj[2][3] + viewProj[2][1];
    planes[2].distance = viewProj[3][3] + viewProj[3][1];
    normalizePlane(planes[2]);

    // Top plane
    planes[3].normal.x = viewProj[0][3] - viewProj[0][1];
    planes[3].normal.y = viewProj[1][3] - viewProj[1][1];
    planes[3].normal.z = viewProj[2][3] - viewProj[2][1];
    planes[3].distance = viewProj[3][3] - viewProj[3][1];
    normalizePlane(planes[3]);

    // Near plane
    planes[4].normal.x = viewProj[0][3] + viewProj[0][2];
    planes[4].normal.y = viewProj[1][3] + viewProj[1][2];
    planes[4].normal.z = viewProj[2][3] + viewProj[2][2];
    planes[4].distance = viewProj[3][3] + viewProj[3][2];
    normalizePlane(planes[4]);

    // Far plane
    planes[5].normal.x = viewProj[0][3] - viewProj[0][2];
    planes[5].normal.y = viewProj[1][3] - viewProj[1][2];
    planes[5].normal.z = viewProj[2][3] - viewProj[2][2];
    planes[5].distance = viewProj[3][3] - viewProj[3][2];
    normalizePlane(planes[5]);
}

bool Frustum::isPointInside(const glm::vec3& point) const {
    for (const auto& plane : planes) {
        if (glm::dot(plane.normal, point) + plane.distance < 0) {
            return false;
        }
    }
    return true;
}

bool Frustum::isSphereInside(const glm::vec3& center, float radius) const {
    for (const auto& plane : planes) {
        if (glm::dot(plane.normal, center) + plane.distance < -radius) {
            return false;
        }
    }
    return true;
}

bool Frustum::isBoxInside(const glm::vec3& min, const glm::vec3& max) const {
    for (const auto& plane : planes) {
        glm::vec3 positiveVertex = min;
        glm::vec3 negativeVertex = max;

        if (plane.normal.x >= 0) {
            positiveVertex.x = max.x;
            negativeVertex.x = min.x;
        }
        if (plane.normal.y >= 0) {
            positiveVertex.y = max.y;
            negativeVertex.y = min.y;
        }
        if (plane.normal.z >= 0) {
            positiveVertex.z = max.z;
            negativeVertex.z = min.z;
        }

        if (glm::dot(plane.normal, negativeVertex) + plane.distance < 0) {
            return false;
        }
    }
    return true;
}

void Frustum::normalizePlane(Plane& plane) {
    float length = glm::length(plane.normal);
    plane.normal /= length;
    plane.distance /= length;
}

} 