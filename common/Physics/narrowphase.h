#pragma once

#include <vector>
#include "physicstypes.h"

class CollisionShape3D;

class NarrowPhase {
public:
    void generateContacts(const std::vector<CollisionPair>& pairs,
                          std::vector<PhysicsContact>& outContacts) const;
    void collide(CollisionShape3D* colliderA,
                 CollisionShape3D* colliderB,
                 std::vector<PhysicsContact>& outContacts) const;

private:
    void addBoxBoxContacts(CollisionShape3D* colliderA,
                           CollisionShape3D* colliderB,
                           std::vector<PhysicsContact>& outContacts) const;
    void addBoxVoxelContacts(CollisionShape3D* boxCollider,
                             CollisionShape3D* voxelCollider,
                             std::vector<PhysicsContact>& outContacts) const;
};
