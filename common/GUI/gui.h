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
        this->m_position = m_position;
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

    void change_tex(const std::string& texturePath) {texture = new Texture(texturePath, true);}


    bool visible;
    glm::vec2 m_size;
    glm::vec2 m_position;
    Texture *texture;
    virtual void Draw();
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
};

struct gui_icon : public gui_element{
    int bloc_type = -1;
    int quantity = -1;
    gui_element *nbr1 = nullptr;
    gui_element *nbr2 = nullptr;
    gui_icon(const std::string& texturePath, glm::vec2 size, glm::vec2 m_position, Scene* scene, bool visible):
        gui_element(texturePath, size, m_position, scene, visible)
    {
        this->bloc_type = -1;
        this->quantity = -1;
    }
    void Draw() override{gui_element::Draw(); if(nbr1->visible)nbr1->Draw(); if(nbr2->visible)nbr2->Draw();}
    void setQuantity(int newval);
    void setType(int newval);
};

class GUI
{
public:
    GUI(CharacterController *player, Scene *scene);
    ~GUI();

    void Draw();
    static std::string nbr_to_string(int i);
    static std::string type_to_string(int i);
    bool hide_gui = false;
private:
    void update_inventory();
    CharacterController *player;
    std::vector<gui_element*> elements = std::vector<gui_element*>();
    GLFWwindow *window;
    gui_element *main_menu;
    Scene *scene;
    std::vector<gui_element*> inventory_ingame = std::vector<gui_element*>();
    std::vector<gui_element*> inventory_selected = std::vector<gui_element*>();
    std::vector<gui_icon*> inventory = std::vector<gui_icon*>();

};
