#include "terrainmanager.h"
#include "../scene.h"

#include <common/3dEntities/Meshes/lod.h>
#include <common/3dEntities/Meshes/Terrain/proceduralvoxelterrain.h>

Mesh* MakeChunk(int x, int y, int size, Material* const terrainMat){
    LOD* lod = new LOD();
    lod->name = "terrain("+std::to_string(x) + ","+std::to_string(y)+")";
    lod->getLocalPosition() = glm::vec3(x*size, 0, y*size);
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
    ProceduralVoxelTerrain* terrain = new ProceduralVoxelTerrain(x*size, y*size, size, size, size, size, 20);
    terrain->setShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader.glsl");
    Material* mat = new Material(glm::vec3(1, 0., 0.));
    Texture tex = Texture("../Resources/Textures/Environement/BlocTextures.png");
    tex.setPixelArt(true);
    mat->addTexture("texture0", tex);
    mat->setLit(0);
    //terrain->meshDisplay = false;
    terrain->setMaterial(mat);
    terrain->name = "voxel terrain ("+std::to_string(x)+","+std::to_string(y)+")";
    return terrain;
}



TerrainManager::TerrainManager() {
}


void TerrainManager::UpdateTerrain(glm::ivec3 newCamPosition){
    for(int i = -chunkRenderDistance; i <= chunkRenderDistance; i ++){
        for(int j = -chunkRenderDistance; j <= chunkRenderDistance; j++){
            glm::ivec3 chunkPos = glm::ivec3(newCamPosition.x, 0, newCamPosition.z) + glm::ivec3(i, 0, j);
            glm::vec3 chunkWorldPos = (glm::vec3)chunkPos*chunkSize;
            float chunkDistance = glm::length(chunkWorldPos-glm::vec3(cam->getGlobalPosition().x,0, cam->getGlobalPosition().z))-chunkSize;
            if(chunkDistance < chunkSize * chunkRenderDistance && chunks.find(chunkPos) == chunks.end()){
                Mesh * chunk = MakeVoxelChunk(chunkPos.x, chunkPos.z, chunkSize, terrainMat);
                instantiate(chunk, this);
                chunks.insert({chunkPos, chunk});
            }
        }
    }
    for (auto it = chunks.begin(); it != chunks.end(); )
    {
        glm::vec3 chunkWorldPos = glm::vec3(it->first) * chunkSize;

        if (glm::length(chunkWorldPos - glm::vec3(cam->getGlobalPosition().x,0, cam->getGlobalPosition().z)) > chunkSize * (chunkRenderDistance + 1))
        {
            (it->second)->erase();
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
        glm::ivec3 newCamPosition = glm::ivec3((roundf(cam->getGlobalPosition().x/chunkSize)), (roundf(cam->getGlobalPosition().y / chunkSize)), (roundf(cam->getGlobalPosition().z/chunkSize)));
        if(newCamPosition != prevCamPosition){
            UpdateTerrain(newCamPosition);
            prevCamPosition = newCamPosition;
        }
    }
}


void TerrainManager::initTerrain(){
    for(int x = -chunkRenderDistance; x <= chunkRenderDistance; x ++){
        for(int y = -chunkRenderDistance; y <= chunkRenderDistance; y ++){
            glm::vec3 chunkPos = prevCamPosition + glm::ivec3(x,0, y);
            Mesh * chunk = MakeVoxelChunk(prevCamPosition.x+x, prevCamPosition.z+y, chunkSize, terrainMat);
            instantiate(chunk, this);
            chunks.insert({chunkPos, chunk});
        }
    }
}
