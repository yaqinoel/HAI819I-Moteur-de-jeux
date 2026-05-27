#pragma once

#include "cameracontrols.h"

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
    CameraControls* cam;
    Material* projectileMaterial = nullptr;
    bool paused;
    int current_inventory_case_selected = 0;
    std::vector<glm::ivec2> inventory;
    const int inventory_size = 10;
    bool add_in_inventory(glm::ivec2 obj, int start=0);
    int remove_one_in_inventory();
private:
    float speed = 5.0f;
    float jumpStrength = 7.0f;
    glm::vec3 axialInputs = glm::vec3(0);
    glm::vec3 cameraForwardxz = glm::vec3(1, 0, 0);
    Collider3D* selectedCollider = nullptr;
    glm::vec3 selectedLocalCenter = glm::vec3(0.0f);
    bool jumpPressed = false;
    void shootPBRPhysicsCube(Material* material);
    bool gravityEnabled = true;
};
