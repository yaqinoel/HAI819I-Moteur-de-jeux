#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>
#include "../physicstypes.h"

class Collider3D;
class RigidBody3D;

struct OrientedBox {
    Collider3D* collider = nullptr;
    RigidBody3D* body = nullptr;
    glm::vec3 center = glm::vec3(0.0f);
    glm::vec3 half = glm::vec3(0.0f);
    glm::vec3 axis[3] = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    };
};

bool isFiniteVec3(const glm::vec3& v);
bool aabbOverlaps(const PhysicsAabb& a, const PhysicsAabb& b);
int normalSlot(const glm::vec3& normal);
bool makeBox(Collider3D* collider, OrientedBox& outBox);
OrientedBox makeStaticAabbBox(Collider3D* collider, const glm::vec3& min, const glm::vec3& max);
std::array<glm::vec3, 8> boxCorners(const OrientedBox& box);
bool boxAabb(Collider3D* collider, PhysicsAabb& outAabb);
bool satContact(const OrientedBox& a, const OrientedBox& b, glm::vec3& normalFromBToA, float& penetration);
std::vector<glm::vec3> contactPoints(const OrientedBox& a, const OrientedBox& b);
glm::vec3 averagePoint(const std::vector<glm::vec3>& points);
