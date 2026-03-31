#include "proceduralvoxelterrain.h"
#include "../../../../Scenes/Cube.h"

ProceduralVoxelTerrain::ProceduralVoxelTerrain()
{

}
ProceduralVoxelTerrain::ProceduralVoxelTerrain(int posX, int posY, int resX, int resY, float sizeX, float sizeY, float sizeZ)
{
    InitMesh(posX, posY, resX, resY, sizeX, sizeY, sizeZ);
}

void ProceduralVoxelTerrain::InitData(glm::ivec3 pos, int res, int size) {
    frequency = 1.0 / size;
    chunkData.resize(res+2, std::vector<std::vector<unsigned short int>>(res+2, std::vector<unsigned short int>(res+2)));
    for (int x = 0; x < res+2; x++) {
        for (int z = 0; z < res+2; z++) {
            float worldX = x-1 + pos.x;
            float worldZ = z-1 + pos.y;
            int height = roundf(fabs(glm::perlin(glm::vec2(worldX, worldZ) * frequency)) * size);
            for (int y = 0; y < res+2; y++) {
                chunkData[x][y][z] = (int)(height >= y-1);
            }
        }
    }
}

void ProceduralVoxelTerrain::CreateTopSquare(int x, int y, int z) {
    int i = vertices.size();
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z - 0.5), UP, glm::vec2(0.625, 0.5)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z + 0.5), UP, glm::vec2(0.625, 0.75)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z - 0.5), UP, glm::vec2(0.875, 0.5)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z + 0.5), UP, glm::vec2(0.875, 0.75)));
    triangles.push_back(Triangle(i + 2, i + 1, i));
    triangles.push_back(Triangle(i + 2, i + 3 , i+1));
}
void ProceduralVoxelTerrain::CreateBottomSquare(int x, int y, int z) {
    int i = vertices.size();
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z - 0.5), DOWN, glm::vec2(0.375, 0.5)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z + 0.5), DOWN, glm::vec2(0.375, 0.75)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z - 0.5), DOWN, glm::vec2(0.125, 0.5)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z + 0.5), DOWN, glm::vec2(0.125, 0.75)));
    triangles.push_back(Triangle(i, i + 3, i+2));
    triangles.push_back(Triangle(i, i + 1, i + 3));
}
void ProceduralVoxelTerrain::CreateRightSquare(int x, int y, int z) {
    int i = vertices.size();
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z - 0.5), RIGHT, glm::vec2(0.625, 0.5)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z + 0.5), RIGHT, glm::vec2(0.625, 0.75)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z + 0.5), RIGHT, glm::vec2(0.375, 0.75)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z - 0.5), RIGHT, glm::vec2(0.375, 0.5)));
    triangles.push_back(Triangle(i, i + 3, i + 2));
    triangles.push_back(Triangle(i, i + 1, i + 3));
}
void ProceduralVoxelTerrain::CreateLeftSquare(int x, int y, int z) {
    int i = vertices.size();
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z - 0.5), LEFT, glm::vec2(0.625, 0.25)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z + 0.5), LEFT, glm::vec2(0.625, 0.0)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z + 0.5), LEFT, glm::vec2(0.375, 0.0)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z - 0.5), LEFT, glm::vec2(0.375, 0.25)));
    triangles.push_back(Triangle(i, i + 3, i + 2));
    triangles.push_back(Triangle(i, i + 1, i + 3));
}

void ProceduralVoxelTerrain::CreateFrontSquare(int x, int y, int z) {
    int i = vertices.size();
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z + 0.5), FORWARD, glm::vec2(0.625, 0.75)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z + 0.5), FORWARD, glm::vec2(0.875, 0.75)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z + 0.5), FORWARD, glm::vec2(0.875, 1.0)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z + 0.5), FORWARD, glm::vec2(0.625, 1.0)));
    triangles.push_back(Triangle(i, i + 3, i + 2));
    triangles.push_back(Triangle(i, i + 1, i + 3));
}

void ProceduralVoxelTerrain::CreateBackSquare(int x, int y, int z) {
    int i = vertices.size();
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z - 0.5), BACKWARDS, glm::vec2(0.625, 0.25)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z - 0.5), BACKWARDS, glm::vec2(0.875, 0.25)));
    vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z - 0.5), BACKWARDS, glm::vec2(0.875, 0.5)));
    vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z - 0.5), BACKWARDS, glm::vec2(0.625, 0.5)));
    triangles.push_back(Triangle(i, i + 2, i + 3));
    triangles.push_back(Triangle(i, i + 3, i + 1));
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
    printSlice(6);
    InitMesh();
    // glm::vec3 center = glm::vec3(sizeX/2.0, 0.0f, sizeY/2.0);
    // frequency = 1/sizeZ;

    // cubes.resize(resX*resY);

    // for(int i = 0; i < resX; i ++){
    //     for(int j = 0; j < resY; j ++){
    //         float x = i * (sizeX/resX);
    //         float y = j * (sizeY/resY);

    //         float worldX = x + posX;
    //         float worldY = y + posY;
    //         float noise = glm::perlin(glm::vec2(worldX, worldY) * frequency);
    //         noise = (noise + 1.0f) * 0.5f;
    //         float height = noise * sizeZ;
    //         glm::vec3 cubePos = glm::vec3((int)x, (int)height, (int)y) - center;
    //         Mesh* cube = makeCube();
    //         instantiate(cube, this);
    //         cubes[i*resY+j] = cube;
    //         cube->setGlobalPosition(cubePos);
    //     }
    // }
}
