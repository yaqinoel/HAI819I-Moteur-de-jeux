// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glut.h>
#include <glm/gtx/string_cast.hpp>
#include <algorithm>

#include <string>
#include <common/shader.hpp>
#include <common/vboindexer.hpp>
#include <common/3dEntities/Mesh.hpp>
#include <common/3dEntities/Meshes/proceduralterrain.h>
#include <common/Controls/cameracontrols.h>
#include <common/Materials/material.h>
#include <common/3dEntities/Meshes/planet.h>

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

CameraControls* cam = new CameraControls(4.0f, 3.0f, 45.0f, 0.1f, 100.0f, glm::vec3(0, 9, -10));
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//rotation
float angle = 0.;
float zoom = 1.;

Node* scene;
int resX = 4;
int resY = 4;
float rotSpeed = 1;

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "TP1 - GLFW", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    //  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    scene = new Node3d();
    std::string filename("../Resources/Models/obj/Planet.obj");
    Planet* Sun = new Planet(scene);{
        Sun->openOBJ(filename);
        Sun->setShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader.glsl");
        Texture tex = Texture("../Resources/Textures/Planets/2k_sun.jpg");
        Material mat = Material(glm::vec3(0.5, 0.5, 0.5));
        mat.addTexture("texture0", tex);
        mat.setLit(0);
        Sun->setMaterial(mat);
        Sun->daySpeed = 1.0;
        Sun->scale = glm::vec3(1.8);
    }

    Planet* Earth = new Planet(Sun, scene);{
        Earth->openOBJ(filename);
        Earth->setShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader.glsl");
        Texture tex = Texture("../Resources/Textures/Planets/Earth_Diffuse_6K.jpg");
        Material mat = Material(glm::vec3(0.5, 0.5, 0.5));
        mat.addTexture("texture0", tex);
        mat.setLit(1);
        Earth->setMaterial(mat);
        Earth->setParentDistance(3);
        Earth->daySpeed = 2.0;
        Earth->yearSpeed = 0.2;
        Earth->scale = glm::vec3(0.6);
        Sun->SetRotation(glm::vec3(0, 0, 23.44));
    }


    Planet* Moon = new Planet(Earth, scene);{
        Moon->openOBJ(filename);
        Moon->setShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader.glsl");
        Texture tex = Texture("../Resources/Textures/Planets/2k_moon.jpg");
        Material mat = Material(glm::vec3(0.5, 0.5, 0.5));
        mat.addTexture("texture0", tex);
        mat.setLit(1);
        Moon->setMaterial(mat);
        Moon->setParentDistance(0.6);
        Moon->daySpeed = 2.0;
        Moon->yearSpeed = 2.0;
        Moon->scale = glm::vec3(0.3);
        Moon->SetRotation(glm::vec3(0, 0, 6.68));
    }


    Planet* Mars = new Planet(Sun, scene);{
        Mars->openOBJ(filename);
        Mars->setShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader.glsl");
        Texture tex = Texture("../Resources/Textures/Planets/2k_mars.jpg");
        Material mat = Material(glm::vec3(0.5, 0.5, 0.5));
        mat.addTexture("texture0", tex);
        mat.setLit(1);
        Mars->setMaterial(mat);
        Mars->setParentDistance(6);
        Mars->daySpeed = 2.0;
        Mars->yearSpeed = 0.15;
        Mars->scale = glm::vec3(0.6);
    }


    Planet* Phobos = new Planet(Mars, scene);{
        Phobos->openOBJ(filename);
        Phobos->setShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader.glsl");
        Texture tex = Texture("../Resources/Textures/Planets/2k_moon.jpg");
        Material mat = Material(glm::vec3(0.5, 0.5, 0.5));
        mat.addTexture("texture0", tex);
        mat.setLit(1);
        Phobos->setMaterial(mat);
        Phobos->setParentDistance(0.5);
        Phobos->daySpeed = 2.0;
        Phobos->yearSpeed = 2.2;
        Phobos->scale = glm::vec3(0.25);
    }

    Planet* Deimos = new Planet(Mars, scene);{
        Deimos->openOBJ(filename);
        Deimos->setShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader.glsl");
        Texture tex = Texture("../Resources/Textures/Planets/2k_moon.jpg");
        Material mat = Material(glm::vec3(0.5, 0.5, 0.5));
        mat.addTexture("texture0", tex);
        mat.setLit(1);
        Deimos->setMaterial(mat);
        Deimos->setParentDistance(0.8);
        Deimos->daySpeed = 2.0;
        Deimos->yearSpeed = 1.8;
        Deimos->scale = glm::vec3(0.35);
    }

    Planet* Saturn = new Planet(Sun, scene);{
        Saturn->openOBJ(filename);
        Saturn->setShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader.glsl");
        Texture tex = Texture("../Resources/Textures/Planets/2k_saturn.jpg");
        Material mat = Material(glm::vec3(0.5, 0.5, 0.5));
        mat.addTexture("texture0", tex);
        mat.setLit(1);
        Saturn->setMaterial(mat);
        Saturn->setParentDistance(10);
        Saturn->daySpeed = 2.0;
        Saturn->yearSpeed = 0.15;
        Saturn->scale = glm::vec3(1.2);
    }
    Planet* Rings = new Planet(Saturn, scene);{
        Rings->openOBJ("../Resources/Models/obj/Rings.obj");
        Rings->setShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader.glsl");
        Texture tex = Texture("../Resources/Textures/Planets/2k_saturn_ring_alpha.png");
        Material mat = Material(glm::vec3(0.5, 0.5, 0.5));
        mat.addTexture("texture0", tex);
        mat.setLit(1);
        Rings->setMaterial(mat);
        Rings->setParentDistance(0);
        Rings->scale = glm::vec3(1);
    }
    cam->setParent(scene);

    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    do{
        //std::cout << "frame start" << std::endl;
        // Measure speed
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        scene->process(deltaTime);


        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene->render(cam);
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    delete(scene);
    scene = nullptr;
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return 0;
}



// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
