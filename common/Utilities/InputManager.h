#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <map>
#include <string>

struct InputState{
    int inputType;
    bool currentlyHeld;
    bool justPressed;
    InputState(int t){
        inputType = t;
        currentlyHeld = false;
        justPressed = false;
    }
};

class InputManager{
public:
    std::map<std::string, InputState*> inputs;
    InputManager(){
        inputs.insert({"action1", new InputState(GLFW_KEY_E)});
        inputs.insert({"jump", new InputState(GLFW_KEY_SPACE)});
        inputs.insert({"down", new InputState(GLFW_KEY_LEFT_SHIFT)});
        inputs.insert({"left", new InputState(GLFW_KEY_A)});
        inputs.insert({"right", new InputState(GLFW_KEY_D)});
        inputs.insert({"backwards", new InputState(GLFW_KEY_S)});
        inputs.insert({"forward", new InputState(GLFW_KEY_W)});
    }
    void UpdateInputs(){
        GLFWwindow* window = glfwGetCurrentContext();
        for(auto input : inputs){
            if (glfwGetKey(window, input.second->inputType) == GLFW_PRESS){
                if(input.second->currentlyHeld){
                    input.second->justPressed = false;
                }
                else{
                    input.second->currentlyHeld = true;
                    input.second->justPressed = true;
                }
            }
            else{
                input.second->currentlyHeld = false;
                input.second->justPressed = false;
            }
        }
    }
};
