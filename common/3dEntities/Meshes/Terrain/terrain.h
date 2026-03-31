#pragma once
#include "../../Mesh.hpp"
#include <string>
#include "../../collisionshape3d.h"
#include "../../../Shapes/terrainshape.h"

class Terrain  : public Mesh
{
public:
    Terrain();
    virtual void InitMesh(int posX, int posY, int resX = 128, int resY = 128, float sizeX = 16, float sizeY = 16, float sizeZ = 1);
protected:
    int resX;
    int resY;
    float sizeX;
    float sizeY;
    float sizeZ;
};
