#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct Character
{
    GLuint TextureID;

    glm::ivec2 Size;
    glm::ivec2 Bearing;

    GLuint Advance;
};

class TextRenderer
{
public:
    TextRenderer(const std::string& text, glm::vec2 position, float scale);
    ~TextRenderer();

    void Draw();

    std::string text;

    glm::vec2 position;

    float scale;

private:
    GLuint VAO;
    GLuint VBO;

    GLuint shaderProgram;

    void SetupShader();
    void SetupMesh();
};
