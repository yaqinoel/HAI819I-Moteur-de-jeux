#include "charactercontroller.h"
#include <Scenes/PhysicsCube.h>
#include <Scenes/Cube.h>

CharacterController::CharacterController() {
    canSleep = false;
}


void CharacterController::process(float deltaTime){
    RigidBody3D::process(deltaTime);
    GLFWwindow* window = glfwGetCurrentContext();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //rotation
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    axialInputs = glm::vec2(0);

    cameraForward = glm::normalize(glm::vec3(cam->forward().x, 0, cam->forward().z));
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
    if (scene->inputPressed("action1")){
        RigidBody3D* cube = makePhysicsCube();
        cube->velocity = cam->forward()*20.0f+UP*7.0f;
        instantiate(cube);
        cube->setGlobalPosition(getGlobalPosition()+cam->forward()*1.0f+up()*0.7f);
        cube->setForward(cameraForward);
    }
}

void CharacterController::physicsProcess(){
    RigidBody3D::physicsProcess();
    glm::vec3 planarVelocity = (axialInputs.x * cameraForward + axialInputs.y * cam->right());
    if(planarVelocity != glm::vec3(0)) planarVelocity = glm::normalize(planarVelocity)*speed;

    velocity = planarVelocity+glm::vec3(0, velocity.y, 0);

    if(jumpPressed){
        velocity.y = jumpStrength;
        jumpPressed = false;
    }
}
