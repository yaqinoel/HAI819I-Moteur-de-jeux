#pragma once

#include <vector>
#include <set>
#include "iostream"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GLFW/glfw3.h>
#include <common/Utilities/InputManager.h>
#include <common/Physics/physicsworld.h>
#include <common/Materials/pbrmaterial.h>
#include "Utilities/rayintersection.h"

class Node;
class Mesh;
class Camera;
class RigidBody3D;
class Collider3D;
class Light;
class IBLEnvironment;

class Scene
{
    friend class RenderSystem;
    friend class ForwardRenderSystem;
public:
    Scene(Node* rootNode);
    virtual ~Scene();
    void instantiate(Node* node, Node* parent);
    void instantiate(Node* node);
    void process(float deltaTime);
    void lateProcess(float deltaTime);
    void updateLights(float deltaTime);
    void physicsProcess();
    void drawLine(glm::vec3 line_start, glm::vec3 line_end, glm::vec3 color = glm::vec3(0));
    void renderLines();
    RayIntersection raycast(glm::vec3 const &origin, glm::vec3 const &direction, float const &length, uint64_t mask = ~0ULL);
    std::vector<Collider3D*> cubeOverlapTest(const glm::vec3& center, const glm::quat& rotation, const glm::vec3& size, uint64_t mask = ~0ULL);
    void updateInterpolation(float alpha);
    void ping(){std::cout << "scene ping" << std::endl;}
    Camera* mainCamera = nullptr;
    IBLEnvironment* iblEnvironment = nullptr;
    InputManager* inputManager = new InputManager();
    bool inputHeld(std::string input){return inputManager->inputs[input]->currentlyHeld;}
    bool inputPressed(std::string input){return inputManager->inputs[input]->justPressed;}
    bool inputReleased(std::string input){return inputManager->inputs[input]->justReleased;}
    constexpr static float fixedDeltaTime = 0.03f;
    bool gameExited = false;
    PBRMaterial* worldMaterial;
private:
    std::vector<Mesh*> meshes = std::vector<Mesh*>();
    std::vector<Light*> lights = std::vector<Light*>();
    std::vector<Node*> nodes = std::vector<Node*>();
    std::vector<RigidBody3D*> rigidBodies = std::vector<RigidBody3D*>();
    std::vector<Collider3D*> colliders = std::vector<Collider3D*>();
    std::vector<Camera*> cameras = std::vector<Camera*>();
    PhysicsWorld physicsWorld;
    void addToTree(Node* node);
    void remove(Node * node);
    void removeFromTree(Node* node);
    Node* root = nullptr;
    void setShader();
    std::vector<glm::vec3> lines = std::vector<glm::vec3>();


    mutable GLuint shaderPID;
    mutable GLuint modelMatrixUniform;
    mutable GLuint viewMatrixUniform;
    mutable GLuint projectionMatrixUniform;
    mutable GLuint lineColorUniform;
};
