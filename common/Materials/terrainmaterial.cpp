#include "terrainmaterial.h"

TerrainMaterial::TerrainMaterial() {}

void TerrainMaterial::setShader(GLuint shaderPID) const{
    Material::setShader(shaderPID);
    frequencyUniform = glGetUniformLocation(shaderPID, "frequency");
    maxHeightUniform = glGetUniformLocation(shaderPID, "maxHeight");
}

void TerrainMaterial::render(GLuint shaderPID) const{
    Material::render(shaderPID);
    glUniform1f(frequencyUniform, frequency);
    glUniform1f(maxHeightUniform, maxHeight);

}

