#include "rigidbody3d.h"
#include "collider3d.h"
#include <cmath>

RigidBody3D::RigidBody3D()
{
    previousPosition = getGlobalPosition();
    currentPosition = getGlobalPosition();
    previousRotation = getGlobalRotation();
    currentRotation = getGlobalRotation();
}

void RigidBody3D::physicsProcess(){
    previousPosition = currentPosition;
    previousRotation = currentRotation;
    velocity += gravity*scene->fixedDeltaTime;
}

void RigidBody3D::postPhysicsProcess(){
    currentPosition += velocity*scene->fixedDeltaTime;
    if(angularVelocity != glm::vec3(0)){
        glm::vec3 axis = glm::normalize(angularVelocity);
        float angle = glm::length(angularVelocity) * scene->fixedDeltaTime;
        glm::quat deltaRot = glm::angleAxis(angle, axis);
        currentRotation = glm::normalize(deltaRot * currentRotation);
    }
}

void RigidBody3D::setPhysicsPosition(const glm::vec3& position) {
    currentPosition = position;
}

void RigidBody3D::setPhysicsRotation(const glm::quat& rotation) {
    currentRotation = glm::normalize(rotation);
}

void RigidBody3D::syncTransformToPhysicsState() {
    Node3d::setGlobalPosition(currentPosition);
    Node3d::setGlobalRotation(currentRotation);
}

void RigidBody3D::keepPhysicsStateForSleep() {
    previousPosition = currentPosition;
    previousRotation = currentRotation;
    syncTransformToPhysicsState();
}

void RigidBody3D::wakeUp() {
    sleeping = false;
    sleepTimer = 0.0f;
}

void RigidBody3D::sleep() {
    if (!canSleep)
        return;
    sleeping = true;
    sleepTimer = 0.0f;
    velocity = glm::vec3(0.0f);
    angularVelocity = glm::vec3(0.0f);
    keepPhysicsStateForSleep();
}

void RigidBody3D::interpolate(float alpha){
    //std::cout << glm::length(position - glm::mix(previousPosition, currentPosition, alpha)) << std::endl;
    Node3d::setGlobalPosition(glm::mix(previousPosition, currentPosition, std::min(1.0f,alpha)));
    Node3d::setGlobalRotation(glm::mix(previousRotation, currentRotation, std::min(1.0f,alpha)));
}
void RigidBody3D::Translate(const glm::vec3 translation){
    wakeUp();
    currentPosition  += translation;
    markDirty();
}

void RigidBody3D::setGlobalPosition(const glm::vec3 &globPos){
    wakeUp();
    Node3d::setGlobalPosition(globPos);
    currentPosition  = globPos;
    previousPosition = globPos;
    markDirty();
}


void RigidBody3D::setGlobalRotation(const glm::quat &globRot){
    wakeUp();
    Node3d::setGlobalRotation(globRot);
    currentRotation  = globRot;
    previousRotation = globRot;
    markDirty();
}

void RigidBody3D::setLocalRotation(const glm::quat rotation){
    wakeUp();
    Node3d::setLocalRotation(rotation);
    currentRotation = getGlobalRotation();
    markDirty();
}

void RigidBody3D::setLocalPosition(const glm::vec3 pos){
    wakeUp();
    Node3d::setLocalPosition(pos);
    currentPosition = getGlobalPosition();
    markDirty();
}

void RigidBody3D::addCollider(Collider3D* c){
    colliders.push_back(c);
    c->rb = this;
    mass += c->mass;
    glm::vec3 colliderCenterOfMass = glm::vec3(c->getGlobalMatrix() * glm::vec4(c->getShape()->getLocalCenterOfMass(), 1.0f));
    glm::vec3 offset = colliderCenterOfMass - getGlobalPosition();
    glm::mat3 parallelAxis = c->mass * (glm::dot(offset, offset) * glm::mat3(1.0f) - glm::outerProduct(offset, offset));
    inertia += c->getInertia() + parallelAxis;

    float determinant = glm::determinant(inertia);
    if(std::abs(determinant) > 1e-8f)
        inverseInertia = glm::inverse(inertia);
    else
        inverseInertia = glm::mat3(0.0f);
}

void RigidBody3D::applyImpulse(glm::vec3 impulse, glm::vec3 worldPoint){
    if(mass != 0 && impulse != glm::vec3(0)){
        wakeUp();
        velocity += impulse / mass;

        glm::vec3 r = worldPoint - currentPosition;
        glm::vec3 angularImpulse = glm::cross(r, impulse);

        glm::mat3 rotation = glm::mat3_cast(currentRotation);
        glm::mat3 worldInverseInertia = rotation * inverseInertia * glm::transpose(rotation);
        angularVelocity += (worldInverseInertia * angularImpulse) * unlockedRotation;
    }
}
