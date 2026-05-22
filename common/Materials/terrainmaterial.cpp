#include "terrainmaterial.h"

TerrainMaterial::TerrainMaterial() {}

void TerrainMaterial::bind() const {
    Material::bind();
    if (!shader) return;
    shader->setFloat("frequency", frequency);
    shader->setFloat("maxHeight", maxHeight);
}
