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

    float effectiveMass  = 0;
    float velocityBias   = 0;
    float accumulatedNormalLambda = 0;

    glm::vec3 tangent1{0}, tangent2{0};
    float effectiveMassTangent1  = 0;
    float effectiveMassTangent2  = 0;
    float accumulatedFrictionLambda1 = 0;
    float accumulatedFrictionLambda2 = 0;
    float friction = 0.5f;

    bool isReused = false;

    ContactConstraint(RigidBody3D* objA, RigidBody3D* objB, glm::vec3 worldPoint, glm::vec3 normal, float penetration, FeatureID featureId, float friction);

    void setCollisionData(glm::vec3 worldPoint, glm::vec3 normal, float penetration);
    void setUp(float dt);
    void solve();

private:
    float computeTangentEffectiveMass(const glm::vec3& t) const;
    void  solveFriction(const glm::vec3& t, float effectiveMassT,
                       float& accumulatedLambdaT);
};
