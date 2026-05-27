#pragma once

#include <common/scene.h>
#include <common/Controls/cameracontrols.h>
#include <common/Controls/freecamera.h>
#include <common/3dEntities/Lights/DirectionalLight.hpp>
#include <common/3dEntities/Lights/PointLight.hpp>
#include <common/shader.hpp>
#include <common/Render/IBLEnvironment.hpp>
#include <common/Render/RenderSystem.hpp>
#include <common/Render/Shader.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <vector>

Scene* makePBRGridScene(RenderSystem* renderer) {
    Node* root = new Node3d();
    Scene* scene = new Scene(root);
    scene->iblEnvironment = new IBLEnvironment("../Resources/Textures/HDR/newport_loft.hdr");

    FreeCamera* cameraNode = new FreeCamera(1024.0f, 768.0f, 45.0f, 0.1f, 100.0f, glm::vec3(0, 0, 26));
    scene->mainCamera = cameraNode;
    scene->instantiate(cameraNode);

    Shader* uniPbrShader = renderer->getOrCreateShader("../Shaders/vertex_shader_pbr.glsl", "../Shaders/fragment_shader_pbr.glsl");

    // PBRMaterial* rustIronMat = new PBRMaterial(uniPbrShader, glm::vec3(1.0f), 0.0f, 0.5f, 1.0f);
    // rustIronMat->addTexture("albedoMap", Texture("../Resources/Textures/RustIron/albedo.png"));
    // rustIronMat->addTexture("normalMap", Texture("../Resources/Textures/RustIron/normal.png"));
    // rustIronMat->addTexture("metallicMap", Texture("../Resources/Textures/RustIron/metallic.png"));
    // rustIronMat->addTexture("roughnessMap", Texture("../Resources/Textures/RustIron/roughness.png"));
    // rustIronMat->addTexture("aoMap", Texture("../Resources/Textures/RustIron/ao.png"));
    
    Mesh* baseSphereMesh = new Mesh();
    baseSphereMesh->openOBJ("../Resources/Models/obj/pbr_sphere.obj");

    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5;

    std::vector<Vertex> groundVertices;
    groundVertices.push_back(Vertex(glm::vec3(-13.0f, -9.0f, -12.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
    groundVertices.push_back(Vertex(glm::vec3(-13.0f, -9.0f,  10.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)));
    groundVertices.push_back(Vertex(glm::vec3( 13.0f, -9.0f,  10.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
    groundVertices.push_back(Vertex(glm::vec3( 13.0f, -9.0f, -12.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)));

    std::vector<Triangle> groundTriangles;
    groundTriangles.push_back(Triangle(0, 1, 2));
    groundTriangles.push_back(Triangle(0, 2, 3));

    Mesh* ground = new Mesh(groundVertices, groundTriangles);
    PBRMaterial* groundMaterial = new PBRMaterial(uniPbrShader, glm::vec3(0.45f, 0.45f, 0.42f), 0.0f, 0.75f, 1.0f);
    ground->setMaterial(groundMaterial);
    scene->instantiate(ground);
    
    for (int row = 0; row < nrRows; ++row) {
        for (int col = 0; col < nrColumns; ++col) {
            Node3d* sphereNode = new Node3d();
            sphereNode->name = "Sphere(" + std::to_string(row) + "," + std::to_string(col) + ")";

            Mesh* instancedMesh = new Mesh(*baseSphereMesh);
            float metallic = (float)row / (float)nrRows;
            float roughness = glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f);
            PBRMaterial* material = new PBRMaterial(uniPbrShader, glm::vec3(0.5f, 0.0f, 0.0f), metallic, roughness, 1.0f);
            instancedMesh->setMaterial(material);
            sphereNode->addChild(instancedMesh);

            // for(Mesh* m : baseSphere->meshes) {
            //     Mesh* instancedMesh = new Mesh(*m);
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
        light->castShadow = true;
        light->shadowNearPlane = 1.0f;
        light->shadowFarPlane = 40.0f;
        scene->instantiate(light);
    }

    DirectionalLight* sunLight = new DirectionalLight(glm::vec3(1.0f), 3.0f);
    sunLight->setForward(glm::vec3(-0.35f, -1.0f, -0.25f));
    sunLight->castShadow = true;
    sunLight->shadowOrthoSize = 13.0f;
    sunLight->shadowNearPlane = 1.0f;
    sunLight->shadowFarPlane = 40.0f;
    scene->instantiate(sunLight);
    
    return scene;
}
