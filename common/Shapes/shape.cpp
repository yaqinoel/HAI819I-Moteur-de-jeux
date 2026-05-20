#include "shape.h"

#include <limits>
#include "common/3dEntities/collisionshape3d.h"

Shape::Shape()
{

}

RayIntersection Shape::raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length, uint64_t layers){
    if((layers & collider->collisionLayers) == 0) return RayIntersection();
    else return this->raycast(origin, direction, length);
}

bool Shape::computeAabb(const CollisionShape3D& collider, PhysicsAabb& outAabb) const {
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
