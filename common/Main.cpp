// Include standard headers
#include "Scenes/Ball.h"
#include "Scenes/Cube.h"
#include "Scenes/SolarSystem.h"
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

#include "Scenes/PBRGridScene.h"
#include "Scenes/InfiniteTerrain.h"
#include "Scenes/PBRInfiniteTerrain.h"
#include "common/Render/RenderSystem.hpp"
#include "common/Render/ForwardRenderSystem.hpp"

GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glut.h>
#include <glm/gtx/string_cast.hpp>
#include <chrono>

#include <string>
#include <common/shader.hpp>
#include <common/vboindexer.hpp>
#include <common/3dEntities/Mesh.hpp>
#include <common/Controls/cameracontrols.h>
#include <common/Controls/freecamera.h>
#include <common/Materials/material.h>
#include <common/3dEntities/Meshes/planet.h>

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//CameraControls* cam = new CameraControls(4.0f, 3.0f, 45.0f, 0.1f, 100.0f, glm::vec3(0, 9, -10));
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//rotation
float angle = 0.;
float zoom = 1.;

Scene* scene;
int resX = 4;
int resY = 4;
float rotSpeed = 1;

RenderSystem* renderer;

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

    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderer = new ForwardRenderSystem();

    scene = makePBRInfiniteTerrain(renderer);
    // scene = makePBRGridScene(renderer);
    // scene = makeInfiniteTerrain(renderer);

    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    float physicsStep = scene->fixedDeltaTime;
    double accumulator = 0.0;
    do{
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        accumulator += deltaTime;

        glfwPollEvents();
        scene->process(deltaTime);

        // Run physics at fixed intervals
        while (accumulator >= physicsStep) {
            scene->physicsProcess();

            accumulator -= physicsStep;
        }



        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float alpha = accumulator / physicsStep;
        scene->updateInterpolation(alpha);
        renderer->render(scene);
        scene->renderLines();

        // Swap buffers
        glfwSwapBuffers(window);

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );


    delete(scene);
    delete(renderer);
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
