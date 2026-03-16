#pragma once

#include "common/3dEntities/Meshes/lod.h"
#include "common/3dEntities/Meshes/proceduralterrain.h"
#include "node3d.h"
#include "camera.h"
#include <map>

#include <future>

struct ivec2Compare {
    bool operator()(const glm::ivec2& a, const glm::ivec2& b) const {
        if (a.x != b.x) return a.x < b.x;
        return a.y < b.y;
    }
};

class TerrainManager : public Node3d
{
public:
    TerrainManager();
    void process(float deltaTime) override;
    void setCam(Camera* cam);
    void UpdateTerrain(glm::ivec2 newCamPosition);
    void initTerrain();
    //Node* MakeChunk(int x, int y, int indexX, int indexY);
    int chunkRenderDistance = 4;
    RayIntersection intersect(glm::vec3 const &origin, glm::vec3 const &direction, float const &length);
    float chunkSize = 40;
    Material* terrainMat;
private:
    glm::ivec2 prevCamPosition;
    Camera* cam;
    //std::vector<std::vector<Mesh*>> chunks = std::vector<std::vector<Mesh*>>();
    void printchunks();
    std::mutex terrainMutex;
    std::map<glm::ivec2, LOD*, ivec2Compare> chunks = std::map<glm::ivec2, LOD*, ivec2Compare>();
};
