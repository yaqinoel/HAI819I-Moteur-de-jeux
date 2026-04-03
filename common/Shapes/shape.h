#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include "common/Utilities/rayintersection.h"
#include "common/Utilities/colliderIntersection.h"
#include "../Utilities/Geometry.h"

class Cube;

enum ShapeType{
    UNDEFINED,
    CONVEX,
    CUBE,
    SPHERE
};

class Shape
{
public:
    Shape();
    virtual RayIntersection raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length)=0;
    virtual ColliderIntersection intersectCube(Cube* cube, bool calculatePoints = false){return ColliderIntersection();}
    virtual ColliderIntersection intersect(Shape* shape, bool calculatePoints = false);
    CollisionShape3D* collider = nullptr;
    float radius = 0;
    ShapeType type;
    virtual void setMesh(std::vector<Vertex> &ver, std::vector<Triangle> &tri){}
    virtual std::vector<glm::vec3> getVertices(){return vertices;}
    virtual std::vector<glm::ivec3> getTriangles(){return triangles;}

protected:
    std::vector<glm::vec3> vertices = std::vector<glm::vec3>() ;
    std::vector<glm::ivec3>triangles = std::vector<glm::ivec3>();
};
