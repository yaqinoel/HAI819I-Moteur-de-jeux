#include "scene.h"
#include "node.h"
#include "3dEntities/Mesh.hpp"
#include "3dEntities/camera.h"
#include "3dEntities/rigidbody3d.h"
#include "3dEntities/collisionshape3d.h"
#include "3dEntities/Lights/Light.hpp"
#include "Render/IBLEnvironment.hpp"
#include "shader.hpp"
#include <ctime>
#include <common/Constraint/contactconstraint.h>
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
    if (CollisionShape3D* c = dynamic_cast<CollisionShape3D*>(node)) {
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
void Scene::addConstraint(ContactConstraint c) {
    for (size_t i = 0; i < previousConstraints.size(); i++) {
        ContactConstraint& old = previousConstraints[i];
        if (old.featureId == c.featureId && old.objA == c.objA && old.objB == c.objB) {
            c.accumulatedNormalLambda = old.accumulatedNormalLambda;
            c.isReused = true;
            newConstraints.push_back(c);
            previousConstraints.erase(previousConstraints.begin() + i);
            return;
        }
    }
    newConstraints.push_back(c);
}

void Scene::physicsProcess() {
    for (RigidBody3D* rb : rigidBodies)
        if (rb && rb->getVisible())
            rb->physicsProcess();

    collisions.clear();
    for (int i = 0; i < colliders.size(); i++)
        for (int j = i + 1; j < colliders.size(); j++) {
            std::vector<ColliderIntersection> cols = colliders[i]->intersect(colliders[j]);
            for(ColliderIntersection col : cols){
                if (col.intersectionExist)
                    collisions.push_back(col);
            }
        }
    for (const ColliderIntersection& col : collisions) {
        RigidBody3D* objA = col.colliderA->rb;
        RigidBody3D* objB = col.colliderB->rb;
        if (!objA && !objB) continue;

        uint16_t ptIndex = 0;
        if (objA) {
            for (const glm::vec3& pt : col.contactPoints) {
                addConstraint(ContactConstraint( objA, objB, pt, col.axis, col.t, FeatureID(col.featureA, col.featureB, ptIndex), objA->friction));
                ptIndex++;
            }
        } else {
            for (const glm::vec3& pt : col.contactPoints) {
                addConstraint(ContactConstraint( objB, objA, pt, -col.axis, col.t, FeatureID(col.featureB, col.featureA, ptIndex), objB->friction));
                ptIndex++;
            }
        }
    }
    for (ContactConstraint& c : newConstraints){
        c.setUp(fixedDeltaTime);
    }

    const int solverIterations = 10;
    for (int i = 0; i < solverIterations; i++)
        for (ContactConstraint& c : newConstraints)
            c.solve();

    for (RigidBody3D* rb : rigidBodies)
        if (rb && rb->getVisible())
            rb->postPhysicsProcess();

    solvePositionConstraints();

    previousConstraints = newConstraints;
    newConstraints.clear();
    newConstraints.reserve(previousConstraints.size() * 2);
}

void Scene::solvePositionConstraints() {
    const float slop = 0.005f;
    const float percent = 0.8f;
    const int positionIterations = 3;

    for (RigidBody3D* rb : rigidBodies) {
        if (rb) rb->solverTempPosition = rb->getCurrentPosition();
    }

    for (int iter = 0; iter < positionIterations; iter++) {
        for (const ColliderIntersection& col : collisions) {
            RigidBody3D* objA = col.colliderA->rb;
            RigidBody3D* objB = col.colliderB->rb;

            float invMassA = (objA && objA->mass > 0.0f) ? 1.0f / objA->mass : 0.0f;
            float invMassB = (objB && objB->mass > 0.0f) ? 1.0f / objB->mass : 0.0f;
            float sumInvMass = invMassA + invMassB;

            if (sumInvMass <= 0.0f) continue;

            glm::vec3 deltaA = objA ? (objA->getCurrentPosition() - objA->solverTempPosition) : glm::vec3(0.0f);
            glm::vec3 deltaB = objB ? (objB->getCurrentPosition() - objB->solverTempPosition) : glm::vec3(0.0f);

            float separationChange = glm::dot(deltaB - deltaA, col.axis);
            float currentPenetration = col.t - separationChange;

            if (currentPenetration <= slop) continue;

            float correctionScalar = (currentPenetration - slop) / sumInvMass * percent;
            glm::vec3 correctionVector = col.axis * correctionScalar;

            if (objA) {
                objA->Translate(-correctionVector * invMassA);
            }
            if (objB) {
                objB->Translate(correctionVector * invMassB);
            }
        }
    }
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
    if (CollisionShape3D* c = dynamic_cast<CollisionShape3D*>(node)) {
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
    for (CollisionShape3D* c: colliders)
    {
        if(c != nullptr && c->getVisible()){
            RayIntersection newIntersection = c->raycast(origin, direction, length, mask);
            if (newIntersection.intersectionExists && newIntersection.t < closestIntersection.t){
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
