#pragma once

#include <vector>
#include <glm/glm.hpp>

class CollisionShape3D;
class RigidBody3D;

struct PhysicsAabb {
    glm::vec3 min = glm::vec3(0.0f);
    glm::vec3 max = glm::vec3(0.0f);
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
};

class PhysicsWorld {
public:
    void step(const std::vector<RigidBody3D*>& rigidBodies,
              const std::vector<CollisionShape3D*>& colliders,
              float fixedDeltaTime);

private:
    std::vector<PhysicsContact> contacts;

    void syncBodies(const std::vector<RigidBody3D*>& rigidBodies) const;
    void collectContacts(const std::vector<CollisionShape3D*>& colliders,
                         std::vector<PhysicsContact>& outContacts) const;
    bool boxAabb(CollisionShape3D* collider, PhysicsAabb& outAabb) const;
    bool addBoxBoxContact(CollisionShape3D* colliderA,
                          CollisionShape3D* colliderB,
                          std::vector<PhysicsContact>& outContacts) const;
    void addBoxVoxelContacts(CollisionShape3D* boxCollider,
                             CollisionShape3D* voxelCollider,
                             std::vector<PhysicsContact>& outContacts) const;
    bool aabbContact(const PhysicsAabb& a,
                     const PhysicsAabb& b,
                     glm::vec3& normalFromBToA,
                     float& penetration,
                     glm::vec3& point) const;
    void solvePositions(const std::vector<PhysicsContact>& contacts) const;
    void solveVelocities(const std::vector<PhysicsContact>& contacts) const;
};
