#ifndef MATERIAL_H
#define MATERIAL_H


#pragma once
#include "texture.h"
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Render/Shader.hpp"

enum class ShadingModel
{
    Unlit,
    BlinnPhong,
    PBR
};

class Material
{
public:
    Material();
    Material(glm::vec3 albedo, float amb = 0.1f, float dif = 0.6f, float spec = 0.5f, float shin = 1);
    Material(Shader* sha, glm::vec3 albedo, float amb = 0.1f, float dif = 0.6f, float spec = 0.5f, float shin = 1);
    Shader* shader = nullptr;
    ShadingModel shadingModel = ShadingModel::BlinnPhong;
    glm::vec3 albedo;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
    float scale = 1.0f;
    int lit = 1;
    std::map<std::string, Texture> texmap;

    void addTexture(const std::string & name, const Texture & texture);
    virtual void bind() const;
    void setLit(int lit);
    bool isPBR() const;
};

#endif // MATERIAL_H
