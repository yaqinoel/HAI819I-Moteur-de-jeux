#include "terrainmanager.h"

#include <common/3dEntities/Meshes/lod.h>
#include <thread>

TerrainManager::TerrainManager() {
}

void TerrainManager::process(float deltaTime){
    glm::vec2 newCamPosition = glm::vec2((int)(roundf(cam->position.x/size)), (int)(roundf(cam->position.z/size)));
    if(newCamPosition != prevCamPosition){
        updateTerrain(newCamPosition);
        prevCamPosition = newCamPosition;
    }
}

void TerrainManager::setCam(Camera* cam){
    this->cam = cam;
    prevCamPosition = glm::vec2((int)(roundf(cam->position.x/size)), (int)(roundf(cam->position.z/size)));
}

void TerrainManager::printchunks(){
    for(int i = 0; i < chunks.size(); i ++){
        std::vector<Mesh*> chunk = *chunks[i];
        for(int j = 0; j < chunk.size(); j ++){
            std::cout << chunk[j]->name << " ";
        }
        std::cout << std::endl;
    }
}

void TerrainManager::updateTerrain(glm::vec2 newCamPosition){
    int lastIndex = chunks.size()-1;
    if(newCamPosition.x > prevCamPosition.x){
        for(int i = 0; i < chunkRenderDistance*2+1; i++){
            delete((*chunks[0])[0]);
            chunks[0]->erase(chunks[0]->begin());
        }
            chunks.erase(chunks.begin());
            chunks.push_back(new std::vector<Mesh*>());
        for(int y = -chunkRenderDistance; y <= chunkRenderDistance; y ++){
            chunks[lastIndex]->push_back(nullptr);
            MakeChunk(newCamPosition.x+lastIndex/2, newCamPosition.y+y, lastIndex, y+chunkRenderDistance);
        }
    }
    if(newCamPosition.x < prevCamPosition.x){
        for(int i = 0; i < chunkRenderDistance*2+1; i++){
            delete((*chunks[lastIndex])[0]);
            chunks[lastIndex]->erase(chunks[lastIndex]->begin());
        }
        chunks.erase(chunks.begin()+lastIndex);
        chunks.insert(chunks.begin(), new std::vector<Mesh*>());
        for(int y = -chunkRenderDistance; y <= chunkRenderDistance; y ++){
            chunks[0]->push_back(nullptr);
            MakeChunk(newCamPosition.x-lastIndex/2, newCamPosition.y+y, 0, y+chunkRenderDistance);
        }
    }
    if(newCamPosition.y > prevCamPosition.y){
        for(int i = 0; i < chunkRenderDistance*2+1; i++){
            delete((*chunks[i])[0]);
            chunks[i]->erase(chunks[i]->begin());
            chunks[i]->push_back(nullptr);
        }
        for(int x = -chunkRenderDistance; x <= chunkRenderDistance; x ++){
            MakeChunk(newCamPosition.x+x, newCamPosition.y+lastIndex/2, x+chunkRenderDistance, lastIndex);
        }
    }
    if(newCamPosition.y < prevCamPosition.y){
        for(int i = 0; i < chunkRenderDistance*2+1; i++){
            delete((*chunks[i])[lastIndex]);
            chunks[i]->erase(chunks[i]->begin()+lastIndex);
            chunks[i]->insert(chunks[i]->begin(), nullptr);
        }
        for(int x = -chunkRenderDistance; x <= chunkRenderDistance; x ++){
            MakeChunk(newCamPosition.x+x, newCamPosition.y-lastIndex/2, x+chunkRenderDistance, 0);
        }
    }
}

void TerrainManager::MakeChunk(int x, int y, int indexX, int indexY){
    LOD* lod = new LOD();
    lod->setCam(cam);
    lod->name = "terrain("+std::to_string(x) + ","+std::to_string(y)+")";
    for(int i = 0; i < 5; i ++){
        ProceduralTerrain* terrain = new ProceduralTerrain(x*size, y*size, 128/pow(2, i), 128/pow(2, i), size, size, 3, 0.8);
        terrain->setMaterial(terrainMat);
        terrain->setShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader_Terrain_Noise.glsl");
        lod->addLOD(terrain, 20*i);
        terrain->name = "lod "+std::to_string(i);
    }
    addChild(lod);
    (*chunks[indexX])[indexY] = lod;
}

void TerrainManager::initTerrain(){
    for(Node * c : children) if(c != nullptr) delete(c);
    chunks = std::vector<std::vector<Mesh*>*>();
    for(int x = -chunkRenderDistance; x <= chunkRenderDistance; x ++){
        chunks.push_back(new std::vector<Mesh*>());
        for(int y = -chunkRenderDistance; y <= chunkRenderDistance; y ++){
            chunks[x+chunkRenderDistance]->push_back(nullptr);
            MakeChunk(prevCamPosition.x+x, prevCamPosition.y+y, x+chunkRenderDistance, y+chunkRenderDistance);
        }
    }
    printchunks();
}
