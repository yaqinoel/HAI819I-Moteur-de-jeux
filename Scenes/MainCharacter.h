#pragma once
#include <common/3dEntities/Mesh.hpp>
#include <common/3dEntities/collisionshape3d.h>
#include <common/Controls/charactercontroller.h>
#include <common/Render/Shader.hpp>

CharacterController* makeKnight(Shader* standardShader = nullptr){
    CharacterController* knight = new CharacterController();
    knight->gravity = glm::vec3(0, -20, 0);
    Mesh* mesh = new Mesh();
    //mesh->openOBJ("../Resources/Models/obj/Knight.obj");
    Material* mat = new Material(glm::vec3(0.5, 0.5, 0.5));
    if (standardShader) {
        mat->shader = standardShader;
    }
    Texture tex = Texture("../Resources/Textures/Characters/LightTextureKnight.png");
    mat->addTexture("texture0", tex);
    mat->setLit(0);
    mesh->setMaterial(mat);
    knight->addChild(mesh);
    mesh->name = "knight mesh";
    mesh->setLocalPosition(glm::vec3(0));
    knight->Translate(glm::vec3(0, 20, 0));


    return knight;
}
