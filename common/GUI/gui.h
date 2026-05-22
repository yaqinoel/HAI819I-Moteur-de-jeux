#pragma once

#include "textrenderer.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <common/Materials/texture.h>
#include <common/Controls/charactercontroller.h>

struct gui_element{
    gui_element(const std::string& texturePath, glm::vec2 size, glm::vec2 m_position, Scene* scene, bool visible){
        this->visible = visible;
        this->scene = scene;
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
    glm::vec2 m_position;
    Texture *texture;
    void Draw();
    void SetupMesh();
    void SetupShader();
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    GLuint shaderProgram;
    GLuint CompileShader(GLenum type, const char* source);
    void TestHovered(GLFWwindow* window);
    bool IsClicked(GLFWwindow* window);
    bool hovered = false;
    std::vector<gui_element*> sub_elements = std::vector<gui_element*>();
    std::function<void()> onHover;
    std::function<void()> onClick;
    Scene *scene;
    TextRenderer *text = nullptr;
};

class GUI
{
public:
    GUI(CharacterController *player, Scene *scene);
    ~GUI();

    void Draw();
    CharacterController *player;

private:
    std::vector<gui_element*> elements = std::vector<gui_element*>();
    GLFWwindow *window;
    gui_element *main_menu;
    Scene *scene;

};
