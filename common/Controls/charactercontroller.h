#pragma once

#include <common/3dEntities/Mesh.hpp>
#include <common/3dEntities/terrainmanager.h>
#include <common/3dEntities/rigidbody3d.h>


class CharacterController : public RigidBody3D
{
public:
    CharacterController();
    void process(float deltaTime) override;
    void physicsProcess(float fixedDeltaTime) override;
    Camera* cam;
private:
    float speed = 5.0f;
    float jumpStrength = 7.0f;
    glm::vec2 axialInputs = glm::vec2(0);
    bool onground = false;
};

