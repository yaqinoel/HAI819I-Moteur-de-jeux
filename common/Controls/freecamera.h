# pragma once
#include "../3dEntities/camera.h"

class FreeCamera : public Camera
{
public:
    FreeCamera(float width, float height, glm::vec3 position);
    FreeCamera(float width, float height, float fov, float nearPlane, float farPlane, glm::vec3 position);
    void process(float deltaTime) override;

    double lastX = 0.0, lastY = 0.0;
    bool firstMouse = true;
    float yaw = -90.0f, pitch = 0.0f;
    float speed = 10.0f, sensitivity = 0.1f;
};
