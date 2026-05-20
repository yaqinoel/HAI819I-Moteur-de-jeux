#include "collisiondispatcher.h"

#include <cstddef>
#include "common/3dEntities/collisionshape3d.h"

namespace {

bool isValidShapeType(ShapeType type) {
    int index = static_cast<int>(type);
    return index >= 0 && index < static_cast<int>(SHAPE_COUNT);
}

std::size_t typeIndex(ShapeType type) {
    return static_cast<std::size_t>(type);
}

}

CollisionDispatcher::CollisionDispatcher() {
    for (auto& row : table)
        row.fill(nullptr);
}

void CollisionDispatcher::registerCollision(ShapeType typeA, ShapeType typeB, CollisionFn collisionFn) {
    if (!isValidShapeType(typeA) || !isValidShapeType(typeB))
        return;

    table[typeIndex(typeA)][typeIndex(typeB)] = collisionFn;
}

bool CollisionDispatcher::dispatch(CollisionShape3D* colliderA,
                                   CollisionShape3D* colliderB,
                                   std::vector<PhysicsContact>& outContacts) const {
    if (!colliderA || !colliderB || !colliderA->getShape() || !colliderB->getShape())
        return false;

    ShapeType typeA = colliderA->getShape()->type;
    ShapeType typeB = colliderB->getShape()->type;
    if (!isValidShapeType(typeA) || !isValidShapeType(typeB))
        return false;

    CollisionFn collisionFn = table[typeIndex(typeA)][typeIndex(typeB)];
    if (!collisionFn)
        return false;

    collisionFn(colliderA, colliderB, outContacts);
    return true;
}
