#pragma once

#include "meshshape.h"
#include <glm/gtc/noise.hpp>

class TerrainShape : public MeshShape
{
public:
    TerrainShape();
    void InitMesh(int posX, int posY , int resX, int resY, float sizeX , float sizeY, float sizeZ, float frequency);
};

