#pragma once

#include "node3d.h"
RayIntersection raycast(glm::vec3 const &origin, glm::vec3 const &direction, float const &length);

class RigidBody3D: public Node3d
{
public:
    RigidBody3D();
    glm::vec3 gravity = glm::vec3(0);
    virtual void process(float deltaTime) override {};
    virtual void physicsProcess(float fixedDeltaTime);
    virtual void interpolate(float alpha);
    glm::vec3 velocity = glm::vec3(0);
    glm::vec3 previousPosition = glm::vec3(0);
    glm::vec3 currentPosition = glm::vec3(0);
    void postPhysicsProcess(float fixedDeltaTime);
    void Translate(const glm::vec3 translation) override;
    void setPosition(const glm::vec3 translation) override;
};

