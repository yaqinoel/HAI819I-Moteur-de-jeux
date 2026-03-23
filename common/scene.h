#pragma once

#include <vector>
#include <set>
#include "iostream"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "Utilities/rayintersection.h"

class Node;
class Mesh;
class Camera;
class RigidBody3D;

class Scene
{
public:
    Scene(Node* rootNode);
    void instantiate(Node* node, Node* parent);
    void instantiate(Node* node);
    void process(float deltaTime);
    RayIntersection raycast(glm::vec3 const &origin, glm::vec3 const &direction, float const &length);
    void render();
    void ping(){std::cout << "scene ping" << std::endl;}
    void remove(Node * node);
private:
    std::set<Mesh*> meshes = std::set<Mesh*>();
    std::set<Node*> nodes = std::set<Node*>();
    std::set<RigidBody3D*> rigidBodies = std::set<RigidBody3D*>();
    std::set<CollisionShape3D*> colliders = std::set<CollisionShape3D*>();
    std::set<Camera*> cameras = std::set<Camera*>();
    void addToTree(Node* node);
    void removeFromTree(Node* node);
    Camera* mainCamera = nullptr;
    Node* root;
};
