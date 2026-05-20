#pragma once
#include "shape.h"
#include "../3dEntities/collisionshape3d.h"

class Sphere : public Shape
{
public:
    Sphere(float rad);
    RayIntersection raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length) override;
    bool computeAabb(const CollisionShape3D& collider, PhysicsAabb& outAabb) const override;
};
