#pragma once

#include "FirstPersonCharacter.h"
#include <common/Controls/cameracontrols.h>
#include <common/3dEntities/Lights/DirectionalLight.hpp>
#include <common/3dEntities/terrainmanager.h>
#include <common/Materials/pbrmaterial.h>
#include <common/Materials/texture.h>
#include <common/Render/IBLEnvironment.hpp>
#include <common/Render/RenderSystem.hpp>
#include <common/Render/Shader.hpp>
#include <common/scene.h>

Scene* makePBRInfiniteTerrain(RenderSystem* renderer) {
    Node* root = new Node3d();
    Scene* scene = new Scene(root);
    scene->iblEnvironment = new IBLEnvironment("../Resources/Textures/HDR/skybox_square_sun.hdr");

    CameraControls* cam = new CameraControls(4.0f, 3.0f, 70.0f, 0.1f, 1000.0f, glm::vec3(0, 9, -10));
    cam->name = "camera";
    scene->instantiate(cam);
    scene->mainCamera = cam;

    root->name = "pbr infinite terrain root";

    Shader* pbrShader = renderer->getOrCreateShader("../Shaders/vertex_shader_pbr.glsl", "../Shaders/fragment_shader_pbr.glsl");
    PBRMaterial* terrainMat = new PBRMaterial(pbrShader, glm::vec3(0.45f, 0.55f, 0.38f), 0.0f, 0.98f, 0.85f);
    Texture blockAlbedo("../Resources/Textures/Environement/BlocTextures.png");
    blockAlbedo.setPixelArt(true);
    terrainMat->addTexture("albedoMap", blockAlbedo);
    scene->worldMaterial = terrainMat;

    PBRMaterial* projectileMat = new PBRMaterial(pbrShader, glm::vec3(1.0f), 0.0f, 0.98f, 0.85f);
    Texture projectileAlbedo("../Resources/Textures/Environement/grassCubeTexSharp.png");
    projectileAlbedo.setPixelArt(true);
    projectileMat->addTexture("albedoMap", projectileAlbedo);

    PBRMaterial* pbrCubeTestMaterial = new PBRMaterial(pbrShader, glm::vec3(0.0f), 0.0f, 0.98f, 0.85f);
    pbrCubeTestMaterial->addTexture("albedoMap", Texture("../Resources/Textures/plastic/albedo.png"));
    pbrCubeTestMaterial->addTexture("normalMap", Texture("../Resources/Textures/plastic/normal.png"));
    pbrCubeTestMaterial->addTexture("metallicMap", Texture("../Resources/Textures/plastic/metallic.png"));
    pbrCubeTestMaterial->addTexture("roughnessMap", Texture("../Resources/Textures/plastic/roughness.png"));
    pbrCubeTestMaterial->addTexture("aoMap", Texture("../Resources/Textures/plastic/ao.png"));
    scene->pbrCubeTestMaterial = pbrCubeTestMaterial;

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
