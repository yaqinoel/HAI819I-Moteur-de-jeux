#include "proceduralterrain.h"
#include "common/Utilities/PerlinNoise.h"
#include <glm/gtc/noise.hpp>


ProceduralTerrain::ProceduralTerrain(){
}

ProceduralTerrain::ProceduralTerrain(int posX, int posY , int resX, int resY, float sizeX , float sizeY, float sizeZ, float frequency) : ProceduralTerrain(){
    InitMesh(posX, posY, resX, resY, sizeX, sizeY, sizeZ, frequency);
}


void ProceduralTerrain::InitMesh(int posX, int posY , int resX, int resY, float sizeX , float sizeY, float sizeZ, float frequency){
    glm::vec3 center = glm::vec3(sizeX/2.0, 0.0f, sizeY/2.0);
    this->resX = resX;
    this->resY = resY;
    this->sizeX = sizeX;
    this->sizeY = sizeY;
    this->sizeZ = sizeZ;
    this->frequency = frequency;
    position.x = posX;
    position.z = posY;
    vertices = std::vector<Vertex>() ;
    triangles = std::vector<Triangle>();
    for(int i = 0; i < resX+1; i ++){
        for(int j = 0; j < resY+1; j++){

            float x = i * (sizeX/resX);
            float y = j * (sizeY/resY);

            float worldX = x + posX;
            float worldY = y + posY;

            float noise = glm::perlin(glm::vec2(worldX, worldY) * frequency);
            noise = (noise + 1.0f) * 0.5f;
            float height = noise * sizeZ;
            Vertex v( glm::vec3(x, height, y) - center, glm::vec2(x, y) );
            vertices.push_back(v);
            verticesPosition.push_back(v.position);
            if(i > 0 && j > 0){
                int i0 = (i-1)*(resY+1)+j-1;
                int i1 = (i-1)*(resY+1)+j;
                int i2 = (i)*(resY+1)+j;
                int i3 = (i)*(resY+1)+j-1;
                triangles.push_back(Triangle(i0, i1, i2));
                triangles.push_back(Triangle(i2, i3, i0));
            }
        }
    }
    recomputeSmoothVertexNormals(1);

    _synchronized = false;
}

void ProceduralTerrain::ApplyHeightMap(std::string hmapName, float scale){
    this->hmapName = hmapName;
    Texture hmap = Texture(hmapName);
    for(int i = 0; i < vertices.size(); i ++){
        Color c = hmap.getPixelSafe(vertices[i].position[0]/sizeX, vertices[i].position[2]/sizeY);
        vertices[i].position[1] = ((float)c.g/255.0) * scale;
    }
    recomputeSmoothVertexNormals(1);
}
