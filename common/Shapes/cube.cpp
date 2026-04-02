#include "cube.h"


Cube::Cube(float size_x, float size_y, float size_z, std::vector<Vertex>& ver, std::vector<Triangle>& tri) : Cube(size_x, size_y, size_z) {
    setMesh(ver, tri);
    type = CUBE;
}

Cube::Cube(float size_x, float size_y, float size_z){
    size = glm::vec3(size_x, size_y, size_z);
    halfSize = size*0.5f;

    vertices.resize(8);
    triangles.resize(12);

    vertices[0] = glm::vec3( halfSize.x,  halfSize.y, -halfSize.z);
    vertices[1] = glm::vec3( halfSize.x, -halfSize.y, -halfSize.z);
    vertices[2] = glm::vec3( halfSize.x,  halfSize.y,  halfSize.z);
    vertices[3] = glm::vec3( halfSize.x, -halfSize.y,  halfSize.z);
    vertices[4] = glm::vec3(-halfSize.x,  halfSize.y, -halfSize.z);
    vertices[5] = glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z);
    vertices[6] = glm::vec3(-halfSize.x,  halfSize.y,  halfSize.z);
    vertices[7] = glm::vec3(-halfSize.x, -halfSize.y,  halfSize.z);
    radius = glm::length(vertices[0]);
    triangles[0] = glm::ivec3(4, 2, 0);
    triangles[1] = glm::ivec3(2, 7, 3);
    triangles[2] = glm::ivec3(6, 5, 7);
    triangles[3] = glm::ivec3(1, 7, 5);
    triangles[4] = glm::ivec3(0, 3, 1);
    triangles[5] = glm::ivec3(4, 1, 5);
    triangles[6] = glm::ivec3(4, 6, 2);
    triangles[7] = glm::ivec3(2, 6, 7);
    triangles[8] = glm::ivec3(6, 4, 5);
    triangles[9] = glm::ivec3(1, 3, 7);
    triangles[10] = glm::ivec3(0, 2, 3);
    triangles[11] = glm::ivec3(4, 0, 1);
}

void Cube::setMesh(std::vector<Vertex> &ver, std::vector<Triangle> &tri){
    ver.resize(24);
    ver[0] = Vertex(glm::vec3( halfSize.x,  halfSize.y, -halfSize.z), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.625, 0.5));
    ver[1] = Vertex(glm::vec3( halfSize.x,  halfSize.y,  halfSize.z), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.625, 0.75));
    ver[2] = Vertex(glm::vec3(-halfSize.x,  halfSize.y, -halfSize.z), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.875, 0.5));
    ver[3] = Vertex(glm::vec3(-halfSize.x,  halfSize.y,  halfSize.z), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.875, 0.75));

    ver[4] = Vertex(glm::vec3( halfSize.x, -halfSize.y, -halfSize.z), glm::vec3(0.0, -1.0, 0.0), glm::vec2(0.375, 0.5));
    ver[5] = Vertex(glm::vec3( halfSize.x, -halfSize.y,  halfSize.z), glm::vec3(0.0, -1.0, 0.0), glm::vec2(0.375, 0.75));
    ver[6] = Vertex(glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z), glm::vec3(0.0, -1.0, 0.0), glm::vec2(0.125, 0.5));
    ver[7] = Vertex(glm::vec3(-halfSize.x, -halfSize.y,  halfSize.z), glm::vec3(0.0, -1.0, 0.0), glm::vec2(0.125, 0.75));

    ver[8]  = Vertex(glm::vec3( halfSize.x,  halfSize.y, -halfSize.z), glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.625, 0.5));
    ver[9]  = Vertex(glm::vec3( halfSize.x,  halfSize.y,  halfSize.z), glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.625, 0.75));
    ver[10] = Vertex(glm::vec3( halfSize.x, -halfSize.y,  halfSize.z), glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.375, 0.75));
    ver[11] = Vertex(glm::vec3( halfSize.x, -halfSize.y, -halfSize.z), glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.375, 0.5));

    ver[12] = Vertex(glm::vec3(-halfSize.x,  halfSize.y, -halfSize.z), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.625, 0.25));
    ver[13] = Vertex(glm::vec3(-halfSize.x,  halfSize.y,  halfSize.z), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.625, 0.0));
    ver[14] = Vertex(glm::vec3(-halfSize.x, -halfSize.y,  halfSize.z), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.375, 0.0));
    ver[15] = Vertex(glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.375, 0.25));

    ver[16] = Vertex(glm::vec3(-halfSize.x,  halfSize.y,  halfSize.z), glm::vec3(0.0, 0.0, 1.0), glm::vec2(0.625, 1.0));
    ver[17] = Vertex(glm::vec3(-halfSize.x, -halfSize.y,  halfSize.z), glm::vec3(0.0, 0.0, 1.0), glm::vec2(0.375, 1.0));
    ver[18] = Vertex(glm::vec3( halfSize.x,  halfSize.y,  halfSize.z), glm::vec3(0.0, 0.0, 1.0), glm::vec2(0.625, 0.75));
    ver[19] = Vertex(glm::vec3( halfSize.x, -halfSize.y,  halfSize.z), glm::vec3(0.0, 0.0, 1.0), glm::vec2(0.375, 0.75));

    ver[20] = Vertex(glm::vec3(-halfSize.x,  halfSize.y, -halfSize.z), glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.625, 0.25));
    ver[21] = Vertex(glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z), glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.375, 0.25));
    ver[22] = Vertex(glm::vec3( halfSize.x,  halfSize.y, -halfSize.z), glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.625, 0.5));
    ver[23] = Vertex(glm::vec3( halfSize.x, -halfSize.y, -halfSize.z), glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.375, 0.5));

    tri.resize(12);
    tri[0] = Triangle(0, 2, 1);
    tri[1] = Triangle(1, 2, 3);

    tri[2] = Triangle(4, 5, 6);
    tri[3] = Triangle(6, 5, 7);

    tri[4] = Triangle(8, 9, 10);
    tri[5] = Triangle(8, 10, 11);

    tri[6] = Triangle(12, 14, 13);
    tri[7] = Triangle(12, 15, 14);

    tri[8] = Triangle(16, 17, 18);
    tri[9] = Triangle(17, 19, 18);

    tri[10] = Triangle(20, 22, 21);
    tri[11] = Triangle(21, 22, 23);
}

RayIntersection Cube::raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length){
    RayIntersection intersection;
    intersection.intersectionExists = false;

    glm::mat4 model = collider->globalMatrix();
    glm::mat4 invertedModel = glm::inverse(model);

    glm::vec3 localOrigin = glm::vec3(invertedModel * glm::vec4(origin, 1.0f));
    glm::vec3 localDir    = glm::normalize(glm::vec3(invertedModel * glm::vec4(direction, 0.0f)));

    glm::vec3 minB = vertices[5];
    glm::vec3 maxB = vertices[2];

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


ColliderIntersection Cube::intersectCube(Cube* cube){
    ColliderIntersection intersection = ColliderIntersection();
    intersection.t = INFINITY;
    glm::vec3 centerA = collider->globalPosition();
    glm::vec3 centerB = cube->collider->globalPosition();
    std::array<glm::vec3, 15> axes = std::array<glm::vec3, 15>();
    glm::quat cubeArotation = collider->globalRotation();
    glm::quat cubeBrotation = cube->collider->globalRotation();
    axes[0] = cubeArotation * RIGHT;
    axes[1] = cubeArotation * UP;
    axes[2] = cubeArotation * FORWARD;
    axes[3] = cubeBrotation * RIGHT;
    axes[4] = cubeBrotation * UP;
    axes[5] = cubeBrotation * FORWARD;
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            axes[6+i*3+j] = glm::cross(axes[i], axes[3+j]);
        }
    }


    for(int i = 0; i < 15; i ++){
        if(glm::length2(axes[i]) > 1e-6f){
            float centerProjectionA = dot(centerA, axes[i]);
            float centerProjectionB = dot(centerB, axes[i]);
            float radiusA = halfSize.x * abs(dot(axes[i], axes[0])) + halfSize.y * abs(dot(axes[i], axes[1])) + halfSize.z * abs(dot(axes[i], axes[2]));
            float radiusB = cube->halfSize.x * abs(dot(axes[i], axes[3])) + cube->halfSize.y * abs(dot(axes[i], axes[4])) + cube->halfSize.z * abs(dot(axes[i], axes[5]));
            float minA = centerProjectionA - radiusA;
            float maxA = centerProjectionA + radiusA;
            float minB = centerProjectionB - radiusB;
            float maxB = centerProjectionB + radiusB;

            if (maxA < minB || maxB < minA) {
                intersection.intersectionExist = false;
                return intersection;
            }

            float overlap = std::min(maxA, maxB) - std::max(minA, minB);
            if(overlap < intersection.t){
                intersection.t = overlap;
                intersection.intersectionExist = true;
                intersection.collider = cube->collider;
                intersection.axis = axes[i];
            }
        }
    }
    return intersection;
}
