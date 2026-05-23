#include "contactsolver.h"

#include <algorithm>
#include <cmath>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include "../physicssettings.h"
#include "../rigidbody3d.h"

using namespace PhysicsSettings;

float inverseMass(RigidBody3D* body) {
    if (!body || body->mass <= 0.0f || body->isSleeping())
        return 0.0f;
    return 1.0f / body->mass;
}

namespace {

glm::mat3 worldInverseInertia(RigidBody3D* body) {
    if (!body || body->mass <= 0.0f || body->isSleeping())
        return glm::mat3(0.0f);

    glm::mat3 rotation = glm::mat3_cast(body->getPhysicsRotation());
    return rotation * body->inverseInertia * glm::transpose(rotation);
}

glm::vec3 angularEffect(RigidBody3D* body, const glm::vec3& angularImpulse) {
    if (!body)
        return glm::vec3(0.0f);
    return (worldInverseInertia(body) * angularImpulse) * body->unlockedRotation;
}

void applySolverImpulse(RigidBody3D* body, const glm::vec3& impulse, const glm::vec3& point) {
    if (!body || body->mass <= 0.0f || glm::length2(impulse) <= 1e-12f)
        return;

    body->velocity += impulse * inverseMass(body);

    glm::vec3 r = point - body->getPhysicsPosition();
    body->angularVelocity += angularEffect(body, glm::cross(r, impulse)) * kAngularImpulseScale;
}

float contactEffectiveMass(RigidBody3D* a,
                           RigidBody3D* b,
                           const glm::vec3& point,
                           const glm::vec3& direction) {
    float mass = inverseMass(a) + inverseMass(b);

    if (a) {
        glm::vec3 rA = point - a->getPhysicsPosition();
        mass += glm::dot(glm::cross(angularEffect(a, glm::cross(rA, direction)), rA), direction);
    }
    if (b) {
        glm::vec3 rB = point - b->getPhysicsPosition();
        mass += glm::dot(glm::cross(angularEffect(b, glm::cross(rB, direction)), rB), direction);
    }

    return mass;
}

}

glm::vec3 pointVelocity(RigidBody3D* body, const glm::vec3& point) {
    if (!body || body->isSleeping())
        return glm::vec3(0.0f);
    glm::vec3 r = point - body->getPhysicsPosition();
    return body->velocity + glm::cross(body->angularVelocity, r);
}

void ContactSolver::warmStartContacts(std::vector<PhysicsContact>& contacts) const {
    for (PhysicsContact& contact : contacts) {
        glm::vec3 impulse = contact.normal * contact.normalImpulse + contact.tangentImpulse;
        if (glm::length2(impulse) <= 1e-12f)
            continue;

        applySolverImpulse(contact.bodyA, impulse, contact.point);
        if (contact.bodyB)
            applySolverImpulse(contact.bodyB, -impulse, contact.point);
    }
}

void ContactSolver::solvePositions(const std::vector<PhysicsContact>& contacts) const {
    for (const PhysicsContact& contact : contacts) {
        if (!contact.bodyA)
            continue;

        float correctionDepth = std::max(contact.penetration - kPositionSlop, 0.0f) * kPositionCorrectionPercent;
        if (correctionDepth <= 0.0f)
            continue;

        float invA = inverseMass(contact.bodyA);
        float invB = inverseMass(contact.bodyB);
        float invSum = invA + invB;
        if (invSum <= 0.0f)
            continue;

        glm::vec3 correction = contact.normal * correctionDepth;
        if (contact.bodyB) {
            contact.bodyA->setPhysicsPosition(contact.bodyA->getPhysicsPosition() + correction * (invA / invSum));
            contact.bodyB->setPhysicsPosition(contact.bodyB->getPhysicsPosition() - correction * (invB / invSum));
        } else {
            contact.bodyA->setPhysicsPosition(contact.bodyA->getPhysicsPosition() + correction);
        }
    }
}

void ContactSolver::solveVelocities(std::vector<PhysicsContact>& contacts) const {
    for (PhysicsContact& contact : contacts) {
        if (!contact.bodyA)
            continue;

        if (contact.normal.y > 0.5f)
            contact.bodyA->setOnGround(true);
        if (contact.bodyB && contact.normal.y < -0.5f)
            contact.bodyB->setOnGround(true);

        glm::vec3 relativeVelocity = pointVelocity(contact.bodyA, contact.point) - pointVelocity(contact.bodyB, contact.point);
        float normalSpeed = glm::dot(relativeVelocity, contact.normal);

        // Damp micro-bounces: if normal speed is very small, scale it down
        // to prevent oscillation on face-to-face contact
        constexpr float kMicroBounceThreshold = 0.1f;
        constexpr float kMicroBounceDamping = 0.4f;
        if (std::abs(normalSpeed) < kMicroBounceThreshold && contact.penetration < 0.08f)
            normalSpeed *= kMicroBounceDamping;

        float effectiveMass = contactEffectiveMass(contact.bodyA, contact.bodyB, contact.point, contact.normal);
        if (effectiveMass > 1e-6f) {
            float restitutionSpeed = 0.0f;
            if (normalSpeed < -kWakeImpactSpeed)
                restitutionSpeed = std::max(0.0f, contact.restitution) * -normalSpeed;

            float impulseDelta = -(normalSpeed - restitutionSpeed) / effectiveMass;
            float oldNormalImpulse = contact.normalImpulse;
            contact.normalImpulse = std::max(oldNormalImpulse + impulseDelta, 0.0f);
            impulseDelta = contact.normalImpulse - oldNormalImpulse;

            if (std::abs(impulseDelta) > 1e-8f) {
                glm::vec3 impulse = contact.normal * impulseDelta;
                applySolverImpulse(contact.bodyA, impulse, contact.point);
                if (contact.bodyB)
                    applySolverImpulse(contact.bodyB, -impulse, contact.point);
            }
        }

        relativeVelocity = pointVelocity(contact.bodyA, contact.point) - pointVelocity(contact.bodyB, contact.point);
        glm::vec3 tangentVelocity = relativeVelocity - glm::dot(relativeVelocity, contact.normal) * contact.normal;
        float maxFriction = contact.friction * contact.normalImpulse;

        if (glm::length2(tangentVelocity) > 1e-8f && contact.friction > 0.0f) {
            glm::vec3 tangent = glm::normalize(tangentVelocity);
            float tangentMass = contactEffectiveMass(contact.bodyA, contact.bodyB, contact.point, tangent);
            if (tangentMass > 1e-6f) {
                glm::vec3 oldTangentImpulse = contact.tangentImpulse;
                float tangentImpulseDelta = -glm::dot(relativeVelocity, tangent) / tangentMass;
                contact.tangentImpulse += tangent * tangentImpulseDelta;

                float tangentMagnitude = glm::length(contact.tangentImpulse);
                if (tangentMagnitude > maxFriction) {
                    if (maxFriction > 0.0f)
                        contact.tangentImpulse *= maxFriction / tangentMagnitude;
                    else
                        contact.tangentImpulse = glm::vec3(0.0f);
                }

                glm::vec3 frictionImpulse = contact.tangentImpulse - oldTangentImpulse;
                if (glm::length2(frictionImpulse) > 1e-12f) {
                    applySolverImpulse(contact.bodyA, frictionImpulse, contact.point);
                    if (contact.bodyB)
                        applySolverImpulse(contact.bodyB, -frictionImpulse, contact.point);
                }
            }
        } else if (maxFriction <= 0.0f && glm::length2(contact.tangentImpulse) > 1e-12f) {
            glm::vec3 frictionImpulse = -contact.tangentImpulse;
            contact.tangentImpulse = glm::vec3(0.0f);
            applySolverImpulse(contact.bodyA, frictionImpulse, contact.point);
            if (contact.bodyB)
                applySolverImpulse(contact.bodyB, -frictionImpulse, contact.point);
        }
    }
}
