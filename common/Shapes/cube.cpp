#include "cube.h"
#include <cmath>


Cube::Cube(float size_x, float size_y, float size_z, std::vector<Vertex>& ver, std::vector<Triangle>& tri) : Cube(size_x, size_y, size_z) {
    setMesh(ver, tri);
    inertia = mass*glm::mat3(1.0/6.0);
    type = CUBE;
}

Cube::Cube(float size_x, float size_y, float size_z){
    size = glm::vec3(size_x, size_y, size_z);
    type = CUBE;
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
    inertia = mass*glm::mat3(1.0/6.0);
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

    glm::mat4 model = collider->getGlobalMatrix();
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


//this algorithm takes a polygon and a plane, and clip every vertices of the polygon so that they are under or on the plane
std::vector<glm::vec3> ClipPolygonToPlane(const std::vector<glm::vec3>& polygon, const glm::vec3& planeNormal, const glm::vec3& planePoint) {
    std::vector<glm::vec3> output;
    short int outputSize = 0;
    if(polygon.empty()) return output;

    for (size_t i = 0; i < polygon.size(); i++) {
        //the algorithm goes from edge to edge by going from vertex to vertex in a linear order
        glm::vec3 currentVertex  = polygon[i];
        glm::vec3 previousVertex = polygon[(i + polygon.size() - 1) % polygon.size()];

        //calculates the signed distance to the plane
        float currentDistance  = glm::dot(currentVertex - planePoint, planeNormal);
        float previousDistance = glm::dot(previousVertex - planePoint, planeNormal);

        if (currentDistance <= 0 && previousDistance <= 0) {
            output.push_back(currentVertex);
            outputSize ++;
        }
        else if (previousDistance <= 0 && currentDistance > 0) {
            // edge goes from inside to outside
            float t = previousDistance / (previousDistance - currentDistance);
            output.push_back(previousVertex + t * (currentVertex - previousVertex));
            outputSize ++;
        }
        else if (previousDistance > 0 && currentDistance <= 0) {
            // edge goes from outside to inside
            float t = previousDistance / (previousDistance - currentDistance);
            output.push_back(previousVertex + t * (currentVertex - previousVertex));
            output.push_back(currentVertex);
            outputSize +=2;
        }
        if(outputSize >= 4){
            return output;
        }
    }

    return output;
}
std::vector<glm::vec3> clipPolygon(const std::vector<glm::vec3>& polygon, const std::vector<glm::vec3>& planeNormals, const std::vector<glm::vec3>& planePoints) {
    std::vector<glm::vec3> result = polygon;
    for (size_t i = 0; i < planeNormals.size(); i++) {
        result = ClipPolygonToPlane(result, planeNormals[i], planePoints[i]);
        if (result.empty()) return result;
    }
    return result;
}

std::vector<glm::vec3> projectToPlane(const std::vector<glm::vec3>& points, const glm::vec3& planeNormal, const glm::vec3& planePoint) {
    std::vector<glm::vec3> projected;
    for (auto& p : points) {
        glm::vec3 proj = p - planeNormal * glm::dot(p - planePoint, planeNormal);
        projected.push_back(proj);
    }
    return projected;
}

ColliderIntersection Cube::intersectCube(Cube* cubeB, bool calculatePoints){
    ColliderIntersection intersection = ColliderIntersection();
    intersection.t = INFINITY;
    glm::vec3 centerA = collider->getGlobalPosition();
    glm::vec3 centerB = cubeB->collider->getGlobalPosition();
    std::array<glm::vec3, 15> axes = std::array<glm::vec3, 15>();
    glm::quat cubeArotation = collider->getGlobalRotation();
    glm::quat cubeBrotation = cubeB->collider->getGlobalRotation();
    axes[0] = cubeArotation * RIGHT;
    axes[1] = cubeArotation * UP;
    axes[2] = cubeArotation * FORWARD;
    axes[3] = cubeBrotation * RIGHT;
    axes[4] = cubeBrotation * UP;
    axes[5] = cubeBrotation * FORWARD;
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            axes[6+i*3+j] = glm::normalize(glm::cross(axes[i], axes[3+j]));
        }
    }


    for(int i = 0; i < 15; i ++){
        if(glm::length2(axes[i]) > 1e-6f){
            float centerProjectionA = dot(centerA, axes[i]);
            float centerProjectionB = dot(centerB, axes[i]);
            float radiusA = halfSize.x * abs(dot(axes[i], axes[0])) + halfSize.y * abs(dot(axes[i], axes[1])) + halfSize.z * abs(dot(axes[i], axes[2]));
            float radiusB = cubeB->halfSize.x * abs(dot(axes[i], axes[3])) + cubeB->halfSize.y * abs(dot(axes[i], axes[4])) + cubeB->halfSize.z * abs(dot(axes[i], axes[5]));
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
                intersection.colliderA = collider;
                intersection.colliderB = cubeB->collider;
                intersection.axis = axes[i];
                if(dot(intersection.axis, centerB-centerA) < 0){
                    intersection.axis *= -1;
                }
            }
        }
    }
    if(calculatePoints && intersection.intersectionExist){
        //getting the reference and incident axes
        float maxA = -FLT_MAX;
        glm::vec3 axisA;
        int axisIndexA;
        bool reversedAxisA;

        for (int i = 0; i < 3; i++) {
            float d = glm::dot(intersection.axis, axes[i]);
            if (fabs(d) > maxA) {
                maxA = fabs(d);
                if(d > 0){
                    reversedAxisA = false;
                    axisA = axes[i];
                }
                else{
                    reversedAxisA = true;
                    axisA = -axes[i];
                }
                axisIndexA = i;
            }
        }

        float maxB = -FLT_MAX;
        glm::vec3 axisB;
        int axisIndexB;
        bool reversedAxisB;

        for (int i = 0; i < 3; i++) {
            float d = glm::dot(intersection.axis, axes[3 + i]);
            if (fabs(d) > maxB) {
                maxB = fabs(d);
                if(d > 0){
                    reversedAxisB = false;
                    axisB = axes[3+i];
                }
                else{
                    reversedAxisB = true;
                    axisB = -axes[3+i];
                }
                axisIndexB = 3+i;
            }
        }
        // Determine reference and incident axes
        glm::vec3 referenceAxis, incidentAxis;
        bool boxAisReference = maxB < maxA;

        glm::vec3 refCenter, incCenter;
        glm::vec3 refAxes[3], incAxes[3];
        glm::vec3 refHalf, incHalf;
        int refIndex, incIndex;

        if (boxAisReference) {
            referenceAxis = axisA;
            incidentAxis = -axisB;

            refAxes[0] = axes[0]; refAxes[1] = axes[1]; refAxes[2] = axes[2];
            refHalf = halfSize;
            refCenter = centerA;
            refIndex = axisIndexA;

            incAxes[0] = axes[3]; incAxes[1] = axes[4]; incAxes[2] = axes[5];
            incHalf = cubeB->halfSize;
            incCenter = centerB;
            incIndex = axisIndexB;

            intersection.featureA = axisIndexA+ 3*reversedAxisA;
            intersection.featureB = axisIndexB+ 3*reversedAxisB;

        } else {
            referenceAxis = axisB;
            incidentAxis = -axisA;
            refAxes[0] = axes[3]; refAxes[1] = axes[4]; refAxes[2] = axes[5];
            refHalf = cubeB->halfSize;
            refCenter = centerB;
            refIndex = axisIndexB;

            incAxes[0] = axes[0]; incAxes[1] = axes[1]; incAxes[2] = axes[2];
            incHalf = halfSize;
            incCenter = centerA;
            incIndex = axisIndexA;

            intersection.colliderA = cubeB->collider;
            intersection.colliderB = collider;
            intersection.axis *= -1;

            intersection.featureA = axisIndexB+ 3*reversedAxisB;
            intersection.featureB = axisIndexA+ 3*reversedAxisA;
        }

        // Compute incident face vertices
        glm::vec3 incU = incAxes[(incIndex+1)%3] * incHalf[(incIndex+1)%3];
        glm::vec3 incV = incAxes[(incIndex+2)%3] * incHalf[(incIndex+2)%3];
        glm::vec3 incidentFaceCenter = incCenter + incidentAxis * incHalf[incIndex%3];

        std::vector<glm::vec3> incidentVertices(4);
        incidentVertices[0] = incidentFaceCenter + incU + incV;
        incidentVertices[1] = incidentFaceCenter - incU + incV;
        incidentVertices[2] = incidentFaceCenter - incU - incV;
        incidentVertices[3] = incidentFaceCenter + incU - incV;

        // Compute reference face planes
        std::vector<glm::vec3> referenceVertices(4);
        glm::vec3 refUaxis = refAxes[(refIndex+1)%3];
        glm::vec3 refVaxis = refAxes[(refIndex+2)%3];
        glm::vec3 refU = refUaxis * refHalf[(refIndex+1)%3];
        glm::vec3 refV = refVaxis * refHalf[(refIndex+2)%3];
        glm::vec3 referenceFaceCenter = refCenter + referenceAxis * refHalf[refIndex%3];

        referenceVertices[0] = referenceFaceCenter + refU + refV;
        referenceVertices[1] = referenceFaceCenter - refU + refV;
        referenceVertices[2] = referenceFaceCenter - refU - refV;
        referenceVertices[3] = referenceFaceCenter + refU - refV;

        if(dot(intersection.axis, intersection.colliderB->getGlobalPosition()-intersection.colliderA->getGlobalPosition()) < 0){
            std::cout << "collA " << glm::to_string(centerA) << std::endl;
            std::cout << "collB " << glm::to_string(centerB) << std::endl;
            std::cout << "dir " << glm::to_string(centerB-centerA) << std::endl;
            std::cout << "collA " << glm::to_string(intersection.colliderA->getGlobalPosition()) << std::endl;
            std::cout << "collB " << glm::to_string(intersection.colliderB->getGlobalPosition()) << std::endl;
        }

        // Compute reference side planes
        std::vector<glm::vec3> sidePlanesNormals = std::vector<glm::vec3>({refUaxis, refVaxis, -refUaxis, -refVaxis});

        std::vector<glm::vec3> clipped = clipPolygon(incidentVertices, sidePlanesNormals, referenceVertices);
        std::vector<glm::vec3> contactPoints = projectToPlane(clipped, referenceAxis, referenceFaceCenter);
        intersection.contactPoints = contactPoints;
    }
    return intersection;
}
