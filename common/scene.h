#pragma once

#include <vector>
#include <set>
#include "iostream"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <common/Utilities/InputManager.h>
#include <common/Utilities/colliderIntersection.h>
#include <common/Constraint/contactconstraint.h>
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
    void physicsProcess();
    void drawLine(glm::vec3 line_start, glm::vec3 line_end, glm::vec3 color = glm::vec3(0));
    RayIntersection raycast(glm::vec3 const &origin, glm::vec3 const &direction, float const &length, uint64_t mask = ~0ULL);
    void render(float alpha);
    void ping(){std::cout << "scene ping" << std::endl;}
    Camera* mainCamera = nullptr;
    InputManager* inputManager = new InputManager();
    bool inputHeld(std::string input){return inputManager->inputs[input]->currentlyHeld;}
    bool inputPressed(std::string input){return inputManager->inputs[input]->justPressed;}
    void addConstraint(ContactConstraint c);
    constexpr static float fixedDeltaTime = 0.03f;
private:
    std::vector<ColliderIntersection> collisions = std::vector<ColliderIntersection>();
    std::vector<Mesh*> meshes = std::vector<Mesh*>();
    std::vector<Node*> nodes = std::vector<Node*>();
    std::vector<RigidBody3D*> rigidBodies = std::vector<RigidBody3D*>();
    std::vector<CollisionShape3D*> colliders = std::vector<CollisionShape3D*>();
    std::vector<Camera*> cameras = std::vector<Camera*>();
    std::vector<ContactConstraint> previousConstraints;
    std::vector<ContactConstraint> newConstraints;
    void addToTree(Node* node);
    void remove(Node * node);
    void removeFromTree(Node* node);
    Node* root = nullptr;
    void setShader();
    void renderLines();
    std::vector<glm::vec3> lines = std::vector<glm::vec3>();


    mutable GLuint shaderPID;
    mutable GLuint modelMatrixUniform;
    mutable GLuint viewMatrixUniform;
    mutable GLuint projectionMatrixUniform;
    mutable GLuint lineColorUniform;
};
