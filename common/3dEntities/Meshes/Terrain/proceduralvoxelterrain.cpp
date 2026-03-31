#include "proceduralvoxelterrain.h"
#include "../../../../Scenes/Cube.h"

ProceduralVoxelTerrain::ProceduralVoxelTerrain()
{

}
ProceduralVoxelTerrain::ProceduralVoxelTerrain(int posX, int posY, int resX, int resY, float sizeX, float sizeY, float sizeZ)
{
    InitMesh(posX, posY, resX, resY, sizeX, sizeY, sizeZ);
}

void ProceduralVoxelTerrain::InitMesh(int posX, int posY, int resX, int resY, float sizeX, float sizeY, float sizeZ){
    Terrain::InitMesh(posX, posY, resX, resY, sizeX, sizeY, sizeZ);
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
