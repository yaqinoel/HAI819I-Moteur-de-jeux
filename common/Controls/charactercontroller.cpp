#include "charactercontroller.h"
#include <Scenes/PhysicsCube.h>
#include <Scenes/Cube.h>
#include <common/3dEntities/Meshes/Terrain/proceduralvoxelterrain.h>

CharacterController::CharacterController() {
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

    cameraForwardxz = glm::normalize(glm::vec3(cam->forward().x, 0, cam->forward().z));
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
        if(velocity.y <= 0.01 && onground){
            jumpPressed = true;
        }
    }
    if (scene->inputPressed("action3")){
        RigidBody3D* cube = makePhysicsCube(projectileMaterial);
        cube->velocity = cam->forward()*20.0f+UP*7.0f;
        instantiate(cube);
        cube->setGlobalPosition(getGlobalPosition()+cam->forward()*1.0f+up()*0.7f);
        cube->setForward(cameraForwardxz);
    }

    glm::vec3 ray_start = cam->getGlobalPosition();
    glm::vec3 camera_forward = cam->forward();
    float ray_length = 4.0;
    RayIntersection forward_raycast = scene->raycast(ray_start, camera_forward, ray_length, 1ULL);
    if(forward_raycast.intersectionExists){
        CollisionShape3D* collider = forward_raycast.collider;
        glm::vec3 worldHit = forward_raycast.point;
        glm::vec3 colliderPos = collider->getGlobalPosition();
        glm::quat colliderRot = collider->getGlobalRotation();
        glm::vec3 localHit = glm::inverse(colliderRot) * (worldHit - colliderPos);
        localHit += glm::inverse(colliderRot) * (camera_forward * 0.01f);
        glm::vec3 voxelCenter = glm::round(localHit);
        glm::vec3 worldVoxelCenter = colliderPos + (colliderRot * voxelCenter);
        drawCube(scene, worldVoxelCenter, colliderRot);
        if (scene->inputPressed("action1")){
            ProceduralVoxelTerrain* voxel = dynamic_cast<ProceduralVoxelTerrain*>(collider->getParent());
            if (voxel)
            {
                voxel->removeTile(worldVoxelCenter);
            }
        }
    }
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
    else if(velocity.y <= 0){
        RayIntersection verticalIntersection = scene->raycast(currentPosition, DOWN, 1.1f, 1ULL);
        //std::cout << glm::to_string(currentPosition) << std::endl;
        if(verticalIntersection.intersectionExists){
            currentPosition = verticalIntersection.point+UP*1.0f;
            velocity.y = 0;
            onground = true;
        }
        else{
            onground = false;
        }
    }
}
