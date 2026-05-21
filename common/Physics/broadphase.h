#pragma once

#include <vector>
#include "physicstypes.h"

class Collider3D;

class BroadPhase {
public:
    virtual ~BroadPhase() = default;
    virtual void computePairs(const std::vector<Collider3D*>& colliders,
                              std::vector<CollisionPair>& outPairs) const = 0;
};

class SweepAndPruneBroadPhase : public BroadPhase {
public:
    void computePairs(const std::vector<Collider3D*>& colliders,
                      std::vector<CollisionPair>& outPairs) const override;
};
