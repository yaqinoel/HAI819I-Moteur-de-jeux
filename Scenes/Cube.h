#pragma once

#include <common/3dEntities/Meshes/lod.h>
#include <common/3dEntities/collisionshape3d.h>
#include <common/Shapes/cube.h>
#include <common/scene.h>
#include <iostream>


Node* makeBall(){
    Mesh* ball = new Mesh();

    Shape* shape = new Cube(1.1, 1.1, 1.1);
    ball->setShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader_lit.glsl");
    Material* ballmat = new Material(glm::vec3(0, 1, 0));
    ball->material = ballmat;

    CollisionShape3D* collider = new CollisionShape3D();
    ball->addChild(collider);
    collider->SetShape(shape);
    collider->setDebug(true);
    collider->name = "ball collider";
    ball->name = "ball";
    ball->position = glm::vec3(0, 10, 1);
    ball->scale = glm::vec3(1, 1, 1);

    return ball;
}
