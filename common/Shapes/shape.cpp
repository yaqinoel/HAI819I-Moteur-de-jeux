#include "shape.h"
#include "common/3dEntities/collisionshape3d.h"

Shape::Shape()
{

}

RayIntersection Shape::raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length, uint64_t layers){
    if((layers & collider->collisionLayers) == 0) return RayIntersection();
    else return this->raycast(origin, direction, length);
}
