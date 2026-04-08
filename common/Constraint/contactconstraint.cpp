#include "contactconstraint.h"
#include <common/3dEntities/rigidbody3d.h>
#include <algorithm>

// ---------------------------------------------------------------------------
// helpers
// ---------------------------------------------------------------------------

// Build an arbitrary orthonormal basis for the tangent plane of `n`.
static void computeTangentBasis(const glm::vec3& n,
                                glm::vec3& t1, glm::vec3& t2)
{
    // Pick the axis least aligned with n to avoid degeneracy
    if (std::abs(n.x) <= std::abs(n.y) && std::abs(n.x) <= std::abs(n.z))
        t1 = glm::vec3(0.f, -n.z,  n.y);
    else if (std::abs(n.y) <= std::abs(n.z))
        t1 = glm::vec3(-n.z, 0.f,  n.x);
    else
        t1 = glm::vec3(-n.y,  n.x, 0.f);

    t1 = glm::normalize(t1);
    t2 = glm::cross(n, t1);          // already unit-length
}

// ---------------------------------------------------------------------------
// ContactConstraint
// ---------------------------------------------------------------------------

ContactConstraint::ContactConstraint(RigidBody3D* objA, RigidBody3D* objB,
                                     glm::vec3 worldPoint, glm::vec3 normal,
                                     float penetration, FeatureID featureId)
    : objA(objA), objB(objB), featureId(featureId)
{
    accumulatedNormalLambda   = 0;
    accumulatedFrictionLambda1 = 0;
    accumulatedFrictionLambda2 = 0;
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

// ---------------------------------------------------------------------------

float ContactConstraint::computeTangentEffectiveMass(const glm::vec3& t) const
{
    glm::vec3 rtA = glm::cross(rA, t);
    float em = invMassA + glm::dot(glm::cross(invIA * rtA, rA), t);
    if (objB) {
        glm::vec3 rtB = glm::cross(rB, t);
        em += invMassB + glm::dot(glm::cross(invIB * rtB, rB), t);
    }
    return em;
}

// ---------------------------------------------------------------------------

void ContactConstraint::setUp(float dt)
{
    // ---- world-space contact points & lever arms ----
    worldA = objA->localToWorld(localA);
    rA     = worldA - objA->getGlobalPosition();
    if (objB) {
        worldB = objB->localToWorld(localB);
        rB     = worldB - objB->getGlobalPosition();
    }

    // ---- normal effective mass ----
    glm::vec3 rnA = glm::cross(rA, normal);
    if (objB) {
        glm::vec3 rnB = glm::cross(rB, normal);
        effectiveMass = invMassA + invMassB
                        + glm::dot(glm::cross(invIA * rnA, rA)
                                       + glm::cross(invIB * rnB, rB), normal);
    } else {
        effectiveMass = invMassA
                        + glm::dot(glm::cross(invIA * rnA, rA), normal);
    }
    if (effectiveMass < 1e-6f) effectiveMass = 0.0f;

    // ---- Baumgarte velocity bias ----
    float separation = std::min(0.0f, -penetration + 0.001f);
    velocityBias = (0.2f / dt) * separation;

    // ---- tangent basis & friction effective masses ----
    computeTangentBasis(normal, tangent1, tangent2);
    effectiveMassTangent1 = computeTangentEffectiveMass(tangent1);
    effectiveMassTangent2 = computeTangentEffectiveMass(tangent2);
    if (effectiveMassTangent1 < 1e-6f) effectiveMassTangent1 = 0.0f;
    if (effectiveMassTangent2 < 1e-6f) effectiveMassTangent2 = 0.0f;

    // ---- warm-start ----
    if (accumulatedNormalLambda != 0.0f ||
        accumulatedFrictionLambda1 != 0.0f ||
        accumulatedFrictionLambda2 != 0.0f)
    {
        glm::vec3 impulse = normal   * accumulatedNormalLambda
                            + tangent1 * accumulatedFrictionLambda1
                            + tangent2 * accumulatedFrictionLambda2;
        objA->applyImpulse(-impulse, worldA);
        if (objB) objB->applyImpulse(impulse, worldB);
    }
}

// ---------------------------------------------------------------------------

void ContactConstraint::solveFriction(const glm::vec3& t,
                                      float             effectiveMassT,
                                      float&            accumulatedLambdaT)
{
    if (effectiveMassT < 1e-6f) return;

    // Relative velocity along tangent
    float Cdot;
    if (objB) {
        glm::vec3 velA = objA->velocity + glm::cross(objA->angularVelocity, rA);
        glm::vec3 velB = objB->velocity + glm::cross(objB->angularVelocity, rB);
        Cdot = glm::dot(t, velB - velA);
    } else {
        glm::vec3 velA = objA->velocity + glm::cross(objA->angularVelocity, rA);
        Cdot = glm::dot(t, -velA);
    }

    float lambda = -Cdot / effectiveMassT;

    // Coulomb cone clamp: |λ_friction| ≤ μ * λ_normal
    float maxFriction = friction * accumulatedNormalLambda;
    float oldAccum    = accumulatedLambdaT;
    accumulatedLambdaT = std::clamp(oldAccum + lambda, -maxFriction, maxFriction);
    lambda = accumulatedLambdaT - oldAccum;

    if (lambda == 0.0f) return;

    glm::vec3 impulse = t * lambda;
    objA->applyImpulse(-impulse, worldA);
    if (objB) objB->applyImpulse(impulse, worldB);
}

// ---------------------------------------------------------------------------

void ContactConstraint::solve()
{
    // ---- 1. Normal (non-penetration) impulse ----
    if (effectiveMass >= 1e-6f) {
        float Cdot;
        if (objB) {
            glm::vec3 velA = objA->velocity + glm::cross(objA->angularVelocity, rA);
            glm::vec3 velB = objB->velocity + glm::cross(objB->angularVelocity, rB);
            Cdot = glm::dot(normal, velB - velA);
        } else {
            glm::vec3 velA = objA->velocity + glm::cross(objA->angularVelocity, rA);
            Cdot = glm::dot(normal, -velA);
        }

        float lambda    = -(Cdot + velocityBias) / effectiveMass;
        float oldAccum  = accumulatedNormalLambda;
        accumulatedNormalLambda = std::max(oldAccum + lambda, 0.0f);
        lambda = accumulatedNormalLambda - oldAccum;

        if (lambda != 0.0f) {
            glm::vec3 impulse = normal * lambda;
            objA->applyImpulse(-impulse, worldA);
            if (objB) objB->applyImpulse(impulse, worldB);
        }
    }

    // ---- 2. Friction impulses (two tangent directions) ----
    solveFriction(tangent1, effectiveMassTangent1, accumulatedFrictionLambda1);
    solveFriction(tangent2, effectiveMassTangent2, accumulatedFrictionLambda2);
}
