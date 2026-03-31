#pragma once

#include "common/3dEntities/Meshes/lod.h"
#include "common/3dEntities/Meshes/Terrain/proceduralterrain.h"
#include "node3d.h"
#include "camera.h"
#include <map>

#include <future>

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
    void UpdateTerrain(glm::ivec2 newCamPosition);
    void initTerrain();
    //Node* MakeChunk(int x, int y, int indexX, int indexY);
    int chunkRenderDistance = 2;
    float chunkSize = 10;
    Material* terrainMat;
private:
    glm::ivec3 prevCamPosition;
    Camera* cam;
    //std::vector<std::vector<Mesh*>> chunks = std::vector<std::vector<Mesh*>>();
    void printchunks();
    std::mutex terrainMutex;
    std::map<glm::ivec3, Mesh*, ivec3Compare> chunks = std::map<glm::ivec3, Mesh*, ivec3Compare>();
};
