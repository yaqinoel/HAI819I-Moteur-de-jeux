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

