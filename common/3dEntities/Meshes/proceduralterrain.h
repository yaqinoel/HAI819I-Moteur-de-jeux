#pragma once

#include "../Mesh.hpp"
#include <string>
#include "../collisionshape3d.h"

class ProceduralTerrain : public Mesh
{
public:
    ProceduralTerrain();
    ProceduralTerrain(int posX, int posY, int resX = 128, int resY = 128, float sizeX = 16, float sizeY = 16, float sizeZ = 1, float frequency = 0.5);
    virtual ~ProceduralTerrain(){}
    void InitMesh(int posX, int posY, int resX = 128, int resY = 128, float sizeX = 16, float sizeY = 16, float sizeZ = 1, float frequency = 0.5);
    void ApplyHeightMap(std::string hmapName, float scale = 1);
private:
    int resX;
    int resY;
    float sizeX;
    float sizeY;
    float sizeZ;
    float frequency;
    std::string hmapName;
    CollisionShape3D* collision = nullptr;
};
