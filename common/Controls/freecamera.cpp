#include "freecamera.h"

FreeCamera::FreeCamera(float width, float height, glm::vec3 position) : Camera(width, height, position){}

FreeCamera::FreeCamera(float width, float height, float fov, float nearPlane, float farPlane, glm::vec3 position): Camera(width, height, fov, nearPlane, farPlane, position){}

void FreeCamera::process(float deltaTime){
    Camera::process(deltaTime);
    GLFWwindow* window = glfwGetCurrentContext();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Mouse look
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    if(firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed
    lastX = xpos; lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f) pitch =  89.0f;
    if(pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    setForward(glm::normalize(front));

    // Keyboard movement
    float velocity = speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        setGlobalPosition(getGlobalPosition() + forward() * velocity);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        setGlobalPosition(getGlobalPosition() - forward() * velocity);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        setGlobalPosition(getGlobalPosition() - right() * velocity);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        setGlobalPosition(getGlobalPosition() + right() * velocity);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        setGlobalPosition(getGlobalPosition() + UP * velocity);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        setGlobalPosition(getGlobalPosition() - UP * velocity);
}