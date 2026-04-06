#include "hoveringcontroller.h"

#include <Scenes/PhysicsCube.h>
#include <Scenes/Cube.h>

HoveringController::HoveringController() {
    RayIntersection intersection = scene->raycast(getGlobalPosition()+UP*30.0f, DOWN, 100);
    if(intersection.intersectionExists) setGlobalPosition(intersection.point);
}


void HoveringController::process(float deltaTime){
    RigidBody3D::process(deltaTime);
    GLFWwindow* window = glfwGetCurrentContext();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //rotation
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    axialInputs = glm::vec3(0);

    if (scene->inputHeld("forward")){
        axialInputs.x += 1;
    }

    if (scene->inputHeld("backwards")){
        axialInputs.x -= 1;
    }
    if (scene->inputHeld("right")){
        axialInputs.z += 1;
    }
    if (scene->inputHeld("left")){
        axialInputs.z -= 1;
    }
    if (scene->inputHeld("jump")){
        axialInputs.y += 1;
    }
    if (scene->inputHeld("down")){
        axialInputs.y -= 1;
    }
    if (scene->inputPressed("action1")){
        applyImpulse(glm::vec3(300, 0, 0), getGlobalPosition()+glm::vec3(1, 0, 1));
    }

    glm::vec3 cameraForward = glm::normalize(glm::vec3(cam->forward().x, 0, cam->forward().z));
    //SetForward(cameraForward);


}

void HoveringController::physicsProcess(){
    RigidBody3D::physicsProcess();
    velocity = glm::vec3(0);
    glm::vec3 planarVelocity = (axialInputs.x * FORWARD+ axialInputs.y * UP + axialInputs.z * RIGHT);
    if(planarVelocity != glm::vec3(0)) velocity = glm::normalize(planarVelocity)*speed;
}
