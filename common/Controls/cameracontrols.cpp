#include "cameracontrols.h"

CameraControls::CameraControls(float width, float height, glm::vec3 position) : Camera(width, height, position){
    targetNode = new Node3d();
    instantiate(targetNode);
}
CameraControls::CameraControls(float width, float height, float fov, float nearPlane, float farPlane, glm::vec3 position): Camera(width, height, fov, nearPlane, farPlane, position){
    targetNode = new Node3d();
    instantiate(targetNode);
    setForward(glm::vec3(0, -1, 1));
}

void CameraControls::process(float deltaTime)
{
    Camera::process(deltaTime);

    GLFWwindow* window = glfwGetCurrentContext();

    // =========================
    // Mouse Input
    // =========================
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    static bool firstMouse = true;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = float(xpos - lastX);
    float yoffset = float(lastY - ypos);

    lastX = xpos;
    lastY = ypos;

    float sensitivityFactor = sensitivity * deltaTime;

    xoffset *= sensitivityFactor;
    yoffset *= sensitivityFactor;

    static float yaw = -90.0f;
    static float pitch = 0.0f;

    yaw += xoffset;
    pitch += yoffset;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    glm::vec3 direction;

    direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));

    direction.y = sin(glm::radians(pitch));

    direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

    direction = glm::normalize(direction);

    if (!paused)
    {
        targetNode->setForward(direction);
    }

    glm::vec3 targetPos = pivot->getGlobalPosition() + pivotOffset;
    glm::vec3 backward = targetNode->getGlobalRotation() * BACKWARDS;

    targetNode->setGlobalPosition(targetPos + backward * pivotDistance );

    setGlobalPosition(targetNode->getGlobalPosition());
    setGlobalRotation(targetNode->getGlobalRotation());
}

void CameraControls::CameraMovement(float deltaTime){
    GLFWwindow* window = glfwGetCurrentContext();
    //rotation
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    float trueSpeed = speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Translate(trueSpeed * forward());
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Translate(-trueSpeed * forward());
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Translate(trueSpeed * right());
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Translate(-trueSpeed * right());
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        Translate(trueSpeed * up());
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        Translate(-trueSpeed * up());
}
