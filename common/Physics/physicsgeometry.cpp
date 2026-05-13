#include "physicsgeometry.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <glm/gtx/norm.hpp>
#include "physicssettings.h"
#include "common/3dEntities/collisionshape3d.h"
#include "common/Shapes/cube.h"
#include "common/Shapes/shape.h"

using namespace PhysicsSettings;

bool isFiniteVec3(const glm::vec3& v) {
    return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
}

int normalSlot(const glm::vec3& normal) {
    glm::vec3 absNormal = glm::abs(normal);
    if (absNormal.x >= absNormal.y && absNormal.x >= absNormal.z)
        return normal.x >= 0.0f ? 0 : 1;
    if (absNormal.y >= absNormal.z)
        return normal.y >= 0.0f ? 2 : 3;
    return normal.z >= 0.0f ? 4 : 5;
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

bool boxAabb(CollisionShape3D* collider, PhysicsAabb& outAabb) {
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

namespace {

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

void addUniquePoint(std::vector<glm::vec3>& points, const glm::vec3& point) {
    for (const glm::vec3& existing : points) {
        if (glm::length2(existing - point) < 1e-5f)
            return;
    }
    points.push_back(point);
}

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
