#pragma once
#include "shape.h"
#include "../3dEntities/collisionshape3d.h"

class Cube : public Shape
{
public:
    Cube(float size_x, float size_y, float size_z);
    RayIntersection raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length) override;
private:
    float size_x;
    float size_y;
    float size_z;
};
