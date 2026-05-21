#pragma once

#include "Cube.h"
#include <common/Physics/rigidbody3d.h>

inline RigidBody3D* makePhysicsCube(){
    RigidBody3D* physicsCube = new RigidBody3D();
    Node3d* child = makeCube();
    physicsCube->addChild(child);
    physicsCube->gravity = glm::vec3(0, -20, 0);
    physicsCube->name = "rigidbody cube";
    physicsCube->addCollider(((Collider3D*)(child->getChildren()[0])));
    return physicsCube;
}
