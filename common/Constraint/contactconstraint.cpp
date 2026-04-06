#include "contactconstraint.h"

#include <common/3dEntities/rigidbody3d.h>
ContactConstraint::ContactConstraint(RigidBody3D* objA, RigidBody3D* objB, glm::vec3 worldPoint, glm::vec3 normal, float penetration, FeatureID featureId){
    this->objA = objA;
    this->objB = objB;
    this->featureId = featureId;
    accumulatedNormalLambda = 0;
    isReused = false;

    invMassA = objA->mass > 0 ? 1.0/objA->mass : 0;
    invIA = objA->inverseInertia;

    if(objB != nullptr){
        invMassB = objB->mass > 0 ? 1.0/objB->mass : 0;
        invIB = objB->inverseInertia;
    }
    setCollisionData(worldPoint, normal, penetration);

}

void ContactConstraint::setCollisionData(glm::vec3 worldPoint, glm::vec3 normal, float penetration){
    this->worldPoint = worldPoint;
    this->normal = normal;
    this->penetration = penetration;
    localA = objA->worldToLocal(worldPoint);
    if(objB != nullptr) localB = objB->worldToLocal(worldPoint);

}

void ContactConstraint::init(){
    worldA = objA->localToWorld(localA);
    rA = worldA - objA->getGlobalPosition();
    if(objB != nullptr){
        worldB = objB->localToWorld(localB);
        rB = worldB - objB->getGlobalPosition();
    }

    // Warm start: re-apply accumulated impulse from previous frame
    if(accumulatedNormalLambda != 0.0f){
        glm::vec3 impulse = normal * accumulatedNormalLambda;
        objA->applyImpulse(-impulse, worldA);   // ← use applyImpulse, consistent with solve()
        if(objB != nullptr){
            objB->applyImpulse(impulse, worldB);
        }
    }
}


void ContactConstraint::solve(float dt){
    if(objB != nullptr){
        glm::vec3 velA = objA->velocity + glm::cross(rA, objA->angularVelocity);
        glm::vec3 velB = objB->velocity + glm::cross(rB, objB->angularVelocity);
        glm::vec3 relVel = velB-velA;
        float Cdot = glm::dot(normal, relVel);

        glm::vec3 rnA = glm::cross(rA, normal);
        glm::vec3 rnB = glm::cross(rB, normal);
        float effectiveMass = invMassA + invMassB + glm::dot(glm::cross(invIA * rnA, rA) + glm::cross(invIB * rnB, rB), normal);
        if(effectiveMass < 1e-6) return;

        float allowedPenetration = 0.01f;
        float baumgarteFactor = 0.2f;
        float separation = std::min(0.0f, -penetration+allowedPenetration); //might need to switch signs
        float velocityBias = (baumgarteFactor/dt) * separation;

        float lambda = -(Cdot + velocityBias)/effectiveMass;

        float oldAccum = accumulatedNormalLambda;
        accumulatedNormalLambda = std::max(oldAccum + lambda, 0.0f);
        lambda = accumulatedNormalLambda-oldAccum;

        if(lambda == 0) return;

        glm::vec3 impulse = normal*lambda;
        objA->applyImpulse(-impulse, worldA);
        objB->applyImpulse(impulse, worldB);
    }
    else{
        glm::vec3 velA = objA->velocity + glm::cross(rA, objA->angularVelocity);
        float Cdot = glm::dot(normal, -velA);

        glm::vec3 rnA = glm::cross(rA, normal);
        float effectiveMass = invMassA + glm::dot(glm::cross(invIA * rnA, rA), normal);
        if(effectiveMass < 1e-6) return;

        float allowedPenetration = 0.001f;
        float baumgarteFactor = 0.1f;
        float separation = std::min(0.0f, -penetration+allowedPenetration);
        float velocityBias = (baumgarteFactor/dt) * separation;

        float lambda = -(Cdot + velocityBias)/effectiveMass;

        float oldAccum = accumulatedNormalLambda;
        accumulatedNormalLambda = std::max(oldAccum + lambda, 0.0f);
        lambda = accumulatedNormalLambda-oldAccum;

        if(lambda == 0) return;

        glm::vec3 impulse = normal*lambda;
        objA->applyImpulse(-impulse, worldA);
    }
}
