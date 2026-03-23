#pragma once

#include <common/3dEntities/Mesh.hpp>
#include <common/3dEntities/terrainmanager.h>


class CharacterController : public Mesh
{
public:
    CharacterController();
    void process(float deltaTime) override;
    Camera* cam;
    void setTerrain(TerrainManager * terrain);
private:
    float speed = 5.0f;
    TerrainManager* terrainManager;
};

