#pragma once
#include "terrain.h"

class HeightMapTerrain : public Terrain
{
public:
    HeightMapTerrain();
    HeightMapTerrain(int posX, int posY, int resX = 128, int resY = 128, float sizeX = 16, float sizeY = 16, float sizeZ = 1);
    virtual ~HeightMapTerrain(){}
    void InitMesh(int posX, int posY, int resX = 128, int resY = 128, float sizeX = 16, float sizeY = 16, float sizeZ = 1) override;
    void ApplyHeightMap(std::string hmapName);
private:
    std::string hmapName;
    CollisionShape3D* collision = nullptr;
};
