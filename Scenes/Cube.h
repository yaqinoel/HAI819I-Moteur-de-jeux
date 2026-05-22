#pragma once

#include "../common/3dEntities/Mesh.hpp"
#include "../common/Physics/collider3d.h"
#include "../common/Physics/Shapes/cube.h"


inline Mesh* makeCube(Material* material = nullptr){
    Mesh* cube = new Mesh();

    Shape* shape = new Cube(1, 1, 1, cube->vertices, cube->triangles);
    Texture tex = Texture("../Resources/Textures/Environement/grassCubeTexSharp.png");
    tex.setPixelArt(true);
    if (material) {
        cube->material = material;
    } else {
        Material* cubeMat = new Material(glm::vec3(1, 1, 1));
        cubeMat->setLit(false);
        cubeMat->addTexture("texture0", tex);
        cube->material = cubeMat;
    }

    Collider3D* collider = new Collider3D();
    cube->addChild(collider);
    collider->setShape(shape);
    collider->name = "cube collider";
    cube->name = "cube mesh";

    return cube;
}
