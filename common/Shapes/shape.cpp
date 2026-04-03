#include "shape.h"
#include "cube.h"

Shape::Shape()
{

}

ColliderIntersection Shape::intersect(Shape* shape, bool calculatePoints){
    if(glm::length(shape->collider->getGlobalPosition()-collider->getGlobalPosition()) < radius+shape->radius){
        if(shape->type == CUBE){
            Cube* c = static_cast<Cube*>(shape);
            if(c){
                return intersectCube(c, calculatePoints);
            }
        }
    }
    return ColliderIntersection();
}
