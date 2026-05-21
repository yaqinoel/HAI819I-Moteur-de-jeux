#pragma once

#include <common/3dEntities/Mesh.hpp>
#include <common/3dEntities/terrainmanager.h>
#include <common/Physics/rigidbody3d.h>

class HoveringController : public RigidBody3D
{
public:
    HoveringController();
    void process(float deltaTime) override;
    void physicsProcess() override;
    Camera* cam;
private:
    float speed = 2.0f;
    glm::vec3 axialInputs = glm::vec3(0);
    bool onground = false;
};

