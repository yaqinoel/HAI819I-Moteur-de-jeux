#include "cube.h"


Cube::Cube(float size_x, float size_y, float size_z, std::vector<Vertex>& ver, std::vector<Triangle>& tri) : Cube(size_x, size_y, size_z) {
    setMesh(ver, tri);
}

Cube::Cube(float size_x, float size_y, float size_z){
    this->size_x = size_x;
    this->size_y = size_y;
    this->size_z = size_z;

    vertices.resize(8);
    triangles.resize(12);

    vertices[0] = glm::vec3(0.5*size_x, 0.5*size_y, -0.5*size_z);
    vertices[1] = glm::vec3(0.5*size_x, -0.5*size_y, -0.5*size_z);
    vertices[2] = glm::vec3(0.5*size_x, 0.5*size_y, 0.5*size_z);
    vertices[3] = glm::vec3(0.5*size_x, -0.5*size_y, 0.5*size_z);
    vertices[4] = glm::vec3(-0.5*size_x, 0.5*size_y, -0.5*size_z);
    vertices[5] = glm::vec3(-0.5*size_x, -0.5*size_y, -0.5*size_z);
    vertices[6] = glm::vec3(-0.5*size_x, 0.5*size_y, 0.5*size_z);
    vertices[7] = glm::vec3(-0.5*size_x, -0.5*size_y, 0.5*size_z);
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
    ver[0] = Vertex(glm::vec3(0.5*size_x, 0.5*size_y, -0.5*size_z), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.625, 0.5));
    ver[1] = Vertex(glm::vec3(0.5*size_x, 0.5*size_y, 0.5*size_z), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.625, 0.75));
    ver[2] = Vertex(glm::vec3(-0.5*size_x, 0.5*size_y, -0.5*size_z), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.875, 0.5));
    ver[3] = Vertex(glm::vec3(-0.5*size_x, 0.5*size_y, 0.5*size_z), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.875, 0.75));

    ver[4] = Vertex(glm::vec3(0.5*size_x, -0.5*size_y, -0.5*size_z), glm::vec3(0.0, -1.0, 0.0), glm::vec2(0.375, 0.5));
    ver[5] = Vertex(glm::vec3(0.5*size_x, -0.5*size_y, 0.5*size_z), glm::vec3(0.0, -1.0, 0.0), glm::vec2(0.375, 0.75));
    ver[6] = Vertex(glm::vec3(-0.5*size_x, -0.5*size_y, -0.5*size_z), glm::vec3(0.0, -1.0, 0.0), glm::vec2(0.125, 0.5));
    ver[7] = Vertex(glm::vec3(-0.5*size_x, -0.5*size_y, 0.5*size_z), glm::vec3(0.0, -1.0, 0.0), glm::vec2(0.125, 0.75));

    ver[8] = Vertex(glm::vec3(0.5*size_x, 0.5*size_y, -0.5*size_z), glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.625, 0.5));
    ver[9] = Vertex(glm::vec3(0.5*size_x, 0.5*size_y, 0.5*size_z), glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.625, 0.75));
    ver[10] = Vertex(glm::vec3(0.5*size_x, -0.5*size_y, 0.5*size_z), glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.375, 0.75));
    ver[11] = Vertex(glm::vec3(0.5*size_x, -0.5*size_y, -0.5*size_z), glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.375, 0.5));

    ver[12] = Vertex(glm::vec3(-0.5*size_x, 0.5*size_y, -0.5*size_z), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.625, 0.25));
    ver[13] = Vertex(glm::vec3(-0.5*size_x, 0.5*size_y, 0.5*size_z), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.625, 0.0));
    ver[14] = Vertex(glm::vec3(-0.5*size_x, - 0.5*size_y, 0.5*size_z), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.375, 0.0));
    ver[15] = Vertex(glm::vec3(-0.5*size_x, - 0.5*size_y, -0.5*size_z), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.375, 0.25));

    ver[16] = Vertex(glm::vec3(-0.5*size_x, 0.5*size_y, 0.5*size_z), glm::vec3(0.0, 0.0, 1.0), glm::vec2(0.625, 1.0));
    ver[17] = Vertex(glm::vec3(-0.5*size_x, -0.5*size_y, 0.5*size_z), glm::vec3(0.0, 0.0, 1.0), glm::vec2(0.375, 1.0));
    ver[18] = Vertex(glm::vec3(0.5*size_x, 0.5*size_y, 0.5*size_z), glm::vec3(0.0, 0.0, 1.0), glm::vec2(0.625, 0.75));
    ver[19] = Vertex(glm::vec3(0.5*size_x, -0.5*size_y, 0.5*size_z), glm::vec3(0.0, 0.0, 1.0), glm::vec2(0.375, 0.75));

    ver[20] = Vertex(glm::vec3(-0.5*size_x, 0.5*size_y, -0.5*size_z), glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.625, 0.25));
    ver[21] = Vertex(glm::vec3(-0.5*size_x, -0.5*size_y, -0.5*size_z), glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.375, 0.25));
    ver[22] = Vertex(glm::vec3(0.5*size_x, 0.5*size_y, -0.5*size_z), glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.625, 0.5));
    ver[23] = Vertex(glm::vec3(0.5*size_x, -0.5*size_y, -0.5*size_z), glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.375, 0.5));

    tri.resize(12);
    tri[0] = Triangle(0, 1, 2);
    tri[1] = Triangle(2, 1, 3);
    tri[2] = Triangle(4, 6, 5);
    tri[3] = Triangle(5, 6, 7);
    tri[4] = Triangle(8, 9, 10);
    tri[5] = Triangle(8, 10, 11);
    tri[6] = Triangle(12, 13, 14);
    tri[7] = Triangle(13, 14, 15);
    tri[8] = Triangle(16, 18, 17);
    tri[9] = Triangle(17, 18, 19);
    tri[10] = Triangle(20, 21, 22);
    tri[11] = Triangle(21, 23, 22);
}


/*
 * v 1.000000, 1.000000, -1.000000
v 1.000000, -1.000000, -1.000000
v 1.000000, 1.000000, 1.000000
v 1.000000, -1.000000, 1.000000
v -1.000000, 1.000000, -1.000000
v -1.000000, -1.000000, -1.000000
v -1.000000, 1.000000, 1.000000
v -1.000000, -1.000000, 1.000000
vn0 -0.0000, 1.0000, -0.0000
vn1 -0.0000, -0.0000, 1.0000
vn2 -1.0000, -0.0000, -0.0000
vn3 -0.0000, -1.0000, -0.0000
vn4 1.0000, -0.0000, -0.0000
vn5 -0.0000, -0.0000, -1.0000
vt0 0.875000, 0.500000
vt1 0.625000, 0.750000
vt2 0.625000, 0.500000
vt3 0.375000, 1.000000
vt4 0.375000, 0.750000
vt5 0.625000, 0.000000
vt6 0.375000, 0.250000
vt7 0.375000, 0.000000
vt8 0.375000, 0.500000
vt9 0.125000, 0.750000
vt10 0.125000, 0.500000
vt11 0.625000, 0.250000
vt12 0.875000, 0.750000
vt13 0.625000, 1.000000
0: 0/2/0
1: 0/2/4
2: 0/2/5
3: 1/8/3
4: 1/8/4
5: 1/8/5
6: 2/1/0
7: 2/1/1
8: 2/1/4
9: 3/4/1
10: 3/4/3
11: 3/4/4
12: 4/0/0
13: 4/11/5
14: 4/11/2
15: 5/6/2
16: 5/6/5
17: 5/10/3
18: 6/5/2
19: 6/12/0
20: 6/13/1
21: 7/3/1
22: 7/7/2
23: 7/9/3

f 12, 6, 0
f 7, 21, 9
f 18, 15, 22
f 3, 23, 17
f 1, 11, 4
f 13, 5, 16
f 12, 19, 6
f 7, 20, 21
f 18, 14, 16
f 3, 10, 23
f 1, 8, 11
f 13, 2, 5
 */

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
