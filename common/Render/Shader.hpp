#ifndef MOTEUR_SHADER_HPP
#define MOTEUR_SHADER_HPP

#pragma once
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

#include "GL/glew.h"

class Shader {
public:
    GLuint m_ID;

    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void use() const;

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    mutable std::unordered_map<std::string, GLint> locationCache;
    GLint getUniformLocation(const std::string &name) const;

    void checkCompileErrors(GLuint shader, std::string type);
};


#endif //MOTEUR_SHADER_HPP