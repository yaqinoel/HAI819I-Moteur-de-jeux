#pragma once

#include "FirstPersonCharacter.h"
#include <common/Controls/cameracontrols.h>
#include <common/3dEntities/Lights/DirectionalLight.hpp>
#include <common/3dEntities/terrainmanager.h>
#include <common/Materials/MaterialLibrary.h>
#include <common/Materials/pbrmaterial.h>
#include <common/Render/IBLEnvironment.hpp>
#include <common/Render/RenderSystem.hpp>
#include <common/scene.h>

Scene* makePBRInfiniteTerrain(RenderSystem* renderer) {
    Node* root = new Node3d();
    Scene* scene = new Scene(root);
    scene->iblEnvironment = new IBLEnvironment("../Resources/Textures/HDR/skybox_square_sun.hdr");
    scene->materialLibrary = new MaterialLibrary(renderer);

    CameraControls* cam = new CameraControls(4.0f, 3.0f, 70.0f, 0.1f, 1000.0f, glm::vec3(0, 9, -10));
    cam->name = "camera";
    scene->instantiate(cam);
    scene->mainCamera = cam;

    root->name = "pbr infinite terrain root";

    PBRMaterial* terrainMat = scene->materialLibrary->getSharedPBRBlockTerrain();
    scene->worldMaterial = terrainMat;

    PBRMaterial* projectileMat = scene->materialLibrary->getSharedPBRGrassCube();
    scene->materialLibrary->getSharedRustIron();
    scene->materialLibrary->getSharedGold();
    scene->materialLibrary->getSharedPlastic();
    scene->materialLibrary->getSharedPBRGrass();
    scene->materialLibrary->getSharedWall();

    TerrainManager* tm = new TerrainManager();
    tm->terrainMat = terrainMat;
    tm->usePBRChunks = true;
    tm->name = "pbr terrain manager";
    tm->initTerrain();
    scene->instantiate(tm);

    CharacterController* character = makeFPSController();
    character->setGlobalPosition(glm::vec3(3, 18, 10));
    character->projectileMaterial = projectileMat;
    cam->pivot = character;
    character->cam = cam;
    scene->instantiate(character);

    DirectionalLight* sunLight = new DirectionalLight(glm::vec3(1.0f), 4.0f);
    sunLight->setForward(glm::vec3(-0.35f, -1.0f, -0.25f));
    sunLight->castShadow = true;
    sunLight->shadowOrthoSize = 80.0f;
    sunLight->shadowNearPlane = 1.0f;
    sunLight->shadowFarPlane = 160.0f;
    scene->instantiate(sunLight);


    GUI *gui = new GUI(character, scene);
    renderer->gui = gui;

    return scene;
}
