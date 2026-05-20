#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include "common/Physics/physicstypes.h"
#include "common/Utilities/rayintersection.h"
#include "../Utilities/Geometry.h"

class CollisionShape3D;

enum ShapeType{
    UNDEFINED,
    CONVEX,
    CUBE,
    SPHERE,
    CAPSULE,
    VOXEL,
    SHAPE_COUNT
};


class Shape
{
public:
    Shape();
    virtual RayIntersection raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length)=0;
    virtual RayIntersection raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length, uint64_t layers);
    virtual bool computeAabb(const CollisionShape3D& collider, PhysicsAabb& outAabb) const;
    CollisionShape3D* collider = nullptr;
    float radius = 0;
    glm::mat3 inertia = glm::mat3(0); //calculated as if the total mass was 1
    ShapeType type;
    virtual void setMesh(std::vector<Vertex> &ver, std::vector<Triangle> &tri){}
    virtual std::vector<glm::vec3> getVertices(){return vertices;}
    virtual std::vector<glm::ivec3> getTriangles(){return triangles;}

    void setMass(float m){inertia*= m/mass;mass = m; }
    float getMass(){return mass;}

protected:
    float mass = 1000;
    std::vector<glm::vec3> vertices = std::vector<glm::vec3>() ;
    std::vector<glm::ivec3>triangles = std::vector<glm::ivec3>();
};
