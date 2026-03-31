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
    frequency = 1 / size;

    chunkData.resize(res);
    for (int x = 0; x < res; x++) {
        chunkData[x].resize(res);
        for (int z = 0; z < res; z++) {
            chunkData[x][z].resize(res);
            float worldX = x + pos.x;
            float worldZ = z + posY;
            int height = roundf(fabs(glm::perlin(glm::vec2(worldX, worldZ) * frequency)) * size);
            for (int y = 0; y < res; y++) {
                chunkData[x][y][z] = height <= y;
            }
        }
    }
}

void CreateTopSquare(int x, int y, int z) {
    int i = mesh->vertices.size();
    mesh->vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z - 0.5), UP, glm::vec2(0.625, 0.5)));
    mesh->vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z + 0.5), UP, glm::vec2(0.625, 0.75)));
    mesh->vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z - 0.5), UP, glm::vec2(0.875, 0.5)));
    mesh->vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z + 0.5), UP, glm::vec2(0.875, 0.75)));
    mesh->triangles.push_back(Triangle(i + 2, i + 1, i));
    mesh->triangles.push_back(Triangle(i + 2, i + 3 , i+1));
}
void CreateBottomSquare(int x, int y, int z) {
    int i = mesh->vertices.size();
    mesh->vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z - 0.5), DOWN, glm::vec2(0.375, 0.5)));
    mesh->vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z + 0.5), DOWN, glm::vec2(0.375, 0.75)));
    mesh->vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z - 0.5), DOWN, glm::vec2(0.125, 0.5)));
    mesh->vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z + 0.5), DOWN, glm::vec2(0.125, 0.75)));
    mesh->triangles.push_back(Triangle(i, i + 3, i+2));
    mesh->triangles.push_back(Triangle(i, i + 1, i + 3));
}
void CreateRightSquare(int x, int y, int z) {
    int i = mesh->vertices.size();
    mesh->vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z - 0.5), RIGHT, glm::vec2(0.625, 0.5)));
    mesh->vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z + 0.5), RIGHT, glm::vec2(0.625, 0.75)));
    mesh->vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z + 0.5), RIGHT, glm::vec2(0.375, 0.75)));
    mesh->vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z - 0.5), RIGHT, glm::vec2(0.375, 0.5)));
    mesh->triangles.push_back(Triangle(i, i + 3, i + 2));
    mesh->triangles.push_back(Triangle(i, i + 1, i + 3));
}
void CreateLeftSquare(int x, int y, int z) {
    int i = mesh->vertices.size();
    mesh->vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z - 0.5), LEFT, glm::vec2(0.625, 0.25)));
    mesh->vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z + 0.5), LEFT, glm::vec2(0.625, 0.0)));
    mesh->vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z + 0.5), LEFT, glm::vec2(0.375, 0.0)));
    mesh->vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z - 0.5), LEFT, glm::vec2(0.375, 0.25)));
    mesh->triangles.push_back(Triangle(i, i + 3, i + 2));
    mesh->triangles.push_back(Triangle(i, i + 1, i + 3));
}

void CreateFrontSquare(int x, int y, int z) {
    int i = mesh->vertices.size();
    mesh->vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z + 0.5), FORWARD, glm::vec2(0.625, 0.75)));
    mesh->vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z + 0.5), FORWARD, glm::vec2(0.875, 0.75)));
    mesh->vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z + 0.5), FORWARD, glm::vec2(0.875, 1.0)));
    mesh->vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z + 0.5), FORWARD, glm::vec2(0.625, 1.0)));
    mesh->triangles.push_back(Triangle(i, i + 3, i + 2));
    mesh->triangles.push_back(Triangle(i, i + 1, i + 3));
}

void CreateBackSquare(int x, int y, int z) {
    int i = mesh->vertices.size();
    mesh->vertices.push_back(Vertex(glm::vec3(x + 0.5, y + 0.5, z - 0.5), BACKWARDS, glm::vec2(0.625, 0.25)));
    mesh->vertices.push_back(Vertex(glm::vec3(x - 0.5, y + 0.5, z - 0.5), BACKWARDS, glm::vec2(0.875, 0.25)));
    mesh->vertices.push_back(Vertex(glm::vec3(x - 0.5, y - 0.5, z - 0.5), BACKWARDS, glm::vec2(0.875, 0.5)));
    mesh->vertices.push_back(Vertex(glm::vec3(x + 0.5, y - 0.5, z - 0.5), BACKWARDS, glm::vec2(0.625, 0.5)));
    mesh->triangles.push_back(Triangle(i, i + 2, i + 3));
    mesh->triangles.push_back(Triangle(i, i + 3, i + 1));
}

void ProceduralVoxelTerrain::InitMesh() {
    glm::vec3 center = glm::vec3(sizeX / 2.0, 0.0f, sizeY / 2.0);
    int res = resX;
    mesh = new Mesh();
    for (int x = 0; x < res; x++) {
        for (int z = 0; z < res; z++) {
            for (int y = 0; y < res; y++) {
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

void ProceduralVoxelTerrain::InitMesh(int posX, int posY, int resX, int resY, float sizeX, float sizeY, float sizeZ){
    Terrain::InitMesh(posX, posY, resX, resY, sizeX, sizeY, sizeZ);
    initData(glm::vec3(posX, 0, posY), resX, sizeX);
    initMesh();
    glm::vec3 center = glm::vec3(sizeX/2.0, 0.0f, sizeY/2.0);
    frequency = 1/sizeZ;

    cubes.resize(resX*resY);

    for(int i = 0; i < resX; i ++){
        for(int j = 0; j < resY; j ++){
            float x = i * (sizeX/resX);
            float y = j * (sizeY/resY);

            float worldX = x + posX;
            float worldY = y + posY;
            float noise = glm::perlin(glm::vec2(worldX, worldY) * frequency);
            noise = (noise + 1.0f) * 0.5f;
            float height = noise * sizeZ;
            glm::vec3 cubePos = glm::vec3((int)x, (int)height, (int)y) - center;
            Mesh* cube = makeCube();
            instantiate(cube, this);
            cubes[i*resY+j] = cube;
            cube->setGlobalPosition(cubePos);
        }
    }
}
