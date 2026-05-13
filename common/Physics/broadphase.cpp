#include "broadphase.h"

#include "common/3dEntities/collisionshape3d.h"

namespace {

bool shouldTestPair(CollisionShape3D* a, CollisionShape3D* b) {
    if (!a || !b || a == b)
        return false;
    if (!a->active || !b->active)
        return false;
    if (!a->getVisible() || !b->getVisible())
        return false;
    if (!a->getShape() || !b->getShape())
        return false;
    if (!a->rb && !b->rb)
        return false;

    bool aDetectsB = (a->collisionDetectionLayers & b->collisionLayers) != 0;
    bool bDetectsA = (b->collisionDetectionLayers & a->collisionLayers) != 0;
    return aDetectsB || bDetectsA;
}

}

void BruteForceBroadPhase::computePairs(const std::vector<CollisionShape3D*>& colliders,
                                        std::vector<CollisionPair>& outPairs) const {
    for (size_t i = 0; i < colliders.size(); ++i) {
        for (size_t j = i + 1; j < colliders.size(); ++j) {
            CollisionShape3D* a = colliders[i];
            CollisionShape3D* b = colliders[j];
            if (!shouldTestPair(a, b))
                continue;

            CollisionPair pair;
            pair.colliderA = a;
            pair.colliderB = b;
            outPairs.push_back(pair);
        }
    }
}
