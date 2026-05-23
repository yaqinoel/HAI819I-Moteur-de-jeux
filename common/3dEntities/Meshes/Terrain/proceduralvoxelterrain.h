#pragma once
#include "terrain.h"

#include <unordered_map>

#include <common/Utilities/hash.h>

class Collider3D;
class TerrainVoxelShape;

class ProceduralVoxelTerrain : public Terrain
{
public:
    ProceduralVoxelTerrain();
    ProceduralVoxelTerrain(int posX, int posY, int resX = 128, int resY = 128, float sizeX = 16, float sizeY = 16, float sizeZ = 16, std::unordered_map<glm::ivec3, int, IVec3Hash> *edited = nullptr);
    void InitData();
    void InitMesh();
    void ResetMesh();
    std::vector<Mesh*> cubes = std::vector<Mesh*>();
    glm::ivec2 texSize = glm::ivec2(1, 1);
    std::map<int, glm::ivec2> texCoord = std::map<int, glm::ivec2>();
    unsigned short int getData(int x, int y, int z);
    void setData(int x, int y, int z, unsigned short int v);
    int removeTile(glm::vec3 world_position);
    void addTile(glm::vec3 world_position, int v);
    ProceduralVoxelTerrain * neighbourX;
    ProceduralVoxelTerrain * neighbour_X;
    ProceduralVoxelTerrain * neighbourZ;
    ProceduralVoxelTerrain * neighbour_Z;
    std::unordered_map<glm::ivec3, int, IVec3Hash> *edited;


private:
    void CreateTopSquare(int x, int y, int z);
    void CreateBottomSquare(int x, int y, int z);
    void CreateRightSquare(int x, int y, int z);
    void CreateLeftSquare(int x, int y, int z);
    void CreateFrontSquare(int x, int y, int z);
    void CreateBackSquare(int x, int y, int z);
    int resolution = 0;
    float frequency;
    std::vector<unsigned short int> chunkData = std::vector<unsigned short int>();
    void printSlice(int z);
    TerrainVoxelShape* shape = nullptr;
    Collider3D* collision = nullptr;
};
