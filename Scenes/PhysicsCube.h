#pragma once

#include "Cube.h"
#include <common/3dEntities/rigidbody3d.h>

inline RigidBody3D* makePhysicsCube(){
    RigidBody3D* physicsCube = new RigidBody3D();
    Node3d* child = makeCube();
    physicsCube->addChild(child);
    physicsCube->gravity = glm::vec3(0, 0, 0);
    physicsCube->name = "rigidbody cube";
    return physicsCube;
}
