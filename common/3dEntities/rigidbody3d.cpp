#include "rigidbody3d.h"

RigidBody3D::RigidBody3D()
{
}

void RigidBody3D::physicsProcess(float fixedDeltaTime){
    velocity += gravity*fixedDeltaTime;
}

void RigidBody3D::postPhysicsProcess(float fixedDeltaTime){
    position += velocity*fixedDeltaTime;
}
