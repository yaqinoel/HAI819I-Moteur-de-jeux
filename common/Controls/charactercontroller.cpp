#include "charactercontroller.h"
#include <Scenes/PhysicsCube.h>
#include <Scenes/Cube.h>

CharacterController::CharacterController() {
    RayIntersection intersection = scene->raycast(getGlobalPosition()+UP*30.0f, DOWN, 100);
    if(intersection.intersectionExists) setGlobalPosition(intersection.point);
}


void CharacterController::process(float deltaTime){
    RigidBody3D::process(deltaTime);
    GLFWwindow* window = glfwGetCurrentContext();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //rotation
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    axialInputs = glm::vec2(0);

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
        if(velocity.y <= 0 && onground){
            velocity.y = jumpStrength;
        }
    }
    if (scene->inputPressed("action1")){
        RigidBody3D* cube = makePhysicsCube();
        cube->velocity = cam->forward()*20.0f+UP*7.0f;
        cube->setGlobalPosition(getGlobalPosition()+cam->forward()*2.0f+up());
        cube->SetForward(forward());
        Node3d * child = static_cast<Node3d*>(cube->getChildren()[0]);
        Node3d * parent = static_cast<Node3d*>(child->getParent());
        instantiate(cube);

    }

    glm::vec3 cameraForward = glm::normalize(glm::vec3(cam->forward().x, 0, cam->forward().z));
    SetForward(cameraForward);


}

void CharacterController::physicsProcess(float fixedDeltaTime){
    RigidBody3D::physicsProcess(fixedDeltaTime);
    float boundary = 0.5f;
    glm::vec3 planarVelocity = (axialInputs.x * forward() + axialInputs.y * right());
    if(planarVelocity != glm::vec3(0)) planarVelocity = glm::normalize(planarVelocity)*speed;
    RayIntersection planarIntersection = scene->raycast(currentPosition+UP*boundary, planarVelocity, boundary+speed/100);
    if(planarIntersection.intersectionExists){
        glm::vec3 nearestPoint = projectPointOnPlane(currentPosition, planarIntersection.point, planarIntersection.normal);
        currentPosition = nearestPoint + planarIntersection.normal * boundary;
        planarVelocity = glm::cross(glm::cross(planarIntersection.normal, planarVelocity), planarIntersection.normal);
    }

    velocity = planarVelocity+glm::vec3(0, velocity.y, 0);

    if(velocity.y <= 0){
        RayIntersection verticalIntersection = scene->raycast(currentPosition+UP*2.0f, DOWN, 2.2);
        if(verticalIntersection.intersectionExists){
            currentPosition = verticalIntersection.point+UP*0.08f;
            velocity.y = 0;
            onground = true;
        }
        else{
            onground = false;
        }
    }
}
