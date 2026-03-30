#include "cube.h"

Cube::Cube(float size_x, float size_y, float size_z){
    this->size_x = size_x;
    this->size_y = size_y;
    this->size_z = size_z;

    vertices.resize(8);
    triangles.resize(12);

    vertices[0] = glm::vec3(-0.5*size_x, -0.5*size_y, 0.5*size_z);
    vertices[1] = glm::vec3(-0.5*size_x, 0.5*size_y, 0.5*size_z);
    vertices[2] = glm::vec3(-0.5*size_x, -0.5*size_y, -0.5*size_z);
    vertices[3] = glm::vec3(-0.5*size_x, 0.5*size_y, -0.5*size_z);
    vertices[4] = glm::vec3(0.5*size_x, -0.5*size_y, 0.5*size_z);
    vertices[5] = glm::vec3(0.5*size_x, 0.5*size_y, 0.5*size_z);
    vertices[6] = glm::vec3(0.5*size_x, -0.5*size_y, -0.5*size_z);
    vertices[7] = glm::vec3(0.5*size_x, 0.5*size_y, -0.5*size_z);
    radius = glm::length(vertices[0]);
    triangles[0] = glm::ivec3(1, 2, 0);
    triangles[1] = glm::ivec3(1, 3, 2);
    triangles[2] = glm::ivec3(3, 6, 2);
    triangles[3] = glm::ivec3(7, 4, 6);
    triangles[4] = glm::ivec3(3, 5, 7);
    triangles[5] = glm::ivec3(3, 7, 6);
    triangles[6] = glm::ivec3(6, 0, 2);
    triangles[7] = glm::ivec3(6, 4, 0);
    triangles[8] = glm::ivec3(5, 0, 4);
    triangles[9] = glm::ivec3(7, 5, 4);
    triangles[10] = glm::ivec3(5, 1, 0);
    triangles[11] = glm::ivec3(3, 1, 5);
}




RayIntersection Cube::raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length){
    RayIntersection intersection;
    intersection.intersectionExists = false;

    glm::mat4 model = collider->globalMatrix();
    glm::mat4 invertedModel = glm::inverse(model);

    glm::vec3 localOrigin = glm::vec3(invertedModel * glm::vec4(origin, 1.0f));
    glm::vec3 localDir    = glm::normalize(glm::vec3(invertedModel * glm::vec4(direction, 0.0f)));

    glm::vec3 minB = vertices[2];
    glm::vec3 maxB = vertices[5];

    glm::vec3 invDir = 1.0f / localDir;

    glm::vec3 t0 = (minB - localOrigin) * invDir;
    glm::vec3 t1 = (maxB - localOrigin) * invDir;

    glm::vec3 tmin = glm::min(t0, t1);
    glm::vec3 tmax = glm::max(t0, t1);

    float tEnter = std::max(std::max(tmin.x, tmin.y), tmin.z);
    float tExit  = std::min(std::min(tmax.x, tmax.y), tmax.z);

    if (tEnter > tExit || tExit < 0.0f)
        return intersection;

    float tLocal = (tEnter >= 0.0f) ? tEnter : tExit;

    if (tLocal < 0.0f)
        return intersection;

    glm::vec3 localPoint = localOrigin + tLocal * localDir;

    glm::vec3 localNormal(0.0f);
    const float eps = 1e-4f;

    if (fabs(localPoint.x - minB.x) < eps) localNormal = glm::vec3(-1, 0, 0);
    else if (fabs(localPoint.x - maxB.x) < eps) localNormal = glm::vec3(1, 0, 0);
    else if (fabs(localPoint.y - minB.y) < eps) localNormal = glm::vec3(0, -1, 0);
    else if (fabs(localPoint.y - maxB.y) < eps) localNormal = glm::vec3(0, 1, 0);
    else if (fabs(localPoint.z - minB.z) < eps) localNormal = glm::vec3(0, 0, -1);
    else if (fabs(localPoint.z - maxB.z) < eps) localNormal = glm::vec3(0, 0, 1);

    glm::vec3 worldPoint = glm::vec3(model * glm::vec4(localPoint, 1.0f));

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    glm::vec3 worldNormal = glm::normalize(normalMatrix * localNormal);

    float tWorld = glm::length(worldPoint - origin);

    if (tWorld > length)
        return intersection;

    intersection.intersectionExists = true;
    intersection.t = tWorld;
    intersection.point = worldPoint;
    intersection.normal = worldNormal;

    return intersection;
}
