#include "charactercontroller.h"

CharacterController::CharacterController() {}

void CharacterController::setTerrain(TerrainManager * terrain){
    terrainManager = terrain;
    RayIntersection intersection = terrainManager->intersect(position+UP*30.0f, DOWN, 100);
    if(intersection.intersectionExists) position = intersection.point;
}

void CharacterController::process(float deltaTime){
    Mesh::process(deltaTime);
    GLFWwindow* window = glfwGetCurrentContext();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //rotation
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    float trueSpeed = speed * deltaTime;
    bool moved = false;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        position += trueSpeed * forward();
        moved = true;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        position -= trueSpeed * forward();
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        position += trueSpeed * right();
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        position -= trueSpeed * right();
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        position += trueSpeed * up();
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        position -= trueSpeed * up();

    if(moved){
        glm::vec3 cameraForward = glm::normalize(glm::vec3(cam->forward().x, 0, cam->forward().z));
        SetForward(cameraForward);
        RayIntersection intersection = terrainManager->intersect(position+UP*30.0f, DOWN, 100);
        if(intersection.intersectionExists) position = intersection.point;
    }

}
