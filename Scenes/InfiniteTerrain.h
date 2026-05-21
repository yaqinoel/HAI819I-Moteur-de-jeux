#pragma once

#include "Scenes/PhysicsCube.h"
#include "common/Controls/charactercontroller.h"
#include <common/node.h>
#include <common/3dEntities/node3d.h>
#include <common/3dEntities/terrainmanager.h>
#include <common/3dEntities/Meshes/lod.h>
#include <common/Controls/cameracontrols.h>
#include <common/Materials/terrainmaterial.h>
#include <common/Render/RenderSystem.hpp>
#include <common/Render/Shader.hpp>
#include <common/scene.h>
#include "Ball.h"
#include "Cube.h"
#include "MainCharacter.h"
#include "HoveringCharacter.h"
#include "FirstPersonCharacter.h"


Scene* makeInfiniteTerrain(RenderSystem* renderer = nullptr){
    Node* root = new Node3d();
    Scene* scene = new Scene(root);
    CameraControls* cam = new CameraControls(4.0f, 3.0f, 70.0f, 0.1f, 1000.0f, glm::vec3(0, 9, -10));
    cam->name = "camera";
    scene->instantiate(cam);
    scene->mainCamera = cam;

    root->name = "scene root";
    TerrainMaterial* mat = new TerrainMaterial(glm::vec3(1, 0, 0));
    if (renderer) {
        mat->shader = renderer->getOrCreateShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader.glsl");
    }
    mat->setLit(0);
    mat->addTexture("texture0", Texture("../Resources/Textures/Environement/grass.png"));
    mat->addTexture("texture1", Texture("../Resources/Textures/Environement/rock.png"));
    mat->addTexture("texture2", Texture("../Resources/Textures/Environement/snowrocks.png"));
    mat->maxHeight = 20.0;
    mat->scale = 10.0;

    Material* projectileMat = new Material(glm::vec3(1.0f));
    if (renderer) {
        projectileMat->shader = renderer->getOrCreateShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader.glsl");
    }
    projectileMat->setLit(0);
    Texture projectileTexture("../Resources/Textures/Environement/grassCubeTexSharp.png");
    projectileTexture.setPixelArt(true);
    projectileMat->addTexture("texture0", projectileTexture);

    TerrainManager* tm = new TerrainManager();
    {
        tm->terrainMat = mat;
        tm->name = "terrain manager";
        tm->initTerrain();
        scene->instantiate(tm);
    }

    //HoveringController* character = makeHoveringController();
    //character->setGlobalPosition(glm::vec3(3, 7, 0.5));

    // CharacterController* character = makeKnight();
    // character->setGlobalPosition(glm::vec3(3, 14, 10));

    CharacterController* character = makeFPSController();
    character->setGlobalPosition(glm::vec3(3, 14, 10));
    character->projectileMaterial = projectileMat;

    cam->pivot = character;
    character->cam = cam;
    scene->instantiate(character);

    // Node3d* cubeA = makeCube();
    // cubeA->Translate(glm::vec3(1, 7, 0));
    // cubeA->Rotate(glm::radians(40.0), glm::vec3(0, 1, 0));
    // scene->instantiate(cubeA);

    // Node3d* cubeA = makePhysicsCube();
    // cubeA->Translate(glm::vec3(1, 7, 0));
    // cubeA->Rotate(glm::radians(40.0), glm::vec3(0, 1, 0));
    // scene->instantiate(cubeA);

    // Node3d* cubeB = makeCube();
    // cubeB->Translate(glm::vec3(1, 6.5, 0.5));
    // scene->instantiate(cubeB);

    return scene;
}
