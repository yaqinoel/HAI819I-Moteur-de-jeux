#pragma once

#include <cstdint>
#include <glm/glm.hpp>

class CollisionShape3D;
class RigidBody3D;

struct PhysicsAabb {
    glm::vec3 min = glm::vec3(0.0f);
    glm::vec3 max = glm::vec3(0.0f);
};

struct CollisionPair {
    CollisionShape3D* colliderA = nullptr;
    CollisionShape3D* colliderB = nullptr;
};

struct PhysicsContact {
    RigidBody3D* bodyA = nullptr;
    RigidBody3D* bodyB = nullptr;
    CollisionShape3D* colliderA = nullptr;
    CollisionShape3D* colliderB = nullptr;
    glm::vec3 normal = glm::vec3(0.0f);
    glm::vec3 point = glm::vec3(0.0f);
    float penetration = 0.0f;
    float friction = 0.0f;
    float restitution = 0.0f;
    std::uint64_t key = 0;
    float normalImpulse = 0.0f;
    glm::vec3 tangentImpulse = glm::vec3(0.0f);
};
