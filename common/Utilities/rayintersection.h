#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "common/Materials/material.h"

class Collider3D;

class RayIntersection
{
public:
    bool intersectionExists;
    Collider3D* collider;
    float t;
    Material material;
    glm::vec3 point;
    glm::vec3 normal;
    void RaySceneIntersection() {
        intersectionExists = false;
        t = FLT_MAX;
    }
};
