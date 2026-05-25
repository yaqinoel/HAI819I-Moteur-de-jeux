#include "narrowphase.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <glm/gtx/norm.hpp>
#include "physicsgeometry.h"
#include "../collider3d.h"
#include "../rigidbody3d.h"
#include "common/Physics/Shapes/capsuleshape.h"
#include "common/Physics/Shapes/shape.h"
#include "common/Physics/Shapes/sphere.h"
#include "common/Physics/Shapes/terrainvoxelshape.h"
#include "common/Physics/Shapes/voxelshape.h"

namespace {

float combinedFriction(RigidBody3D* a, RigidBody3D* b) {
    if (a && b)
        return std::sqrt(std::max(0.0f, a->friction) * std::max(0.0f, b->friction));
    return a ? std::max(0.0f, a->friction) : 0.0f;
}

float combinedRestitution(RigidBody3D* a, RigidBody3D* b) {
    float ra = a ? std::max(0.0f, a->restitution) : 0.0f;
    float rb = b ? std::max(0.0f, b->restitution) : 0.0f;
    return std::max(ra, rb);
}

struct ContactCandidate {
    glm::vec3 normal = glm::vec3(0.0f);
    glm::vec3 point = glm::vec3(0.0f);
    float penetration = 0.0f;
};

bool buildContact(Collider3D* colliderA,
                  Collider3D* colliderB,
                  const glm::vec3& normalFromBToA,
                  const glm::vec3& point,
                  float penetration,
                  PhysicsContact& outContact);

class ManifoldAccumulator {
public:
    void add(Collider3D* colliderA,
             Collider3D* colliderB,
             const glm::vec3& normalFromBToA,
             const glm::vec3& point,
             float penetration) {
        PhysicsContact contact;
        if (!buildContact(colliderA, colliderB, normalFromBToA, point, penetration, contact))
            return;

        int slot = normalSlot(normalFromBToA);
        auto& group = groups[slot];
        group.push_back(contact);
    }

    void flush(std::vector<PhysicsContact>& outContacts) {
        for (int slot = 0; slot < 6; ++slot) {
            auto& group = groups[slot];
            if (group.empty())
                continue;

            // Find the deepest penetration contact for normal/penetration reference
            PhysicsContact* deepest = &group[0];
            for (auto& c : group) {
                if (c.penetration > deepest->penetration)
                    deepest = &c;
            }

            if (group.size() <= kMaxManifoldPoints) {
                for (auto& c : group) {
                    // Use the deepest contact's normal and penetration, but keep individual points
                    c.normal = deepest->normal;
                    outContacts.push_back(c);
                }
            } else {
                // Reduce to kMaxManifoldPoints using farthest-point spread heuristic
                std::vector<PhysicsContact> selected;
                selected.reserve(kMaxManifoldPoints);

                // Start with the deepest penetration contact
                selected.push_back(*deepest);

                while (selected.size() < kMaxManifoldPoints && selected.size() < group.size()) {
                    float bestDist2 = -1.0f;
                    size_t bestIdx = 0;
                    for (size_t i = 0; i < group.size(); ++i) {
                        bool alreadySelected = false;
                        float minDist2 = std::numeric_limits<float>::infinity();
                        for (auto& sel : selected) {
                            float d2 = glm::length2(group[i].point - sel.point);
                            if (d2 < 1e-6f) {
                                alreadySelected = true;
                                break;
                            }
                            minDist2 = std::min(minDist2, d2);
                        }
                        if (alreadySelected)
                            continue;
                        if (minDist2 > bestDist2) {
                            bestDist2 = minDist2;
                            bestIdx = i;
                        }
                    }
                    if (bestDist2 <= 0.0f)
                        break;
                    selected.push_back(group[bestIdx]);
                }

                for (auto& c : selected) {
                    c.normal = deepest->normal;
                    outContacts.push_back(c);
                }
            }
        }
    }

private:
    static constexpr size_t kMaxManifoldPoints = 4;
    std::array<std::vector<PhysicsContact>, 6> groups;
};

bool buildContact(Collider3D* colliderA,
                  Collider3D* colliderB,
                  const glm::vec3& normalFromBToA,
                  const glm::vec3& point,
                  float penetration,
                  PhysicsContact& outContact) {
    if (!colliderA || !colliderB || penetration <= 0.0f || !isFiniteVec3(normalFromBToA))
        return false;

    float normalLength2 = glm::length2(normalFromBToA);
    if (normalLength2 <= 1e-10f)
        return false;

    RigidBody3D* bodyA = colliderA->rb;
    RigidBody3D* bodyB = colliderB->rb;
    if (!bodyA && !bodyB)
        return false;

    glm::vec3 normal = normalFromBToA / std::sqrt(normalLength2);
    outContact = PhysicsContact();
    outContact.point = point;
    outContact.penetration = penetration;

    if (bodyA) {
        outContact.bodyA = bodyA;
        outContact.bodyB = bodyB;
        outContact.colliderA = colliderA;
        outContact.colliderB = colliderB;
        outContact.normal = normal;
        outContact.friction = combinedFriction(bodyA, bodyB);
        outContact.restitution = combinedRestitution(bodyA, bodyB);
        return true;
    }

    outContact.bodyA = bodyB;
    outContact.colliderA = colliderB;
    outContact.colliderB = colliderA;
    outContact.normal = -normal;
    outContact.friction = combinedFriction(bodyB, nullptr);
    outContact.restitution = combinedRestitution(bodyB, nullptr);
    return true;
}

void pushContact(Collider3D* colliderA,
                 Collider3D* colliderB,
                 const glm::vec3& normalFromBToA,
                 const glm::vec3& point,
                 float penetration,
                 std::vector<PhysicsContact>& outContacts) {
    PhysicsContact contact;
    if (buildContact(colliderA, colliderB, normalFromBToA, point, penetration, contact))
        outContacts.push_back(contact);
}

bool voxelCellRangeForAabb(VoxelShape* voxel,
                           Collider3D* voxelCollider,
                           const PhysicsAabb& worldAabb,
                           glm::ivec3& minCell,
                           glm::ivec3& maxCell) {
    if (!voxel || !voxelCollider)
        return false;

    minCell = glm::ivec3(std::numeric_limits<int>::max());
    maxCell = glm::ivec3(std::numeric_limits<int>::lowest());

    for (int x = 0; x <= 1; ++x) {
        for (int y = 0; y <= 1; ++y) {
            for (int z = 0; z <= 1; ++z) {
                glm::vec3 corner(
                    x == 0 ? worldAabb.min.x : worldAabb.max.x,
                    y == 0 ? worldAabb.min.y : worldAabb.max.y,
                    z == 0 ? worldAabb.min.z : worldAabb.max.z
                );
                glm::ivec3 cell = voxel->worldToCell(*voxelCollider, corner);
                minCell = glm::min(minCell, cell);
                maxCell = glm::max(maxCell, cell);
            }
        }
    }

    minCell.x = std::max(0, minCell.x);
    minCell.y = std::max(0, minCell.y);
    minCell.z = std::max(0, minCell.z);
    maxCell.x = std::min(voxel->getWidth() - 1, maxCell.x);
    maxCell.y = std::min(voxel->getHeight() - 1, maxCell.y);
    maxCell.z = std::min(voxel->getDepth() - 1, maxCell.z);

    return minCell.x <= maxCell.x && minCell.y <= maxCell.y && minCell.z <= maxCell.z;
}

glm::ivec3 cellStepForNormal(const glm::vec3& normal) {
    switch (normalSlot(normal)) {
        case 0: return glm::ivec3(1, 0, 0);
        case 1: return glm::ivec3(-1, 0, 0);
        case 2: return glm::ivec3(0, 1, 0);
        case 3: return glm::ivec3(0, -1, 0);
        case 4: return glm::ivec3(0, 0, 1);
        default: return glm::ivec3(0, 0, -1);
    }
}

bool isExposedVoxelContact(VoxelShape* voxel, int x, int y, int z, const glm::vec3& normal) {
    if (!voxel || glm::length2(normal) <= 1e-10f)
        return false;

    glm::ivec3 step = cellStepForNormal(normal);
    return !voxel->isSolid(x + step.x, y + step.y, z + step.z);
}

bool acceptsVoxelFaceContact(VoxelShape* voxel, int x, int y, int z, const glm::vec3& normal) {
    if (!voxel)
        return false;
    if (voxel->type != TERRAIN_VOXEL)
        return true;
    return isExposedVoxelContact(voxel, x, y, z, normal);
}

glm::vec3 closestPointOnSegment(const glm::vec3& point,
                                const glm::vec3& segmentA,
                                const glm::vec3& segmentB) {
    glm::vec3 segment = segmentB - segmentA;
    float length2 = glm::length2(segment);
    if (length2 <= 1e-10f)
        return segmentA;

    float t = glm::dot(point - segmentA, segment) / length2;
    t = glm::clamp(t, 0.0f, 1.0f);
    return segmentA + segment * t;
}

glm::vec3 closestPointOnBox(const OrientedBox& box, const glm::vec3& point) {
    glm::vec3 local = point - box.center;
    glm::vec3 closest = box.center;
    for (int i = 0; i < 3; ++i) {
        float distance = glm::dot(local, box.axis[i]);
        float half = box.half[i];
        closest += box.axis[i] * glm::clamp(distance, -half, half);
    }
    return closest;
}

bool nearestBoxFace(const OrientedBox& box,
                    const glm::vec3& point,
                    glm::vec3& normal,
                    glm::vec3& pointOnFace,
                    float& distanceToFace) {
    distanceToFace = std::numeric_limits<float>::infinity();
    normal = glm::vec3(0.0f);
    pointOnFace = point;

    for (int i = 0; i < 3; ++i) {
        float local = glm::dot(point - box.center, box.axis[i]);
        float positiveDistance = box.half[i] - local;
        if (positiveDistance < distanceToFace) {
            distanceToFace = positiveDistance;
            normal = box.axis[i];
            pointOnFace = point + normal * positiveDistance;
        }

        float negativeDistance = box.half[i] + local;
        if (negativeDistance < distanceToFace) {
            distanceToFace = negativeDistance;
            normal = -box.axis[i];
            pointOnFace = point + normal * negativeDistance;
        }
    }

    return distanceToFace < std::numeric_limits<float>::infinity() && isFiniteVec3(normal);
}

void capsuleSegment(Collider3D* collider,
                    CapsuleShape* capsule,
                    glm::vec3& segmentA,
                    glm::vec3& segmentB) {
    glm::vec3 center = collider->getGlobalPosition();
    glm::vec3 axis = collider->getGlobalRotation() * glm::vec3(0.0f, 1.0f, 0.0f);
    if (glm::length2(axis) <= 1e-10f)
        axis = glm::vec3(0.0f, 1.0f, 0.0f);
    axis = glm::normalize(axis);

    segmentA = center + axis * capsule->halfSegment;
    segmentB = center - axis * capsule->halfSegment;
}

void closestPointsSegments(const glm::vec3& p1,
                           const glm::vec3& q1,
                           const glm::vec3& p2,
                           const glm::vec3& q2,
                           glm::vec3& c1,
                           glm::vec3& c2) {
    glm::vec3 d1 = q1 - p1;
    glm::vec3 d2 = q2 - p2;
    glm::vec3 r = p1 - p2;
    float a = glm::dot(d1, d1);
    float e = glm::dot(d2, d2);
    float f = glm::dot(d2, r);
    float s = 0.0f;
    float t = 0.0f;

    if (a <= 1e-10f && e <= 1e-10f) {
        c1 = p1;
        c2 = p2;
        return;
    }

    if (a <= 1e-10f) {
        t = glm::clamp(f / e, 0.0f, 1.0f);
    } else {
        float c = glm::dot(d1, r);
        if (e <= 1e-10f) {
            s = glm::clamp(-c / a, 0.0f, 1.0f);
        } else {
            float b = glm::dot(d1, d2);
            float denom = a * e - b * b;
            if (denom != 0.0f)
                s = glm::clamp((b * f - c * e) / denom, 0.0f, 1.0f);

            t = (b * s + f) / e;
            if (t < 0.0f) {
                t = 0.0f;
                s = glm::clamp(-c / a, 0.0f, 1.0f);
            } else if (t > 1.0f) {
                t = 1.0f;
                s = glm::clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }

    c1 = p1 + d1 * s;
    c2 = p2 + d2 * t;
}

void closestSegmentBox(const glm::vec3& segmentA,
                       const glm::vec3& segmentB,
                       const OrientedBox& box,
                       glm::vec3& pointOnSegment,
                       glm::vec3& pointOnBox) {
    float lo = 0.0f;
    float hi = 1.0f;
    glm::vec3 segment = segmentB - segmentA;

    auto distanceAt = [&](float t) {
        glm::vec3 point = segmentA + segment * t;
        return glm::length2(point - closestPointOnBox(box, point));
    };

    for (int i = 0; i < 24; ++i) {
        float m1 = lo + (hi - lo) / 3.0f;
        float m2 = hi - (hi - lo) / 3.0f;
        if (distanceAt(m1) < distanceAt(m2))
            hi = m2;
        else
            lo = m1;
    }

    float t = (lo + hi) * 0.5f;
    pointOnSegment = segmentA + segment * t;
    pointOnBox = closestPointOnBox(box, pointOnSegment);
}

bool sphereBoxCandidate(Collider3D* sphereCollider,
                        Sphere* sphere,
                        const OrientedBox& box,
                        ContactCandidate& outCandidate) {
    glm::vec3 center = sphereCollider->getGlobalPosition();
    glm::vec3 closest = closestPointOnBox(box, center);
    glm::vec3 delta = center - closest;
    float distance2 = glm::length2(delta);

    if (distance2 > 1e-10f) {
        float distance = std::sqrt(distance2);
        float penetration = sphere->radius - distance;
        if (penetration <= 0.0f)
            return false;

        outCandidate.normal = delta / distance;
        outCandidate.point = closest;
        outCandidate.penetration = penetration;
        return true;
    }

    glm::vec3 normal;
    glm::vec3 pointOnFace;
    float distanceToFace = 0.0f;
    if (!nearestBoxFace(box, center, normal, pointOnFace, distanceToFace))
        return false;

    outCandidate.normal = normal;
    outCandidate.point = pointOnFace;
    outCandidate.penetration = sphere->radius + std::max(0.0f, distanceToFace);
    return true;
}

bool capsuleBoxCandidate(Collider3D* capsuleCollider,
                         CapsuleShape* capsule,
                         const OrientedBox& box,
                         ContactCandidate& outCandidate) {
    glm::vec3 segmentA;
    glm::vec3 segmentB;
    capsuleSegment(capsuleCollider, capsule, segmentA, segmentB);

    glm::vec3 pointOnSegment;
    glm::vec3 pointOnBox;
    closestSegmentBox(segmentA, segmentB, box, pointOnSegment, pointOnBox);

    glm::vec3 delta = pointOnSegment - pointOnBox;
    float distance2 = glm::length2(delta);
    if (distance2 > 1e-10f) {
        float distance = std::sqrt(distance2);
        float penetration = capsule->capsuleRadius - distance;
        if (penetration <= 0.0f)
            return false;

        outCandidate.normal = delta / distance;
        outCandidate.point = pointOnBox;
        outCandidate.penetration = penetration;
        return true;
    }

    glm::vec3 normal;
    glm::vec3 pointOnFace;
    float distanceToFace = 0.0f;
    if (!nearestBoxFace(box, pointOnSegment, normal, pointOnFace, distanceToFace))
        return false;

    outCandidate.normal = normal;
    outCandidate.point = pointOnFace;
    outCandidate.penetration = capsule->capsuleRadius + std::max(0.0f, distanceToFace);
    return true;
}

template <typename CandidateFn>
void addShapeVoxelContacts(Collider3D* dynamicCollider,
                           Collider3D* voxelCollider,
                           CandidateFn&& candidateFn,
                           std::vector<PhysicsContact>& outContacts) {
    if (!dynamicCollider || !dynamicCollider->rb || !voxelCollider)
        return;

    PhysicsAabb broadAabb;
    if (!dynamicCollider->computeAabb(broadAabb))
        return;

    VoxelShape* voxel = static_cast<VoxelShape*>(voxelCollider->getShape());
    glm::ivec3 minCell;
    glm::ivec3 maxCell;
    if (!voxelCellRangeForAabb(voxel, voxelCollider, broadAabb, minCell, maxCell))
        return;

    ManifoldAccumulator accumulator;

    for (int x = minCell.x; x <= maxCell.x; ++x) {
        for (int y = minCell.y; y <= maxCell.y; ++y) {
            for (int z = minCell.z; z <= maxCell.z; ++z) {
                if (!voxel->isSolid(x, y, z))
                    continue;

                OrientedBox cell = voxel->cellWorldBox(*voxelCollider, x, y, z);
                ContactCandidate candidate;
                if (!candidateFn(cell, candidate))
                    continue;
                if (!acceptsVoxelFaceContact(voxel, x, y, z, candidate.normal))
                    continue;

                accumulator.add(dynamicCollider,
                                voxelCollider,
                                candidate.normal,
                                candidate.point,
                                candidate.penetration);
            }
        }
    }

    accumulator.flush(outContacts);
}

void addBoxBoxContacts(Collider3D* colliderA,
                       Collider3D* colliderB,
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

void addBoxVoxelContacts(Collider3D* boxCollider,
                         Collider3D* voxelCollider,
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
    glm::ivec3 minCell;
    glm::ivec3 maxCell;
    if (!voxelCellRangeForAabb(voxel, voxelCollider, broadAabb, minCell, maxCell))
        return;

    ManifoldAccumulator accumulator;

    for (int x = minCell.x; x <= maxCell.x; ++x) {
        for (int y = minCell.y; y <= maxCell.y; ++y) {
            for (int z = minCell.z; z <= maxCell.z; ++z) {
                if (!voxel->isSolid(x, y, z))
                    continue;

                OrientedBox cell = voxel->cellWorldBox(*voxelCollider, x, y, z);
                glm::vec3 normal;
                float penetration = 0.0f;
                if (!satContact(box, cell, normal, penetration))
                    continue;
                if (!acceptsVoxelFaceContact(voxel, x, y, z, normal))
                    continue;

                accumulator.add(boxCollider,
                                voxelCollider,
                                normal,
                                averagePoint(contactPoints(box, cell)),
                                penetration);
            }
        }
    }

    accumulator.flush(outContacts);
}

void addVoxelBoxContacts(Collider3D* voxelCollider,
                         Collider3D* boxCollider,
                         std::vector<PhysicsContact>& outContacts) {
    addBoxVoxelContacts(boxCollider, voxelCollider, outContacts);
}

void addVoxelVoxelContacts(Collider3D* colliderA,
                           Collider3D* colliderB,
                           std::vector<PhysicsContact>& outContacts) {
    if (!colliderA || !colliderB || (!colliderA->rb && !colliderB->rb))
        return;

    VoxelShape* voxelA = static_cast<VoxelShape*>(colliderA->getShape());
    VoxelShape* voxelB = static_cast<VoxelShape*>(colliderB->getShape());
    if (!voxelA || !voxelB)
        return;

    PhysicsAabb aabbA;
    PhysicsAabb aabbB;
    if (!colliderA->computeAabb(aabbA) || !colliderB->computeAabb(aabbB))
        return;
    if (!aabbOverlaps(aabbA, aabbB))
        return;

    int solidA = voxelA->solidCellCount();
    int solidB = voxelB->solidCellCount();
    if (solidA <= 0 || solidB <= 0)
        return;

    bool iterateA = solidA <= solidB;
    VoxelShape* outerVoxel = iterateA ? voxelA : voxelB;
    VoxelShape* innerVoxel = iterateA ? voxelB : voxelA;
    Collider3D* outerCollider = iterateA ? colliderA : colliderB;
    Collider3D* innerCollider = iterateA ? colliderB : colliderA;

    ManifoldAccumulator accumulator;

    outerVoxel->forEachSolidCell([&](int outerX, int outerY, int outerZ) {
        PhysicsAabb outerCellAabb;
        if (!outerVoxel->cellWorldAabb(*outerCollider, outerX, outerY, outerZ, outerCellAabb))
            return;

        glm::ivec3 minCell;
        glm::ivec3 maxCell;
        if (!voxelCellRangeForAabb(innerVoxel, innerCollider, outerCellAabb, minCell, maxCell))
            return;

        OrientedBox outerBox = outerVoxel->cellWorldBox(*outerCollider, outerX, outerY, outerZ);
        for (int x = minCell.x; x <= maxCell.x; ++x) {
            for (int y = minCell.y; y <= maxCell.y; ++y) {
                for (int z = minCell.z; z <= maxCell.z; ++z) {
                    if (!innerVoxel->isSolid(x, y, z))
                        continue;

                    OrientedBox innerBox = innerVoxel->cellWorldBox(*innerCollider, x, y, z);
                    glm::vec3 normal;
                    float penetration = 0.0f;

                    if (iterateA) {
                        if (!satContact(outerBox, innerBox, normal, penetration))
                            continue;
                        if (!acceptsVoxelFaceContact(outerVoxel, outerX, outerY, outerZ, -normal)
                            || !acceptsVoxelFaceContact(innerVoxel, x, y, z, normal))
                            continue;
                        accumulator.add(colliderA,
                                        colliderB,
                                        normal,
                                        averagePoint(contactPoints(outerBox, innerBox)),
                                        penetration);
                    } else {
                        if (!satContact(innerBox, outerBox, normal, penetration))
                            continue;
                        if (!acceptsVoxelFaceContact(innerVoxel, x, y, z, -normal)
                            || !acceptsVoxelFaceContact(outerVoxel, outerX, outerY, outerZ, normal))
                            continue;
                        accumulator.add(colliderA,
                                        colliderB,
                                        normal,
                                        averagePoint(contactPoints(innerBox, outerBox)),
                                        penetration);
                    }
                }
            }
        }
    });

    accumulator.flush(outContacts);
}

void addSphereSphereContacts(Collider3D* colliderA,
                             Collider3D* colliderB,
                             std::vector<PhysicsContact>& outContacts) {
    Sphere* sphereA = static_cast<Sphere*>(colliderA->getShape());
    Sphere* sphereB = static_cast<Sphere*>(colliderB->getShape());

    glm::vec3 centerA = colliderA->getGlobalPosition();
    glm::vec3 centerB = colliderB->getGlobalPosition();
    glm::vec3 delta = centerA - centerB;
    float radiusSum = sphereA->radius + sphereB->radius;
    float distance2 = glm::length2(delta);
    if (distance2 >= radiusSum * radiusSum)
        return;

    float distance = std::sqrt(std::max(distance2, 0.0f));
    glm::vec3 normal = distance > 1e-5f ? delta / distance : glm::vec3(0.0f, 1.0f, 0.0f);
    float penetration = radiusSum - distance;
    glm::vec3 pointA = centerA - normal * sphereA->radius;
    glm::vec3 pointB = centerB + normal * sphereB->radius;
    pushContact(colliderA, colliderB, normal, (pointA + pointB) * 0.5f, penetration, outContacts);
}

void addSphereBoxContacts(Collider3D* sphereCollider,
                          Collider3D* boxCollider,
                          std::vector<PhysicsContact>& outContacts) {
    OrientedBox box;
    if (!makeBox(boxCollider, box))
        return;

    ContactCandidate candidate;
    if (!sphereBoxCandidate(sphereCollider, static_cast<Sphere*>(sphereCollider->getShape()), box, candidate))
        return;

    pushContact(sphereCollider, boxCollider, candidate.normal, candidate.point, candidate.penetration, outContacts);
}

void addBoxSphereContacts(Collider3D* boxCollider,
                          Collider3D* sphereCollider,
                          std::vector<PhysicsContact>& outContacts) {
    addSphereBoxContacts(sphereCollider, boxCollider, outContacts);
}

void addSphereVoxelContacts(Collider3D* sphereCollider,
                            Collider3D* voxelCollider,
                            std::vector<PhysicsContact>& outContacts) {
    Sphere* sphere = static_cast<Sphere*>(sphereCollider->getShape());
    addShapeVoxelContacts(
        sphereCollider,
        voxelCollider,
        [&](const OrientedBox& cell, ContactCandidate& candidate) {
            return sphereBoxCandidate(sphereCollider, sphere, cell, candidate);
        },
        outContacts);
}

void addVoxelSphereContacts(Collider3D* voxelCollider,
                            Collider3D* sphereCollider,
                            std::vector<PhysicsContact>& outContacts) {
    addSphereVoxelContacts(sphereCollider, voxelCollider, outContacts);
}

void addCapsuleSphereContacts(Collider3D* capsuleCollider,
                              Collider3D* sphereCollider,
                              std::vector<PhysicsContact>& outContacts) {
    CapsuleShape* capsule = static_cast<CapsuleShape*>(capsuleCollider->getShape());
    Sphere* sphere = static_cast<Sphere*>(sphereCollider->getShape());

    glm::vec3 segmentA;
    glm::vec3 segmentB;
    capsuleSegment(capsuleCollider, capsule, segmentA, segmentB);

    glm::vec3 sphereCenter = sphereCollider->getGlobalPosition();
    glm::vec3 capsulePoint = closestPointOnSegment(sphereCenter, segmentA, segmentB);
    glm::vec3 delta = capsulePoint - sphereCenter;
    float radiusSum = capsule->capsuleRadius + sphere->radius;
    float distance2 = glm::length2(delta);
    if (distance2 >= radiusSum * radiusSum)
        return;

    float distance = std::sqrt(std::max(distance2, 0.0f));
    glm::vec3 normal = distance > 1e-5f ? delta / distance : glm::vec3(0.0f, 1.0f, 0.0f);
    float penetration = radiusSum - distance;
    glm::vec3 pointA = capsulePoint - normal * capsule->capsuleRadius;
    glm::vec3 pointB = sphereCenter + normal * sphere->radius;
    pushContact(capsuleCollider, sphereCollider, normal, (pointA + pointB) * 0.5f, penetration, outContacts);
}

void addSphereCapsuleContacts(Collider3D* sphereCollider,
                              Collider3D* capsuleCollider,
                              std::vector<PhysicsContact>& outContacts) {
    addCapsuleSphereContacts(capsuleCollider, sphereCollider, outContacts);
}

void addCapsuleCapsuleContacts(Collider3D* colliderA,
                               Collider3D* colliderB,
                               std::vector<PhysicsContact>& outContacts) {
    CapsuleShape* capsuleA = static_cast<CapsuleShape*>(colliderA->getShape());
    CapsuleShape* capsuleB = static_cast<CapsuleShape*>(colliderB->getShape());

    glm::vec3 a0;
    glm::vec3 a1;
    glm::vec3 b0;
    glm::vec3 b1;
    capsuleSegment(colliderA, capsuleA, a0, a1);
    capsuleSegment(colliderB, capsuleB, b0, b1);

    glm::vec3 pointA;
    glm::vec3 pointB;
    closestPointsSegments(a0, a1, b0, b1, pointA, pointB);

    glm::vec3 delta = pointA - pointB;
    float radiusSum = capsuleA->capsuleRadius + capsuleB->capsuleRadius;
    float distance2 = glm::length2(delta);
    if (distance2 >= radiusSum * radiusSum)
        return;

    float distance = std::sqrt(std::max(distance2, 0.0f));
    glm::vec3 normal = distance > 1e-5f ? delta / distance : glm::vec3(0.0f, 1.0f, 0.0f);
    float penetration = radiusSum - distance;
    glm::vec3 surfaceA = pointA - normal * capsuleA->capsuleRadius;
    glm::vec3 surfaceB = pointB + normal * capsuleB->capsuleRadius;
    pushContact(colliderA, colliderB, normal, (surfaceA + surfaceB) * 0.5f, penetration, outContacts);
}

void addCapsuleBoxContacts(Collider3D* capsuleCollider,
                           Collider3D* boxCollider,
                           std::vector<PhysicsContact>& outContacts) {
    OrientedBox box;
    if (!makeBox(boxCollider, box))
        return;

    ContactCandidate candidate;
    if (!capsuleBoxCandidate(capsuleCollider, static_cast<CapsuleShape*>(capsuleCollider->getShape()), box, candidate))
        return;

    pushContact(capsuleCollider, boxCollider, candidate.normal, candidate.point, candidate.penetration, outContacts);
}

void addBoxCapsuleContacts(Collider3D* boxCollider,
                           Collider3D* capsuleCollider,
                           std::vector<PhysicsContact>& outContacts) {
    addCapsuleBoxContacts(capsuleCollider, boxCollider, outContacts);
}

void addCapsuleVoxelContacts(Collider3D* capsuleCollider,
                             Collider3D* voxelCollider,
                             std::vector<PhysicsContact>& outContacts) {
    CapsuleShape* capsule = static_cast<CapsuleShape*>(capsuleCollider->getShape());
    addShapeVoxelContacts(
        capsuleCollider,
        voxelCollider,
        [&](const OrientedBox& cell, ContactCandidate& candidate) {
            return capsuleBoxCandidate(capsuleCollider, capsule, cell, candidate);
        },
        outContacts);
}

void addVoxelCapsuleContacts(Collider3D* voxelCollider,
                             Collider3D* capsuleCollider,
                             std::vector<PhysicsContact>& outContacts) {
    addCapsuleVoxelContacts(capsuleCollider, voxelCollider, outContacts);
}

}

NarrowPhase::NarrowPhase() {
    dispatcher.registerCollision(CUBE, CUBE, addBoxBoxContacts);
    dispatcher.registerCollision(CUBE, VOXEL, addBoxVoxelContacts);
    dispatcher.registerCollision(VOXEL, CUBE, addVoxelBoxContacts);
    dispatcher.registerCollision(VOXEL, VOXEL, addVoxelVoxelContacts);
    dispatcher.registerCollision(CUBE, TERRAIN_VOXEL, addBoxVoxelContacts);
    dispatcher.registerCollision(TERRAIN_VOXEL, CUBE, addVoxelBoxContacts);
    dispatcher.registerCollision(VOXEL, TERRAIN_VOXEL, addVoxelVoxelContacts);
    dispatcher.registerCollision(TERRAIN_VOXEL, VOXEL, addVoxelVoxelContacts);
    dispatcher.registerCollision(TERRAIN_VOXEL, TERRAIN_VOXEL, addVoxelVoxelContacts);
    dispatcher.registerCollision(SPHERE, SPHERE, addSphereSphereContacts);
    dispatcher.registerCollision(SPHERE, CUBE, addSphereBoxContacts);
    dispatcher.registerCollision(CUBE, SPHERE, addBoxSphereContacts);
    dispatcher.registerCollision(SPHERE, VOXEL, addSphereVoxelContacts);
    dispatcher.registerCollision(VOXEL, SPHERE, addVoxelSphereContacts);
    dispatcher.registerCollision(SPHERE, TERRAIN_VOXEL, addSphereVoxelContacts);
    dispatcher.registerCollision(TERRAIN_VOXEL, SPHERE, addVoxelSphereContacts);
    dispatcher.registerCollision(CAPSULE, SPHERE, addCapsuleSphereContacts);
    dispatcher.registerCollision(SPHERE, CAPSULE, addSphereCapsuleContacts);
    dispatcher.registerCollision(CAPSULE, CAPSULE, addCapsuleCapsuleContacts);
    dispatcher.registerCollision(CAPSULE, CUBE, addCapsuleBoxContacts);
    dispatcher.registerCollision(CUBE, CAPSULE, addBoxCapsuleContacts);
    dispatcher.registerCollision(CAPSULE, VOXEL, addCapsuleVoxelContacts);
    dispatcher.registerCollision(VOXEL, CAPSULE, addVoxelCapsuleContacts);
    dispatcher.registerCollision(CAPSULE, TERRAIN_VOXEL, addCapsuleVoxelContacts);
    dispatcher.registerCollision(TERRAIN_VOXEL, CAPSULE, addVoxelCapsuleContacts);
}

void NarrowPhase::generateContacts(const std::vector<CollisionPair>& pairs,
                                   std::vector<PhysicsContact>& outContacts) const {
    for (const CollisionPair& pair : pairs)
        collide(pair.colliderA, pair.colliderB, outContacts);
}

void NarrowPhase::collide(Collider3D* colliderA,
                          Collider3D* colliderB,
                          std::vector<PhysicsContact>& outContacts) const {
    dispatcher.dispatch(colliderA, colliderB, outContacts);
}
