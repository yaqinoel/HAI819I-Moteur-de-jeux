#include "terrainvoxelshape.h"

#include "../collider3d.h"

#include <cmath>
#include <limits>

TerrainVoxelShape::TerrainVoxelShape() {
    type = TERRAIN_VOXEL;
}

void TerrainVoxelShape::InitMeshFromTerrain(int sourceWidth,
                                            int sourceHeight,
                                            int sourceDepth,
                                            std::vector<unsigned short int> voxelData) {
    width = sourceWidth - 2;
    height = sourceHeight - 2;
    depth = sourceDepth - 2;
    radius = glm::length(glm::vec3(sourceWidth, sourceHeight, sourceDepth));
    voxels.assign(width * height * depth, 0);

    for (int x = 1; x < sourceWidth - 1; x++) {
        for (int y = 1; y < sourceHeight - 1; y++) {
            for (int z = 1; z < sourceDepth - 1; z++) {
                setCell(x - 1,
                        y - 1,
                        z - 1,
                        voxelData[x * sourceHeight * sourceDepth + y * sourceDepth + z]);
            }
        }
    }

    localCenterOfMass = glm::vec3(0.0f);
    inertia = glm::mat3(0.0f);
}

RayIntersection TerrainVoxelShape::raycast(glm::vec3 const &origin,
                                           glm::vec3 const &direction,
                                           float const &length) {
    RayIntersection intersection;
    intersection.intersectionExists = false;
    if (direction == glm::vec3(0) || length <= 0)
        return intersection;

    glm::vec3 dir = glm::normalize(direction);
    glm::vec3 globalPos = collider->getGlobalPosition();

    glm::vec3 localOrigin = (origin - (globalPos + glm::vec3(voxelSize / 2.0f))) / voxelSize;

    int x = static_cast<int>(std::floor(localOrigin.x));
    int y = static_cast<int>(std::floor(localOrigin.y));
    int z = static_cast<int>(std::floor(localOrigin.z));

    int stepX = (dir.x >= 0) ? 1 : -1;
    int stepY = (dir.y >= 0) ? 1 : -1;
    int stepZ = (dir.z >= 0) ? 1 : -1;

    float infinity = std::numeric_limits<float>::infinity();
    float tDeltaX = (dir.x != 0) ? std::abs(voxelSize / dir.x) : infinity;
    float tDeltaY = (dir.y != 0) ? std::abs(voxelSize / dir.y) : infinity;
    float tDeltaZ = (dir.z != 0) ? std::abs(voxelSize / dir.z) : infinity;

    auto tBoundary = [infinity](float origin, float dir, int step) -> float {
        if (dir == 0.0f)
            return infinity;

        float boundary;
        if (step > 0) {
            boundary = std::floor(origin);
            if (origin > boundary)
                boundary += 1.0f;
        } else {
            boundary = std::ceil(origin);
            if (origin < boundary)
                boundary -= 1.0f;
        }

        return std::abs((boundary - origin) / dir);
    };

    float tMaxX = tBoundary(localOrigin.x, dir.x, stepX);
    float tMaxY = tBoundary(localOrigin.y, dir.y, stepY);
    float tMaxZ = tBoundary(localOrigin.z, dir.z, stepZ);

    float t = 0.0f;
    int face = -1;

    while (t <= length) {
        if (x >= 0 && x < width && y >= 0 && y < height && z >= 0 && z < depth) {
            if (get(x, y, z) != 0 && t <= length) {
                intersection.intersectionExists = true;
                intersection.t = t;
                intersection.point = origin + dir * t;
                intersection.collider = collider;

                glm::vec3 normal(0.0f);
                if (face == 0)
                    normal.x = -stepX;
                else if (face == 1)
                    normal.y = -stepY;
                else if (face == 2)
                    normal.z = -stepZ;
                intersection.normal = normal;

                return intersection;
            }
        } else {
            bool pastX = (x < 0 && stepX < 0) || (x >= width && stepX > 0);
            bool pastY = (y < 0 && stepY < 0) || (y >= height && stepY > 0);
            bool pastZ = (z < 0 && stepZ < 0) || (z >= depth && stepZ > 0);
            if (pastX || pastY || pastZ)
                break;
        }

        if (tMaxX < tMaxY && tMaxX < tMaxZ) {
            t = tMaxX;
            tMaxX += tDeltaX;
            x += stepX;
            face = 0;
        } else if (tMaxY < tMaxZ) {
            t = tMaxY;
            tMaxY += tDeltaY;
            y += stepY;
            face = 1;
        } else {
            t = tMaxZ;
            tMaxZ += tDeltaZ;
            z += stepZ;
            face = 2;
        }
    }

    return intersection;
}
