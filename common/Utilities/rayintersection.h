#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <cfloat>
#include "common/Materials/material.h"

class Collider3D;

class RayIntersection
{
public:
    RayIntersection()
        : intersectionExists(false),
          collider(nullptr),
          t(FLT_MAX),
          point(0.0f),
          normal(0.0f)
    {
    }

    bool intersectionExists;
    Collider3D* collider;
    float t;
    Material material;
    glm::vec3 point;
    glm::vec3 normal;
    void RaySceneIntersection() {
        intersectionExists = false;
        collider = nullptr;
        t = FLT_MAX;
        point = glm::vec3(0.0f);
        normal = glm::vec3(0.0f);
    }
};
