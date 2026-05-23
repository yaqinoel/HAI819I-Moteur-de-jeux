#include "physicsworld.h"

#include <cmath>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include "Collision/contactsolver.h"
#include "physicssettings.h"
#include "collider3d.h"
#include "rigidbody3d.h"
#include "Shapes/cube.h"

using namespace PhysicsSettings;

namespace {

glm::vec3 contactNormalForBody(const PhysicsContact& contact, RigidBody3D* body) {
    if (contact.bodyA == body)
        return contact.normal;
    if (contact.bodyB == body)
        return -contact.normal;
    return glm::vec3(0.0f);
}

RigidBody3D* otherBodyForContact(const PhysicsContact& contact, RigidBody3D* body) {
    if (contact.bodyA == body)
        return contact.bodyB;
    if (contact.bodyB == body)
        return contact.bodyA;
    return nullptr;
}

bool bodyIsMovingForWake(RigidBody3D* body) {
    if (!body || body->isSleeping())
        return false;

    return glm::length2(body->velocity) > kMovingSupportSpeed * kMovingSupportSpeed
        || glm::length2(body->angularVelocity) > kMovingSupportSpeed * kMovingSupportSpeed;
}

void stabilizeBody(RigidBody3D* body) {
    if (!body || body->mass <= 0.0f)
        return;

    float angularDamping = body->isOnGround() ? kGroundAngularDamping : kAirAngularDamping;
    body->angularVelocity *= angularDamping;

    float angularSpeed = glm::length(body->angularVelocity);
    if (angularSpeed > kMaxAngularSpeed)
        body->angularVelocity = body->angularVelocity * (kMaxAngularSpeed / angularSpeed);
    else if (body->isOnGround() && angularSpeed < kRestingAngularSpeed)
        body->angularVelocity = glm::vec3(0.0f);

    if (body->isOnGround()) {
        // Damp linear velocity on ground to reduce sliding/jitter
        constexpr float kGroundLinearDamping = 0.92f;
        body->velocity.x *= kGroundLinearDamping;
        body->velocity.z *= kGroundLinearDamping;

        if (std::abs(body->velocity.y) < kRestingVerticalSpeed)
            body->velocity.y = 0.0f;

        // Zero out very small horizontal velocities
        constexpr float kRestingHorizontalSpeed = 0.06f;
        float horizontalSpeed2 = body->velocity.x * body->velocity.x + body->velocity.z * body->velocity.z;
        if (horizontalSpeed2 < kRestingHorizontalSpeed * kRestingHorizontalSpeed) {
            body->velocity.x = 0.0f;
            body->velocity.z = 0.0f;
        }
    }
}

void updateSleepState(RigidBody3D* body, float dt) {
    if (!body || !body->canSleep || body->mass <= 0.0f) {
        if (body)
            body->sleepTimer = 0.0f;
        return;
    }

    bool slowLinear = glm::length2(body->velocity) < kSleepLinearSpeed * kSleepLinearSpeed;
    bool slowAngular = glm::length2(body->angularVelocity) < kSleepAngularSpeed * kSleepAngularSpeed;
    if (body->isOnGround() && slowLinear && slowAngular) {
        body->sleepTimer += dt;
        if (body->sleepTimer >= kSleepDelay)
            body->sleep();
    } else {
        body->sleepTimer = 0.0f;
    }
}

}

void PhysicsWorld::step(const std::vector<RigidBody3D*>& rigidBodies,
                        const std::vector<Collider3D*>& colliders,
                        float fixedDeltaTime) {
    if (fixedDeltaTime <= 0.0f)
        return;

    for (RigidBody3D* body : rigidBodies) {
        if (!body || !body->getVisible())
            continue;

        if (body->isSleeping()) {
            body->setOnGround(true);
            body->keepPhysicsStateForSleep();
            continue;
        }

        body->physicsProcess();
        body->setOnGround(false);

        glm::vec3 position = body->getPhysicsPosition();
        position += body->velocity * fixedDeltaTime;
        body->setPhysicsPosition(position);

        if (glm::length2(body->angularVelocity) > 1e-8f) {
            glm::vec3 axis = glm::normalize(body->angularVelocity);
            float angle = glm::length(body->angularVelocity) * fixedDeltaTime;
            body->setPhysicsRotation(glm::normalize(glm::angleAxis(angle, axis) * body->getPhysicsRotation()));
        }
    }

    syncBodies(rigidBodies);

    const int positionIterations = 4;
    for (int i = 0; i < positionIterations; ++i) {
        contacts.clear();
        collectContacts(colliders, contacts);
        contactCache.prepareContacts(contacts);
        wakeImpactedSleepingBodies(contacts);
        wakeUnstableSleepingBodies(rigidBodies, contacts);
        if (contacts.empty())
            break;

        contactSolver.solvePositions(contacts);
        syncBodies(rigidBodies);
    }

    contacts.clear();
    collectContacts(colliders, contacts);
    contactCache.prepareContacts(contacts);
    wakeImpactedSleepingBodies(contacts);
    wakeUnstableSleepingBodies(rigidBodies, contacts);
    contactSolver.warmStartContacts(contacts);

    const int velocityIterations = 8;
    for (int i = 0; i < velocityIterations; ++i)
        contactSolver.solveVelocities(contacts);

    contactCache.storeContactImpulses(contacts);

    for (RigidBody3D* body : rigidBodies) {
        stabilizeBody(body);
        updateSleepState(body, fixedDeltaTime);
    }
}

std::vector<Collider3D*> PhysicsWorld::cubeOverlapTest(const glm::vec3& center,
                                                const glm::quat& rotation,
                                                const glm::vec3& size,
                                                const std::vector<Collider3D*>& colliders,
                                                std::uint64_t mask) const {
    std::vector<Collider3D*> hits;
    if (size.x <= 0.0f || size.y <= 0.0f || size.z <= 0.0f)
        return hits;

    Cube queryShape(size.x, size.y, size.z);
    Collider3D queryCollider;
    RigidBody3D queryBody;
    queryBody.mass = 1.0f;

    queryCollider.setShape(&queryShape);
    queryCollider.rb = &queryBody;
    queryCollider.collisionLayers = 0ULL;
    queryCollider.collisionDetectionLayers = mask;
    queryCollider.setGlobalPosition(center);
    queryCollider.setGlobalRotation(rotation);
    queryBody.setPhysicsPosition(center);
    queryBody.setPhysicsRotation(rotation);

    std::vector<Collider3D*> queryColliders = colliders;
    queryColliders.push_back(&queryCollider);

    std::vector<CollisionPair> pairs;
    broadPhase.computePairs(queryColliders, pairs);

    for (const CollisionPair& pair : pairs) {
        Collider3D* other = nullptr;
        if (pair.colliderA == &queryCollider)
            other = pair.colliderB;
        else if (pair.colliderB == &queryCollider)
            other = pair.colliderA;
        else
            continue;

        if (!other || other == &queryCollider)
            continue;
        if ((mask & other->collisionLayers) == 0)
            continue;

        std::vector<PhysicsContact> queryContacts;
        narrowPhase.collide(&queryCollider, other, queryContacts);
        if (!queryContacts.empty())
            hits.push_back(other);
    }

    return hits;
}

void PhysicsWorld::syncBodies(const std::vector<RigidBody3D*>& rigidBodies) const {
    for (RigidBody3D* body : rigidBodies) {
        if (body && body->getVisible())
            body->syncTransformToPhysicsState();
    }
}

void PhysicsWorld::collectContacts(const std::vector<Collider3D*>& colliders,
                                   std::vector<PhysicsContact>& outContacts) const {
    std::vector<CollisionPair> pairs;
    broadPhase.computePairs(colliders, pairs);
    narrowPhase.generateContacts(pairs, outContacts);
}

void PhysicsWorld::wakeImpactedSleepingBodies(const std::vector<PhysicsContact>& contactsToCheck) const {
    for (const PhysicsContact& contact : contactsToCheck) {
        if (!contact.bodyA || !contact.bodyB)
            continue;

        bool aSleeping = contact.bodyA->isSleeping();
        bool bSleeping = contact.bodyB->isSleeping();
        if (aSleeping == bSleeping)
            continue;

        RigidBody3D* awakeBody = aSleeping ? contact.bodyB : contact.bodyA;
        RigidBody3D* sleepingBody = aSleeping ? contact.bodyA : contact.bodyB;
        if (!awakeBody || !sleepingBody || awakeBody->isSleeping())
            continue;

        glm::vec3 relativeVelocity = pointVelocity(contact.bodyA, contact.point) - pointVelocity(contact.bodyB, contact.point);
        float normalSpeed = glm::dot(relativeVelocity, contact.normal);
        bool strongImpact = normalSpeed < -kWakeImpactSpeed;
        bool forcedOverlap = contact.penetration > kWakePenetration && glm::length2(awakeBody->velocity) > kSleepLinearSpeed * kSleepLinearSpeed;

        if (strongImpact || forcedOverlap)
            sleepingBody->wakeUp();
    }
}

void PhysicsWorld::wakeUnstableSleepingBodies(const std::vector<RigidBody3D*>& rigidBodies,
                                              const std::vector<PhysicsContact>& contactsToCheck) const {
    for (RigidBody3D* body : rigidBodies) {
        if (!body || !body->getVisible() || !body->canSleep || body->mass <= 0.0f || !body->isSleeping())
            continue;

        bool hasSupport = false;
        bool supportIsUnstable = false;
        bool touchingMovingAwakeBody = false;

        for (const PhysicsContact& contact : contactsToCheck) {
            if (contact.bodyA != body && contact.bodyB != body)
                continue;

            glm::vec3 normalForBody = contactNormalForBody(contact, body);
            RigidBody3D* otherBody = otherBodyForContact(contact, body);
            bool upwardSupport = normalForBody.y > kSupportNormalY;

            if (upwardSupport) {
                hasSupport = true;
                if (otherBody && !otherBody->isSleeping() && (otherBody->canSleep || bodyIsMovingForWake(otherBody)))
                    supportIsUnstable = true;
            } else if (bodyIsMovingForWake(otherBody) && contact.penetration > kContactEpsilon) {
                touchingMovingAwakeBody = true;
            }
        }

        if (!hasSupport || supportIsUnstable || touchingMovingAwakeBody)
            body->wakeUp();
    }
}
