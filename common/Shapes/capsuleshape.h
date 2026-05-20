#pragma once

#include "shape.h"
#include "../3dEntities/collisionshape3d.h"

class CapsuleShape : public Shape
{
public:
    CapsuleShape(float radius = 0.5f, float height = 2.0f);

    RayIntersection raycast(glm::vec3 const& origin,
                            glm::vec3 const& direction,
                            float const& length) override;
    bool computeAabb(const CollisionShape3D& collider, PhysicsAabb& outAabb) const override;

    float capsuleRadius = 0.5f;
    float height = 2.0f;
    float halfSegment = 0.5f;
};
