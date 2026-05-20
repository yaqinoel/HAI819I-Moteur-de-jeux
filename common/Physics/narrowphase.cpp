#include "narrowphase.h"

#include <algorithm>
#include <array>
#include <cmath>
#include "physicsgeometry.h"
#include "common/3dEntities/collisionshape3d.h"
#include "common/3dEntities/rigidbody3d.h"
#include "common/Shapes/shape.h"
#include "common/Shapes/voxelshape.h"

namespace {

float combinedFriction(RigidBody3D* a, RigidBody3D* b) {
    if (a && b)
        return std::sqrt(std::max(0.0f, a->friction) * std::max(0.0f, b->friction));
    return a ? std::max(0.0f, a->friction) : 0.0f;
}

void addBoxBoxContacts(CollisionShape3D* colliderA,
                       CollisionShape3D* colliderB,
                       std::vector<PhysicsContact>& outContacts) {
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

void addBoxVoxelContacts(CollisionShape3D* boxCollider,
                         CollisionShape3D* voxelCollider,
                         std::vector<PhysicsContact>& outContacts) {
    if (!boxCollider || !boxCollider->rb || !voxelCollider)
        return;

    OrientedBox box;
    if (!makeBox(boxCollider, box))
        return;

    PhysicsAabb broadAabb;
    if (!boxCollider->computeAabb(broadAabb))
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

void addVoxelBoxContacts(CollisionShape3D* voxelCollider,
                         CollisionShape3D* boxCollider,
                         std::vector<PhysicsContact>& outContacts) {
    addBoxVoxelContacts(boxCollider, voxelCollider, outContacts);
}

}

NarrowPhase::NarrowPhase() {
    dispatcher.registerCollision(CUBE, CUBE, addBoxBoxContacts);
    dispatcher.registerCollision(CUBE, VOXEL, addBoxVoxelContacts);
    dispatcher.registerCollision(VOXEL, CUBE, addVoxelBoxContacts);
}

void NarrowPhase::generateContacts(const std::vector<CollisionPair>& pairs,
                                   std::vector<PhysicsContact>& outContacts) const {
    for (const CollisionPair& pair : pairs)
        collide(pair.colliderA, pair.colliderB, outContacts);
}

void NarrowPhase::collide(CollisionShape3D* colliderA,
                          CollisionShape3D* colliderB,
                          std::vector<PhysicsContact>& outContacts) const {
    dispatcher.dispatch(colliderA, colliderB, outContacts);
}
