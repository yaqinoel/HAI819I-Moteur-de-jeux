#pragma once

#include <array>
#include <vector>
#include "physicstypes.h"
#include "common/Shapes/shape.h"

class CollisionShape3D;

using CollisionFn = void (*)(CollisionShape3D*, CollisionShape3D*, std::vector<PhysicsContact>&);

class CollisionDispatcher {
public:
    CollisionDispatcher();

    void registerCollision(ShapeType typeA, ShapeType typeB, CollisionFn collisionFn);
    bool dispatch(CollisionShape3D* colliderA,
                  CollisionShape3D* colliderB,
                  std::vector<PhysicsContact>& outContacts) const;

private:
    std::array<std::array<CollisionFn, SHAPE_COUNT>, SHAPE_COUNT> table;
};
