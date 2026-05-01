#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char* vertexPath, const char* fragmentPath){
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try{
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }catch(std::ifstream::failure e){
        std::cout << "ERROR::SHADER::FILE NOT SUCCESFULLY READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex,1,&vShaderCode,NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment,1,&fShaderCode,NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    m_ID = glCreateProgram();
    glAttachShader(m_ID,vertex);
    glAttachShader(m_ID,fragment);
    glLinkProgram(m_ID);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    checkCompileErrors(m_ID, "PROGRAM");
}

void Shader::use() const { 
    glUseProgram(m_ID);
}

GLint Shader::getUniformLocation(const std::string &name) const {
    if (locationCache.find(name) != locationCache.end())
        return locationCache[name];

    GLint location = glGetUniformLocation(m_ID, name.c_str());
    locationCache[name] = location;
    return location;
}

void Shader:: setBool(const std::string &name,bool value) const{
    glUniform1i(getUniformLocation(name), (int)value);
}

void Shader:: setInt(const std::string &name,int value) const{
    glUniform1i(getUniformLocation(name), value);
}

void Shader:: setFloat(const std::string &name,float value) const{
    glUniform1f(getUniformLocation(name), value);
}

void Shader:: setMat4(const std::string &name, const glm::mat4 &mat) const{
    glUniformMatrix4fv(getUniformLocation(name),1,GL_FALSE, &mat[0][0]);
}

void Shader:: setVec3(const std::string &name, const glm::vec3 &vec) const{
    glUniform3fv(getUniformLocation(name), 1, &vec[0]);
}

void Shader::checkCompileErrors(GLuint shader, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

Shader::~Shader() {
    glDeleteProgram(m_ID);
}