#pragma once

#include "convexshape.h"
#include <glm/gtc/noise.hpp>

class TerrainShape : public ConvexShape
{
public:
    TerrainShape();
    void InitMesh(int posX, int posY , int resX, int resY, float sizeX , float sizeY, float sizeZ, float frequency);
};

