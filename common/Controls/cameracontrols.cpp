#include "cameracontrols.h"

CameraControls::CameraControls(float width, float height, glm::vec3 position) : Camera(width, height, position){}
CameraControls::CameraControls(float width, float height, float fov, float nearPlane, float farPlane, glm::vec3 position):
    Camera(width, height, fov, nearPlane, farPlane, position){}

void CameraControls::process(float deltaTime){
    GLFWwindow* window = glfwGetCurrentContext();
    timer -= deltaTime;
    if(!orbital){
        float trueSpeed = speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            position += trueSpeed * forward();
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            position -= trueSpeed * forward();
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            position += trueSpeed * right();
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            position -= trueSpeed * right();
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            position += trueSpeed * up();
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            position -= trueSpeed * up();
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && timer < 0){
        timer = 1;
        if(orbital){
            orbital = false;
            SetForward(glm::vec3(0, 0, 1));
            position += glm::vec3(0, -4, 0);
        }
        else{
            orbital = !orbital;
            position = orbitalPos;
            SetForward(glm::vec3(0, -1, 1));
        }
    }

}
