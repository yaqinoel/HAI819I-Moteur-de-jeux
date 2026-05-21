#pragma once

#include <array>
#include <vector>
#include "physicstypes.h"
#include "Shapes/shape.h"

class Collider3D;

using CollisionFn = void (*)(Collider3D*, Collider3D*, std::vector<PhysicsContact>&);

class CollisionDispatcher {
public:
    CollisionDispatcher();

    void registerCollision(ShapeType typeA, ShapeType typeB, CollisionFn collisionFn);
    bool dispatch(Collider3D* colliderA,
                  Collider3D* colliderB,
                  std::vector<PhysicsContact>& outContacts) const;

private:
    std::array<std::array<CollisionFn, SHAPE_COUNT>, SHAPE_COUNT> table;
};
