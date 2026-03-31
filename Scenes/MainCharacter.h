#pragma
#include <common/3dEntities/Mesh.hpp>
#include <common/3dEntities/collisionshape3d.h>
#include <common/Controls/charactercontroller.h>

CharacterController* makeKnight(){
    CharacterController* knight = new CharacterController();
    knight->gravity = glm::vec3(0, -20, 0);
    Mesh* mesh = new Mesh();
    mesh->openOBJ("../Resources/Models/obj/Knight.obj");
    mesh->setShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader.glsl");
    Material* mat = new Material(glm::vec3(0.5, 0.5, 0.5));
    Texture tex = Texture("../Resources/Textures/Characters/LightTextureKnight.png");
    mat->addTexture("texture0", tex);
    mat->setLit(0);
    mesh->setMaterial(mat);
    knight->addChild(mesh);
    mesh->name = "knight mesh";
    mesh->position = glm::vec3(0);


    return knight;
}
