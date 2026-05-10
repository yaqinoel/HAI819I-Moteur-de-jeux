#pragma once

#include <common/scene.h>
#include <common/3dEntities/Model.hpp>
#include <common/Controls/cameracontrols.h>
#include <common/Controls/freecamera.h>
#include <common/3dEntities/Lights/PointLight.hpp>
#include <common/shader.hpp>
#include <common/Render/IBLEnvironment.hpp>
#include <common/Render/RenderSystem.hpp>
#include <common/Render/Shader.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>

Scene* makePBRGridScene(RenderSystem* renderer) {
    Node* root = new Node3d();
    Scene* scene = new Scene(root);
    scene->iblEnvironment = new IBLEnvironment("../Resources/Textures/HDR/newport_loft.hdr");

    FreeCamera* cameraNode = new FreeCamera(1024.0f, 768.0f, 45.0f, 0.1f, 100.0f, glm::vec3(0, 0, 20));
    scene->mainCamera = cameraNode;
    scene->instantiate(cameraNode);

    // GLuint pbrShader = LoadShaders("../Shaders/vertex_shader_pbr.glsl", "../Shaders/fragment_shader_pbr.glsl");
    Shader* uniPbrShader = renderer->getOrCreateShader("../Shaders/vertex_shader_pbr.glsl", "../Shaders/fragment_shader_pbr.glsl");

    // Textures are disabled in this demo so metallic/roughness values are easy to observe.
    // PBRMaterial* rustIronMat = new PBRMaterial(uniPbrShader, glm::vec3(1.0f), 0.0f, 0.5f, 1.0f);
    // rustIronMat->addTexture("albedoMap", Texture("../Resources/Textures/RustIron/albedo.png"));
    // rustIronMat->addTexture("normalMap", Texture("../Resources/Textures/RustIron/normal.png"));
    // rustIronMat->addTexture("metallicMap", Texture("../Resources/Textures/RustIron/metallic.png"));
    // rustIronMat->addTexture("roughnessMap", Texture("../Resources/Textures/RustIron/roughness.png"));
    // rustIronMat->addTexture("aoMap", Texture("../Resources/Textures/RustIron/ao.png"));
    
    // Model* baseSphere = new Model("../Resources/Models/obj/pbr_sphere.obj", true);
    Mesh* baseSphereMesh = new Mesh();
    baseSphereMesh->openOBJ("../Resources/Models/obj/pbr_sphere.obj");

    // Sphere Grid Setup
    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5;
    
    for (int row = 0; row < nrRows; ++row) {
        for (int col = 0; col < nrColumns; ++col) {
            Node3d* sphereNode = new Node3d();
            sphereNode->name = "Sphere(" + std::to_string(row) + "," + std::to_string(col) + ")";

            Mesh* instancedMesh = new Mesh(*baseSphereMesh);
            // instancedMesh->setShader(pbrShader);
            float metallic = (float)row / (float)nrRows;
            float roughness = glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f);
            PBRMaterial* material = new PBRMaterial(uniPbrShader, glm::vec3(0.5f, 0.0f, 0.0f), metallic, roughness, 1.0f);
            instancedMesh->setMaterial(material);
            sphereNode->addChild(instancedMesh);

            // for(Mesh* m : baseSphere->meshes) {
            //     Mesh* instancedMesh = new Mesh(*m);
            //     instancedMesh->setShader(pbrShader);
            //     instancedMesh->setMaterial(rustIronMat);
            //     sphereNode->addChild(instancedMesh);
            // }

            glm::vec3 pos(
                (float)(col - (nrColumns / 2)) * spacing,
                (float)(row - (nrRows / 2)) * spacing,
                -2.0f
            );
            sphereNode->setLocalPosition(pos);
            
            scene->instantiate(sphereNode);
        }
    }
    
    delete baseSphereMesh;
    
    glm::vec3 lightPositions[] = {
        glm::vec3(-10.0f,  10.0f, 10.0f),
        glm::vec3( 10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3( 10.0f, -10.0f, 10.0f),
    };

    for (const glm::vec3& position : lightPositions) {
        PointLight* light = new PointLight(glm::vec3(1.0f), 300.0f, 20.0f);
        light->setLocalPosition(position);
        scene->instantiate(light);
    }
    
    return scene;
}
