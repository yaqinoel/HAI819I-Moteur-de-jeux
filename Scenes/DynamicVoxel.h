#pragma once

#include <common/3dEntities/Mesh.hpp>
#include <common/3dEntities/Meshes/dynamicvoxel.h>
#include <common/Materials/material.h>
#include <common/Physics/Shapes/cube.h>
#include <common/Physics/Shapes/voxelshape.h>
#include <common/Physics/collider3d.h>
#include <common/Physics/rigidbody3d.h>

inline RigidBody3D* makeDynamicVoxel(int width, int height, int depth, const std::vector<unsigned short int>& voxelData, float massPerCell = 1000.0f, Material* material = nullptr)
{
    RigidBody3D* body = new RigidBody3D();
    body->name = "dynamic voxel rigidbody";
    body->gravity = glm::vec3(0.0f, -20.0f, 0.0f);

    DynamicVoxel *mesh = new DynamicVoxel(glm::vec3(0), glm::vec3(width, height, depth), body);
    mesh->setMaterial(material);
    mesh->name = "dynamic voxel mesh";
    mesh->setVoxelData(voxelData);
    mesh->InitMesh();

    return body;
}

inline RigidBody3D* makeDynamicVoxel(unsigned short int value, Material* material = nullptr) {
    return makeDynamicVoxel(1, 1, 1, {value}, 1000.0f, material);
}

inline RigidBody3D* makeDynamicVoxelSquare(Material* material = nullptr) {
    return makeDynamicVoxel(2, 2, 1, {1,1, 1,1}, 1000.0f, material);
}

inline RigidBody3D* makeDynamicVoxelCube(Material* material = nullptr) {
    return makeDynamicVoxel(2, 2, 2, {1,1, 1,1, 1,1, 1,1}, 1000.0f, material);
}

inline RigidBody3D* makeDynamicVoxelL(Material* material = nullptr) {
    return makeDynamicVoxel(3, 2, 1, {
        1,1,
        1,0,
        1,0
    }, 1000.0f, material);
}

inline RigidBody3D* makeDynamicVoxelT(Material* material = nullptr) {
    return makeDynamicVoxel(3, 2, 1, {
        1,0,
        1,1,
        1,0
    }, 1000.0f, material);
}

inline RigidBody3D* makeDynamicHugeVoxel(Material* material = nullptr) {
    return makeDynamicVoxel(9, 3, 1, {
        1,0,0,
        1,0,0,
        1,1,1,
        1,0,0,
        1,0,0,
        1,0,0,
        1,0,0,
        1,0,0,
        1,0,0,
    }, 1000.0f, material);
}

inline RigidBody3D* makeDynamicVoxelIsland(Material* material = nullptr) {
    return makeDynamicHugeVoxel(material);
}
