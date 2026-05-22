#include "material.h"
#include "texture.h"


Material::Material()
{
    albedo = glm::vec3(1.0f);
    ambient = 0.1f;
    diffuse = 0.6f;
    specular = 0.5f;
    shininess = 1.0f;
}

Material::Material(glm::vec3 alb, float amb, float dif, float spec, float shin){
    albedo = alb;
    ambient = amb;
    diffuse = dif;
    specular = spec;
    shininess = shin;
}

Material::Material(Shader* sha, glm::vec3 alb, float amb, float dif, float spec, float shin){
    shader = sha;
    albedo = alb;
    ambient = amb;
    diffuse = dif;
    specular = spec;
    shininess = shin;
}

void Material::addTexture(const std::string & name, const Texture & texture){
    texmap.insert({name, texture});
}

void Material::setLit(int l) {
    lit = l;
    if (lit == 0) {
        shadingModel = ShadingModel::Unlit;
    } else if (shadingModel == ShadingModel::Unlit) {
        shadingModel = ShadingModel::BlinnPhong;
    }
}

bool Material::isPBR() const {
    return shadingModel == ShadingModel::PBR;
}

void Material::bind() const {
    if (!shader) return;
    shader->use();
    shader->setInt("hasTexture", 0);
    shader->setInt("shadingModel", static_cast<int>(shadingModel));

    int i = 0;
    for (const auto& [name, texture] : texmap) {
        texture.bind(i);
        shader->setInt(name, i);
        i++;
        shader->setInt("hasTexture", 1);
    }

    shader->setVec3("material.albedo", albedo);
    shader->setFloat("material.ambient", ambient);
    shader->setFloat("material.diffuse", diffuse);
    shader->setFloat("material.specular", specular);
    shader->setFloat("material.shininess", shininess);
    shader->setFloat("scale", scale);
    shader->setInt("lit", lit);
}
