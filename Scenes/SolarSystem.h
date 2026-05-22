#pragma once

#include <common/node.h>

#include <common/3dEntities/node3d.h>

#include <common/3dEntities/Meshes/planet.h>
#include <common/Render/Shader.hpp>

inline void setStandardShader(Mesh* mesh, Material* material, Shader* standardShader) {
    (void)mesh;
    if (standardShader) {
        material->shader = standardShader;
    }
}

Node* makeSolarSystem(Shader* standardShader = nullptr){
    Node* scene = new Node3d();
    std::string filename("../Resources/Models/obj/Planet.obj");
    Planet* Sun = new Planet(scene);{
        Sun->openOBJ(filename);
        Texture tex = Texture("../Resources/Textures/Planets/2k_sun.jpg");
        Material* mat = new Material(glm::vec3(0.5, 0.5, 0.5));
        setStandardShader(Sun, mat, standardShader);
        mat->addTexture("texture0", tex);
        mat->setLit(0);
        Sun->setMaterial(mat);
        Sun->daySpeed = 1.0;
        Sun->setScale(glm::vec3(1.8));
    }

    Planet* Earth = new Planet(Sun, scene);{
        Earth->openOBJ(filename);
        Material* mat = new Material(glm::vec3(0.5, 0.5, 0.5));
        setStandardShader(Earth, mat, standardShader);
        Texture tex = Texture("../Resources/Textures/Planets/Earth_Diffuse_6K.jpg");
        mat->addTexture("texture0", tex);
        mat->setLit(1);
        Earth->setMaterial(mat);
        Earth->setParentDistance(3);
        Earth->daySpeed = 2.0;
        Earth->yearSpeed = 0.2;
        Earth->setScale(glm::vec3(0.6));
        Earth->setLocalRotation(glm::vec3(0, 0, 23.44));
    }


    Planet* Moon = new Planet(Earth, scene);{
        Moon->openOBJ(filename);
        Texture tex = Texture("../Resources/Textures/Planets/2k_moon.jpg");
        Material* mat = new Material(glm::vec3(0.5, 0.5, 0.5));
        setStandardShader(Moon, mat, standardShader);
        mat->addTexture("texture0", tex);
        mat->setLit(1);
        Moon->setMaterial(mat);
        Moon->setParentDistance(0.6);
        Moon->daySpeed = 2.0;
        Moon->yearSpeed = 2.0;
        Moon->setScale(glm::vec3(0.3));
        Moon->setLocalRotation(glm::vec3(0, 0, 6.68));
    }


    Planet* Mars = new Planet(Sun, scene);{
        Mars->openOBJ(filename);
        Texture tex = Texture("../Resources/Textures/Planets/2k_mars.jpg");
        Material* mat = new Material(glm::vec3(0.5, 0.5, 0.5));
        setStandardShader(Mars, mat, standardShader);
        mat->addTexture("texture0", tex);
        mat->setLit(1);
        Mars->setMaterial(mat);
        Mars->setParentDistance(6);
        Mars->daySpeed = 2.0;
        Mars->yearSpeed = 0.15;
        Mars->setScale(glm::vec3(0.6));
    }


    Planet* Phobos = new Planet(Mars, scene);{
        Phobos->openOBJ(filename);
        Texture tex = Texture("../Resources/Textures/Planets/2k_moon.jpg");
        Material* mat = new Material(glm::vec3(0.5, 0.5, 0.5));
        setStandardShader(Phobos, mat, standardShader);
        mat->addTexture("texture0", tex);
        mat->setLit(1);
        Phobos->setMaterial(mat);
        Phobos->setParentDistance(0.5);
        Phobos->daySpeed = 2.0;
        Phobos->yearSpeed = 2.2;
        Phobos->setScale(glm::vec3(0.25));
    }

    Planet* Deimos = new Planet(Mars, scene);{
        Deimos->openOBJ(filename);
        Texture tex = Texture("../Resources/Textures/Planets/2k_moon.jpg");
        Material* mat = new Material(glm::vec3(0.5, 0.5, 0.5));
        setStandardShader(Deimos, mat, standardShader);
        mat->addTexture("texture0", tex);
        mat->setLit(1);
        Deimos->setMaterial(mat);
        Deimos->setParentDistance(0.8);
        Deimos->daySpeed = 2.0;
        Deimos->yearSpeed = 1.8;
        Deimos->setScale(glm::vec3(0.35));
    }

    Planet* Saturn = new Planet(Sun, scene);{
        Saturn->openOBJ(filename);
        Texture tex = Texture("../Resources/Textures/Planets/2k_saturn.jpg");
        Material* mat = new Material(glm::vec3(0.5, 0.5, 0.5));
        setStandardShader(Saturn, mat, standardShader);
        mat->addTexture("texture0", tex);
        mat->setLit(1);
        Saturn->setMaterial(mat);
        Saturn->setParentDistance(10);
        Saturn->daySpeed = 2.0;
        Saturn->yearSpeed = 0.15;
        Saturn->setScale(glm::vec3(1.2));
    }
    Planet* Rings = new Planet(Saturn, scene);{
        Rings->openOBJ("../Resources/Models/obj/Rings.obj");
        Texture tex = Texture("../Resources/Textures/Planets/2k_saturn_ring_alpha.png");
        Material* mat = new Material(glm::vec3(0.5, 0.5, 0.5));
        setStandardShader(Rings, mat, standardShader);
        mat->addTexture("texture0", tex);
        mat->setLit(1);
        Rings->setMaterial(mat);
        Rings->setParentDistance(0);
        Rings->setScale(glm::vec3(1));
    }
    return scene;
}
