#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include "common/Utilities/rayintersection.h"


class Shape
{
public:
    Shape();
    virtual RayIntersection raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length)=0;
    CollisionShape3D* collider = nullptr;
    float radius = 0;
    virtual std::vector<glm::vec3> getVertices(){return vertices;}
    virtual std::vector<glm::ivec3> getTriangles(){return triangles;}
protected:
    std::vector<glm::vec3> vertices = std::vector<glm::vec3>() ;
    std::vector<glm::ivec3>triangles = std::vector<glm::ivec3>();
};
