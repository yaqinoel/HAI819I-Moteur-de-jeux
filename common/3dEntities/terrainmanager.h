#pragma once

#include "common/3dEntities/Meshes/proceduralterrain.h"
#include "node3d.h"
#include "camera.h"

#include <mutex>
#include <queue>


class TerrainManager : public Node3d
{
public:
    TerrainManager();
    void process(float deltaTime) override;
    void setCam(Camera* cam);
    void updateTerrain(glm::vec2 newCamPosition);
    void initTerrain();
    void MakeChunk(int x, int y, int indexX, int indexY);
    int chunkRenderDistance = 2;
    float size = 5;
    Material* terrainMat;
private:
    glm::vec2 prevCamPosition;
    Camera* cam;
    std::vector<std::vector<Mesh*>*> chunks = std::vector<std::vector<Mesh*>*>();
    void printchunks();

};
