#include "shape.h"

#include <limits>
#include "../collider3d.h"

Shape::Shape()
{

}

RayIntersection Shape::raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length, uint64_t layers){
    if(!collider || (layers & collider->collisionLayers) == 0) return RayIntersection();

    RayIntersection intersection = this->raycast(origin, direction, length);
    if (intersection.intersectionExists && intersection.collider == nullptr) {
        intersection.collider = collider;
    }
    return intersection;
}

bool Shape::computeAabb(const Collider3D& collider, PhysicsAabb& outAabb) const {
    glm::mat4 model = collider.getGlobalMatrix();

    if (!vertices.empty()) {
        outAabb.min = glm::vec3(std::numeric_limits<float>::infinity());
        outAabb.max = glm::vec3(-std::numeric_limits<float>::infinity());

        for (const glm::vec3& vertex : vertices) {
            glm::vec3 worldVertex = glm::vec3(model * glm::vec4(vertex, 1.0f));
            outAabb.min = glm::min(outAabb.min, worldVertex);
            outAabb.max = glm::max(outAabb.max, worldVertex);
        }
        return true;
    }

    glm::vec3 center = collider.getGlobalPosition();
    glm::vec3 extents(radius);
    outAabb.min = center - extents;
    outAabb.max = center + extents;
    return radius >= 0.0f;
}
