#ifndef MATERIAL_H
#define MATERIAL_H


#pragma once
#include "texture.h"
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Material
{
public:
    Material();
    Material(glm::vec3 albedo, float amb = 0.1f, float dif = 0.6f, float spec = 0.5f, float shin = 1);
    glm::vec3 albedo;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
    int lit = 1;
    std::map<std::string, Texture> texmap;



    void addTexture(const std::string & name, const Texture & texture);
    void render(GLuint shaderPID) const;
    void setShader(GLuint shaderPID) const;
    void setLit(int lit);
private:
    mutable GLuint albedoUniform;
    mutable GLuint ambientUniform;
    mutable GLuint diffuseUniform;
    mutable GLuint specularUniform;
    mutable GLuint shininessUniform;
    mutable GLuint litUniform;
    mutable bool shaderSet = false;
};

#endif // MATERIAL_H
