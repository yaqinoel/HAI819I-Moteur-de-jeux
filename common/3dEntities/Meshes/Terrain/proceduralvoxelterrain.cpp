#include "proceduralvoxelterrain.h"

ProceduralVoxelTerrain::ProceduralVoxelTerrain()
{
    collision = new CollisionShape3D();
    instantiate(collision, this);
    texSize = glm::ivec2(2, 1);
    texCoord.insert({1, glm::vec2(0, 0)});
    texCoord.insert({2, glm::vec2(1, 0)});
}
ProceduralVoxelTerrain::ProceduralVoxelTerrain(int posX, int posY, int resX, int resY, float sizeX, float sizeY, float sizeZ) : ProceduralVoxelTerrain()
{
    InitMesh(posX, posY, resX, resY, sizeX, sizeY, sizeZ);
}

void ProceduralVoxelTerrain::InitData(glm::ivec3 pos, int res, int size) {
    frequency = 1.0 / size;
    chunkData.resize(res+2, std::vector<std::vector<unsigned short int>>(res+2, std::vector<unsigned short int>(res+2)));
    for (int x = 0; x < res+2; x++) {
        for (int z = 0; z < res+2; z++) {
            float worldX = x-1 + pos.x;
            float worldZ = z-1 + pos.z;
            int height = roundf(fabs(glm::perlin(glm::vec2(worldX, worldZ) * frequency)) * size);
            for (int y = 0; y < res+2; y++) {
                chunkData[x][y][z] = 0;
                if(height >= y-1){
                    if((height >= y)){
                        chunkData[x][y][z] = 2;
                    }
                    else{
                        chunkData[x][y][z] = 1;
                    }
                }
            }
        }
    }
}

void ProceduralVoxelTerrain::CreateTopSquare(int x, int y, int z) {
    int i = vertices.size();
    int type = chunkData[x][y][z];
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z - 0.5), UP, glm::vec2(0.625/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z + 0.5), UP, glm::vec2(0.625/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z - 0.5), UP, glm::vec2(0.875/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z + 0.5), UP, glm::vec2(0.875/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    triangles.push_back(Triangle(i + 2, i + 1, i));
    triangles.push_back(Triangle(i + 2, i + 3 , i+1));
}
void ProceduralVoxelTerrain::CreateBottomSquare(int x, int y, int z) {
    int i = vertices.size();
    int type = chunkData[x][y][z];
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z - 0.5), DOWN, glm::vec2(0.375/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z + 0.5), DOWN, glm::vec2(0.375/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z - 0.5), DOWN, glm::vec2(0.125/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z + 0.5), DOWN, glm::vec2(0.125/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    triangles.push_back(Triangle(i, i + 1, i+2));
    triangles.push_back(Triangle(i, i + 2, i + 3));
}
void ProceduralVoxelTerrain::CreateRightSquare(int x, int y, int z) {
    int i = vertices.size();
    int type = chunkData[x][y][z];
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z - 0.5), RIGHT, glm::vec2(0.625/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z + 0.5), RIGHT, glm::vec2(0.625/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z + 0.5), RIGHT, glm::vec2(0.375/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z - 0.5), RIGHT, glm::vec2(0.375/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    triangles.push_back(Triangle(i, i + 1, i+2));
    triangles.push_back(Triangle(i, i + 2, i + 3));
}
void ProceduralVoxelTerrain::CreateLeftSquare(int x, int y, int z) {
    int i = vertices.size();
    int type = chunkData[x][y][z];
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z - 0.5), LEFT, glm::vec2(0.625/texSize.x, 0.25/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z + 0.5), LEFT, glm::vec2(0.625/texSize.x, 0.0/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z + 0.5), LEFT, glm::vec2(0.375/texSize.x, 0.0/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z - 0.5), LEFT, glm::vec2(0.375/texSize.x, 0.25/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    triangles.push_back(Triangle(i, i + 1, i+2));
    triangles.push_back(Triangle(i, i + 2, i + 3));
}

void ProceduralVoxelTerrain::CreateFrontSquare(int x, int y, int z) {
    int i = vertices.size();
    int type = chunkData[x][y][z];
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z + 0.5), FORWARD, glm::vec2(0.625/texSize.x, 1.0/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z + 0.5), FORWARD, glm::vec2(0.375/texSize.x, 1.0/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z + 0.5), FORWARD, glm::vec2(0.625/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z + 0.5), FORWARD, glm::vec2(0.375/texSize.x, 0.75/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    triangles.push_back(Triangle(i, i + 2, i + 1));
    triangles.push_back(Triangle(i + 1, i + 2, i + 3));
}

void ProceduralVoxelTerrain::CreateBackSquare(int x, int y, int z) {
    int i = vertices.size();
    int type = chunkData[x][y][z];
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z - 0.5), BACKWARDS, glm::vec2(0.625/texSize.x, 0.25/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z - 0.5), BACKWARDS, glm::vec2(0.375/texSize.x, 0.25/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z - 0.5), BACKWARDS, glm::vec2(0.625/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z - 0.5), BACKWARDS, glm::vec2(0.375/texSize.x, 0.5/texSize.y)+glm::vec2(texCoord[type].x/(float)texSize.x,texCoord[type].y/(float)texSize.y)));
    triangles.push_back(Triangle(i, i + 1, i+2));
    triangles.push_back(Triangle(i +1, i + 3, i + 2));
}

void ProceduralVoxelTerrain::InitMesh() {
    glm::vec3 center = glm::vec3(sizeX / 2.0, 0.0f, sizeY / 2.0);
    int res = resX;
    for (int x = 1; x < res+1; x++) {
        for (int z = 1; z < res+1; z++) {
            for (int y = 1; y < res+1; y++) {
                if (chunkData[x][y][z] != 0) {
                    if (chunkData[x][y + 1][z] == 0) CreateTopSquare(x, y, z);
                    if (chunkData[x][y - 1][z] == 0) CreateBottomSquare(x, y, z);
                    if (chunkData[x + 1][y][z] == 0) CreateRightSquare(x, y, z);
                    if (chunkData[x - 1][y][z] == 0) CreateLeftSquare(x, y, z);
                    if (chunkData[x][y][z + 1] == 0) CreateFrontSquare(x, y, z);
                    if (chunkData[x][y][z - 1] == 0) CreateBackSquare(x, y, z);
                }
            }
        }
    }
}

void ProceduralVoxelTerrain::printSlice(int z){
    for(int y = resX-1; y >= 0; y --){
        for(int x = 0; x < resX; x ++){
            std::cout << chunkData[x][y][z];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void ProceduralVoxelTerrain::InitMesh(int posX, int posY, int resX, int resY, float sizeX, float sizeY, float sizeZ){
    Terrain::InitMesh(posX, posY, resX, resY, sizeX, sizeY, sizeZ);
    InitData(glm::vec3(posX, 0, posY), resX, sizeX);
    //printSlice(6);
    InitMesh();

    TerrainShape* shape = new TerrainShape();
    shape->ConvexShape::InitMesh(vertices, triangles);
    collision->SetShape(shape);
}
