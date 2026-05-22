#pragma once

#include <common/3dEntities/Meshes/lod.h>
#include <common/3dEntities/collisionshape3d.h>
#include <common/Shapes/sphere.h>
#include <common/scene.h>
#include <common/Render/Shader.hpp>
#include <iostream>


Mesh* makeBall(Material* material = nullptr, Shader* debugShader = nullptr){
    Mesh* ball = new Mesh();

    ball->openOBJ("../Resources/Models/obj/sphere.obj");
    if (material) {
        ball->material = material;
    } else {
        Material* ballmat = new Material(glm::vec3(1, 0, 0));
        ball->material = ballmat;
    }

    CollisionShape3D* collider = new CollisionShape3D();
    Shape* shape = new Sphere(1.1);
    ball->addChild(collider);
    collider->SetShape(shape);
    collider->setDebug(true, debugShader);
    collider->name = "ball collider";
    ball->name = "ball";
    ball->setLocalPosition(glm::vec3(0, 10, 1));
    ball->setScale(glm::vec3(1, 1, 1));

    return ball;
}
