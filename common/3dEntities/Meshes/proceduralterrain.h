#pragma once

#include "../Mesh.hpp"
#include <string>

class ProceduralTerrain : public Mesh
{
public:
    ProceduralTerrain();
    void InitMesh(int resX = 16, int resY = 16, float sizeX = 16, float sizeY = 16, float sizeZ = 1);
    void ApplyHeightMap(std::string hmapName, float scale = 1);
    void process(float deltaTime) override {Rotate(deltaTime*2, up());};
private:
    int resX;
    int resY;
    float sizeX;
    float sizeY;
    float sizeZ;
    std::string hmapName;
};
