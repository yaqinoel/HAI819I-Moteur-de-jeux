// contactconstraint.cpp
#include "contactconstraint.h"
#include <common/3dEntities/rigidbody3d.h>
#include <algorithm>

ContactConstraint::ContactConstraint(RigidBody3D* objA, RigidBody3D* objB,
                                     glm::vec3 worldPoint, glm::vec3 normal,
                                     float penetration, FeatureID featureId)
    : objA(objA), objB(objB), featureId(featureId)
{
    accumulatedNormalLambda = 0;
    isReused = false;

    invMassA = objA->mass > 0 ? 1.0f / objA->mass : 0.0f;
    invIA    = objA->inverseInertia;

    if (objB) {
        invMassB = objB->mass > 0 ? 1.0f / objB->mass : 0.0f;
        invIB    = objB->inverseInertia;
    }

    setCollisionData(worldPoint, normal, penetration);
}

void ContactConstraint::setCollisionData(glm::vec3 wp, glm::vec3 n, float pen) {
    worldPoint  = wp;
    normal      = n;
    penetration = pen;
    localA = objA->worldToLocal(wp);
    if (objB) localB = objB->worldToLocal(wp);
}

// only calculates geometry
void ContactConstraint::init() {
    worldA = objA->localToWorld(localA);
    rA     = worldA - objA->getGlobalPosition();

    if (objB) {
        worldB = objB->localToWorld(localB);
        rB     = worldB - objB->getGlobalPosition();
    }
}

// warm start
void ContactConstraint::warmStart() {
    if (accumulatedNormalLambda == 0.0f) return;

    glm::vec3 impulse = normal * accumulatedNormalLambda;
    objA->applyImpulse(-impulse, worldA);
    if (objB) objB->applyImpulse(impulse, worldB);
}

// precomputes other values
void ContactConstraint::setUp(float dt) {
    glm::vec3 rnA = glm::cross(rA, normal);

    if (objB) {
        glm::vec3 rnB = glm::cross(rB, normal);
        effectiveMass = invMassA + invMassB
                        + glm::dot(glm::cross(invIA * rnA, rA) + glm::cross(invIB * rnB, rB), normal);
    } else {
        effectiveMass = invMassA
                        + glm::dot(glm::cross(invIA * rnA, rA), normal);
    }

    if (effectiveMass < 1e-6f) effectiveMass = 0.0f;

    const float allowedPenetration = 0.001f;
    const float baumgarteFactor    = 0.2f;
    float separation = std::min(0.0f, -penetration + allowedPenetration);
    velocityBias = (baumgarteFactor / dt) * separation;
}

void ContactConstraint::solve() {
    if (effectiveMass < 1e-6f) return;

    float Cdot;
    if (objB) {
        glm::vec3 velA = objA->velocity + glm::cross(objA->angularVelocity, rA);
        glm::vec3 velB = objB->velocity + glm::cross(objB->angularVelocity, rB);
        Cdot = glm::dot(normal, velB - velA);
    } else {
        glm::vec3 velA = objA->velocity + glm::cross(objA->angularVelocity, rA);
        Cdot = glm::dot(normal, -velA);
    }

    float lambda = -(Cdot + velocityBias) / effectiveMass;

    float oldAccum = accumulatedNormalLambda;
    accumulatedNormalLambda = std::max(oldAccum + lambda, 0.0f);
    lambda = accumulatedNormalLambda - oldAccum;

    if (lambda == 0.0f) return;

    glm::vec3 impulse = normal * lambda;
    objA->applyImpulse(-impulse, worldA);
    if (objB) objB->applyImpulse(impulse, worldB);
}
