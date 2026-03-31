#include "material.h"
#include "texture.h"


Material::Material()
{
}

Material::Material(glm::vec3 alb, float amb, float dif, float spec, float shin){
    albedo = alb;
    ambient = amb;
    diffuse = dif;
    specular = spec;
    shininess = shin;
}

void Material::addTexture(const std::string & name, const Texture & texture){
    texmap.insert({name, texture});
}

void Material::setShader(GLuint shaderPID) const{
    albedoUniform = glGetUniformLocation(shaderPID, "material.albedo");
    ambientUniform = glGetUniformLocation(shaderPID, "material.ambiant");
    diffuseUniform = glGetUniformLocation(shaderPID, "material.diffuse");
    specularUniform = glGetUniformLocation(shaderPID, "material.specular");
    shininessUniform = glGetUniformLocation(shaderPID, "material.shininess");
    scaleUniform = glGetUniformLocation(shaderPID, "scale");
    litUniform = glGetUniformLocation(shaderPID, "lit");
    shaderSet = true;
}


void Material::setLit(int l) {
    lit = l;
}

void Material::render(GLuint shaderPID) const{
    int i = 0;
    for(const auto&[name, texture]:texmap){
        texture.bind(i);
        GLuint textureUniformLocation = glGetUniformLocation(shaderPID, name.c_str());
        glUniform1i(textureUniformLocation, i);
        i++;
        GLuint hasTexture = glGetUniformLocation(shaderPID, "hasTexture");
        glUniform1i(hasTexture, 1);
    }
    if(!shaderSet){
        setShader(shaderPID);
    }
    glUniform1i(litUniform, lit);
    glUniform3fv(albedoUniform,1, glm::value_ptr(albedo));
    glUniform1f(ambientUniform, ambient);
    glUniform1f(diffuseUniform, diffuse);
    glUniform1f(specularUniform, specular);
    glUniform1f(shininessUniform, shininess);
    glUniform1f(scaleUniform, scale);
}
