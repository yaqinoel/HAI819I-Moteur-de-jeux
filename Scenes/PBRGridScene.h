#pragma once

#include <common/scene.h>
#include <common/3dEntities/Model.hpp>
#include <common/Controls/cameracontrols.h>
#include <common/Controls/freecamera.h>
#include <common/shader.hpp>
#include <GLFW/glfw3.h>
#include <iostream>


// Node to update lights dynamically
class PBRLightUpdater : public Node3d {
public:
    GLuint shaderID;
    
    PBRLightUpdater(GLuint shader) : shaderID(shader) {}
    
    virtual void process(float deltaTime) override {
        glUseProgram(shaderID);
        
        glm::vec3 lightPositions[] = {
            glm::vec3(-10.0f,  10.0f, 10.0f),
            glm::vec3( 10.0f,  10.0f, 10.0f),
            glm::vec3(-10.0f, -10.0f, 10.0f),
            glm::vec3( 10.0f, -10.0f, 10.0f),
        };
        glm::vec3 lightColors[] = {
            glm::vec3(300.0f, 300.0f, 300.0f),
            glm::vec3(300.0f, 300.0f, 300.0f),
            glm::vec3(300.0f, 300.0f, 300.0f),
            glm::vec3(300.0f, 300.0f, 300.0f)
        };
        
        for (unsigned int i = 0; i < 4; ++i) {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            
            std::string posStr = "lightPositions[" + std::to_string(i) + "]";
            std::string colStr = "lightColors[" + std::to_string(i) + "]";
            
            glUniform3fv(glGetUniformLocation(shaderID, posStr.c_str()), 1, glm::value_ptr(newPos));
            glUniform3fv(glGetUniformLocation(shaderID, colStr.c_str()), 1, glm::value_ptr(lightColors[i]));
        }
        
        if (scene && scene->mainCamera) {
            glUniform3fv(glGetUniformLocation(shaderID, "camPos"), 1, glm::value_ptr(scene->mainCamera->getGlobalPosition()));
        }
    }
};

Scene* makePBRGridScene() {
    Node* root = new Node3d();
    Scene* scene = new Scene(root);

    FreeCamera* cameraNode = new FreeCamera(1024.0f, 768.0f, 45.0f, 0.1f, 100.0f, glm::vec3(0, 0, 20));
    scene->mainCamera = cameraNode;
    scene->instantiate(cameraNode);

    GLuint pbrShader = LoadShaders("../Shaders/vertex_shader_pbr.glsl", "../Shaders/fragment_shader_pbr.glsl");
    
    PBRMaterial* rustIronMat = new PBRMaterial();
    rustIronMat->addTexture("albedoMap", Texture("../Resources/Textures/RustIron/albedo.png"));
    rustIronMat->addTexture("normalMap", Texture("../Resources/Textures/RustIron/normal.png"));
    rustIronMat->addTexture("metallicMap", Texture("../Resources/Textures/RustIron/metallic.png"));
    rustIronMat->addTexture("roughnessMap", Texture("../Resources/Textures/RustIron/roughness.png"));
    rustIronMat->addTexture("aoMap", Texture("../Resources/Textures/RustIron/ao.png"));
    
    Model* baseSphere = new Model("../Resources/Models/obj/pbr_sphere.obj", true);
    
    // Sphere Grid Setup
    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5;
    
    for (int row = 0; row < nrRows; ++row) {
        for (int col = 0; col < nrColumns; ++col) {
            Node3d* sphereNode = new Node3d();
            sphereNode->name = "Sphere(" + std::to_string(row) + "," + std::to_string(col) + ")";

            for(Mesh* m : baseSphere->meshes) {
                Mesh* instancedMesh = new Mesh(*m);
                instancedMesh->setShader(pbrShader);
                instancedMesh->setMaterial(rustIronMat);
                sphereNode->addChild(instancedMesh);
            }

            glm::vec3 pos(
                (float)(col - (nrColumns / 2)) * spacing,
                (float)(row - (nrRows / 2)) * spacing,
                0.0f
            );
            sphereNode->setLocalPosition(pos);
            
            scene->instantiate(sphereNode);
        }
    }
    
    delete baseSphere;
    
    PBRLightUpdater* lightUpdater = new PBRLightUpdater(pbrShader);
    scene->instantiate(lightUpdater);
    
    return scene;
}
