#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include "common/Utilities/rayintersection.h"
#include "common/Utilities/colliderIntersection.h"
#include "../Utilities/Geometry.h"

class Cube;
class VoxelShape;

enum ShapeType{
    UNDEFINED,
    CONVEX,
    CUBE,
    SPHERE,
    VOXEL
};


class Shape
{
public:
    Shape();
    virtual RayIntersection raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length)=0;
    virtual std::vector<ColliderIntersection> intersectCube(Cube* cube, bool calculatePoints = false){return std::vector<ColliderIntersection>();}
    virtual std::vector<ColliderIntersection> intersectVoxel(VoxelShape* voxel, bool calculatePoints = false){return std::vector<ColliderIntersection>();}
    virtual std::vector<ColliderIntersection> intersect(Shape* shape, bool calculatePoints = false);
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
    std::vector<glm::vec3> ClipPolygonToPlane(const std::vector<glm::vec3>& polygon, const glm::vec3& planeNormal, const glm::vec3& planePoint);
    std::vector<glm::vec3> clipPolygon(const std::vector<glm::vec3>& polygon, const std::vector<glm::vec3>& planeNormals, const std::vector<glm::vec3>& planePoints);
    std::vector<glm::vec3> projectToPlane(const std::vector<glm::vec3>& points, const glm::vec3& planeNormal, const glm::vec3& planePoint);
};

