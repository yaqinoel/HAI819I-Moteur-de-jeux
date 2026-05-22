#include "textrenderer.h"
#include "GLFW/glfw3.h"

#include <iostream>

#define STB_EASY_FONT_IMPLEMENTATION
#include "stb_easy_font.h"

TextRenderer::TextRenderer(const std::string& text,
                           glm::vec2 position,
                           float scale)
{
    this->text = text;
    this->position = position;
    this->scale = scale;

    SetupShader();
    SetupMesh();
}

TextRenderer::~TextRenderer()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glDeleteProgram(shaderProgram);
}

void TextRenderer::SetupShader()
{
    const char* vertexShaderSource = R"(
        #version 330 core

        layout(location = 0) in vec2 aPos;

        uniform vec2 screenSize;

        void main()
        {
            vec2 pos = aPos / screenSize * 2.0 - 1.0;

            pos.y = -pos.y;

            gl_Position = vec4(pos, 0.0, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core

        out vec4 FragColor;

        uniform vec3 textColor;

        void main()
        {
            FragColor = vec4(textColor, 1.0);
        }
    )";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vs,
                   1,
                   &vertexShaderSource,
                   nullptr);

    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fs,
                   1,
                   &fragmentShaderSource,
                   nullptr);

    glCompileShader(fs);

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);

    glLinkProgram(shaderProgram);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

void TextRenderer::SetupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}

void TextRenderer::Draw()
{
    GLFWwindow* window = glfwGetCurrentContext();

    int width, height;

    glfwGetFramebufferSize(window,
                           &width,
                           &height);

    char buffer[99999];

    int num_quads = stb_easy_font_print(
            position.x,
            position.y,
            const_cast<char*>(text.c_str()),
            nullptr,
            buffer,
            sizeof(buffer)
            );

    glDisable(GL_DEPTH_TEST);

    glUseProgram(shaderProgram);

    glUniform2f(
        glGetUniformLocation(shaderProgram,
                             "screenSize"),
        (float)width,
        (float)height
        );

    glUniform3f(
        glGetUniformLocation(shaderProgram,
                             "textColor"),
        1.0f,
        1.0f,
        1.0f
        );

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER,
                 num_quads * 4 * 16,
                 buffer,
                 GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        16,
        (void*)0
        );

    glDrawArrays(GL_QUADS,
                 0,
                 num_quads * 4);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}
