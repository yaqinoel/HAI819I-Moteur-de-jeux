#ifndef MATERIAL_H
#define MATERIAL_H


#pragma once
#include "texture.h"
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/Utilities/Shader.hpp"

class Material
{
public:
    Material();
    Material(glm::vec3 albedo, float amb = 0.1f, float dif = 0.6f, float spec = 0.5f, float shin = 1);
    Material(Shader* sha, glm::vec3 albedo, float amb = 0.1f, float dif = 0.6f, float spec = 0.5f, float shin = 1);
    Shader* shader;
    glm::vec3 albedo;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
    float scale = 1.0f;
    int lit = 1;
    std::map<std::string, Texture> texmap;

    void addTexture(const std::string & name, const Texture & texture);
    virtual void render(GLuint shaderPID) const;
    virtual void bind() const;
    virtual void setShader(GLuint shaderPID) const;
    void setLit(int lit);
private:
    mutable GLuint albedoUniform;
    mutable GLuint ambientUniform;
    mutable GLuint diffuseUniform;
    mutable GLuint specularUniform;
    mutable GLuint shininessUniform;
    mutable GLuint scaleUniform;
    mutable GLuint litUniform;
    mutable bool shaderSet = false;
};

#endif // MATERIAL_H
