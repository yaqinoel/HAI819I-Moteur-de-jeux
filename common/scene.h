#pragma once

#include <vector>
#include <set>
#include "iostream"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <common/Utilities/InputManager.h>
#include <common/Physics/physicsworld.h>
#include "Utilities/rayintersection.h"

class Node;
class Mesh;
class Camera;
class RigidBody3D;
class Collider3D;

class Scene
{
public:
    Scene(Node* rootNode);
    void instantiate(Node* node, Node* parent);
    void instantiate(Node* node);
    void process(float deltaTime);
    void physicsProcess();
    RayIntersection raycast(glm::vec3 const &origin, glm::vec3 const &direction, float const &length, uint64_t mask = ~0ULL);
    void render(float alpha);
    void ping(){std::cout << "scene ping" << std::endl;}
    Camera* mainCamera = nullptr;
    InputManager* inputManager = new InputManager();
    bool inputHeld(std::string input){return inputManager->inputs[input]->currentlyHeld;}
    bool inputPressed(std::string input){return inputManager->inputs[input]->justPressed;}
    constexpr static float fixedDeltaTime = 0.03f;
private:
    std::vector<Mesh*> meshes = std::vector<Mesh*>();
    std::vector<Node*> nodes = std::vector<Node*>();
    std::vector<RigidBody3D*> rigidBodies = std::vector<RigidBody3D*>();
    std::vector<Collider3D*> colliders = std::vector<Collider3D*>();
    std::vector<Camera*> cameras = std::vector<Camera*>();
    PhysicsWorld physicsWorld;
    void addToTree(Node* node);
    void remove(Node * node);
    void removeFromTree(Node* node);
    Node* root = nullptr;
};
