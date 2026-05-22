#pragma once

#include "material.h"
#include <glm/glm.hpp>

class TerrainMaterial : public Material
{
public:
    TerrainMaterial();
    TerrainMaterial(glm::vec3 albedo, float amb = 0.1f, float dif = 0.6f, float spec = 0.5f, float shin = 1) : Material(albedo, amb, dif, spec, shin){}
    void bind() const override;
    float frequency = 1.0f;
    float maxHeight = 1.0f;
};
