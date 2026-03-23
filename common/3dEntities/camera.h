
#pragma once
#include "node3d.h"
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera : public Node3d
{
public:
    Camera(float width, float height, glm::vec3 position);
    Camera(float width, float height, float fov, float nearPlane, float farPlane, glm::vec3 position);
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

    float fov;
    float nearPlane;
    float farPlane;
    float width;
    float height;
};


