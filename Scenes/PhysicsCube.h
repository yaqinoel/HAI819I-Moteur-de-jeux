#pragma once

#include "Cube.h"
#include <common/Physics/rigidbody3d.h>

inline RigidBody3D* makePhysicsCube(Material* material = nullptr){
    RigidBody3D* physicsCube = new RigidBody3D();
    Node3d* child = makeCube(material);
    physicsCube->addChild(child);
    physicsCube->gravity = glm::vec3(0, -20, 0);
    physicsCube->name = "rigidbody cube";
    Collider3D* cubeCollider = ((Collider3D*)(child->getChildren()[0]));
    cubeCollider->mass = 10.0f;
    physicsCube->addCollider(cubeCollider);
    return physicsCube;
}
