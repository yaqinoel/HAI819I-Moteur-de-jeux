#pragma once

#include <common/node.h>
#include <common/3dEntities/node3d.h>
#include <common/3dEntities/terrainmanager.h>
#include <common/3dEntities/Meshes/lod.h>
#include <common/3dEntities/Meshes/proceduralterrain.h>


Node* makeInfiniteTerrain(Camera* cam){
    Node* scene = new Node3d();
    scene->name = "scene";
    Material* mat = new Material(glm::vec3(1, 0, 0));
    mat->addTexture("texture0", Texture("../Resources/Textures/Environement/grass.png"));
    mat->addTexture("texture1", Texture("../Resources/Textures/Environement/rock.png"));
    mat->addTexture("texture2", Texture("../Resources/Textures/Environement/snowrocks.png"));

    {
        TerrainManager* tm = new TerrainManager();
        tm->terrainMat = mat;
        tm->setCam(cam);
        tm->name = "terrain manager";
        tm->initTerrain();
        scene->addChild(tm);
    }

    return scene;
}
