#pragma once
#include "constraint.h"
#include <glm/glm.hpp>

class RigidBody3D;

struct FeatureID {
    FeatureID(){
        this->featureA = 0;
        this->featureB = 0;
    }
    FeatureID(uint16_t featureA, uint16_t featureB){
        this->featureA = featureA;
        this->featureB = featureB;
    }
    uint16_t featureA;
    uint16_t featureB;

    bool operator==(const FeatureID& other) const {
        return featureA == other.featureA &&
               featureB == other.featureB;
    }
};

class ContactConstraint {
public:
    RigidBody3D* objA;
    RigidBody3D* objB;
    FeatureID featureId;

    glm::vec3 worldPoint;
    glm::vec3 normal;
    float penetration;

    glm::vec3 localA, localB;
    glm::vec3 worldA, worldB;
    glm::vec3 rA, rB;

    float invMassA = 0, invMassB = 0;
    glm::mat3 invIA{0}, invIB{0};

    // Precomputed once before the solver loop (like VPE's K_inv)
    float effectiveMass = 0;
    float velocityBias  = 0;

    float accumulatedNormalLambda = 0;
    bool  isReused = false;

    ContactConstraint(RigidBody3D* objA, RigidBody3D* objB,
                      glm::vec3 worldPoint, glm::vec3 normal,
                      float penetration, FeatureID featureId);

    void setCollisionData(glm::vec3 worldPoint, glm::vec3 normal, float penetration);

    // Step 1 — only geometry, no velocity changes (mirrors VPE's narrowPhase geometry work)
    void init();

    // Step 2 — warm-start: re-apply last frame's accumulated impulse once (mirrors VPE's warmStart())
    void warmStart();

    // Step 3 — precompute effective mass and bias (mirrors VPE's setupConstraints())
    void setUp(float dt);

    // Step 4 — iterative velocity solve (mirrors VPE's calculateContactPointImpulses())
    void solve();
};

