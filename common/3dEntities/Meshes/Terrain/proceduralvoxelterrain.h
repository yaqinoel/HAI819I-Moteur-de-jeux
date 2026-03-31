#pragma once
#include "terrain.h"

class ProceduralVoxelTerrain : public Terrain
{
public:
    ProceduralVoxelTerrain();
    ProceduralVoxelTerrain(int posX, int posY, int resX = 128, int resY = 128, float sizeX = 16, float sizeY = 16, float sizeZ = 1);
    void InitMesh(int posX, int posY, int resX = 128, int resY = 128, float sizeX = 16, float sizeY = 16, float sizeZ = 1) override;
    void InitData(glm::ivec3 pos, int res, int size);
    void InitMesh();
    std::vector<Mesh*> cubes = std::vector<Mesh*>();
private:
    void CreateTopSquare(int x, int y, int z);
    void CreateBottomSquare(int x, int y, int z);
    void CreateRightSquare(int x, int y, int z);
    void CreateLeftSquare(int x, int y, int z);
    void CreateFrontSquare(int x, int y, int z);
    void CreateBackSquare(int x, int y, int z);
    float frequency;
    std::vector<std::vector<std::vector<unsigned short int>>> chunkData = std::vector<std::vector<std::vector<unsigned short int>>>();
    Mesh* mesh;
};

