#pragma once

#include "../common/3dEntities/Meshes/lod.h"
#include "../common/3dEntities/collisionshape3d.h"
#include "../common/Shapes/cube.h"
#include "../common/scene.h"
#include <iostream>


Node* makeCube(){
    Mesh* cube = new Mesh();

    Shape* shape = new Cube(1.1, 1.1, 1.1);
    cube->setShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader_lit.glsl");
    Material* cubeMat = new Material(glm::vec3(1, 0, 0));
    cube->material = cubeMat;
    cube->setVertices(shape->getVertices());
    cube->setTriangles(shape->getTriangles());

    CollisionShape3D* collider = new CollisionShape3D();
    cube->addChild(collider);
    collider->SetShape(shape);
    //collider->setDebug(true);
    collider->name = "cube collider";
    cube->name = "cube";
    cube->position = glm::vec3(0, 11, 3);
    cube->scale = glm::vec3(1, 2, 1);
    cube->Rotate(12, glm::vec3(1, 2, 4));

    return cube;
}
