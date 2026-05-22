#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <map>
#include <string>

struct InputState{
    int inputType;
    bool mouseInput;
    bool currentlyHeld;
    bool justPressed;
    InputState(int t, bool mouse = false){
        inputType = t;
        mouseInput = mouse;
        currentlyHeld = false;
        justPressed = false;
    }
};

class InputManager{
public:
    std::map<std::string, InputState*> inputs;
    InputManager(){
        inputs.insert({"action1", new InputState(GLFW_MOUSE_BUTTON_LEFT, true)});
        inputs.insert({"action2", new InputState(GLFW_MOUSE_BUTTON_RIGHT, true)});
        inputs.insert({"action3", new InputState(GLFW_KEY_E)});
        inputs.insert({"jump", new InputState(GLFW_KEY_SPACE)});
        inputs.insert({"down", new InputState(GLFW_KEY_LEFT_SHIFT)});
        inputs.insert({"left", new InputState(GLFW_KEY_A)});
        inputs.insert({"right", new InputState(GLFW_KEY_D)});
        inputs.insert({"backwards", new InputState(GLFW_KEY_S)});
        inputs.insert({"forward", new InputState(GLFW_KEY_W)});
        inputs.insert({"action4", new InputState(GLFW_KEY_Q)});
    }void UpdateInputs()
    {
        GLFWwindow* window = glfwGetCurrentContext();
        for(auto input : inputs)
        {
            int state;
            if(input.second->mouseInput)
            {
                state = glfwGetMouseButton(window, input.second->inputType);
            }
            else
            {
                state = glfwGetKey(window, input.second->inputType);
            }

            if(state == GLFW_PRESS)
            {
                if(input.second->currentlyHeld)
                {
                    input.second->justPressed = false;
                }
                else
                {
                    input.second->currentlyHeld = true;
                    input.second->justPressed = true;
                }
            }
            else
            {
                input.second->currentlyHeld = false;
                input.second->justPressed = false;
            }
        }
    }
};
