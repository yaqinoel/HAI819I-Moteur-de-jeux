#pragma once
#include "terrain.h"

class ProceduralVoxelTerrain : public Terrain
{
public:
    ProceduralVoxelTerrain();
    ProceduralVoxelTerrain(int posX, int posY, int resX = 128, int resY = 128, float sizeX = 16, float sizeY = 16, float sizeZ = 16);
    void InitMesh(int posX, int posY, int resX = 128, int resY = 128, float sizeX = 16, float sizeY = 16, float sizeZ = 16) override;
    void InitData(glm::ivec3 pos, int res, int size);
    void InitMesh();
    std::vector<Mesh*> cubes = std::vector<Mesh*>();
    glm::ivec2 texSize = glm::ivec2(1, 1);
    std::map<int, glm::ivec2> texCoord = std::map<int, glm::ivec2>();
    unsigned short int getData(int x, int y, int z){return chunkData[x*resolution*resolution+y*resolution+z];}
    void setData(int x, int y, int z, unsigned short int v){chunkData[x*resolution*resolution+y*resolution+z] = v;}
private:
    void CreateTopSquare(int x, int y, int z);
    void CreateBottomSquare(int x, int y, int z);
    void CreateRightSquare(int x, int y, int z);
    void CreateLeftSquare(int x, int y, int z);
    void CreateFrontSquare(int x, int y, int z);
    void CreateBackSquare(int x, int y, int z);
    int resolution = 0;
    float frequency;
    //std::vector<std::vector<std::vector<unsigned short int>>> chunkData = std::vector<std::vector<std::vector<unsigned short int>>>();
    std::vector<unsigned short int> chunkData = std::vector<unsigned short int>();
    void printSlice(int z);
    Collider3D* collision = nullptr;
};

