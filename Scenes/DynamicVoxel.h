#pragma once

#include <common/3dEntities/Mesh.hpp>
#include <common/Materials/material.h>
#include <common/Physics/Shapes/cube.h>
#include <common/Physics/Shapes/voxelshape.h>
#include <common/Physics/collider3d.h>
#include <common/Physics/rigidbody3d.h>

inline Mesh* makeVoxelCellVisual(const glm::vec3& localCenter, Material* material = nullptr) {
    Mesh* mesh = new Mesh();
    Cube visualCube(1.0f, 1.0f, 1.0f, mesh->vertices, mesh->triangles);
    mesh->name = "dynamic voxel cell visual";
    mesh->setLocalPosition(localCenter);
    mesh->material = material;
    return mesh;
}

inline RigidBody3D* makeDynamicVoxelIsland(Material* material = nullptr) {
    RigidBody3D* body = new RigidBody3D();
    body->name = "dynamic voxel island";
    body->gravity = glm::vec3(0.0f, -20.0f, 0.0f);

    std::vector<unsigned short int> voxelData = {
        1, 1,
        1, 1
    };

    VoxelShape* shape = new VoxelShape();
    shape->InitMesh(2, 2, 1, voxelData);
    glm::vec3 shapeCenter = glm::vec3(1.5f, 1.5f, 1.0f);

    Collider3D* collider = new Collider3D();
    collider->name = "dynamic voxel island collider";
    collider->setShape(shape);
    collider->mass = 24.0f;
    collider->setLocalPosition(-shapeCenter);
    body->addChild(collider);
    body->addCollider(collider);

    body->addChild(makeVoxelCellVisual(glm::vec3(1.0f, 1.0f, 1.0f) - shapeCenter, material));
    body->addChild(makeVoxelCellVisual(glm::vec3(1.0f, 2.0f, 1.0f) - shapeCenter, material));
    body->addChild(makeVoxelCellVisual(glm::vec3(2.0f, 1.0f, 1.0f) - shapeCenter, material));
    body->addChild(makeVoxelCellVisual(glm::vec3(2.0f, 2.0f, 1.0f) - shapeCenter, material));

    return body;
}
