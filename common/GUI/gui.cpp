#include "gui.h"


void exit(Scene *scene){
    scene->gameExited = true;
}

GUI::GUI(CharacterController *player, Scene *scene){
    this->player = player;
    this->scene = scene;
    window = glfwGetCurrentContext();
    int width, height;

    glfwGetWindowSize(window, &width, &height);
    float ratio = width/(height*1.0);

    //ingame
    elements.push_back(new gui_element("../Resources/Textures/GUI/Crosshair.png", glm::vec2(0.02f, 0.02f*ratio), glm::vec2(0), scene, true));
    for(int i = 0; i < 10; i ++){
        gui_element *new_case = new gui_element("../Resources/Textures/GUI/InventoryCase.png", glm::vec2(0.05f, 0.05f*ratio), glm::vec2(-0.5f + 0.1*i, -1.0+0.05f*ratio), scene, true);
        inventory_ingame.push_back(new_case);
        elements.push_back(new_case);
    }
    for(int i = 0; i < 10; i ++){
        gui_element *new_case = new gui_element("../Resources/Textures/GUI/InventorySelect.png", glm::vec2(0.055f, 0.055f*ratio), glm::vec2(-0.5f + 0.1*i, -1.0+0.05f*ratio), scene, false);
        inventory_selected.push_back(new_case);
        elements.push_back(new_case);
    }

    //inventory
    for(int i = 0; i < 10; i ++){
        glm::vec2 pos = glm::vec2(-0.5f + 0.1*i, -1.0+0.05f*ratio);
        gui_icon *new_icon = new gui_icon("../Resources/Textures/GUI/blocs/Dirt_type2.png", glm::vec2(0.045f, 0.045f*ratio), pos, scene, false);
        new_icon->nbr1 = new gui_element("../Resources/Textures/GUI/numbers/nbr_00.png", glm::vec2(0.015f, 0.025f*ratio), pos+glm::vec2(0.0, -0.025), scene, true);
        new_icon->nbr2 = new gui_element("../Resources/Textures/GUI/numbers/nbr_00.png", glm::vec2(0.015f, 0.025f*ratio), pos+glm::vec2(0.035, -0.025), scene, true);
        elements.push_back(new_icon);
        inventory.push_back(new_icon);
    }

    //main menu
    main_menu = new gui_element("../Resources/Textures/GUI/MenuOut.png", glm::vec2(0.9f, 0.75f*ratio), glm::vec2(0), scene, false);
    gui_element *exit_button = new gui_element("../Resources/Textures/GUI/exitButton.png", glm::vec2(0.25f, 0.15f*ratio), glm::vec2(0, 0.5), scene, true);
    main_menu->sub_elements.push_back(exit_button);
    exit_button->onClick = [scene](){exit(scene);};
    elements.push_back(main_menu);

}

void gui_icon::setQuantity(int newval){
    if(quantity == newval) return;
    quantity = newval;
    nbr1->visible = newval >= 10;
    nbr1->change_tex(GUI::nbr_to_string(newval/10));
    nbr2->change_tex(GUI::nbr_to_string(newval%10));

}

void gui_icon::setType(int newval){
    if(bloc_type == newval) return;
    bloc_type = newval;
    change_tex(GUI::type_to_string(newval));
}

std::string GUI::nbr_to_string(int i){
    switch(i){
    case(0): return "../Resources/Textures/GUI/numbers/nbr_00.png"; break;
    case(1): return "../Resources/Textures/GUI/numbers/nbr_01.png"; break;
    case(2): return "../Resources/Textures/GUI/numbers/nbr_02.png"; break;
    case(3): return "../Resources/Textures/GUI/numbers/nbr_03.png"; break;
    case(4): return "../Resources/Textures/GUI/numbers/nbr_04.png"; break;
    case(5): return "../Resources/Textures/GUI/numbers/nbr_05.png"; break;
    case(6): return "../Resources/Textures/GUI/numbers/nbr_06.png"; break;
    case(7): return "../Resources/Textures/GUI/numbers/nbr_07.png"; break;
    case(8): return "../Resources/Textures/GUI/numbers/nbr_08.png"; break;
    case(9): return "../Resources/Textures/GUI/numbers/nbr_09.png"; break;
    default: return "../Resources/Textures/GUI/numbers/nbr_00.png"; break;
    }
}

std::string GUI::type_to_string(int i){
    switch(i){
    case(3): return "../Resources/Textures/GUI/blocs/Stone_type3.png"; break;
    default: return "../Resources/Textures/GUI/blocs/Dirt_type2.png"; break;
    }
}

void gui_element::TestHovered(GLFWwindow* window)
{
    int width, height;

    glfwGetFramebufferSize(window,
                           &width,
                           &height);

    double mouseX, mouseY;

    glfwGetCursorPos(window,
                     &mouseX,
                     &mouseY);

    // Convert to NDC
    float x = (2.0f * mouseX) / width - 1.0f;

    float y = 1.0f - (2.0f * mouseY) / height;

    float left   = m_position.x - m_size.x;
    float right  = m_position.x + m_size.x;

    float bottom = m_position.y - m_size.y;
    float top    = m_position.y + m_size.y;
    hovered = (x >= left  && x <= right && y >= bottom && y <= top);
}

bool gui_element::IsClicked(GLFWwindow* window)
{
    return ( hovered && scene->inputReleased("action1"));
}

GUI::~GUI()
{
    for(gui_element *e : elements){
        free(e);
    }
}

void GUI::update_inventory(){
    for(int i = 0; i < inventory.size(); i ++){
        glm::ivec2 obj = player->inventory[i];
        if(obj[1] > 0){
            inventory[i]->visible = true;
            inventory[i]->setType(obj[0]);
            inventory[i]->setQuantity(obj[1]);
        }
        else{
            inventory[i]->visible = false;
        }
    }
}

void GUI::Draw(){
    if (scene->inputPressed("hideUI")){
        hide_gui = !hide_gui;
    }
    update_inventory();
    for(gui_element *e : inventory_selected){
        e->visible = false;
    }
    inventory_selected[player->current_inventory_case_selected]->visible = true;
    main_menu->visible = player->paused;
    if(hide_gui){
        if(main_menu->visible){
            main_menu->Draw();
            main_menu->TestHovered(window);
            if(main_menu->IsClicked(window) and main_menu->onClick) main_menu->onClick();
        }
    }
    else
        for(gui_element *e : elements){
            if(e->visible){
                e->Draw();
                e->TestHovered(window);
                if(e->IsClicked(window) and e->onClick) e->onClick();
            }
        }
}

void gui_element::SetupMesh()
{
    float vertices[] =
        {
            // positions                                            // tex coords
            m_position.x-m_size.x, m_position.y-m_size.y,           0.0f, 1.0f,
            m_position.x+m_size.x, m_position.y-m_size.y,           1.0f, 1.0f,
            m_position.x+m_size.x,  m_position.y+m_size.y,           1.0f, 0.0f,
            m_position.x-m_size.x,  m_position.y+m_size.y,           0.0f, 0.0f
        };

    unsigned int indices[] =
        {
            0, 1, 2,
            2, 3, 0
        };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(vertices),
                 vertices,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(indices),
                 indices,
                 GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);

    // UV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

GLuint gui_element::CompileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);

    glShaderSource(shader,
                   1,
                   &source,
                   nullptr);

    glCompileShader(shader);

    return shader;
}

void gui_element::SetupShader()
{
    const char* vertexShaderSource = R"(
        #version 330 core

        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aTex;

        out vec2 TexCoord;

        void main()
        {
            gl_Position = vec4(aPos, 0.0, 1.0);
            TexCoord = aTex;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core

        out vec4 FragColor;

        in vec2 TexCoord;

        uniform sampler2D tex;

        void main()
        {
            FragColor = texture(tex, TexCoord);
        }
    )";

    GLuint vs = CompileShader(GL_VERTEX_SHADER,
                      vertexShaderSource);

    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);

    glLinkProgram(shaderProgram);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

void gui_element::Draw()
{
    if(texture != nullptr){
        glDisable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA,
                    GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(shaderProgram);

        texture->bind(0);

        glUniform1i(
            glGetUniformLocation(shaderProgram, "tex"),
            0
            );

        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES,
                       6,
                       GL_UNSIGNED_INT,
                       0);

        glBindVertexArray(0);

        glDisable(GL_BLEND);

        glEnable(GL_DEPTH_TEST);
    }

    for(gui_element *e : sub_elements){
        GLFWwindow *window = glfwGetCurrentContext();
        if(e->visible){
            e->Draw();
            e->TestHovered(window);
            if(e->IsClicked(window) and e->onClick) e->onClick();
        }
    }
}
