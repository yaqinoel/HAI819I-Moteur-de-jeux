#pragma once

#include <common/3dEntities/Mesh.hpp>
#include <common/Materials/material.h>
#include <common/Physics/Shapes/cube.h>
#include <common/Physics/Shapes/voxelshape.h>
#include <common/Physics/collider3d.h>
#include <common/Physics/rigidbody3d.h>

inline RigidBody3D* makeDynamicVoxel(int width, int height, int depth,
                                     const std::vector<unsigned short int>& voxelData,
                                     float massPerCell = 6.0f,
                                     Material* material = nullptr)
{
    RigidBody3D* body = new RigidBody3D();
    body->name = "dynamic voxel";
    body->gravity = glm::vec3(0.0f, -20.0f, 0.0f);

    VoxelShape* shape = new VoxelShape();
    shape->InitMesh(width, height, depth, voxelData);

    glm::vec3 shapeCenter = shape->getLocalCenterOfMass();

    int solidCount = shape->solidCellCount();
    Collider3D* collider = new Collider3D();
    collider->name = "dynamic voxel collider";
    collider->setShape(shape);
    collider->mass = massPerCell * static_cast<float>(solidCount);
    collider->setLocalPosition(-shapeCenter);
    body->addChild(collider);
    body->addCollider(collider);

    float vs = shape->getVoxelSize();
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            for (int z = 0; z < depth; ++z) {
                if (!shape->isSolid(x, y, z))
                    continue;

                glm::vec3 cellCenter = glm::vec3(x + 1, y + 1, z + 1) * vs;
                Mesh* mesh = new Mesh();
                Cube visualCube(vs, vs, vs, mesh->vertices, mesh->triangles);
                mesh->name = "dynamic voxel cell";
                mesh->setLocalPosition(cellCenter - shapeCenter);
                mesh->material = material;
                body->addChild(mesh);
            }
        }
    }

    return body;
}

inline RigidBody3D* makeDynamicVoxelSquare(Material* material = nullptr) {
    return makeDynamicVoxel(2, 2, 1, {1,1, 1,1}, 6.0f, material);
}

inline RigidBody3D* makeDynamicVoxelCube(Material* material = nullptr) {
    return makeDynamicVoxel(2, 2, 2, {1,1, 1,1, 1,1, 1,1}, 6.0f, material);
}

inline RigidBody3D* makeDynamicVoxelL(Material* material = nullptr) {
    return makeDynamicVoxel(3, 2, 1, {
        1,1,
        1,0,
        1,0
    }, 6.0f, material);
}

inline RigidBody3D* makeDynamicVoxelT(Material* material = nullptr) {
    return makeDynamicVoxel(3, 2, 1, {
        1,0,
        1,1,
        1,0
    }, 6.0f, material);
}

inline RigidBody3D* makeDynamicVoxelIsland(Material* material = nullptr) {
    return makeDynamicVoxelT(material);
}
