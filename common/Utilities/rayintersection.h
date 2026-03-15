#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "common/Materials/material.h"

class Mesh;

class RayIntersection
{
public:
    bool intersectionExists;
    unsigned int typeOfIntersectedObject;
    Mesh* intersectedMesh;
    float t;
    Material material;
    glm::vec3 point;
    glm::vec3 normal;
    void RaySceneIntersection() {
        intersectionExists = false;
        t = FLT_MAX;
    }
};
