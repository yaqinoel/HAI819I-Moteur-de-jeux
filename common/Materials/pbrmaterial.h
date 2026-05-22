#ifndef PBRMATERIAL_H
#define PBRMATERIAL_H

#pragma once
#include "material.h"

class PBRMaterial : public Material
{
public:
    PBRMaterial();
    PBRMaterial(glm::vec3 albedo, float metallic = 0.0f, float roughness = 0.5f, float ao = 1.0f);
    PBRMaterial(Shader* sha, glm::vec3 albedo, float metallic = 0.0f, float roughness = 0.5f, float ao = 1.0f);

    float metallic;
    float roughness;
    float ao;

    virtual void bind() const override;
};

#endif // PBRMATERIAL_H
