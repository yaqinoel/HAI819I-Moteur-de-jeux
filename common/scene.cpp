#include "scene.h"
#include "node.h"
#include "3dEntities/Mesh.hpp"
#include "3dEntities/camera.h"
#include "Physics/rigidbody3d.h"
#include "Physics/collider3d.h"
#include "3dEntities/Lights/Light.hpp"
#include "Render/IBLEnvironment.hpp"
#include "shader.hpp"
#include <ctime>
#include <algorithm>
#include <cmath>
#include <glm/gtc/type_ptr.hpp>

Scene::Scene(Node* node)
{
    setShader();
    if(root == nullptr){
        root = node;
        root->scene = this;
        addToTree(root);
    }
    else{
        root->addChild(node);
    }
}

Scene::~Scene() {
    delete iblEnvironment;
}

void Scene::instantiate(Node* node, Node* parent){
    if(node->scene != nullptr){
        return;
    }
    addToTree(node);
    parent->addChild(node);
}


void Scene::addToTree(Node* node){
    node->scene = this;
    nodes.push_back(node);
    if (Mesh* m = dynamic_cast<Mesh*>(node)) {
        meshes.push_back(m);
    }
    if (Camera* c = dynamic_cast<Camera*>(node)) {
        cameras.push_back(c);
    }
    if (RigidBody3D* r = dynamic_cast<RigidBody3D*>(node)) {
        rigidBodies.push_back(r);
    }
    if (Collider3D* c = dynamic_cast<Collider3D*>(node)) {
        colliders.push_back(c);
    }
    if (Light* l = dynamic_cast<Light*>(node)) {
        lights.push_back(l);
    }
    for(Node* child: node->getChildren()){
        addToTree(child);
    }
}

void Scene::instantiate(Node* node){
    instantiate(node, root);
}

void Scene::updateLights(float deltaTime) {
    (void)deltaTime;
}

void Scene::process(float deltaTime){
    inputManager->UpdateInputs();
    updateLights(deltaTime);
    for(int i = 0; i < nodes.size();){
        Node* n = nodes[i];
        if(n->markedForErasure){
            remove(n);
        }
        else{
            if(n != nullptr && n->getVisible()){
                n->process(deltaTime);
            }
            i++;
        }
    }
}

void Scene::lateProcess(float deltaTime){
    for(Node* n : nodes){
        if(n != nullptr && !n->markedForErasure && n->getVisible()){
            n->lateProcess(deltaTime);
        }
    }
}

void Scene::physicsProcess() {
    physicsWorld.step(rigidBodies, colliders, fixedDeltaTime);
}

void Scene::updateInterpolation(float alpha) {
    for(RigidBody3D* rb : rigidBodies) {
        if(rb && rb->getVisible()) {
            rb->interpolate(alpha);
        }
    }
}

void Scene::removeFromTree(Node* node){
    nodes.erase(std::remove(nodes.begin(), nodes.end(), node), nodes.end());
    if (Mesh* m = dynamic_cast<Mesh*>(node)) {
        meshes.erase(std::remove(meshes.begin(), meshes.end(), m), meshes.end());
    }
    if (Camera* c = dynamic_cast<Camera*>(node)) {
        cameras.erase(std::remove(cameras.begin(), cameras.end(), c), cameras.end());
    }
    if (RigidBody3D* r = dynamic_cast<RigidBody3D*>(node)) {
        rigidBodies.erase(std::remove(rigidBodies.begin(), rigidBodies.end(), r), rigidBodies.end());
    }
    if (Collider3D* c = dynamic_cast<Collider3D*>(node)) {
        colliders.erase(std::remove(colliders.begin(), colliders.end(), c), colliders.end());
    }
    node->removeParent();
}


void Scene::setShader(){
    shaderPID = LoadShaders("../Shaders/vertex_shader_line.glsl", "../Shaders/fragment_shader_line.glsl");
    viewMatrixUniform = glGetUniformLocation(shaderPID, "view");
    modelMatrixUniform = glGetUniformLocation(shaderPID, "model");
    projectionMatrixUniform = glGetUniformLocation(shaderPID, "projection");
    lineColorUniform = glGetUniformLocation(shaderPID, "lineColor");
}

void Scene::remove(Node * node){
    std::vector<Node*>tempChildren = node->getChildren();
    for(Node* child: tempChildren){
        remove(child);
    }
    removeFromTree(node);
    delete node;
}

RayIntersection Scene::raycast(glm::vec3 const &origin, glm::vec3 const &direction, float const &length, uint64_t mask) {
    std::clock_t start = std::clock();
    RayIntersection closestIntersection;
    closestIntersection.t = FLT_MAX;
    closestIntersection.intersectionExists = false;
    for (Collider3D* c: colliders)
    {
        if(c != nullptr && c->getVisible()){
            RayIntersection newIntersection = c->raycast(origin, direction, length, mask);
            if (newIntersection.intersectionExists && newIntersection.collider == nullptr) {
                newIntersection.collider = c;
            }
            if (newIntersection.intersectionExists && newIntersection.collider != nullptr && newIntersection.t < closestIntersection.t){
                closestIntersection = newIntersection;
            }
        }
    }
    //std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;
    return closestIntersection;
}


void Scene::drawLine(glm::vec3 line_start, glm::vec3 line_end, glm::vec3 color){
    lines.push_back(line_start);
    lines.push_back(line_end);
    lines.push_back(color);
}

void Scene::renderLines()
{
    if (mainCamera == nullptr)
        return;

    if (lines.size() < 3 || (lines.size() % 3) != 0)
        return;

    GLuint lineVAO, lineVBO;

    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);

    glBindVertexArray(lineVAO);

    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);

    glUseProgram(shaderPID);

    glm::mat4 model(1.0f);

    glUniformMatrix4fv(
        modelMatrixUniform,
        1,
        GL_FALSE,
        glm::value_ptr(model)
        );

    glUniformMatrix4fv(
        viewMatrixUniform,
        1,
        GL_FALSE,
        glm::value_ptr(mainCamera->getViewMatrix())
        );

    glUniformMatrix4fv(
        projectionMatrixUniform,
        1,
        GL_FALSE,
        glm::value_ptr(mainCamera->getProjectionMatrix())
        );

    glEnableVertexAttribArray(0);

    for (size_t i = 0; i < lines.size(); i += 3)
    {
        glm::vec3 lineData[2] = {
            lines[i],     // start
            lines[i + 1]  // end
        };

        glm::vec3 color = lines[i + 2];

        glBufferData(GL_ARRAY_BUFFER, sizeof(lineData), lineData, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        glUniform3f(lineColorUniform, color.r, color.g, color.b);

        glDrawArrays(GL_LINES, 0, 2);
    }

    glDisableVertexAttribArray(0);

    glBindVertexArray(0);
    glUseProgram(0);

    glDeleteBuffers(1, &lineVBO);
    glDeleteVertexArrays(1, &lineVAO);
    lines.clear();
}
