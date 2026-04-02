#pragma once

#include "common/Controls/charactercontroller.h"
#include <common/node.h>
#include <common/3dEntities/node3d.h>
#include <common/3dEntities/terrainmanager.h>
#include <common/3dEntities/Meshes/lod.h>
#include <common/Controls/cameracontrols.h>
#include <common/Materials/terrainmaterial.h>
#include <common/scene.h>
#include "Ball.h"
#include "Cube.h"
#include "MainCharacter.h"


Scene* makeInfiniteTerrain(){
    Node* root = new Node3d();
    Scene* scene = new Scene(root);
    CameraControls* cam = new CameraControls(4.0f, 3.0f, 45.0f, 0.1f, 1000.0f, glm::vec3(0, 9, -10));
    cam->name = "camera";
    scene->instantiate(cam);

    root->name = "scene root";
    TerrainMaterial* mat = new TerrainMaterial(glm::vec3(1, 0, 0));
    mat->addTexture("texture0", Texture("../Resources/Textures/Environement/grass.png"));
    mat->addTexture("texture1", Texture("../Resources/Textures/Environement/rock.png"));
    mat->addTexture("texture2", Texture("../Resources/Textures/Environement/snowrocks.png"));
    mat->maxHeight = 20.0;
    mat->scale = 10.0;

    TerrainManager* tm = new TerrainManager();
    {
        tm->terrainMat = mat;
        tm->name = "terrain manager";
        tm->initTerrain();
        scene->instantiate(tm);
    }

    CharacterController* knight = makeKnight();
    cam->pivot = knight;
    knight->cam = cam;
    scene->instantiate(knight);
    // Node3d* cubeA = makeCube();
    // cubeA->Translate(glm::vec3(1, 7, 0));
    // scene->instantiate(cubeA);
    // Node3d* cubeB = makeCube();
    // cubeB->Translate(glm::vec3(1, 7, 0.5));
    // scene->instantiate(cubeB);

    return scene;
}
