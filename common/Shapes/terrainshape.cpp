#include "terrainshape.h"

TerrainShape::TerrainShape()
{

}


void TerrainShape::InitMesh(int posX, int posY , int resX, int resY, float sizeX , float sizeY, float sizeZ, float frequency){
    glm::vec3 center = glm::vec3(sizeX/2.0, 0.0f, sizeY/2.0);
    vertices = std::vector<glm::vec3>() ;
    triangles = std::vector<glm::ivec3>();
    for(int i = 0; i < resX+1; i ++){
        for(int j = 0; j < resY+1; j++){

            float x = i * (sizeX/resX);
            float y = j * (sizeY/resY);

            float worldX = x + posX;
            float worldY = y + posY;

            float noise = glm::perlin(glm::vec2(worldX, worldY) * frequency);
            noise = (noise + 1.0f) * 0.5f;
            float height = noise * sizeZ;
            glm::vec3 v = glm::vec3(x, height, y) - center;
            if(radius*radius < glm::dot(v, v)){
                radius = glm::length(v);
            }

            vertices.push_back(v);
            if(i > 0 && j > 0){
                int i0 = (i-1)*(resY+1)+j-1;
                int i1 = (i-1)*(resY+1)+j;
                int i2 = (i)*(resY+1)+j;
                int i3 = (i)*(resY+1)+j-1;
                triangles.push_back(glm::ivec3(i0, i1, i2));
                triangles.push_back(glm::ivec3(i2, i3, i0));
            }
        }
    }
    inertia = glm::mat3(INFINITY);
}
