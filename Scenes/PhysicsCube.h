#pragma once

#include "Cube.h"
#include <common/Physics/rigidbody3d.h>

#include <cmath>

inline RigidBody3D* makePhysicsCube(Material* material = nullptr){
    RigidBody3D* physicsCube = new RigidBody3D();
    Node3d* child = makeCube(material);
    physicsCube->addChild(child);
    physicsCube->gravity = glm::vec3(0, -20, 0);
    physicsCube->name = "rigidbody cube";
    Collider3D* cubeCollider = ((Collider3D*)(child->getChildren()[0]));
    cubeCollider->mass = 10.0f;
    physicsCube->addCollider(cubeCollider);
    return physicsCube;
}

inline void setFullFaceCubeUVs(Mesh* cube) {
    if (!cube) return;

    for (Vertex& vertex : cube->vertices) {
        const glm::vec3 normal = vertex.normal;
        const glm::vec3 position = vertex.position + glm::vec3(0.5f);

        if (std::abs(normal.y) > 0.5f) {
            vertex.texCoord = glm::vec2(position.x, position.z);
            vertex.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
            vertex.bitangent = glm::vec3(0.0f, 0.0f, normal.y > 0.0f ? 1.0f : -1.0f);
        } else if (std::abs(normal.x) > 0.5f) {
            vertex.texCoord = glm::vec2(position.z, position.y);
            vertex.tangent = glm::vec3(0.0f, 0.0f, normal.x > 0.0f ? 1.0f : -1.0f);
            vertex.bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
        } else {
            vertex.texCoord = glm::vec2(position.x, position.y);
            vertex.tangent = glm::vec3(normal.z > 0.0f ? 1.0f : -1.0f, 0.0f, 0.0f);
            vertex.bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }
}

inline Mesh* makeFullFaceUVCube(Material* material = nullptr) {
    Mesh* cube = new Mesh();
    Shape* shape = new Cube(1, 1, 1, cube->vertices, cube->triangles);
    setFullFaceCubeUVs(cube);
    cube->material = material;

    Collider3D* collider = new Collider3D();
    cube->addChild(collider);
    collider->setShape(shape);
    collider->name = "full face uv cube collider";
    cube->name = "full face uv cube mesh";

    return cube;
}

inline RigidBody3D* makePBRPhysicsCube(Material* material = nullptr) {
    RigidBody3D* physicsCube = new RigidBody3D();
    physicsCube->sleepSettings = SleepSettings::DynamicCube();
    Node3d* child = makeFullFaceUVCube(material);
    physicsCube->addChild(child);
    physicsCube->gravity = glm::vec3(0, -20, 0);
    physicsCube->name = "pbr rigidbody cube";
    Collider3D* cubeCollider = static_cast<Collider3D*>(child->getChildren()[0]);
    cubeCollider->mass = 1000.0f;
    physicsCube->addCollider(cubeCollider);
    return physicsCube;
}
