#pragma once
#include "shape.h"
#include "../3dEntities/collisionshape3d.h"

class Cube : public Shape
{
public:
    Cube(float size_x, float size_y, float size_z);
    Cube(float size_x, float size_y, float size_z, std::vector<Vertex> &ver, std::vector<Triangle> &tri);
    RayIntersection raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length) override;
    void setMesh(std::vector<Vertex> &ver, std::vector<Triangle> &tri) override;
private:
    float size_x;
    float size_y;
    float size_z;
};
