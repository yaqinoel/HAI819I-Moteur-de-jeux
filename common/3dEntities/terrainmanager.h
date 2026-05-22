#pragma once

#include "common/3dEntities/Meshes/lod.h"
#include "common/3dEntities/Meshes/Terrain/proceduralterrain.h"
#include "node3d.h"
#include "camera.h"
#include <map>
#include <unordered_map>
#include <common/Utilities/hash.h>

class ProceduralVoxelTerrain;
struct ivec3Compare {
    bool operator()(const glm::ivec3& a, const glm::ivec3& b) const {
        if (a.x != b.x) return a.x < b.x;
        if (a.y != b.y) return a.y < b.y;
        return a.z < b.z;
    }
};

class TerrainManager : public Node3d
{
public:
    TerrainManager();
    void process(float deltaTime) override;
    void UpdateTerrain(glm::ivec3 newCamPosition);
    void initTerrain();
    //Node* MakeChunk(int x, int y, int indexX, int indexY);
    int chunkRenderDistance = 3;
    float chunkSize = 30;
    Material* terrainMat;
    bool usePBRChunks = false;
    std::unordered_map<glm::ivec3, int, IVec3Hash> *edited = new std::unordered_map<glm::ivec3, int, IVec3Hash>();
private:
    ProceduralVoxelTerrain* MakeVoxelChunk(int x, int y);
    glm::ivec3 prevCamPosition;
    Camera* cam;
    //std::vector<std::vector<Mesh*>> chunks = std::vector<std::vector<Mesh*>>();
    void printchunks();
    std::vector<glm::ivec3> not_initiated = std::vector<glm::ivec3>();
    std::map<glm::ivec3, ProceduralVoxelTerrain*, ivec3Compare> chunks = std::map<glm::ivec3, ProceduralVoxelTerrain*, ivec3Compare>();
};
