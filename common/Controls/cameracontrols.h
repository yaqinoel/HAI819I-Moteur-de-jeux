
#pragma once

#include<glm/glm.hpp>
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include "../3dEntities/camera.h"

class CameraControls : public Camera
{
public:
    CameraControls(float width, float height, glm::vec3 position);
    CameraControls(float width, float height, float fov, float nearPlane, float farPlane, glm::vec3 position);
    void process(float deltaTime) override;
    float speed = 5.0f;
    float sensitivity = 15.0f;
    bool orbital = false;
    glm::vec3 orbitalPos = glm::vec3(0, 9, -10);
    float timer = 1;

    double lastX = 0.0;
    double lastY = 0.0;
};
