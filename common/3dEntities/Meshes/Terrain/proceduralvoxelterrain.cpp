#include "proceduralvoxelterrain.h"

#include <common/Shapes/voxelshape.h>

ProceduralVoxelTerrain::ProceduralVoxelTerrain()
{
    collision = new CollisionShape3D();
    instantiate(collision, this);
    texSize = glm::ivec2(2, 1);
    texCoord.insert({1, glm::vec2(0, 0)});
    texCoord.insert({2, glm::vec2(1, 0)});
}
ProceduralVoxelTerrain::ProceduralVoxelTerrain(int posX, int posY, int resX, int resY, float sizeX, float sizeY, float sizeZ, std::unordered_map<glm::ivec3, int, IVec3Hash> *edited) : ProceduralVoxelTerrain()
{
    this->edited = edited;
    this->resX = resX;
    this->resY = resY;
    this->sizeX = sizeX;
    this->sizeY = sizeY;
    this->sizeZ = sizeZ;
    setLocalPosition(glm::vec3(posX, getLocalPosition().y, posY));
}


unsigned short int ProceduralVoxelTerrain::getData(int x, int y, int z){
    return chunkData[x*resolution*resolution+y*resolution+z];
}


void ProceduralVoxelTerrain::setData(int x, int y, int z, unsigned short int v){
    chunkData[x*resolution*resolution+y*resolution+z] = v;
}

void ProceduralVoxelTerrain::InitData() {
    glm::ivec3 pos = glm::vec3(getLocalPosition().x, 0, getLocalPosition().z);

    resolution = resX+2;
    frequency = 1.0 / sizeX;
    chunkData.resize(resolution*resolution*resolution);
    for (int x = 0; x < resolution; x++) {
        for (int z = 0; z < resolution; z++) {
            float worldX = x-1 + pos.x;
            float worldZ = z-1 + pos.z;
            int height = roundf(fabs(glm::perlin(glm::vec2(worldX, worldZ) * frequency)) * sizeX);
            for (int y = 0; y < resolution; y++) {
                glm::vec4 global = getGlobalMatrix() * glm::vec4(x, y, z, 1.0);
                glm::ivec3 iglobal = glm::ivec3(global[0], global[1], global[2]);
                if(edited->find(iglobal) != edited->end()) {
                    setData(x, y, z, (*edited)[iglobal]);
                }
                else{
                    setData(x, y, z, 0);
                    if(height >= y-1){
                        if((height >= y)){
                            setData(x, y, z, 2);
                        }
                        else{
                            setData(x, y, z, 1);
                        }
                    }
                }
            }
        }
    }
}

void ProceduralVoxelTerrain::removeTile(glm::vec3 world_position){
    glm::vec4 local = getInverseGlobalMatrix() * glm::vec4(world_position, 1.0);
    glm::ivec3 ilocal = glm::ivec3(local[0], local[1], local[2]);
    shape->set(ilocal.x, ilocal.y, ilocal.z, 0);
    (*edited)[world_position] = 0;
    InitData();
    ResetMesh();
    synchronize();
    if(ilocal.x == 1){
        neighbour_X->InitData();
        neighbour_X->ResetMesh();
        neighbour_X->synchronize();
    }
    if(ilocal.x == sizeX){
        neighbourX->InitData();
        neighbourX->ResetMesh();
        neighbourX->synchronize();
    }
    if(ilocal.z == 1){
        neighbour_Z->InitData();
        neighbour_Z->ResetMesh();
        neighbour_Z->synchronize();
    }
    if(ilocal.z == sizeZ){
        neighbourZ->InitData();
        neighbourZ->ResetMesh();
        neighbourZ->synchronize();
    }
}

void ProceduralVoxelTerrain::CreateTopSquare(int x, int y, int z) {
    int i = vertices.size();
    int type = getData(x, y, z);
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z - 0.5), UP, glm::vec2(0.625/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z + 0.5), UP, glm::vec2(0.625/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z - 0.5), UP, glm::vec2(0.875/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z + 0.5), UP, glm::vec2(0.875/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    triangles.push_back(Triangle(i, i+2, i+1));
    triangles.push_back(Triangle(i+1, i+2 , i+3));
}
void ProceduralVoxelTerrain::CreateBottomSquare(int x, int y, int z) {
    int i = vertices.size();
    int type = getData(x, y, z);
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z - 0.5), DOWN, glm::vec2(0.375/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z + 0.5), DOWN, glm::vec2(0.375/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z - 0.5), DOWN, glm::vec2(0.125/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z + 0.5), DOWN, glm::vec2(0.125/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    triangles.push_back(Triangle(i, i+1, i+2));
    triangles.push_back(Triangle(i+2, i+1, i+3));
}
void ProceduralVoxelTerrain::CreateRightSquare(int x, int y, int z) {
    int i = vertices.size();
    int type = getData(x, y, z);
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z - 0.5), RIGHT, glm::vec2(0.625/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z + 0.5), RIGHT, glm::vec2(0.625/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z + 0.5), RIGHT, glm::vec2(0.375/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z - 0.5), RIGHT, glm::vec2(0.375/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    triangles.push_back(Triangle(i, i+1, i+2));
    triangles.push_back(Triangle(i, i+2, i+3));
}
void ProceduralVoxelTerrain::CreateLeftSquare(int x, int y, int z) {
    int i = vertices.size();
    int type = getData(x, y, z);
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z - 0.5), LEFT, glm::vec2(0.625/texSize.x, 0.25/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z + 0.5), LEFT, glm::vec2(0.625/texSize.x, 0.0/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z + 0.5), LEFT, glm::vec2(0.375/texSize.x, 0.0/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z - 0.5), LEFT, glm::vec2(0.375/texSize.x, 0.25/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    triangles.push_back(Triangle(i, i+2, i+1));
    triangles.push_back(Triangle(i, i+3, i+2));
}

void ProceduralVoxelTerrain::CreateFrontSquare(int x, int y, int z) {
    int i = vertices.size();
    int type = getData(x, y, z);
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z + 0.5), FORWARD, glm::vec2(0.625/texSize.x, 1.0/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z + 0.5), FORWARD, glm::vec2(0.375/texSize.x, 1.0/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z + 0.5), FORWARD, glm::vec2(0.625/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z + 0.5), FORWARD, glm::vec2(0.375/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    triangles.push_back(Triangle(i, i+1, i+2));
    triangles.push_back(Triangle(i+1, i+3, i+2));
}

void ProceduralVoxelTerrain::CreateBackSquare(int x, int y, int z) {
    int i = vertices.size();
    int type = getData(x, y, z);
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z - 0.5), BACKWARDS, glm::vec2(0.625/texSize.x, 0.25/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z - 0.5), BACKWARDS, glm::vec2(0.375/texSize.x, 0.25/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z - 0.5), BACKWARDS, glm::vec2(0.625/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z - 0.5), BACKWARDS, glm::vec2(0.375/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    triangles.push_back(Triangle(i, i+2, i+1));
    triangles.push_back(Triangle(i+1, i+2, i+3));
}

void ProceduralVoxelTerrain::ResetMesh() {
    vertices.clear();
    triangles.clear();
    for (int x = 1; x < resolution-1; x++) {
        for (int y = 1; y < resolution-1; y++) {
            for (int z = 1; z < resolution-1; z++) {
                if (getData(x, y, z) != 0) {
                    if (getData(x, y+1, z) == 0) CreateTopSquare(x, y, z);
                    if (getData(x, y-1, z) == 0) CreateBottomSquare(x, y, z);
                    if (getData(x+1, y, z) == 0) CreateRightSquare(x, y, z);
                    if (getData(x-1, y, z) == 0) CreateLeftSquare(x, y, z);
                    if (getData(x, y, z+1) == 0) CreateFrontSquare(x, y, z);
                    if (getData(x, y, z-1) == 0) CreateBackSquare(x, y, z);
                }
            }
        }
    }
}

void ProceduralVoxelTerrain::printSlice(int z){
    for(int y = resX-1; y >= 0; y --){
        for(int x = 0; x < resX; x ++){
            std::cout << getData(x, y, z);
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void ProceduralVoxelTerrain::InitMesh(){
    InitData();
    //printSlice(6);
    ResetMesh();

    shape = new VoxelShape();
    shape->VoxelShape::InitMeshFromTerrain(resolution, resolution, resolution, chunkData);
    collision->SetShape(shape);
    collision->name = "collision mesh for position ("+std::to_string(getLocalPosition().x)+","+std::to_string(getLocalPosition().z)+")";
}
