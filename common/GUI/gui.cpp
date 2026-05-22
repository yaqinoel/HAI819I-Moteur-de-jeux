#include "gui.h"


void debugClick(){
    std::cout << "clicked" << std::endl;
}


GUI::GUI(CharacterController *player, Scene *scene){
    this->player = player;
    this->scene = scene;
    window = glfwGetCurrentContext();
    int width, height;

    glfwGetWindowSize(window, &width, &height);
    float ratio = width/(height*1.0);
    elements.push_back(new gui_element("../Resources/Textures/GUI/Crosshair.png", glm::vec2(0.02f, 0.02f*ratio), glm::vec2(0), scene, true));
    main_menu = new gui_element("../Resources/Textures/GUI/MenuOut.png", glm::vec2(0.9f, 0.75f*ratio), glm::vec2(0), scene, false);
    main_menu->onClick = debugClick;
    main_menu->text = new TextRenderer("Menu", glm::vec2(0, 0.5), 10);
    elements.push_back(main_menu);
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
    return ( hovered && scene->inputPressed("action1"));
}

GUI::~GUI()
{
    for(gui_element *e : elements){
        free(e);
    }
}

void GUI::Draw(){
    main_menu->visible = player->paused;
    for(gui_element *e : elements){
        if(e->visible){
            e->Draw();
            if(e->text != nullptr) e->text->Draw();
            e->TestHovered(window);
            if(e->IsClicked(window)) e->onClick();
        }
    }
}

void gui_element::SetupMesh()
{
    float vertices[] =
        {
            // positions                                            // tex coords
            m_position.x-m_size.x, m_position.y-m_size.y,           0.0f, 0.0f,
            m_position.x+m_size.x, m_position.y-m_size.y,           1.0f, 0.0f,
            m_position.x+m_size.x,  m_position.y+m_size.y,           1.0f, 1.0f,
            m_position.x-m_size.x,  m_position.y+m_size.y,           0.0f, 1.0f
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
            if(e->text != nullptr) e->text->Draw();
            e->TestHovered(window);
            if(e->IsClicked(window)) e->onClick();
        }
    }
}
