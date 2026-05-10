#include "physicsworld.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include "common/3dEntities/collisionshape3d.h"
#include "common/3dEntities/rigidbody3d.h"
#include "common/Shapes/cube.h"
#include "common/Shapes/shape.h"
#include "common/Shapes/voxelshape.h"

namespace {

float invMass(RigidBody3D* body) {
    if (!body || body->mass <= 0.0f)
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

int normalSlot(const glm::vec3& normal) {
    if (normal.x > 0.5f) return 0;
    if (normal.x < -0.5f) return 1;
    if (normal.y > 0.5f) return 2;
    if (normal.y < -0.5f) return 3;
    if (normal.z > 0.5f) return 4;
    return 5;
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

} // namespace

void PhysicsWorld::step(const std::vector<RigidBody3D*>& rigidBodies,
                        const std::vector<CollisionShape3D*>& colliders,
                        float fixedDeltaTime) {
    if (fixedDeltaTime <= 0.0f)
        return;

    for (RigidBody3D* body : rigidBodies) {
        if (!body || !body->getVisible())
            continue;

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
        if (contacts.empty())
            break;

        solvePositions(contacts);
        syncBodies(rigidBodies);
    }

    contacts.clear();
    collectContacts(colliders, contacts);
    solveVelocities(contacts);
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
                addBoxBoxContact(a, b, outContacts);
            } else if (shapeA->type == CUBE && shapeB->type == VOXEL) {
                addBoxVoxelContacts(a, b, outContacts);
            } else if (shapeA->type == VOXEL && shapeB->type == CUBE) {
                addBoxVoxelContacts(b, a, outContacts);
            }
        }
    }
}

bool PhysicsWorld::boxAabb(CollisionShape3D* collider, PhysicsAabb& outAabb) const {
    if (!collider || !collider->getShape() || collider->getShape()->type != CUBE)
        return false;

    Cube* cube = static_cast<Cube*>(collider->getShape());
    glm::vec3 half = cube->halfSize;
    glm::mat4 model = collider->getGlobalMatrix();

    outAabb.min = glm::vec3(std::numeric_limits<float>::infinity());
    outAabb.max = glm::vec3(-std::numeric_limits<float>::infinity());

    for (int x = -1; x <= 1; x += 2) {
        for (int y = -1; y <= 1; y += 2) {
            for (int z = -1; z <= 1; z += 2) {
                glm::vec3 local = glm::vec3(x * half.x, y * half.y, z * half.z);
                glm::vec3 world = glm::vec3(model * glm::vec4(local, 1.0f));
                outAabb.min = glm::min(outAabb.min, world);
                outAabb.max = glm::max(outAabb.max, world);
            }
        }
    }

    return isFiniteVec3(outAabb.min) && isFiniteVec3(outAabb.max);
}

bool PhysicsWorld::aabbContact(const PhysicsAabb& a,
                               const PhysicsAabb& b,
                               glm::vec3& normalFromBToA,
                               float& penetration,
                               glm::vec3& point) const {
    float overlapX = std::min(a.max.x, b.max.x) - std::max(a.min.x, b.min.x);
    float overlapY = std::min(a.max.y, b.max.y) - std::max(a.min.y, b.min.y);
    float overlapZ = std::min(a.max.z, b.max.z) - std::max(a.min.z, b.min.z);

    if (overlapX <= 0.0f || overlapY <= 0.0f || overlapZ <= 0.0f)
        return false;

    glm::vec3 centerA = (a.min + a.max) * 0.5f;
    glm::vec3 centerB = (b.min + b.max) * 0.5f;
    glm::vec3 delta = centerA - centerB;

    penetration = overlapX;
    normalFromBToA = glm::vec3(delta.x >= 0.0f ? 1.0f : -1.0f, 0.0f, 0.0f);

    if (overlapY < penetration) {
        penetration = overlapY;
        normalFromBToA = glm::vec3(0.0f, delta.y >= 0.0f ? 1.0f : -1.0f, 0.0f);
    }
    if (overlapZ < penetration) {
        penetration = overlapZ;
        normalFromBToA = glm::vec3(0.0f, 0.0f, delta.z >= 0.0f ? 1.0f : -1.0f);
    }

    glm::vec3 contactMin = glm::max(a.min, b.min);
    glm::vec3 contactMax = glm::min(a.max, b.max);
    point = (contactMin + contactMax) * 0.5f;
    return true;
}

bool PhysicsWorld::addBoxBoxContact(CollisionShape3D* colliderA,
                                    CollisionShape3D* colliderB,
                                    std::vector<PhysicsContact>& outContacts) const {
    PhysicsAabb aabbA;
    PhysicsAabb aabbB;
    if (!boxAabb(colliderA, aabbA) || !boxAabb(colliderB, aabbB))
        return false;

    RigidBody3D* bodyA = colliderA->rb;
    RigidBody3D* bodyB = colliderB->rb;
    if (!bodyA && !bodyB)
        return false;

    glm::vec3 normal;
    float penetration = 0.0f;
    glm::vec3 point(0.0f);

    if (bodyA) {
        if (!aabbContact(aabbA, aabbB, normal, penetration, point))
            return false;

        PhysicsContact contact;
        contact.bodyA = bodyA;
        contact.bodyB = bodyB;
        contact.colliderA = colliderA;
        contact.colliderB = colliderB;
        contact.normal = normal;
        contact.penetration = penetration;
        contact.point = point;
        contact.friction = combinedFriction(bodyA, bodyB);
        outContacts.push_back(contact);
        return true;
    }

    if (!aabbContact(aabbB, aabbA, normal, penetration, point))
        return false;

    PhysicsContact contact;
    contact.bodyA = bodyB;
    contact.colliderA = colliderB;
    contact.colliderB = colliderA;
    contact.normal = normal;
    contact.penetration = penetration;
    contact.point = point;
    contact.friction = combinedFriction(bodyB, nullptr);
    outContacts.push_back(contact);
    return true;
}

void PhysicsWorld::addBoxVoxelContacts(CollisionShape3D* boxCollider,
                                       CollisionShape3D* voxelCollider,
                                       std::vector<PhysicsContact>& outContacts) const {
    if (!boxCollider || !boxCollider->rb || !voxelCollider)
        return;

    PhysicsAabb box;
    if (!boxAabb(boxCollider, box))
        return;

    VoxelShape* voxel = static_cast<VoxelShape*>(voxelCollider->getShape());
    glm::ivec3 minCell = voxel->worldToCell(box.min);
    glm::ivec3 maxCell = voxel->worldToCell(box.max);

    minCell.x = std::max(0, minCell.x);
    minCell.y = std::max(0, minCell.y);
    minCell.z = std::max(0, minCell.z);
    maxCell.x = std::min(voxel->getWidth() - 1, maxCell.x);
    maxCell.y = std::min(voxel->getHeight() - 1, maxCell.y);
    maxCell.z = std::min(voxel->getDepth() - 1, maxCell.z);

    if (minCell.x > maxCell.x || minCell.y > maxCell.y || minCell.z > maxCell.z)
        return;

    std::array<PhysicsContact, 6> bestContacts;
    std::array<bool, 6> hasContact = {false, false, false, false, false, false};

    for (int x = minCell.x; x <= maxCell.x; ++x) {
        for (int y = minCell.y; y <= maxCell.y; ++y) {
            for (int z = minCell.z; z <= maxCell.z; ++z) {
                if (!voxel->isSolid(x, y, z))
                    continue;

                PhysicsAabb cell;
                cell.min = voxel->cellMin(x, y, z);
                cell.max = voxel->cellMax(x, y, z);

                glm::vec3 normal;
                float penetration = 0.0f;
                glm::vec3 point(0.0f);
                if (!aabbContact(box, cell, normal, penetration, point))
                    continue;

                int slot = normalSlot(normal);
                if (!hasContact[slot] || penetration > bestContacts[slot].penetration) {
                    PhysicsContact contact;
                    contact.bodyA = boxCollider->rb;
                    contact.colliderA = boxCollider;
                    contact.colliderB = voxelCollider;
                    contact.normal = normal;
                    contact.penetration = penetration;
                    contact.point = point;
                    contact.friction = combinedFriction(boxCollider->rb, nullptr);
                    bestContacts[slot] = contact;
                    hasContact[slot] = true;
                }
            }
        }
    }

    for (size_t i = 0; i < bestContacts.size(); ++i) {
        if (hasContact[i])
            outContacts.push_back(bestContacts[i]);
    }
}

void PhysicsWorld::solvePositions(const std::vector<PhysicsContact>& contactsToSolve) const {
    constexpr float slop = 0.001f;
    constexpr float percent = 0.85f;

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

void PhysicsWorld::solveVelocities(const std::vector<PhysicsContact>& contactsToSolve) const {
    for (const PhysicsContact& contact : contactsToSolve) {
        if (!contact.bodyA)
            continue;

        if (contact.normal.y > 0.5f)
            contact.bodyA->setOnGround(true);
        if (contact.bodyB && contact.normal.y < -0.5f)
            contact.bodyB->setOnGround(true);

        float invA = invMass(contact.bodyA);
        float invB = invMass(contact.bodyB);
        float invSum = invA + invB;
        if (invSum <= 0.0f)
            continue;

        glm::vec3 velA = contact.bodyA->velocity;
        glm::vec3 velB = contact.bodyB ? contact.bodyB->velocity : glm::vec3(0.0f);
        glm::vec3 relativeVelocity = velA - velB;
        float normalSpeed = glm::dot(relativeVelocity, contact.normal);

        if (normalSpeed < 0.0f) {
            float restitution = std::max(0.0f, contact.restitution);
            float impulseMagnitude = -(1.0f + restitution) * normalSpeed / invSum;
            glm::vec3 impulse = contact.normal * impulseMagnitude;
            contact.bodyA->velocity += impulse * invA;
            if (contact.bodyB)
                contact.bodyB->velocity -= impulse * invB;
        }

        if (!contact.bodyB && contact.normal.y > 0.5f && contact.friction > 0.0f) {
            glm::vec3 tangentVelocity = contact.bodyA->velocity - glm::dot(contact.bodyA->velocity, contact.normal) * contact.normal;
            float friction = std::min(contact.friction, 1.0f);
            contact.bodyA->velocity -= tangentVelocity * friction;
        }
    }
}
