#pragma once

#include <common/3dEntities/Meshes/lod.h>
#include <common/3dEntities/collisionshape3d.h>
#include <common/Shapes/sphere.h>
#include <common/scene.h>
#include <iostream>


Mesh* makeBall(){
    Mesh* ball = new Mesh();

    ball->openOBJ("../Resources/Models/obj/sphere.obj");
    ball->setShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader_lit.glsl");
    Material* ballmat = new Material(glm::vec3(1, 0, 0));
    ball->material = ballmat;

    CollisionShape3D* collider = new CollisionShape3D();
    Shape* shape = new Sphere(1.1);
    ball->addChild(collider);
    collider->SetShape(shape);
    collider->setDebug(true);
    collider->name = "ball collider";
    ball->name = "ball";
    ball->setLocalPosition(glm::vec3(0, 10, 1));
    ball->setScale(glm::vec3(1, 1, 1));

    return ball;
}
