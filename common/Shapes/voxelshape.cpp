#include "voxelshape.h"
#include "cube.h"

VoxelShape::VoxelShape() {
    type = VOXEL;
}

void VoxelShape::InitMesh(int width, int height, int depth, std::vector<unsigned short int> voxelData){
    this->width = width;
    this->height = height;
    this->depth = depth;
    radius = glm::length(glm::vec3(width, height, depth));
    voxels.resize(voxelData.size());
    for(int i = 0; i < voxelData.size(); i++){
        voxels[i] = voxelData[i] > 0;
    }
}
void VoxelShape::InitMeshFromTerrain(int width, int height, int depth, std::vector<unsigned short int> voxelData){
    this->width = width-2;
    this->height = height-2;
    this->depth = depth-2;
    radius = glm::length(glm::vec3(width, height, depth));
    voxels.resize(width*height*depth);

    for (int x = 1; x < width-1; x++) {
        for (int y = 1; y < height-1; y++) {
            for (int z = 1; z < depth-1; z++) {
                set(x-1, y-1, z-1, voxelData[x*width*height+ y*height+ z]);
            }
        }
    }
}

RayIntersection VoxelShape::raycast(glm::vec3 const &origin, glm::vec3 const &direction, float const &length)
{
    RayIntersection intersection;
    intersection.intersectionExists = false;
    if(direction == glm::vec3(0) || length <= 0) return intersection;

    glm::vec3 dir = glm::normalize(direction);
    glm::vec3 globalPos = collider->getGlobalPosition();

    // Transform ray into local voxel space
    glm::vec3 localOrigin = (origin - (globalPos + glm::vec3(voxelSize/2.0))) / voxelSize;

    // Current voxel cell
    int x = (int)std::floor(localOrigin.x);
    int y = (int)std::floor(localOrigin.y);
    int z = (int)std::floor(localOrigin.z);

    // Step direction per axis
    int stepX = (dir.x >= 0) ? 1 : -1;
    int stepY = (dir.y >= 0) ? 1 : -1;
    int stepZ = (dir.z >= 0) ? 1 : -1;

    // How far along the ray we must travel to cross one voxel boundary per axis
    float tDeltaX = (dir.x != 0) ? std::abs(voxelSize / dir.x) : std::numeric_limits<float>::infinity();
    float tDeltaY = (dir.y != 0) ? std::abs(voxelSize / dir.y) : std::numeric_limits<float>::infinity();
    float tDeltaZ = (dir.z != 0) ? std::abs(voxelSize / dir.z) : std::numeric_limits<float>::infinity();

    // Distance to first voxel boundary from origin
    auto tBoundary = [](float origin, float dir, int step) -> float {
        if (dir == 0) return std::numeric_limits<float>::infinity();
        float boundary = (step > 0) ? std::floor(origin) + 1.0f : std::ceil(origin) - 1.0f;
        return std::abs((boundary - origin) / dir);
    };

    float tMaxX = tBoundary(localOrigin.x, dir.x, stepX);
    float tMaxY = tBoundary(localOrigin.y, dir.y, stepY);
    float tMaxZ = tBoundary(localOrigin.z, dir.z, stepZ);

    float t = 0.0f;
    int face = -1; // 0=X, 1=Y, 2=Z — tracks which face was entered last

    while (t <= length)
    {
        // Check bounds
        if (x >= 0 && x < width && y >= 0 && y < height && z >= 0 && z < depth)
        {
            if (get(x, y, z) != 0)
            {
                intersection.intersectionExists = true;
                intersection.t = t;
                intersection.point = origin + dir * t;
                intersection.collider = collider;

                // Normal points opposite to the entered face
                glm::vec3 normal(0.0f);
                if      (face == 0) normal.x = -stepX;
                else if (face == 1) normal.y = -stepY;
                else if (face == 2) normal.z = -stepZ;
                intersection.normal = normal;

                return intersection;
            }
        }
        else if (t > 0) break;

        if (tMaxX < tMaxY && tMaxX < tMaxZ)
        {
            t = tMaxX;
            tMaxX += tDeltaX;
            x += stepX;
            face = 0;
        }
        else if (tMaxY < tMaxZ)
        {
            t = tMaxY;
            tMaxY += tDeltaY;
            y += stepY;
            face = 1;
        }
        else
        {
            t = tMaxZ;
            tMaxZ += tDeltaZ;
            z += stepZ;
            face = 2;
        }
    }

    return intersection;
}

std::vector<ColliderIntersection> VoxelShape::intersectCube(Cube* cube, bool calculatePoints)
{
    std::vector<ColliderIntersection> results;

    glm::vec3 globalPos  = collider->getGlobalPosition() + glm::vec3(voxelSize/2.0);
    glm::vec3 cubeCenter = cube->collider->getGlobalPosition();
    glm::quat cubeRot    = cube->collider->getGlobalRotation();

    glm::vec3 voxelAxes[3] = { glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1) };
    glm::vec3 cubeAxes[3]  = { cubeRot * RIGHT, cubeRot * UP, cubeRot * FORWARD };

    std::array<glm::vec3, 15> axes;
    axes[0] = voxelAxes[0]; axes[1] = voxelAxes[1]; axes[2] = voxelAxes[2];
    axes[3] = cubeAxes[0];  axes[4] = cubeAxes[1];  axes[5] = cubeAxes[2];
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            axes[6 + i*3 + j] = glm::normalize(glm::cross(voxelAxes[i], cubeAxes[j]));

    glm::vec3 voxelHalf = glm::vec3(voxelSize * 0.5f);

    // --- Broad phase: find AABB of the cube in voxel local space ---
    // Transform cube OBB into local voxel space to skip empty voxels cheaply
    glm::vec3 localCubeCenter = (cubeCenter - globalPos) / voxelSize;

    // Project cube half-extents onto each world axis to get a conservative AABB
    glm::vec3 cubeAABBHalf = glm::vec3(
                                 cube->halfSize.x * std::abs(cubeAxes[0].x) + cube->halfSize.y * std::abs(cubeAxes[1].x) + cube->halfSize.z * std::abs(cubeAxes[2].x),
                                 cube->halfSize.x * std::abs(cubeAxes[0].y) + cube->halfSize.y * std::abs(cubeAxes[1].y) + cube->halfSize.z * std::abs(cubeAxes[2].y),
                                 cube->halfSize.x * std::abs(cubeAxes[0].z) + cube->halfSize.y * std::abs(cubeAxes[1].z) + cube->halfSize.z * std::abs(cubeAxes[2].z)
                                 ) / voxelSize;

    int xMin = std::max(0, (int)std::floor(localCubeCenter.x - cubeAABBHalf.x));
    int xMax = std::min(width  - 1, (int)std::ceil (localCubeCenter.x + cubeAABBHalf.x));
    int yMin = std::max(0, (int)std::floor(localCubeCenter.y - cubeAABBHalf.y));
    int yMax = std::min(height - 1, (int)std::ceil (localCubeCenter.y + cubeAABBHalf.y));
    int zMin = std::max(0, (int)std::floor(localCubeCenter.z - cubeAABBHalf.z));
    int zMax = std::min(depth  - 1, (int)std::ceil (localCubeCenter.z + cubeAABBHalf.z));

    for (int x = xMin; x <= xMax; x++)
        for (int y = yMin; y <= yMax; y++)
            for (int z = zMin; z <= zMax; z++)
            {
                if (get(x, y, z) == 0) continue;

                glm::vec3 voxelCenter = globalPos + glm::vec3(
                                            (x + 0.5f) * voxelSize,
                                            (y + 0.5f) * voxelSize,
                                            (z + 0.5f) * voxelSize
                                            );

                ColliderIntersection local;
                local.intersectionExist = true;
                local.t = INFINITY;
                bool separated = false;

                for (int i = 0; i < 15 && !separated; i++)
                {
                    if (glm::length2(axes[i]) <= 1e-6f) continue;

                    glm::vec3 axis = axes[i];

                    float centerProjV = glm::dot(voxelCenter, axis);
                    float radiusV = voxelHalf.x * std::abs(glm::dot(axis, voxelAxes[0]))
                                    + voxelHalf.y * std::abs(glm::dot(axis, voxelAxes[1]))
                                    + voxelHalf.z * std::abs(glm::dot(axis, voxelAxes[2]));

                    float centerProjC = glm::dot(cubeCenter, axis);
                    float radiusC = cube->halfSize.x * std::abs(glm::dot(axis, cubeAxes[0]))
                                    + cube->halfSize.y * std::abs(glm::dot(axis, cubeAxes[1]))
                                    + cube->halfSize.z * std::abs(glm::dot(axis, cubeAxes[2]));

                    float minV = centerProjV - radiusV, maxV = centerProjV + radiusV;
                    float minC = centerProjC - radiusC, maxC = centerProjC + radiusC;

                    if (maxV < minC || maxC < minV) { separated = true; break; }

                    float overlap = std::min(maxV, maxC) - std::max(minV, minC);
                    if (overlap < local.t)
                    {
                        local.t    = overlap;
                        local.axis = axis;
                        local.colliderA = collider;
                        local.colliderB = cube->collider;
                        if (glm::dot(local.axis, cubeCenter - voxelCenter) < 0)
                            local.axis *= -1;
                    }
                }

                if (separated) continue;

                // --- Contact point generation for this voxel ---
                if (calculatePoints)
                {
                    float maxDotV = -FLT_MAX;
                    int   axisIndexV = 0;
                    bool  reversedV  = false;
                    glm::vec3 axisV;

                    for (int i = 0; i < 3; i++) {
                        float d = glm::dot(local.axis, voxelAxes[i]);
                        if (std::fabs(d) > maxDotV) {
                            maxDotV    = std::fabs(d);
                            reversedV  = d < 0;
                            axisV      = reversedV ? -voxelAxes[i] : voxelAxes[i];
                            axisIndexV = i;
                        }
                    }

                    float maxDotC = -FLT_MAX;
                    int   axisIndexC = 0;
                    bool  reversedC  = false;
                    glm::vec3 axisC;

                    for (int i = 0; i < 3; i++) {
                        float d = glm::dot(local.axis, cubeAxes[i]);
                        if (std::fabs(d) > maxDotC) {
                            maxDotC    = std::fabs(d);
                            reversedC  = d < 0;
                            axisC      = reversedC ? -cubeAxes[i] : cubeAxes[i];
                            axisIndexC = i;
                        }
                    }

                    bool voxelIsReference = maxDotC < maxDotV;

                    glm::vec3 refCenter, incCenter;
                    glm::vec3 refAxes_[3], incAxes_[3];
                    glm::vec3 refHalf,  incHalf;
                    glm::vec3 referenceAxis, incidentAxis;
                    int refIndex, incIndex;

                    if (voxelIsReference) {
                        referenceAxis  = axisV;
                        incidentAxis   = -axisC;
                        refAxes_[0]    = voxelAxes[0]; refAxes_[1] = voxelAxes[1]; refAxes_[2] = voxelAxes[2];
                        refHalf        = voxelHalf;
                        refCenter      = voxelCenter;
                        refIndex       = axisIndexV;
                        incAxes_[0]    = cubeAxes[0];  incAxes_[1] = cubeAxes[1];  incAxes_[2] = cubeAxes[2];
                        incHalf        = cube->halfSize;
                        incCenter      = cubeCenter;
                        incIndex       = axisIndexC;
                        local.featureA = axisIndexV + 3 * (int)reversedV;
                        local.featureB = axisIndexC + 3 * (int)reversedC;
                    } else {
                        referenceAxis  = axisC;
                        incidentAxis   = -axisV;
                        refAxes_[0]    = cubeAxes[0];  refAxes_[1] = cubeAxes[1];  refAxes_[2] = cubeAxes[2];
                        refHalf        = cube->halfSize;
                        refCenter      = cubeCenter;
                        refIndex       = axisIndexC;
                        incAxes_[0]    = voxelAxes[0]; incAxes_[1] = voxelAxes[1]; incAxes_[2] = voxelAxes[2];
                        incHalf        = voxelHalf;
                        incCenter      = voxelCenter;
                        incIndex       = axisIndexV;
                        local.colliderA = cube->collider;
                        local.colliderB = collider;
                        local.axis     *= -1;
                        local.featureA  = axisIndexC + 3 * (int)reversedC;
                        local.featureB  = axisIndexV + 3 * (int)reversedV;
                    }

                    glm::vec3 incU = incAxes_[(incIndex+1)%3] * incHalf[(incIndex+1)%3];
                    glm::vec3 incV = incAxes_[(incIndex+2)%3] * incHalf[(incIndex+2)%3];
                    glm::vec3 incFaceCenter = incCenter + incidentAxis * incHalf[incIndex];

                    std::vector<glm::vec3> incidentVerts = {
                        incFaceCenter + incU + incV,
                        incFaceCenter - incU + incV,
                        incFaceCenter - incU - incV,
                        incFaceCenter + incU - incV
                    };

                    glm::vec3 refU = refAxes_[(refIndex+1)%3] * refHalf[(refIndex+1)%3];
                    glm::vec3 refV = refAxes_[(refIndex+2)%3] * refHalf[(refIndex+2)%3];
                    glm::vec3 refFaceCenter = refCenter + referenceAxis * refHalf[refIndex];

                    std::vector<glm::vec3> referenceVerts = {
                        refFaceCenter + refU + refV,
                        refFaceCenter - refU + refV,
                        refFaceCenter - refU - refV,
                        refFaceCenter + refU - refV
                    };

                    glm::vec3 refUaxis = refAxes_[(refIndex+1)%3];
                    glm::vec3 refVaxis = refAxes_[(refIndex+2)%3];

                    std::vector<glm::vec3> sidePlaneNormals = { refUaxis, refVaxis, -refUaxis, -refVaxis };
                    std::vector<glm::vec3> clipped = clipPolygon(incidentVerts, sidePlaneNormals, referenceVerts);
                    local.contactPoints = projectToPlane(clipped, referenceAxis, refFaceCenter);
                    local.intersectionExist = true;
                }

                results.push_back(local);
            }

    return results;
}
