#include "pbrmaterial.h"
#include "texture.h"

PBRMaterial::PBRMaterial()
{
    shadingModel = ShadingModel::PBR;
    albedo = glm::vec3(1.0f);
    metallic = 0.0f;
    roughness = 0.5f;
    ao = 1.0f;
}

PBRMaterial::PBRMaterial(glm::vec3 alb, float met, float rough, float ambientOcclusion)
    :Material(alb)
{
    shadingModel = ShadingModel::PBR;
    metallic = met;
    roughness = rough;
    ao = ambientOcclusion;
}

PBRMaterial::PBRMaterial(Shader* sha, glm::vec3 alb, float met, float rough, float ambientOcclusion)
    : Material(sha, alb)
{
    shadingModel = ShadingModel::PBR;
    metallic = met;
    roughness = rough;
    ao = ambientOcclusion;
}

void PBRMaterial::bind() const {
    if (!shader) return;

    shader->use();
    shader->setInt("shadingModel", static_cast<int>(shadingModel));

    shader->setInt("has_albedoMap", 0);
    shader->setInt("has_normalMap", 0);
    shader->setInt("has_metallicMap", 0);
    shader->setInt("has_roughnessMap", 0);
    shader->setInt("has_aoMap", 0);

    int i = 0;
    for (const auto& [name, texture] : texmap) {
        texture.bind(i);
        shader->setInt(name, i);
        shader->setInt("has_" + name, 1);
        i++;
    }

    shader->setVec3("pbrMaterial.albedo", albedo);
    shader->setFloat("pbrMaterial.metallic", metallic);
    shader->setFloat("pbrMaterial.roughness", roughness);
    shader->setFloat("pbrMaterial.ao", ao);
}
