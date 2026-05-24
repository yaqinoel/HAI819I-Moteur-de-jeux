#pragma once

#include "voxelshape.h"

class TerrainVoxelShape : public VoxelShape
{
public:
    TerrainVoxelShape();
    void InitMeshFromTerrain(int width, int height, int depth, std::vector<unsigned short int> voxelData);
    void set(int x, int y, int z, uint8_t v) override{ setCell(x - 1, y - 1, z - 1, v); }
};
