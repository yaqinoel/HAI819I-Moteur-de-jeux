#include "terrainmanager.h"
#include "../scene.h"

#include <common/3dEntities/Meshes/lod.h>
#include <common/3dEntities/Meshes/Terrain/proceduralvoxelterrain.h>

Mesh* MakeChunk(int x, int y, int size, Material* const terrainMat){
    LOD* lod = new LOD();
    lod->name = "terrain("+std::to_string(x) + ","+std::to_string(y)+")";
    lod->position = glm::vec3(x*size, 0, y*size);
    for(int i = 0; i < 5; i ++){
        Terrain* terrain = new ProceduralTerrain(x*size, y*size, 64/pow(2, i), 64/pow(2, i), size, size, 20);
        terrain->setMaterial(terrainMat);
        terrain->setShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader_Terrain_HeightMap.glsl");
        lod->addLOD(terrain, 40*i);
        terrain->name = "lod "+std::to_string(i);
    }
    return lod;
}

Mesh* MakeVoxelChunk(int x, int y, int size, Material* const terrainMat){
    Terrain* terrain = new ProceduralVoxelTerrain(x*size, y*size, size, size, size, size, 20);
    return terrain;
}



TerrainManager::TerrainManager() {
}


void TerrainManager::UpdateTerrain(glm::ivec2 newCamPosition){
    for(int i = -chunkRenderDistance; i <= chunkRenderDistance; i ++){
        for(int j = -chunkRenderDistance; j <= chunkRenderDistance; j++){
            glm::ivec2 chunkPos = newCamPosition + glm::ivec2(i, j);
            glm::vec2 chunkWorldPos = (glm::vec2)chunkPos*chunkSize;
            float chunkDistance = glm::length(chunkWorldPos-glm::vec2(cam->position.x, cam->position.z))-chunkSize;
            if(chunkDistance < chunkSize * chunkRenderDistance && chunks.find(chunkPos) == chunks.end()){
                Mesh * chunk = MakeVoxelChunk(chunkPos.x, chunkPos.y, chunkSize, terrainMat);
                instantiate(chunk, this);
                chunks.insert({chunkPos, chunk});
            }
        }
    }
    for (auto it = chunks.begin(); it != chunks.end(); )
    {
        glm::vec2 chunkWorldPos = glm::vec2(it->first) * chunkSize;

        if (glm::length(chunkWorldPos - glm::vec2(cam->position.x, cam->position.z)) > chunkSize * (chunkRenderDistance + 1))
        {
            scene->remove(it->second);
            it = chunks.erase(it);
        }
        else
        {
            ++it;
        }
    }
}


void TerrainManager::process(float deltaTime){
    Node3d::process(deltaTime);
    cam = scene->mainCamera;
    if(cam != nullptr){
        glm::ivec2 newCamPosition = glm::ivec2((roundf(cam->position.x/chunkSize)), (roundf(cam->position.z/chunkSize)));
        if(newCamPosition != prevCamPosition){
            UpdateTerrain(newCamPosition);
            prevCamPosition = newCamPosition;
        }
    }
}


void TerrainManager::initTerrain(){
    for(int x = -chunkRenderDistance; x <= chunkRenderDistance; x ++){
        for(int y = -chunkRenderDistance; y <= chunkRenderDistance; y ++){
            glm::vec2 chunkPos = prevCamPosition + glm::ivec2(x, y);
            Mesh * chunk = MakeVoxelChunk(prevCamPosition.x+x, prevCamPosition.y+y, chunkSize, terrainMat);
            instantiate(chunk, this);
            chunks.insert({chunkPos, chunk});
        }
    }
}
