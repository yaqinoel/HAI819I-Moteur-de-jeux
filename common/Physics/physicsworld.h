#pragma once

#include <vector>
#include "broadphase.h"
#include "contactcache.h"
#include "contactsolver.h"
#include "narrowphase.h"
#include "physicstypes.h"

class CollisionShape3D;
class RigidBody3D;

class PhysicsWorld {
public:
    void step(const std::vector<RigidBody3D*>& rigidBodies,
              const std::vector<CollisionShape3D*>& colliders,
              float fixedDeltaTime);

private:
    std::vector<PhysicsContact> contacts;
    BruteForceBroadPhase broadPhase;
    NarrowPhase narrowPhase;
    ContactSolver contactSolver;
    ContactCache contactCache;

    void syncBodies(const std::vector<RigidBody3D*>& rigidBodies) const;
    void collectContacts(const std::vector<CollisionShape3D*>& colliders,
                         std::vector<PhysicsContact>& outContacts) const;
    void wakeImpactedSleepingBodies(const std::vector<PhysicsContact>& contactsToCheck) const;
    void wakeUnstableSleepingBodies(const std::vector<RigidBody3D*>& rigidBodies,
                                    const std::vector<PhysicsContact>& contactsToCheck) const;
};
