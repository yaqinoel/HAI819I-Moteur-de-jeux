#include "charactercontroller.h"
#include <Scenes/PhysicsCube.h>
#include <Scenes/Cube.h>
#include <common/3dEntities/Meshes/Terrain/proceduralvoxelterrain.h>

CharacterController::CharacterController() {
    canSleep = false;
}
void drawCube(Scene *scene, glm::vec3 center, glm::quat rotation)
{
    float sizemult = 1.001f;

    auto rotatePoint = [&](glm::vec3 p)
    {
        return center + (rotation * (p * sizemult));
    };

    glm::vec3 p000 = rotatePoint(glm::vec3(-0.5f, -0.5f, -0.5f));
    glm::vec3 p001 = rotatePoint(glm::vec3(-0.5f, -0.5f,  0.5f));
    glm::vec3 p010 = rotatePoint(glm::vec3(-0.5f,  0.5f, -0.5f));
    glm::vec3 p011 = rotatePoint(glm::vec3(-0.5f,  0.5f,  0.5f));

    glm::vec3 p100 = rotatePoint(glm::vec3( 0.5f, -0.5f, -0.5f));
    glm::vec3 p101 = rotatePoint(glm::vec3( 0.5f, -0.5f,  0.5f));
    glm::vec3 p110 = rotatePoint(glm::vec3( 0.5f,  0.5f, -0.5f));
    glm::vec3 p111 = rotatePoint(glm::vec3( 0.5f,  0.5f,  0.5f));

    // Top face
    scene->drawLine(p111, p110);
    scene->drawLine(p110, p010);
    scene->drawLine(p010, p011);
    scene->drawLine(p011, p111);

    // Bottom face
    scene->drawLine(p101, p100);
    scene->drawLine(p100, p000);
    scene->drawLine(p000, p001);
    scene->drawLine(p001, p101);

    // Vertical edges
    scene->drawLine(p111, p101);
    scene->drawLine(p110, p100);
    scene->drawLine(p010, p000);
    scene->drawLine(p011, p001);
}

void CharacterController::process(float deltaTime){
    RigidBody3D::process(deltaTime);
    GLFWwindow* window = glfwGetCurrentContext();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //rotation
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    axialInputs = glm::vec2(0);

    glm::vec3 cameraForward = cam->forward();
    glm::vec3 planarCameraForward = glm::vec3(cameraForward.x, 0.0f, cameraForward.z);
    if (glm::dot(planarCameraForward, planarCameraForward) > 0.0001f) {
        cameraForwardxz = glm::normalize(planarCameraForward);
    }
    //setForward(cameraForward);

    if (scene->inputHeld("forward")){
        axialInputs.x += 1;
    }

    if (scene->inputHeld("backwards")){
        axialInputs.x -= 1;
    }
    if (scene->inputHeld("right")){
        axialInputs.y += 1;
    }
    if (scene->inputHeld("left")){
        axialInputs.y -= 1;
    }
    if (scene->inputPressed("jump")){
        if(velocity.y <= 0.01 && isOnGround()){
            jumpPressed = true;
        }
    }
    if (scene->inputPressed("action3")){
        const float projectileSpawnDistance = 2.0f;
        const float projectileHalfExtent = 0.5f;
        const float projectileSpawnPadding = 0.15f;
        const float projectileMinSpawnDistance = projectileHalfExtent + projectileSpawnPadding;

        float spawnDistance = projectileSpawnDistance;
        RayIntersection spawnRaycast = scene->raycast(
            cam->getGlobalPosition(),
            cameraForward,
            projectileSpawnDistance + projectileHalfExtent + projectileSpawnPadding,
            1ULL
        );
        if (spawnRaycast.intersectionExists) {
            spawnDistance = spawnRaycast.t - projectileHalfExtent - projectileSpawnPadding;
        }

        if (spawnDistance < projectileMinSpawnDistance) {
            return;
        }

        RigidBody3D* cube = makePhysicsCube(projectileMaterial);
        cube->setGlobalPosition(cam->getGlobalPosition() + cameraForward * spawnDistance);
        cube->setForward(cameraForwardxz);
        cube->velocity = cameraForward * 20.0f + UP * 7.0f;
        instantiate(cube);
    }

    glm::vec3 ray_start = cam->getGlobalPosition();
    glm::vec3 camera_forward = cam->forward();
    float ray_length = 4.0;
    RayIntersection forward_raycast = scene->raycast(ray_start, camera_forward, ray_length, 1ULL);
    selectedCollider = nullptr;
    if(forward_raycast.intersectionExists && forward_raycast.collider != nullptr){
        Collider3D* collider = forward_raycast.collider;
        glm::vec3 worldHit = forward_raycast.point;
        glm::vec3 colliderPos = collider->getGlobalPosition();
        glm::quat colliderRot = collider->getGlobalRotation();
        glm::vec3 localHit = glm::inverse(colliderRot) * (worldHit - colliderPos);
        localHit += glm::inverse(colliderRot) * (camera_forward * 0.01f);
        selectedLocalCenter = glm::round(localHit);
        selectedCollider = collider;
        if (scene->inputPressed("action1")){
            ProceduralVoxelTerrain* voxel = dynamic_cast<ProceduralVoxelTerrain*>(collider->getParent());
            if (voxel)
            {
                glm::vec3 worldVoxelCenter = colliderPos + (colliderRot * selectedLocalCenter);
                voxel->removeTile(worldVoxelCenter);
            }
        }
    }
}

void CharacterController::lateProcess(float deltaTime){
    (void)deltaTime;
    if (selectedCollider == nullptr || !selectedCollider->getVisible()) {
        return;
    }

    glm::quat colliderRot = selectedCollider->getGlobalRotation();
    glm::vec3 worldVoxelCenter = selectedCollider->getGlobalPosition() + (colliderRot * selectedLocalCenter);
    drawCube(scene, worldVoxelCenter, colliderRot);
}


void CharacterController::physicsProcess(){
    RigidBody3D::physicsProcess();
    glm::vec3 planarVelocity = (axialInputs.x * cameraForwardxz + axialInputs.y * cam->right());
    if(planarVelocity != glm::vec3(0)) planarVelocity = glm::normalize(planarVelocity)*speed;

    velocity = planarVelocity+glm::vec3(0, velocity.y, 0);

    if(jumpPressed){
        velocity.y = jumpStrength;
        jumpPressed = false;
    }
}
