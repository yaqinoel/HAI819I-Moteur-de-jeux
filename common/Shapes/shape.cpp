#include "shape.h"
#include "cube.h"

Shape::Shape()
{

}

ColliderIntersection Shape::intersect(Shape* shape){
    if(glm::length(shape->collider->globalPosition()-collider->globalPosition()) < radius+shape->radius){
        if(shape->type == CUBE){
            Cube* c = static_cast<Cube*>(shape);
            if(c){
                return intersectCube(c);
            }
        }
    }
    return ColliderIntersection();
}
