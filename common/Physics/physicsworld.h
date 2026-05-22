#pragma once

#include <vector>
#include <cstdint>
#include <glm/gtc/quaternion.hpp>
#include "Collision/broadphase.h"
#include "Collision/contactsolver.h"
#include "Collision/narrowphase.h"
#include "contactcache.h"
#include "physicstypes.h"

class Collider3D;
class RigidBody3D;

class PhysicsWorld {
public:
    void step(const std::vector<RigidBody3D*>& rigidBodies,
              const std::vector<Collider3D*>& colliders,
              float fixedDeltaTime);
    std::vector<Collider3D*> cubeOverlapTest(const glm::vec3& center,
                                      const glm::quat& rotation,
                                      const glm::vec3& size,
                                      const std::vector<Collider3D*>& colliders,
                                      std::uint64_t mask = ~0ULL) const;

private:
    std::vector<PhysicsContact> contacts;
    SweepAndPruneBroadPhase broadPhase;
    NarrowPhase narrowPhase;
    ContactSolver contactSolver;
    ContactCache contactCache;

    void syncBodies(const std::vector<RigidBody3D*>& rigidBodies) const;
    void collectContacts(const std::vector<Collider3D*>& colliders,
                         std::vector<PhysicsContact>& outContacts) const;
    void wakeImpactedSleepingBodies(const std::vector<PhysicsContact>& contactsToCheck) const;
    void wakeUnstableSleepingBodies(const std::vector<RigidBody3D*>& rigidBodies,
                                    const std::vector<PhysicsContact>& contactsToCheck) const;
};
