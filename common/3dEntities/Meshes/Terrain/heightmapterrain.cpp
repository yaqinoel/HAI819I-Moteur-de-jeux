#include "heightmapterrain.h"

HeightMapTerrain::HeightMapTerrain()
{
    collision = new Collider3D();
    instantiate(collision, this);
}

HeightMapTerrain::HeightMapTerrain(int posX, int posY , int resX, int resY, float sizeX , float sizeY, float sizeZ) : HeightMapTerrain(){
    InitMesh(posX, posY, resX, resY, sizeX, sizeY, sizeZ);
}


void HeightMapTerrain::InitMesh(int posX, int posY , int resX, int resY, float sizeX , float sizeY, float sizeZ){
    Terrain::InitMesh(posX, posY, resX, resY, sizeX, sizeY, sizeZ);
    glm::vec3 center = glm::vec3(sizeX/2.0, 0.0f, sizeY/2.0);
    vertices = std::vector<Vertex>() ;
    triangles = std::vector<Triangle>();
    for(int i = 0; i < resX+1; i ++){
        for(int j = 0; j < resY+1; j++){

            float x = i * (sizeX/resX);
            float y = j * (sizeY/resY);

            float worldX = x + posX;
            float worldY = y + posY;
            Vertex v( glm::vec3(x, 0, y) - center, glm::vec2(x, y) );
            vertices.push_back(v);
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

    TerrainShape* shape = new TerrainShape();
    shape->MeshShape::InitMesh(vertices, triangles);
    collision->setShape(shape);

    _synchronized = false;
}


void HeightMapTerrain::ApplyHeightMap(std::string hmapName){
    this->hmapName = hmapName;
    Texture hmap = Texture(hmapName);
    for(int i = 0; i < vertices.size(); i ++){
        Color c = hmap.getPixelSafe(vertices[i].position[0]/sizeX, vertices[i].position[2]/sizeY);
        vertices[i].position[1] = ((float)c.g/255.0) * sizeZ;
    }

    recomputeSmoothVertexNormals(1);

    TerrainShape* shape = new TerrainShape();
    shape->MeshShape::InitMesh(vertices, triangles);
    collision->setShape(shape);

    _synchronized = false;
}
