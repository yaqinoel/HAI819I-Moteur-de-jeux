#pragma once

#include <common/3dEntities/Mesh.hpp>
#include <common/3dEntities/terrainmanager.h>
#include <common/Physics/rigidbody3d.h>


class CharacterController : public RigidBody3D
{
public:
    CharacterController();
    void process(float deltaTime) override;
    void lateProcess(float deltaTime) override;
    void physicsProcess() override;
    Camera* cam;
    Material* projectileMaterial = nullptr;
private:
    float speed = 5.0f;
    float jumpStrength = 7.0f;
    glm::vec2 axialInputs = glm::vec2(0);
    glm::vec3 cameraForwardxz = glm::vec3(1, 0, 0);
    Collider3D* selectedCollider = nullptr;
    glm::vec3 selectedLocalCenter = glm::vec3(0.0f);
    bool jumpPressed = false;
};
