#pragma once
#include "shape.h"
#include "../3dEntities/collisionshape3d.h"

class Cube : public Shape
{
public:
    Cube(float size_x=1, float size_y=1, float size_z=1);
    Cube(float size_x, float size_y, float size_z, std::vector<Vertex> &ver, std::vector<Triangle> &tri);
    RayIntersection raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length) override;
    void setMesh(std::vector<Vertex> &ver, std::vector<Triangle> &tri) override;
    virtual std::vector<ColliderIntersection> intersectCube(Cube* cube, bool calculatePoints = false) override;
    virtual std::vector<ColliderIntersection> intersectVoxel(VoxelShape* voxel, bool calculatePoints = false) override;
    glm::vec3 size;
    glm::vec3 halfSize;
};
