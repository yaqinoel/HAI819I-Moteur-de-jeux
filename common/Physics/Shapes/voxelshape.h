#pragma once

#include "meshshape.h"
#include <functional>

struct OrientedBox;

class VoxelShape : public MeshShape
{
public:
    VoxelShape();
    void InitMesh(int width, int height, int depth, std::vector<unsigned short int> voxelData);
    virtual RayIntersection raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length) override ;
    bool computeAabb(const Collider3D& collider, PhysicsAabb& outAabb) const override;
    bool isSolid(int x, int y, int z) const;
    glm::ivec3 worldToCell(const glm::vec3& worldPoint) const;
    glm::ivec3 worldToCell(const Collider3D& collider, const glm::vec3& worldPoint) const;
    glm::vec3 cellLocalMin(int x, int y, int z) const;
    glm::vec3 cellLocalMax(int x, int y, int z) const;
    glm::vec3 cellMin(int x, int y, int z) const;
    glm::vec3 cellMax(int x, int y, int z) const;
    OrientedBox cellWorldBox(const Collider3D& collider, int x, int y, int z) const;
    bool cellWorldAabb(const Collider3D& collider, int x, int y, int z, PhysicsAabb& outAabb) const;
    void forEachSolidCell(const std::function<void(int, int, int)>& callback) const;
    int solidCellCount() const;
    glm::vec3 getLocalCenterOfMass() const override { return localCenterOfMass; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getDepth() const { return depth; }
    float getVoxelSize() const { return voxelSize; }
    virtual void set(int x, int y, int z, uint8_t v) { setCell(x, y, z, v); }

protected:
    std::vector<uint8_t> voxels;
    float voxelSize = 1.0f;
    glm::vec3 localCenterOfMass = glm::vec3(0.0f);
    int width = 0;
    int height = 0;
    int depth = 0;
    int index(int x, int y, int z) const { return x*height*depth+y*depth+z; }
    uint8_t get(int x, int y, int z) const {return voxels[x*height*depth+y*depth+z];}
    void setCell(int x, int y, int z, uint8_t v);
    void recomputeInertia();
};
