#include "pbrmaterial.h"
#include "texture.h"

PBRMaterial::PBRMaterial()
{
    albedo = glm::vec3(1.0f);
    metallic = 0.0f;
    roughness = 0.5f;
    ao = 1.0f;
}

PBRMaterial::PBRMaterial(glm::vec3 alb, float met, float rough, float ambientOcclusion){
    albedo = alb;
    metallic = met;
    roughness = rough;
    ao = ambientOcclusion;
}

void PBRMaterial::setShader(GLuint shaderPID) const{
    albedoUniform = glGetUniformLocation(shaderPID, "pbrMaterial.albedo");
    metallicUniform = glGetUniformLocation(shaderPID, "pbrMaterial.metallic");
    roughnessUniform = glGetUniformLocation(shaderPID, "pbrMaterial.roughness");
    aoUniform = glGetUniformLocation(shaderPID, "pbrMaterial.ao");
    scaleUniform = glGetUniformLocation(shaderPID, "scale");
    litUniform = glGetUniformLocation(shaderPID, "lit");
    shaderSet = true;
}

void PBRMaterial::render(GLuint shaderPID) const{
    int i = 0;
    for(const auto&[name, texture]:texmap){
        texture.bind(i);
        GLuint textureUniformLocation = glGetUniformLocation(shaderPID, name.c_str());
        glUniform1i(textureUniformLocation, i);
        i++;
        std::string hasName = "has_" + name;
        GLuint hasTexture = glGetUniformLocation(shaderPID, hasName.c_str());
        glUniform1i(hasTexture, 1);
    }
    if(!shaderSet){
        setShader(shaderPID);
    }
    
    glUniform1i(litUniform, lit);
    glUniform3fv(albedoUniform, 1, glm::value_ptr(albedo));
    glUniform1f(metallicUniform, metallic);
    glUniform1f(roughnessUniform, roughness);
    glUniform1f(aoUniform, ao);
    glUniform1f(scaleUniform, scale);
}
