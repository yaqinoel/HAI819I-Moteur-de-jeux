#pragma once

#include <vector>
#include "collisiondispatcher.h"
#include "physicstypes.h"

class CollisionShape3D;

class NarrowPhase {
public:
    NarrowPhase();

    void generateContacts(const std::vector<CollisionPair>& pairs,
                          std::vector<PhysicsContact>& outContacts) const;
    void collide(CollisionShape3D* colliderA,
                 CollisionShape3D* colliderB,
                 std::vector<PhysicsContact>& outContacts) const;

private:
    CollisionDispatcher dispatcher;
};
