#include "rigidbody3d.h"

RigidBody3D::RigidBody3D()
{
    previousPosition = getGlobalPosition();
    currentPosition = getGlobalPosition();
}

void RigidBody3D::physicsProcess(float fixedDeltaTime){
    previousPosition = currentPosition;
    velocity += gravity*fixedDeltaTime;
}

void RigidBody3D::postPhysicsProcess(float fixedDeltaTime){
    currentPosition += velocity*fixedDeltaTime;
}

void RigidBody3D::interpolate(float alpha){
    //std::cout << glm::length(position - glm::mix(previousPosition, currentPosition, alpha)) << std::endl;
    Node3d::setGlobalPosition(glm::mix(previousPosition, currentPosition, std::min(1.0f,alpha)));
}
void RigidBody3D::Translate(const glm::vec3 translation){
    currentPosition  += translation;
    markDirty();
}

void RigidBody3D::setGlobalPosition(const glm::vec3 &globPos){
    currentPosition  = globPos;
    markDirty();
}
