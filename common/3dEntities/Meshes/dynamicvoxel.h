#pragma once


#include <unordered_map>

#include <common/Utilities/hash.h>

#include <common/3dEntities/Mesh.hpp>

#include <common/Physics/Shapes/voxelshape.h>

class Collider3D;
class TerrainVoxelShape;

class DynamicVoxel : public Mesh
{
public:
    DynamicVoxel();
    DynamicVoxel(glm::vec3 position, glm::vec3 size, RigidBody3D *parent);
    void InitMesh();
    void ResetMesh();
    glm::ivec2 texSize = glm::ivec2(1, 1);
    std::map<int, glm::ivec2> texCoord = std::map<int, glm::ivec2>();
    unsigned short int getData(int x, int y, int z);
    void setData(int x, int y, int z, unsigned short int v);
    int removeTile(glm::vec3 world_position);
    void addTile(glm::vec3 world_position, int v);
    void addX(bool atStart);
    void addY(bool atStart);
    void addZ(bool atStart);
    RigidBody3D *parent;
    void setVoxelData(std::vector<unsigned short int> d){voxelData = d;}
    void setMassPerCell(float mass){massPerCell = mass;}


private:
    void refreshPhysicsFromVoxelData(bool preserveVoxelWorldTransform);
    void CreateTopSquare(int x, int y, int z);
    void CreateBottomSquare(int x, int y, int z);
    void CreateRightSquare(int x, int y, int z);
    void CreateLeftSquare(int x, int y, int z);
    void CreateFrontSquare(int x, int y, int z);
    void CreateBackSquare(int x, int y, int z);
    int sizeX, sizeY, sizeZ;
    std::vector<unsigned short int> voxelData = std::vector<unsigned short int>();
    VoxelShape* shape = nullptr;
    Collider3D* collision = nullptr;
    int voxel_nbr = 0;
    float massPerCell = 1000.0f;
};
