#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <common/Materials/texture.h>
#include <common/Controls/charactercontroller.h>

struct gui_element{
    gui_element(const std::string& texturePath, glm::vec2 size, bool visible){
        this->visible = visible;
        m_size = size;
        texture = new Texture(texturePath, true);
        SetupMesh();
        SetupShader();
    }

    ~gui_element(){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
    bool visible;
    glm::vec2 m_size;
    Texture *texture;
    void Draw();
    void SetupMesh();
    void SetupShader();
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    GLuint shaderProgram;
    GLuint CompileShader(GLenum type, const char* source);
    std::vector<gui_element*> sub_elements = std::vector<gui_element*>();
};

class GUI
{
public:
    GUI(CharacterController *player);
    ~GUI();

    void Draw();
    CharacterController *player;

private:
    std::vector<gui_element*> elements = std::vector<gui_element*>();
    GLFWwindow *window;

};
