#include "dynamicvoxel.h"

#include <common/Physics/Shapes/terrainvoxelshape.h>
#include <common/Physics/rigidbody3d.h>

DynamicVoxel::DynamicVoxel()
{
    texSize = glm::ivec2(3, 2);
    texCoord.insert({1, glm::vec2(0, 0)});//grass
    texCoord.insert({2, glm::vec2(1, 0)});//dirt
    texCoord.insert({3, glm::vec2(0, 1)});//stone
    texCoord.insert({4, glm::vec2(1, 1)});//cobble
    texCoord.insert({5, glm::vec2(2, 0)});//wood
    texCoord.insert({6, glm::vec2(2, 1)});//leaves
}


DynamicVoxel::DynamicVoxel(glm::vec3 position, glm::vec3 size, RigidBody3D *parent) : DynamicVoxel(){
    this->sizeX = size.x;
    this->sizeY = size.y;
    this->sizeZ = size.z;
    this->parent = parent;
    parent->setGlobalPosition(position);
}

void DynamicVoxel::addX(bool atStart) {
    int newSizeX = sizeX + 1;
    std::vector<unsigned short int> newData(newSizeX * sizeY * sizeZ, 0);

    for (int x = 0; x < sizeX; x++)
        for (int y = 0; y < sizeY; y++)
            for (int z = 0; z < sizeZ; z++) {
                int destX = atStart ? x + 1 : x;
                newData[destX * (sizeY * sizeZ) + y * sizeZ + z] = voxelData[x * (sizeY * sizeZ) + y * sizeZ + z];
                std::cout << "shifting " << destX * (sizeY * sizeZ) + y * sizeZ + z << " " << x * (sizeY * sizeZ) + y * sizeZ + z << std::endl;
            }

    voxelData = std::move(newData);
    sizeX = newSizeX;
    if(atStart){
        glm::vec3 localPos = getLocalPosition();
        setLocalPosition(glm::vec3(localPos.x-1, localPos.y,localPos.z ));
    }
}

void DynamicVoxel::addY(bool atStart) {
    int newSizeY = sizeY + 1;
    std::vector<unsigned short int> newData(sizeX * newSizeY * sizeZ, 0);

    for (int x = 0; x < sizeX; x++)
        for (int y = 0; y < sizeY; y++)
            for (int z = 0; z < sizeZ; z++) {
                int destY = atStart ? y + 1 : y;
                newData[x * (newSizeY * sizeZ) + destY * sizeZ + z] = voxelData[x * (sizeY * sizeZ) + y * sizeZ + z];
            }

    voxelData = std::move(newData);
    sizeY = newSizeY;
    if(atStart){
        glm::vec3 localPos = getLocalPosition();
        setLocalPosition(glm::vec3(localPos.x, localPos.y-1,localPos.z ));
    }
}

void DynamicVoxel::addZ(bool atStart) {
    int newSizeZ = sizeZ + 1;
    std::vector<unsigned short int> newData(sizeX * sizeY * newSizeZ, 0);


    for (int x = 0; x < sizeX; x++)
        for (int y = 0; y < sizeY; y++)
            for (int z = 0; z < sizeZ; z++) {
                int destZ = atStart ? z + 1 : z;
                newData[x * sizeY * newSizeZ + y * newSizeZ + destZ] = voxelData[x * sizeY * sizeZ + y * sizeZ + z];
            }

    voxelData = std::move(newData);
    sizeZ = newSizeZ;
    if(atStart){
        glm::vec3 localPos = getLocalPosition();
        setLocalPosition(glm::vec3(localPos.x, localPos.y,localPos.z -1));
    }
}

unsigned short int DynamicVoxel::getData(int x, int y, int z){
    return voxelData[x*sizeY*sizeZ+y*sizeZ+z];
}


void DynamicVoxel::setData(int x, int y, int z, unsigned short int v){
    if(v > 0){
        if(x < 0) {
            addX(true);
            voxelData[(x+1)*sizeY*sizeZ+y*sizeZ+z] = v;
        }
        else if(y < 0) {
            addY(true);
            voxelData[x*sizeY*sizeZ+(y+1)*sizeZ+z] = v;
        }
        else if(z < 0) {
            addZ(true);
            voxelData[x*sizeY*sizeZ+y*sizeZ+z+1] = v;
        }
        else if(x >= sizeX) {
            addX(false);
            voxelData[x*sizeY*sizeZ+y*sizeZ+z] = v;
        }
        else if(y >= sizeY) {
            addY(false);
            voxelData[x*sizeY*sizeZ+y*sizeZ+z] = v;
        }
        else if(z >= sizeZ) {
            addZ(false);
            voxelData[x*sizeY*sizeZ+y*sizeZ+z] = v;
        }
        else{
            voxelData[x*sizeY*sizeZ+y*sizeZ+z] = v;
        }
    }
    else{
        voxelData[x*sizeY*sizeZ+y*sizeZ+z] = v;
    }
    shape->InitMesh(sizeX, sizeY, sizeZ, voxelData);
}

int DynamicVoxel::removeTile(glm::vec3 world_position){
    glm::vec4 local = getInverseGlobalMatrix() * glm::vec4(world_position, 1.0);
    glm::ivec3 ilocal = glm::ivec3(round(local[0]), round(local[1]), round(local[2]));
    int val = getData(ilocal.x, ilocal.y, ilocal.z);
    setData(ilocal.x, ilocal.y, ilocal.z, 0);
    ResetMesh();
    synchronize();
    if(voxel_nbr <= 0) {
        parent->erase();
    }
    return val;
}

void DynamicVoxel::addTile(glm::vec3 world_position, int v){
    glm::vec4 local = getInverseGlobalMatrix() * glm::vec4(world_position, 1.0);
    glm::ivec3 ilocal = glm::ivec3(round(local[0]), round(local[1]), round(local[2]));
    setData(ilocal.x, ilocal.y, ilocal.z, v);
    ResetMesh();
    synchronize();
}

void DynamicVoxel::CreateTopSquare(int x, int y, int z) {
    int i = vertices.size();
    int type = getData(x, y, z);
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z - 0.5), UP, glm::vec2(0.625/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z + 0.5), UP, glm::vec2(0.625/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z - 0.5), UP, glm::vec2(0.875/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z + 0.5), UP, glm::vec2(0.875/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    triangles.push_back(Triangle(i, i+2, i+1));
    triangles.push_back(Triangle(i+1, i+2 , i+3));
}
void DynamicVoxel::CreateBottomSquare(int x, int y, int z) {
    int i = vertices.size();
    int type = getData(x, y, z);
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z - 0.5), DOWN, glm::vec2(0.375/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z + 0.5), DOWN, glm::vec2(0.375/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z - 0.5), DOWN, glm::vec2(0.125/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z + 0.5), DOWN, glm::vec2(0.125/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    triangles.push_back(Triangle(i, i+1, i+2));
    triangles.push_back(Triangle(i+2, i+1, i+3));
}
void DynamicVoxel::CreateRightSquare(int x, int y, int z) {
    int i = vertices.size();
    int type = getData(x, y, z);
    const glm::vec3 normal(1.0f, 0.0f, 0.0f);
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z - 0.5), normal, glm::vec2(0.625/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z + 0.5), normal, glm::vec2(0.625/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z + 0.5), normal, glm::vec2(0.375/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z - 0.5), normal, glm::vec2(0.375/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    triangles.push_back(Triangle(i, i+1, i+2));
    triangles.push_back(Triangle(i, i+2, i+3));
}
void DynamicVoxel::CreateLeftSquare(int x, int y, int z) {
    int i = vertices.size();
    int type = getData(x, y, z);
    const glm::vec3 normal(-1.0f, 0.0f, 0.0f);
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z - 0.5), normal, glm::vec2(0.625/texSize.x, 0.25/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z + 0.5), normal, glm::vec2(0.625/texSize.x, 0.0/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z + 0.5), normal, glm::vec2(0.375/texSize.x, 0.0/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z - 0.5), normal, glm::vec2(0.375/texSize.x, 0.25/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    triangles.push_back(Triangle(i, i+2, i+1));
    triangles.push_back(Triangle(i, i+3, i+2));
}

void DynamicVoxel::CreateFrontSquare(int x, int y, int z) {
    int i = vertices.size();
    int type = getData(x, y, z);
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z + 0.5), FORWARD, glm::vec2(0.625/texSize.x, 1.0/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z + 0.5), FORWARD, glm::vec2(0.375/texSize.x, 1.0/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z + 0.5), FORWARD, glm::vec2(0.625/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z + 0.5), FORWARD, glm::vec2(0.375/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    triangles.push_back(Triangle(i, i+1, i+2));
    triangles.push_back(Triangle(i+1, i+3, i+2));
}

void DynamicVoxel::CreateBackSquare(int x, int y, int z) {
    int i = vertices.size();
    int type = getData(x, y, z);
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z - 0.5), BACKWARDS, glm::vec2(0.625/texSize.x, 0.25/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z - 0.5), BACKWARDS, glm::vec2(0.375/texSize.x, 0.25/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z - 0.5), BACKWARDS, glm::vec2(0.625/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z - 0.5), BACKWARDS, glm::vec2(0.375/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    triangles.push_back(Triangle(i, i+2, i+1));
    triangles.push_back(Triangle(i+1, i+2, i+3));
}

void DynamicVoxel::ResetMesh() {
    voxel_nbr = 0;
    vertices.clear();
    triangles.clear();
    for (int x = 0; x < sizeX; x++) {
        for (int y = 0; y < sizeY; y++) {
            for (int z = 0; z < sizeZ; z++) {
                if (getData(x, y, z) != 0) {
                    if (y+1 >= sizeY  ||  getData(x, y+1, z) == 0) CreateTopSquare(x, y, z);
                    if (y-1 < 0       ||  getData(x, y-1, z) == 0) CreateBottomSquare(x, y, z);
                    if (x+1 >= sizeX  ||  getData(x+1, y, z) == 0) CreateRightSquare(x, y, z);
                    if (x-1 < 0       ||  getData(x-1, y, z) == 0) CreateLeftSquare(x, y, z);
                    if (z+1 >= sizeZ  ||  getData(x, y, z+1) == 0) CreateFrontSquare(x, y, z);
                    if (z-1 < 0       ||  getData(x, y, z-1) == 0) CreateBackSquare(x, y, z);
                    voxel_nbr ++;
                }
            }
        }
    }
}


void DynamicVoxel::InitMesh(){
    ResetMesh();
    shape = new VoxelShape();
    shape->InitMesh(sizeX, sizeY, sizeZ, voxelData);
    parent->addChild(this);
    glm::vec3 shapeCenter = shape->getLocalCenterOfMass();
    glm::vec3 shapeOriginOffset(shape->getVoxelSize());
    glm::vec3 visualCenter = shapeCenter - shapeOriginOffset;
    setLocalPosition(-visualCenter);

    if(collision != nullptr) collision->erase();
    collision = new Collider3D();
    collision->name = "dynamic voxel collider";
    collision->setShape(shape);
    collision->mass = 1000 * shape->solidCellCount();
    addChild(collision);
    collision->setLocalPosition(-shapeOriginOffset);
    parent->addCollider(collision);
}
