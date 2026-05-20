#include "proceduralterrain.h"
#include "common/Utilities/PerlinNoise.h"
#include <glm/gtc/noise.hpp>


ProceduralTerrain::ProceduralTerrain(){
    collision = new CollisionShape3D();
    instantiate(collision, this);
}

ProceduralTerrain::ProceduralTerrain(int posX, int posY , int resX, int resY, float sizeX , float sizeY, float sizeZ) : ProceduralTerrain(){
    InitMesh(posX, posY, resX, resY, sizeX, sizeY, sizeZ);
}


void ProceduralTerrain::InitMesh(int posX, int posY , int resX, int resY, float sizeX , float sizeY, float sizeZ){
    Terrain::InitMesh(posX, posY, resX, resY, sizeX, sizeY, sizeZ);
    glm::vec3 center = glm::vec3(sizeX/2.0, 0.0f, sizeY/2.0);
    frequency = 1/sizeZ;
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
            if(i > 0 && j > 0){//you only add triangles when you're not on an edge, but you do add vertices on the edges
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

    TerrainShape* shape = new TerrainShape();
    shape->MeshShape::InitMesh(vertices, triangles);
    collision->setShape(shape);

    _synchronized = false;
}
