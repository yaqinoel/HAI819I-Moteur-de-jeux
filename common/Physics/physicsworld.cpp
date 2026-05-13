#include "physicsworld.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <vector>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include "common/3dEntities/collisionshape3d.h"
#include "common/3dEntities/rigidbody3d.h"
#include "common/Shapes/cube.h"
#include "common/Shapes/shape.h"
#include "common/Shapes/voxelshape.h"

namespace {

constexpr float kAxisEpsilon = 1e-6f;
constexpr float kContactEpsilon = 1e-3f;
constexpr float kAngularImpulseScale = 0.55f;
constexpr float kAirAngularDamping = 0.985f;
constexpr float kGroundAngularDamping = 0.82f;
constexpr float kMaxAngularSpeed = 10.0f;
constexpr float kRestingVerticalSpeed = 0.08f;
constexpr float kRestingAngularSpeed = 0.08f;
constexpr float kSleepLinearSpeed = 0.25f;
constexpr float kSleepAngularSpeed = 0.25f;
constexpr float kSleepDelay = 0.1f;
constexpr float kWarmStartScale = 0.85f;
constexpr float kWakeImpactSpeed = 0.35f;
constexpr float kWakePenetration = 0.03f;
constexpr float kSupportNormalY = 0.45f;
constexpr float kMovingSupportSpeed = 0.02f;
constexpr int kContactCacheKeepFrames = 3;

struct OrientedBox {
    CollisionShape3D* collider = nullptr;
    RigidBody3D* body = nullptr;
    glm::vec3 center = glm::vec3(0.0f);
    glm::vec3 half = glm::vec3(0.0f);
    glm::vec3 axis[3] = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    };
};

float invMass(RigidBody3D* body) {
    if (!body || body->mass <= 0.0f || body->isSleeping())
        return 0.0f;
    return 1.0f / body->mass;
}

float combinedFriction(RigidBody3D* a, RigidBody3D* b) {
    if (a && b)
        return std::sqrt(std::max(0.0f, a->friction) * std::max(0.0f, b->friction));
    return a ? std::max(0.0f, a->friction) : 0.0f;
}

bool isFiniteVec3(const glm::vec3& v) {
    return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
}

bool shouldTestPair(CollisionShape3D* a, CollisionShape3D* b) {
    if (!a || !b || a == b)
        return false;
    if (!a->active || !b->active)
        return false;
    if (!a->getVisible() || !b->getVisible())
        return false;
    if (!a->getShape() || !b->getShape())
        return false;
    bool aDetectsB = (a->collisionDetectionLayers & b->collisionLayers) != 0;
    bool bDetectsA = (b->collisionDetectionLayers & a->collisionLayers) != 0;
    return aDetectsB || bDetectsA;
}

int normalSlot(const glm::vec3& normal) {
    glm::vec3 absNormal = glm::abs(normal);
    if (absNormal.x >= absNormal.y && absNormal.x >= absNormal.z)
        return normal.x >= 0.0f ? 0 : 1;
    if (absNormal.y >= absNormal.z)
        return normal.y >= 0.0f ? 2 : 3;
    return normal.z >= 0.0f ? 4 : 5;
}

std::uint64_t mixHash(std::uint64_t seed, std::uint64_t value) {
    seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
    return seed;
}

std::uint64_t pointerHash(const void* ptr) {
    return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(ptr) >> 4);
}

std::uint64_t makeContactKey(const PhysicsContact& contact) {
    std::uint64_t seed = 1469598103934665603ULL;
    seed = mixHash(seed, pointerHash(contact.colliderA));
    seed = mixHash(seed, pointerHash(contact.colliderB));
    seed = mixHash(seed, static_cast<std::uint64_t>(normalSlot(contact.normal)));
    return seed == 0 ? 1 : seed;
}

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

glm::vec3 pointVelocity(RigidBody3D* body, const glm::vec3& point) {
    if (!body || body->isSleeping())
        return glm::vec3(0.0f);
    glm::vec3 r = point - body->getPhysicsPosition();
    return body->velocity + glm::cross(body->angularVelocity, r);
}

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

void applySolverImpulse(RigidBody3D* body, const glm::vec3& impulse, const glm::vec3& point) {
    if (!body || body->mass <= 0.0f || glm::length2(impulse) <= 1e-12f)
        return;

    body->velocity += impulse * invMass(body);

    glm::vec3 r = point - body->getPhysicsPosition();
    body->angularVelocity += angularEffect(body, glm::cross(r, impulse)) * kAngularImpulseScale;
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

    if (body->isOnGround() && std::abs(body->velocity.y) < kRestingVerticalSpeed)
        body->velocity.y = 0.0f;
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

bool makeBox(CollisionShape3D* collider, OrientedBox& outBox) {
    if (!collider || !collider->getShape() || collider->getShape()->type != CUBE)
        return false;

    Cube* cube = static_cast<Cube*>(collider->getShape());
    glm::mat4 model = collider->getGlobalMatrix();

    outBox.collider = collider;
    outBox.body = collider->rb;
    outBox.center = glm::vec3(model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    outBox.half = cube->halfSize;

    glm::vec3 xAxis = glm::vec3(model * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
    glm::vec3 yAxis = glm::vec3(model * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
    glm::vec3 zAxis = glm::vec3(model * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
    if (glm::length2(xAxis) < kAxisEpsilon || glm::length2(yAxis) < kAxisEpsilon || glm::length2(zAxis) < kAxisEpsilon)
        return false;

    outBox.axis[0] = glm::normalize(xAxis);
    outBox.axis[1] = glm::normalize(yAxis);
    outBox.axis[2] = glm::normalize(zAxis);
    return true;
}

OrientedBox makeStaticAabbBox(CollisionShape3D* collider, const glm::vec3& min, const glm::vec3& max) {
    OrientedBox box;
    box.collider = collider;
    box.body = nullptr;
    box.center = (min + max) * 0.5f;
    box.half = (max - min) * 0.5f;
    box.axis[0] = glm::vec3(1.0f, 0.0f, 0.0f);
    box.axis[1] = glm::vec3(0.0f, 1.0f, 0.0f);
    box.axis[2] = glm::vec3(0.0f, 0.0f, 1.0f);
    return box;
}

std::array<glm::vec3, 8> boxCorners(const OrientedBox& box) {
    std::array<glm::vec3, 8> corners;
    int index = 0;
    for (int x = -1; x <= 1; x += 2) {
        for (int y = -1; y <= 1; y += 2) {
            for (int z = -1; z <= 1; z += 2) {
                corners[index++] = box.center
                    + box.axis[0] * (box.half.x * static_cast<float>(x))
                    + box.axis[1] * (box.half.y * static_cast<float>(y))
                    + box.axis[2] * (box.half.z * static_cast<float>(z));
            }
        }
    }
    return corners;
}

bool containsPoint(const OrientedBox& box, const glm::vec3& point) {
    glm::vec3 d = point - box.center;
    return std::abs(glm::dot(d, box.axis[0])) <= box.half.x + kContactEpsilon
        && std::abs(glm::dot(d, box.axis[1])) <= box.half.y + kContactEpsilon
        && std::abs(glm::dot(d, box.axis[2])) <= box.half.z + kContactEpsilon;
}

float projectedRadius(const OrientedBox& box, const glm::vec3& axis) {
    return box.half.x * std::abs(glm::dot(axis, box.axis[0]))
         + box.half.y * std::abs(glm::dot(axis, box.axis[1]))
         + box.half.z * std::abs(glm::dot(axis, box.axis[2]));
}

bool satContact(const OrientedBox& a,
                const OrientedBox& b,
                glm::vec3& normalFromBToA,
                float& penetration) {
    std::array<glm::vec3, 15> axes;
    axes[0] = a.axis[0];
    axes[1] = a.axis[1];
    axes[2] = a.axis[2];
    axes[3] = b.axis[0];
    axes[4] = b.axis[1];
    axes[5] = b.axis[2];

    int index = 6;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            axes[index++] = glm::cross(a.axis[i], b.axis[j]);
        }
    }

    penetration = std::numeric_limits<float>::infinity();
    normalFromBToA = glm::vec3(0.0f);
    glm::vec3 centerDelta = a.center - b.center;

    for (glm::vec3 axis : axes) {
        float len2 = glm::length2(axis);
        if (len2 < kAxisEpsilon)
            continue;

        axis /= std::sqrt(len2);
        float distance = std::abs(glm::dot(centerDelta, axis));
        float overlap = projectedRadius(a, axis) + projectedRadius(b, axis) - distance;
        if (overlap <= 0.0f)
            return false;

        if (overlap < penetration) {
            penetration = overlap;
            normalFromBToA = glm::dot(centerDelta, axis) >= 0.0f ? axis : -axis;
        }
    }

    return isFiniteVec3(normalFromBToA) && penetration < std::numeric_limits<float>::infinity();
}

void addUniquePoint(std::vector<glm::vec3>& points, const glm::vec3& point) {
    for (const glm::vec3& existing : points) {
        if (glm::length2(existing - point) < 1e-5f)
            return;
    }
    points.push_back(point);
}

std::vector<glm::vec3> contactPoints(const OrientedBox& a, const OrientedBox& b) {
    std::vector<glm::vec3> points;

    for (const glm::vec3& corner : boxCorners(a)) {
        if (containsPoint(b, corner))
            addUniquePoint(points, corner);
    }
    for (const glm::vec3& corner : boxCorners(b)) {
        if (containsPoint(a, corner))
            addUniquePoint(points, corner);
    }

    if (points.empty()) {
        glm::vec3 fallback = (a.center + b.center) * 0.5f;
        points.push_back(fallback);
    }

    constexpr size_t maxPoints = 4;
    if (points.size() > maxPoints)
        points.resize(maxPoints);

    return points;
}

glm::vec3 averagePoint(const std::vector<glm::vec3>& points) {
    glm::vec3 sum(0.0f);
    for (const glm::vec3& point : points)
        sum += point;
    return points.empty() ? glm::vec3(0.0f) : sum / static_cast<float>(points.size());
}

float contactEffectiveMass(RigidBody3D* a,
                           RigidBody3D* b,
                           const glm::vec3& point,
                           const glm::vec3& direction) {
    float mass = invMass(a) + invMass(b);

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

} // namespace

void PhysicsWorld::step(const std::vector<RigidBody3D*>& rigidBodies,
                        const std::vector<CollisionShape3D*>& colliders,
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
        prepareContacts(contacts);
        wakeImpactedSleepingBodies(contacts);
        wakeUnstableSleepingBodies(rigidBodies, contacts);
        if (contacts.empty())
            break;

        solvePositions(contacts);
        syncBodies(rigidBodies);
    }

    contacts.clear();
    collectContacts(colliders, contacts);
    prepareContacts(contacts);
    wakeImpactedSleepingBodies(contacts);
    wakeUnstableSleepingBodies(rigidBodies, contacts);
    warmStartContacts(contacts);

    const int velocityIterations = 8;
    for (int i = 0; i < velocityIterations; ++i)
        solveVelocities(contacts);

    storeContactImpulses(contacts);

    for (RigidBody3D* body : rigidBodies) {
        stabilizeBody(body);
        updateSleepState(body, fixedDeltaTime);
    }
}

void PhysicsWorld::syncBodies(const std::vector<RigidBody3D*>& rigidBodies) const {
    for (RigidBody3D* body : rigidBodies) {
        if (body && body->getVisible())
            body->syncTransformToPhysicsState();
    }
}

void PhysicsWorld::collectContacts(const std::vector<CollisionShape3D*>& colliders,
                                   std::vector<PhysicsContact>& outContacts) const {
    for (size_t i = 0; i < colliders.size(); ++i) {
        for (size_t j = i + 1; j < colliders.size(); ++j) {
            CollisionShape3D* a = colliders[i];
            CollisionShape3D* b = colliders[j];
            if (!shouldTestPair(a, b))
                continue;
            if (!a->rb && !b->rb)
                continue;

            Shape* shapeA = a->getShape();
            Shape* shapeB = b->getShape();
            if (shapeA->type == CUBE && shapeB->type == CUBE) {
                addBoxBoxContacts(a, b, outContacts);
            } else if (shapeA->type == CUBE && shapeB->type == VOXEL) {
                addBoxVoxelContacts(a, b, outContacts);
            } else if (shapeA->type == VOXEL && shapeB->type == CUBE) {
                addBoxVoxelContacts(b, a, outContacts);
            }
        }
    }
}

void PhysicsWorld::prepareContacts(std::vector<PhysicsContact>& contactsToPrepare) const {
    for (PhysicsContact& contact : contactsToPrepare) {
        contact.key = makeContactKey(contact);
        auto cached = impulseCache.find(contact.key);
        if (cached == impulseCache.end())
            continue;

        contact.normalImpulse = cached->second.normalImpulse * kWarmStartScale;
        contact.tangentImpulse = cached->second.tangentImpulse * kWarmStartScale;
    }
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

void PhysicsWorld::warmStartContacts(std::vector<PhysicsContact>& contactsToWarm) const {
    for (PhysicsContact& contact : contactsToWarm) {
        glm::vec3 impulse = contact.normal * contact.normalImpulse + contact.tangentImpulse;
        if (glm::length2(impulse) <= 1e-12f)
            continue;

        applySolverImpulse(contact.bodyA, impulse, contact.point);
        if (contact.bodyB)
            applySolverImpulse(contact.bodyB, -impulse, contact.point);
    }
}

void PhysicsWorld::storeContactImpulses(const std::vector<PhysicsContact>& contactsToStore) {
    ++solverFrame;

    for (const PhysicsContact& contact : contactsToStore) {
        if (contact.key == 0)
            continue;

        CachedContactImpulse& cached = impulseCache[contact.key];
        cached.normalImpulse = contact.normalImpulse;
        cached.tangentImpulse = contact.tangentImpulse;
        cached.lastSeenFrame = solverFrame;
    }

    for (auto it = impulseCache.begin(); it != impulseCache.end();) {
        if (solverFrame - it->second.lastSeenFrame > kContactCacheKeepFrames)
            it = impulseCache.erase(it);
        else
            ++it;
    }
}

bool PhysicsWorld::boxAabb(CollisionShape3D* collider, PhysicsAabb& outAabb) const {
    OrientedBox box;
    if (!makeBox(collider, box))
        return false;

    outAabb.min = glm::vec3(std::numeric_limits<float>::infinity());
    outAabb.max = glm::vec3(-std::numeric_limits<float>::infinity());

    for (const glm::vec3& corner : boxCorners(box)) {
        outAabb.min = glm::min(outAabb.min, corner);
        outAabb.max = glm::max(outAabb.max, corner);
    }

    return isFiniteVec3(outAabb.min) && isFiniteVec3(outAabb.max);
}

void PhysicsWorld::addBoxBoxContacts(CollisionShape3D* colliderA,
                                     CollisionShape3D* colliderB,
                                     std::vector<PhysicsContact>& outContacts) const {
    OrientedBox boxA;
    OrientedBox boxB;
    if (!makeBox(colliderA, boxA) || !makeBox(colliderB, boxB))
        return;

    RigidBody3D* bodyA = colliderA->rb;
    RigidBody3D* bodyB = colliderB->rb;
    if (!bodyA && !bodyB)
        return;

    glm::vec3 normal;
    float penetration = 0.0f;

    if (bodyA) {
        if (!satContact(boxA, boxB, normal, penetration))
            return;

        PhysicsContact contact;
        contact.bodyA = bodyA;
        contact.bodyB = bodyB;
        contact.colliderA = colliderA;
        contact.colliderB = colliderB;
        contact.normal = normal;
        contact.penetration = penetration;
        contact.point = averagePoint(contactPoints(boxA, boxB));
        contact.friction = combinedFriction(bodyA, bodyB);
        outContacts.push_back(contact);
        return;
    }

    if (!satContact(boxB, boxA, normal, penetration))
        return;

    PhysicsContact contact;
    contact.bodyA = bodyB;
    contact.colliderA = colliderB;
    contact.colliderB = colliderA;
    contact.normal = normal;
    contact.penetration = penetration;
    contact.point = averagePoint(contactPoints(boxB, boxA));
    contact.friction = combinedFriction(bodyB, nullptr);
    outContacts.push_back(contact);
}

void PhysicsWorld::addBoxVoxelContacts(CollisionShape3D* boxCollider,
                                       CollisionShape3D* voxelCollider,
                                       std::vector<PhysicsContact>& outContacts) const {
    if (!boxCollider || !boxCollider->rb || !voxelCollider)
        return;

    OrientedBox box;
    if (!makeBox(boxCollider, box))
        return;

    PhysicsAabb broadAabb;
    if (!boxAabb(boxCollider, broadAabb))
        return;

    VoxelShape* voxel = static_cast<VoxelShape*>(voxelCollider->getShape());
    glm::ivec3 minCell = voxel->worldToCell(broadAabb.min);
    glm::ivec3 maxCell = voxel->worldToCell(broadAabb.max);

    minCell.x = std::max(0, minCell.x);
    minCell.y = std::max(0, minCell.y);
    minCell.z = std::max(0, minCell.z);
    maxCell.x = std::min(voxel->getWidth() - 1, maxCell.x);
    maxCell.y = std::min(voxel->getHeight() - 1, maxCell.y);
    maxCell.z = std::min(voxel->getDepth() - 1, maxCell.z);

    if (minCell.x > maxCell.x || minCell.y > maxCell.y || minCell.z > maxCell.z)
        return;

    std::array<PhysicsContact, 6> contactsByDirection;
    std::array<glm::vec3, 6> pointSums = {
        glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f),
        glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)
    };
    std::array<int, 6> pointCounts = {0, 0, 0, 0, 0, 0};

    for (int x = minCell.x; x <= maxCell.x; ++x) {
        for (int y = minCell.y; y <= maxCell.y; ++y) {
            for (int z = minCell.z; z <= maxCell.z; ++z) {
                if (!voxel->isSolid(x, y, z))
                    continue;

                OrientedBox cell = makeStaticAabbBox(voxelCollider, voxel->cellMin(x, y, z), voxel->cellMax(x, y, z));
                glm::vec3 normal;
                float penetration = 0.0f;
                if (!satContact(box, cell, normal, penetration))
                    continue;

                int slot = normalSlot(normal);
                std::vector<glm::vec3> points = contactPoints(box, cell);
                pointSums[slot] += averagePoint(points);
                pointCounts[slot]++;

                if (pointCounts[slot] == 1 || penetration > contactsByDirection[slot].penetration) {
                    PhysicsContact contact;
                    contact.bodyA = boxCollider->rb;
                    contact.colliderA = boxCollider;
                    contact.colliderB = voxelCollider;
                    contact.normal = normal;
                    contact.penetration = penetration;
                    contact.friction = combinedFriction(boxCollider->rb, nullptr);
                    contactsByDirection[slot] = contact;
                }
            }
        }
    }

    for (size_t i = 0; i < contactsByDirection.size(); ++i) {
        if (pointCounts[i] <= 0)
            continue;
        contactsByDirection[i].point = pointSums[i] / static_cast<float>(pointCounts[i]);
        outContacts.push_back(contactsByDirection[i]);
    }
}

void PhysicsWorld::solvePositions(const std::vector<PhysicsContact>& contactsToSolve) const {
    constexpr float slop = 0.004f;
    constexpr float percent = 0.25f;

    for (const PhysicsContact& contact : contactsToSolve) {
        if (!contact.bodyA)
            continue;

        float correctionDepth = std::max(contact.penetration - slop, 0.0f) * percent;
        if (correctionDepth <= 0.0f)
            continue;

        float invA = invMass(contact.bodyA);
        float invB = invMass(contact.bodyB);
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

void PhysicsWorld::solveVelocities(std::vector<PhysicsContact>& contactsToSolve) const {
    for (PhysicsContact& contact : contactsToSolve) {
        if (!contact.bodyA)
            continue;

        if (contact.normal.y > 0.5f)
            contact.bodyA->setOnGround(true);
        if (contact.bodyB && contact.normal.y < -0.5f)
            contact.bodyB->setOnGround(true);

        glm::vec3 relativeVelocity = pointVelocity(contact.bodyA, contact.point) - pointVelocity(contact.bodyB, contact.point);
        float normalSpeed = glm::dot(relativeVelocity, contact.normal);

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
