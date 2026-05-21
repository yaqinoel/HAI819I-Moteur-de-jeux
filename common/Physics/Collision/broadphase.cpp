#include "broadphase.h"

#include <algorithm>
#include <cstddef>
#include "physicsgeometry.h"
#include "../collider3d.h"

namespace {

struct SapProxy {
    Collider3D* collider = nullptr;
    PhysicsAabb aabb;
};

bool isValidCollider(Collider3D* collider) {
    return collider
        && collider->active
        && collider->getVisible()
        && collider->getShape();
}

bool shouldTestPair(Collider3D* a, Collider3D* b) {
    if (!a || !b || a == b)
        return false;
    if (!isValidCollider(a) || !isValidCollider(b))
        return false;
    if (!a->rb && !b->rb)
        return false;

    bool aDetectsB = (a->collisionDetectionLayers & b->collisionLayers) != 0;
    bool bDetectsA = (b->collisionDetectionLayers & a->collisionLayers) != 0;
    return aDetectsB || bDetectsA;
}

}

void SweepAndPruneBroadPhase::computePairs(const std::vector<Collider3D*>& colliders,
                                           std::vector<CollisionPair>& outPairs) const {
    std::vector<SapProxy> proxies;
    proxies.reserve(colliders.size());

    for (Collider3D* collider : colliders) {
        if (!isValidCollider(collider))
            continue;

        PhysicsAabb aabb;
        if (!collider->computeAabb(aabb))
            continue;
        if (!isFiniteVec3(aabb.min) || !isFiniteVec3(aabb.max))
            continue;

        SapProxy proxy;
        proxy.collider = collider;
        proxy.aabb = aabb;
        proxies.push_back(proxy);
    }

    std::sort(proxies.begin(), proxies.end(), [](const SapProxy& a, const SapProxy& b) {
        if (a.aabb.min.x == b.aabb.min.x)
            return a.collider < b.collider;
        return a.aabb.min.x < b.aabb.min.x;
    });

    std::vector<std::size_t> active;
    active.reserve(proxies.size());

    for (std::size_t currentIndex = 0; currentIndex < proxies.size(); ++currentIndex) {
        const SapProxy& current = proxies[currentIndex];

        active.erase(
            std::remove_if(active.begin(), active.end(), [&](std::size_t activeIndex) {
                return proxies[activeIndex].aabb.max.x < current.aabb.min.x;
            }),
            active.end());

        for (std::size_t activeIndex : active) {
            const SapProxy& candidate = proxies[activeIndex];
            if (!aabbOverlaps(candidate.aabb, current.aabb))
                continue;
            if (!shouldTestPair(candidate.collider, current.collider))
                continue;

            CollisionPair pair;
            pair.colliderA = candidate.collider;
            pair.colliderB = current.collider;
            outPairs.push_back(pair);
        }

        active.push_back(currentIndex);
    }
}
