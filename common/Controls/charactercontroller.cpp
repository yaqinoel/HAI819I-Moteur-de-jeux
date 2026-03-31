#include "charactercontroller.h"

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

    jumpPressed = false;
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
        jumpPressed = true;
        if(velocity.y <= 0 && onground){
            velocity.y = jumpStrength;
        }
    }

    glm::vec3 cameraForward = glm::normalize(glm::vec3(cam->forward().x, 0, cam->forward().z));
    SetForward(cameraForward);


}

void CharacterController::physicsProcess(float fixedDeltaTime){
    RigidBody3D::physicsProcess(fixedDeltaTime);

    glm::vec3 planarVelocity = (axialInputs.x * forward() + axialInputs.y * right())*speed;
    velocity = planarVelocity+glm::vec3(0, velocity.y, 0);

    if(velocity.y <= 0){
        RayIntersection intersection = scene->raycast(globalPosition()+UP*10.0f, DOWN, 10.2);
        if(intersection.intersectionExists){
            position = intersection.point+UP*0.08f;
            velocity.y = 0;
            onground = true;
        }
        else{
            onground = false;
        }
    }
}
