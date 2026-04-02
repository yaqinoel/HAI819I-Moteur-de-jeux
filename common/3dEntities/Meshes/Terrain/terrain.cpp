#include "terrain.h"

Terrain::Terrain()
{

}

void Terrain::InitMesh(int posX, int posY, int resX, int resY, float sizeX, float sizeY, float sizeZ){
    this->resX = resX;
    this->resY = resY;
    this->sizeX = sizeX;
    this->sizeY = sizeY;
    this->sizeZ = sizeZ;
    setLocalPosition(glm::vec3(posX, getLocalPosition().y, posY));
}
