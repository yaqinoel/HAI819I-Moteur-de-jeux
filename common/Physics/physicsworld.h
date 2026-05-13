#pragma once

#include <cstdint>
#include <unordered_map>
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
    std::uint64_t key = 0;
    float normalImpulse = 0.0f;
    glm::vec3 tangentImpulse = glm::vec3(0.0f);
};

struct CachedContactImpulse {
    float normalImpulse = 0.0f;
    glm::vec3 tangentImpulse = glm::vec3(0.0f);
    int lastSeenFrame = 0;
};

class PhysicsWorld {
public:
    void step(const std::vector<RigidBody3D*>& rigidBodies,
              const std::vector<CollisionShape3D*>& colliders,
              float fixedDeltaTime);

private:
    std::vector<PhysicsContact> contacts;
    std::unordered_map<std::uint64_t, CachedContactImpulse> impulseCache;
    int solverFrame = 0;

    void syncBodies(const std::vector<RigidBody3D*>& rigidBodies) const;
    void collectContacts(const std::vector<CollisionShape3D*>& colliders,
                         std::vector<PhysicsContact>& outContacts) const;
    void prepareContacts(std::vector<PhysicsContact>& contactsToPrepare) const;
    void wakeImpactedSleepingBodies(const std::vector<PhysicsContact>& contactsToCheck) const;
    void wakeUnstableSleepingBodies(const std::vector<RigidBody3D*>& rigidBodies,
                                    const std::vector<PhysicsContact>& contactsToCheck) const;
    void warmStartContacts(std::vector<PhysicsContact>& contactsToWarm) const;
    void storeContactImpulses(const std::vector<PhysicsContact>& contactsToStore);
    bool boxAabb(CollisionShape3D* collider, PhysicsAabb& outAabb) const;
    void addBoxBoxContacts(CollisionShape3D* colliderA,
                           CollisionShape3D* colliderB,
                           std::vector<PhysicsContact>& outContacts) const;
    void addBoxVoxelContacts(CollisionShape3D* boxCollider,
                             CollisionShape3D* voxelCollider,
                             std::vector<PhysicsContact>& outContacts) const;
    void solvePositions(const std::vector<PhysicsContact>& contacts) const;
    void solveVelocities(std::vector<PhysicsContact>& contacts) const;
};
