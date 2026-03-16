#include "cameracontrols.h"

CameraControls::CameraControls(float width, float height, glm::vec3 position) : Camera(width, height, position){}
CameraControls::CameraControls(float width, float height, float fov, float nearPlane, float farPlane, glm::vec3 position):
    Camera(width, height, fov, nearPlane, farPlane, position){
    SetForward(glm::vec3(0, -1, 1));}

void CameraControls::process(float deltaTime){
    Camera::process(deltaTime);
    GLFWwindow* window = glfwGetCurrentContext();
    timer -= deltaTime;
    if(!orbital){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        //rotation
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        static bool firstMouse = true;
        if(firstMouse){
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = float(lastX - xpos);
        float yoffset = float(lastY - ypos);

        lastX = xpos;
        lastY = ypos;

        float sensitivityFactor = sensitivity * deltaTime;
        xoffset *= sensitivityFactor;
        yoffset *= sensitivityFactor;

        glm::vec3 f = forward();
        glm::vec3 flatForward = glm::normalize(glm::vec3(f.x, 0, f.z));
        glm::quat yaw = glm::angleAxis(glm::radians(xoffset), UP);
        glm::vec3 pitchAxis = glm::normalize(glm::cross(flatForward, UP));
        glm::quat pitch = glm::angleAxis(glm::radians(yoffset), pitchAxis);
        glm::quat newRot = glm::normalize(pitch * yaw);
        SetForward(newRot * forward());
        position = pivot->up()+ pivot->position +pivotDistance*(rotation*BACKWARDS);


    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && timer < 0){
        timer = 1;
        if(orbital){
            orbital = false;
        }
        else{
            orbital = !orbital;
            position = orbitalPos;
            SetForward(glm::vec3(0, -1, 1));
        }
    }

}

void CameraControls::CameraMovement(float deltaTime){
    GLFWwindow* window = glfwGetCurrentContext();
    //rotation
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

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
