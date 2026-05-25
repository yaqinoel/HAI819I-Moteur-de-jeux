#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>
#include <string>
#define INPUT_MOUSE_WHEEL_UP    10000
#define INPUT_MOUSE_WHEEL_DOWN  10001

struct InputState{
    int inputType;
    bool mouseInput;
    bool currentlyHeld;
    bool justPressed;
    bool justReleased;
    InputState(int t, bool mouse = false){
        inputType = t;
        mouseInput = mouse;
        currentlyHeld = false;
        justPressed = false;
        justReleased = false;
    }
};
class InputManager{
public:
    std::map<std::string, InputState*> inputs;
    bool wheelUpThisFrame = false;
    bool wheelDownThisFrame = false;
    inline static InputManager* instance = nullptr;
    GLFWwindow* window;

    InputManager(){
        window = glfwGetCurrentContext();
        instance = this;
        inputs.insert({"action1", new InputState(GLFW_MOUSE_BUTTON_LEFT, true)});
        inputs.insert({"action2", new InputState(GLFW_MOUSE_BUTTON_RIGHT, true)});
        inputs.insert({"action3", new InputState(GLFW_KEY_E)});
        inputs.insert({"testBigVoxel", new InputState(GLFW_KEY_V)});
        inputs.insert({"testPBRCube", new InputState(GLFW_KEY_P)});
        inputs.insert({"jump", new InputState(GLFW_KEY_SPACE)});
        inputs.insert({"down", new InputState(GLFW_KEY_LEFT_SHIFT)});
        inputs.insert({"left", new InputState(GLFW_KEY_A)});
        inputs.insert({"right", new InputState(GLFW_KEY_D)});
        inputs.insert({"backwards", new InputState(GLFW_KEY_S)});
        inputs.insert({"forward", new InputState(GLFW_KEY_W)});
        inputs.insert({"action4", new InputState(GLFW_KEY_Q)});
        inputs.insert({"escape", new InputState(GLFW_KEY_ESCAPE)});
        inputs.insert({"wheelUp", new InputState(INPUT_MOUSE_WHEEL_UP)});
        inputs.insert({"wheelDown", new InputState(INPUT_MOUSE_WHEEL_DOWN)});
        glfwSetScrollCallback(window, scroll_callback);
    }

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        instance->wheelUpThisFrame = yoffset > 0;
        instance->wheelDownThisFrame = yoffset < 0;
    }

    void UpdateInputs()
    {
        for (auto input : inputs)
        {
            bool pressed;
            if (input.second->inputType == INPUT_MOUSE_WHEEL_UP)
            {
                pressed = wheelUpThisFrame;
                wheelUpThisFrame = false;
            }
            else if (input.second->inputType == INPUT_MOUSE_WHEEL_DOWN)
            {
                pressed = wheelDownThisFrame;
                wheelDownThisFrame = false;
            }
            else if (input.second->mouseInput)
            {
                pressed = glfwGetMouseButton(window, input.second->inputType);
            }
            else
            {
                pressed = glfwGetKey(window, input.second->inputType);
            }

            input.second->justPressed = false;
            input.second->justReleased = false;

            if (pressed && !input.second->currentlyHeld)
            {
                input.second->currentlyHeld = true;
                input.second->justPressed = true;
            }

            else if (!pressed && input.second->currentlyHeld)
            {
                input.second->currentlyHeld = false;
                input.second->justReleased = true;
            }
        }
    }
};
