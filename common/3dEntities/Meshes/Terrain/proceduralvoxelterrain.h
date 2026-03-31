#pragma once
#include "terrain.h"

class ProceduralVoxelTerrain : public Terrain
{
public:
    ProceduralVoxelTerrain();
    ProceduralVoxelTerrain(int posX, int posY, int resX = 128, int resY = 128, float sizeX = 16, float sizeY = 16, float sizeZ = 1);
    void InitMesh(int posX, int posY, int resX = 128, int resY = 128, float sizeX = 16, float sizeY = 16, float sizeZ = 1) override;
    std::vector<Mesh*> cubes = std::vector<Mesh*>();
private:
    float frequency;
};

