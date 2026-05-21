#include "capsuleshape.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <glm/gtx/norm.hpp>

namespace {

constexpr float kPi = 3.14159265358979323846f;
constexpr float kRayEpsilon = 1e-6f;

void setClosestHit(RayIntersection& hit,
                   float localT,
                   const glm::vec3& localOrigin,
                   const glm::vec3& localDirection,
                   const glm::vec3& localNormal,
                   const glm::mat4& model,
                   const glm::vec3& worldOrigin,
                   float maxLength) {
    if (localT < 0.0f)
        return;

    glm::vec3 localPoint = localOrigin + localDirection * localT;
    glm::vec3 worldPoint = glm::vec3(model * glm::vec4(localPoint, 1.0f));
    float worldT = glm::length(worldPoint - worldOrigin);
    if (worldT > maxLength || worldT >= hit.t)
        return;

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    glm::vec3 worldNormal = normalMatrix * localNormal;
    if (glm::length2(worldNormal) <= kRayEpsilon)
        return;

    hit.intersectionExists = true;
    hit.t = worldT;
    hit.point = worldPoint;
    hit.normal = glm::normalize(worldNormal);
}

}

CapsuleShape::CapsuleShape(float radius, float height) {
    type = CAPSULE;
    capsuleRadius = std::max(0.0f, radius);
    this->height = std::max(height, capsuleRadius * 2.0f);
    halfSegment = std::max(0.0f, this->height * 0.5f - capsuleRadius);
    this->radius = halfSegment + capsuleRadius;

    float diameter = capsuleRadius * 2.0f;
    inertia = glm::mat3(0.0f);
    inertia[0][0] = mass * (this->height * this->height + diameter * diameter) / 12.0f;
    inertia[1][1] = mass * (diameter * diameter + diameter * diameter) / 12.0f;
    inertia[2][2] = mass * (diameter * diameter + this->height * this->height) / 12.0f;

    int slices = 16;
    int hemisphereRings = 8;
    std::vector<float> ringY;
    std::vector<float> ringRadius;

    for (int i = 0; i <= hemisphereRings; ++i) {
        float phi = (static_cast<float>(i) / static_cast<float>(hemisphereRings)) * (kPi * 0.5f);
        ringY.push_back(halfSegment + std::cos(phi) * capsuleRadius);
        ringRadius.push_back(std::sin(phi) * capsuleRadius);
    }
    for (int i = 0; i <= hemisphereRings; ++i) {
        float phi = (kPi * 0.5f) + (static_cast<float>(i) / static_cast<float>(hemisphereRings)) * (kPi * 0.5f);
        ringY.push_back(-halfSegment + std::cos(phi) * capsuleRadius);
        ringRadius.push_back(std::sin(phi) * capsuleRadius);
    }

    vertices.clear();
    triangles.clear();
    for (size_t ring = 0; ring < ringY.size(); ++ring) {
        for (int slice = 0; slice <= slices; ++slice) {
            float theta = (static_cast<float>(slice) / static_cast<float>(slices)) * kPi * 2.0f;
            vertices.emplace_back(std::cos(theta) * ringRadius[ring],
                                  ringY[ring],
                                  std::sin(theta) * ringRadius[ring]);
        }
    }

    for (size_t ring = 0; ring + 1 < ringY.size(); ++ring) {
        int rowA = static_cast<int>(ring) * (slices + 1);
        int rowB = static_cast<int>(ring + 1) * (slices + 1);
        for (int slice = 0; slice < slices; ++slice) {
            int a = rowA + slice;
            int b = rowA + slice + 1;
            int c = rowB + slice;
            int d = rowB + slice + 1;
            triangles.emplace_back(a, c, b);
            triangles.emplace_back(b, c, d);
        }
    }
}

bool CapsuleShape::computeAabb(const Collider3D& collider, PhysicsAabb& outAabb) const {
    glm::vec3 center = collider.getGlobalPosition();
    glm::vec3 axis = collider.getGlobalRotation() * glm::vec3(0.0f, 1.0f, 0.0f);
    if (glm::length2(axis) <= kRayEpsilon)
        axis = glm::vec3(0.0f, 1.0f, 0.0f);
    axis = glm::normalize(axis);

    glm::vec3 endpointA = center + axis * halfSegment;
    glm::vec3 endpointB = center - axis * halfSegment;
    glm::vec3 extents(capsuleRadius);

    outAabb.min = glm::min(endpointA, endpointB) - extents;
    outAabb.max = glm::max(endpointA, endpointB) + extents;
    return capsuleRadius >= 0.0f && height >= capsuleRadius * 2.0f;
}

RayIntersection CapsuleShape::raycast(glm::vec3 const& origin,
                                      glm::vec3 const& direction,
                                      float const& length) {
    RayIntersection intersection;
    intersection.intersectionExists = false;
    intersection.collider = collider;
    intersection.t = std::numeric_limits<float>::max();

    if (!collider || glm::length2(direction) <= kRayEpsilon || length <= 0.0f)
        return intersection;

    glm::mat4 model = collider->getGlobalMatrix();
    glm::mat4 inverseModel = glm::inverse(model);
    glm::vec3 localOrigin = glm::vec3(inverseModel * glm::vec4(origin, 1.0f));
    glm::vec3 localDirection = glm::vec3(inverseModel * glm::vec4(glm::normalize(direction), 0.0f));
    if (glm::length2(localDirection) <= kRayEpsilon)
        return intersection;
    localDirection = glm::normalize(localDirection);

    auto testSphere = [&](const glm::vec3& center) {
        glm::vec3 oc = localOrigin - center;
        float b = 2.0f * glm::dot(oc, localDirection);
        float c = glm::dot(oc, oc) - capsuleRadius * capsuleRadius;
        float discriminant = b * b - 4.0f * c;
        if (discriminant < 0.0f)
            return;

        float root = std::sqrt(discriminant);
        float t0 = (-b - root) * 0.5f;
        float t1 = (-b + root) * 0.5f;
        float t = t0 >= 0.0f ? t0 : t1;
        if (t < 0.0f)
            return;

        glm::vec3 localPoint = localOrigin + localDirection * t;
        glm::vec3 localNormal = localPoint - center;
        if (glm::length2(localNormal) <= kRayEpsilon)
            localNormal = glm::vec3(0.0f, 1.0f, 0.0f);
        setClosestHit(intersection, t, localOrigin, localDirection, glm::normalize(localNormal), model, origin, length);
    };

    testSphere(glm::vec3(0.0f, halfSegment, 0.0f));
    testSphere(glm::vec3(0.0f, -halfSegment, 0.0f));

    float a = localDirection.x * localDirection.x + localDirection.z * localDirection.z;
    if (a > kRayEpsilon) {
        float b = 2.0f * (localOrigin.x * localDirection.x + localOrigin.z * localDirection.z);
        float c = localOrigin.x * localOrigin.x + localOrigin.z * localOrigin.z - capsuleRadius * capsuleRadius;
        float discriminant = b * b - 4.0f * a * c;
        if (discriminant >= 0.0f) {
            float root = std::sqrt(discriminant);
            float invDenom = 1.0f / (2.0f * a);
            float candidates[2] = {(-b - root) * invDenom, (-b + root) * invDenom};
            for (float t : candidates) {
                if (t < 0.0f)
                    continue;
                glm::vec3 localPoint = localOrigin + localDirection * t;
                if (localPoint.y < -halfSegment || localPoint.y > halfSegment)
                    continue;
                glm::vec3 localNormal(localPoint.x, 0.0f, localPoint.z);
                if (glm::length2(localNormal) <= kRayEpsilon)
                    continue;
                setClosestHit(intersection, t, localOrigin, localDirection, glm::normalize(localNormal), model, origin, length);
            }
        }
    }

    return intersection;
}
