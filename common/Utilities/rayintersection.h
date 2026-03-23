#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "common/Materials/material.h"

class CollisionShape3D;

class Mesh;

class RayIntersection
{
public:
    bool intersectionExists;
    unsigned int typeOfIntersectedObject;
    CollisionShape3D* collider;
    float t;
    Material material;
    glm::vec3 point;
    glm::vec3 normal;
    void RaySceneIntersection() {
        intersectionExists = false;
        t = FLT_MAX;
    }
};
