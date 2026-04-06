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

class ContactConstraint : public Constraint
{
public:
    ContactConstraint(RigidBody3D* objA, RigidBody3D* objB, glm::vec3 worldPoint, glm::vec3 normal, float penetration, FeatureID featureId);
    void setCollisionData(glm::vec3 worldPoint, glm::vec3 normal, float penetration);
    void init() override;
    void solve(float dt) override;
    bool isReused = false;
    float accumulatedNormalLambda;
    FeatureID featureId;
    RigidBody3D* objA;
    RigidBody3D* objB;

private:
    glm::vec3 worldPoint;
    glm::vec3 normal;
    float penetration;
    float invMassA;
    float invMassB;
    glm::mat3 invIA;
    glm::mat3 invIB;
    glm::vec3 localA;
    glm::vec3 localB;
    glm::vec3 worldA;
    glm::vec3 worldB;
    glm::vec3 rA;
    glm::vec3 rB;

};

