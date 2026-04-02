#include "charactercontroller.h"
#include <Scenes/PhysicsCube.h>
#include <Scenes/Cube.h>

CharacterController::CharacterController() {
    RayIntersection intersection = scene->raycast(position+UP*30.0f, DOWN, 100);
    if(intersection.intersectionExists) position = intersection.point;
}


void CharacterController::process(float deltaTime){
    RigidBody3D::process(deltaTime);
    GLFWwindow* window = glfwGetCurrentContext();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //rotation
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    axialInputs = glm::vec2(0);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        axialInputs.x += 1;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        axialInputs.x -= 1;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        axialInputs.y += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        axialInputs.y -= 1;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        if(velocity.y <= 0 && onground){
            velocity.y = jumpStrength;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
        RigidBody3D* cube = makePhysicsCube();
        cube->velocity = cam->forward()*20.0f;
        cube->setPosition(globalPosition()+cam->forward()*2.0f);
        // Node3d* cube = makeCube();
        // cube->setGlobalPosition(globalPosition()+forward()*2.0f);
        Node3d * child = static_cast<Node3d*>(cube->getChildren()[0]);
        Node3d * parent = static_cast<Node3d*>(child->getParent());
        instantiate(cube);
        //std::cout << parent->name << " = " <<child->getParent()->name << " is the parent of " << child->name <<std::endl;
        //std::cout << glm::to_string(cube->position) << " " << glm::to_string(cube->globalPosition()) << std::endl;
        //std::cout << glm::to_string(child->position) << " " << glm::to_string(child->globalPosition()) << " " << glm::to_string(child->globalMatrix()) << std::endl;
        //std::cout << glm::to_string(parent->globalMatrix()* child->localMatrix()) << std::endl; works as intended
        //std::cout << glm::to_string(parent->position) << " " << glm::to_string(parent->globalPosition()) << std::endl;

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
