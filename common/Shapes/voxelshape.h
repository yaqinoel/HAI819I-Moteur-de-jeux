#pragma once

#include "meshshape.h"

class VoxelShape : public MeshShape
{
public:
    VoxelShape();
    void InitMesh(int width, int height, int depth, std::vector<unsigned short int> voxelData);
    void InitMeshFromTerrain(int width, int height, int depth, std::vector<unsigned short int> voxelData);
    virtual RayIntersection raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length) override ;
    virtual std::vector<ColliderIntersection> intersectCube(Cube* cube, bool calculatePoints = false) override;
    void set(int x, int y, int z, uint8_t v) {voxels[(x-1)*height*depth+(y-1)*depth+(z-1)] = v;}
private:
    std::vector<uint8_t> voxels;
    float voxelSize = 1.0f;
    int width;
    int height;
    int depth;
    int index(int x, int y, int z) const { return x*height*depth+y*depth+z; }
    uint8_t get(int x, int y, int z) const {return voxels[x*height*depth+y*depth+z];}
};

