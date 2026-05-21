#pragma once
#include <common/3dEntities/Mesh.hpp>
#include <common/Physics/collider3d.h>
#include <common/Controls/charactercontroller.h>
#include "Cube.h"

CharacterController* makeFPSController(){
    CharacterController* character = new CharacterController();
    character->gravity = glm::vec3(0, -20, 0);

    Mesh* cube = new Mesh();


    Collider3D* collider = new Collider3D();
    character->addChild(collider);
    Shape* shape = new Cube(0.8, 1.9, 0.8, cube->vertices, cube->triangles);
    collider->setShape(shape);
    collider->name = "character collider";
    character->name = "FPS character";
    character->addCollider(collider);
    collider->collisionLayers = 1ULL << 1;
    character->setUnlockedRotation(false, false, false);
    character->friction = 0.0f;



    // cube->setShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader.glsl");
    // Material* cubeMat = new Material(glm::vec3(1, 1, 1));
    // cubeMat->setLit(false);
    // cube->material = cubeMat;
    // Texture tex = Texture("../Resources/Textures/Environement/grassCubeTexSharp.png");
    // tex.setPixelArt(true);
    // cubeMat->addTexture("texture0", tex);
    // character->addChild(cube);



    return character;
}
