#pragma once

#include <vector>
#include "../collisiondispatcher.h"
#include "../physicstypes.h"

class Collider3D;

class NarrowPhase {
public:
    NarrowPhase();

    void generateContacts(const std::vector<CollisionPair>& pairs,
                          std::vector<PhysicsContact>& outContacts) const;
    void collide(Collider3D* colliderA,
                 Collider3D* colliderB,
                 std::vector<PhysicsContact>& outContacts) const;

private:
    CollisionDispatcher dispatcher;
};
