#pragma once
#include "terrain.h"

class ProceduralTerrain : public Terrain
{
public:
    ProceduralTerrain();
    ProceduralTerrain(int posX, int posY, int resX = 128, int resY = 128, float sizeX = 16, float sizeY = 16, float sizeZ = 1);
    virtual ~ProceduralTerrain(){}
    void InitMesh(int posX, int posY, int resX = 128, int resY = 128, float sizeX = 16, float sizeY = 16, float sizeZ = 1) override;
protected:
    float frequency;
    Collider3D* collision = nullptr;
};
