#pragma once

#include "common/Utilities/colliderIntersection.h"
#include "node3d.h"
RayIntersection raycast(glm::vec3 const &origin, glm::vec3 const &direction, float const &length);

class RigidBody3D: public Node3d
{
public:
    RigidBody3D();
    glm::vec3 gravity = glm::vec3(0);
    virtual void process(float deltaTime) override {};
    virtual void physicsProcess();
    virtual void interpolate(float alpha);
    glm::vec3 getPhysicsPosition() const { return currentPosition; }
    glm::quat getPhysicsRotation() const { return currentRotation; }
    void setPhysicsPosition(const glm::vec3& position);
    void setPhysicsRotation(const glm::quat& rotation);
    void syncTransformToPhysicsState();
    bool isOnGround() const { return onGround; }
    void setOnGround(bool value) { onGround = value; }
    void setLocalRotation(const glm::quat rotation) override;
    void setLocalPosition(const glm::vec3 pos) override;
    void postPhysicsProcess();
    void solveCollision(ColliderIntersection collision);
    void Translate(const glm::vec3 translation) override;
    void setGlobalPosition(const glm::vec3 &globPos) override;
    void setGlobalRotation(const glm::quat &globRot) override;
    void addCollisionShape(CollisionShape3D* c);
    glm::vec3 getMomentum(){return velocity * mass;}
    float getKineticEnergy(){return 0.5f*mass*glm::dot(velocity,velocity);}
    glm::vec3 velocity = glm::vec3(0);
    glm::vec3 angularVelocity = glm::vec3(0);
    float mass = 0;
    float friction = 0.5f;
    glm::mat3 inertia=glm::mat3(0);
    glm::mat3 inverseInertia=glm::mat3(0);
    std::vector<CollisionShape3D*> collisions = std::vector<CollisionShape3D*>();
    glm::vec3 getTorque(glm::vec3 point, glm::vec3 momentum){return glm::cross(point-getGlobalPosition(), momentum);}
    void applyImpulse(glm::vec3 impulse, glm::vec3 worldPoint);

    glm::vec3 unlockedRotation = glm::vec3(1);
    void setUnlockedRotation(bool x, bool y, bool z){unlockedRotation.x = x; unlockedRotation.y = y; unlockedRotation.z = z;}

protected:
    glm::vec3 previousPosition = glm::vec3(0);
    glm::vec3 currentPosition = glm::vec3(0);
    glm::quat previousRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::quat currentRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    bool onGround = false;

};
