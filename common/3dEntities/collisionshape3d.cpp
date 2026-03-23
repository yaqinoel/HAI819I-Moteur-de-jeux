#include "collisionshape3d.h"

CollisionShape3D::CollisionShape3D()
{

}
RayIntersection CollisionShape3D::raycast(glm::vec3 const &origin, glm::vec3 const &direction, float const &length){
    if(shape != nullptr){
        std::cout << "a" << std::endl;
        return shape->raycast(origin, direction, length);
        std::cout << "b" << std::endl;
    }
    else{
        std::cerr << "Error no shape associated with the CollisionShape" << std::endl;
        RayIntersection closestIntersection;
        closestIntersection.t = FLT_MAX;
        closestIntersection.intersectionExists = false;
        return closestIntersection;
    }
}

void CollisionShape3D::SetShape(Shape* s){
    shape = s;
    s->collider = this;
}
