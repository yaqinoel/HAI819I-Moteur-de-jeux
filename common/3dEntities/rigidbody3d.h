#pragma once

#include "node3d.h"
RayIntersection raycast(glm::vec3 const &origin, glm::vec3 const &direction, float const &length);

class RigidBody3D: public Node3d
{
public:
    RigidBody3D();
};

