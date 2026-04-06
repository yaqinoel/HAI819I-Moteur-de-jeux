#include "scene.h"
#include "node.h"
#include "3dEntities/Mesh.hpp"
#include "3dEntities/camera.h"
#include "3dEntities/rigidbody3d.h"
#include "3dEntities/collisionshape3d.h"
#include <ctime>
#include <common/Constraint/contactconstraint.h>

Scene::Scene(Node* node)
{
    if(root == nullptr){
        root = node;
        root->scene = this;
        addToTree(root);
    }
    else{
        root->addChild(node);
    }
}

void Scene::instantiate(Node* node, Node* parent){
    addToTree(node);
    parent->addChild(node);
}


void Scene::addToTree(Node* node){
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
    for(Node* child: node->getChildren()){
        addToTree(child);
    }
}

void Scene::instantiate(Node* node){
    instantiate(node, root);
}

void Scene::process(float deltaTime){
    inputManager->UpdateInputs();
    for(Node* n : nodes){
        if(n != nullptr && n->getVisible()){
            n->process(deltaTime);
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
            auto col = colliders[i]->intersect(colliders[j]);
            if (col.intersectionExist)
                collisions.push_back(col);
        }

    for (const ColliderIntersection& col : collisions) {
        RigidBody3D* objA = col.colliderA->rb;
        RigidBody3D* objB = col.colliderB->rb;
        if (!objA && !objB) continue;

        if (objA) {
            for (const glm::vec3& pt : col.contactPoints)
                addConstraint(ContactConstraint( objA, objB, pt, col.axis, col.t, FeatureID(col.featureA, col.featureB)));
        } else {
            for (const glm::vec3& pt : col.contactPoints)
                addConstraint(ContactConstraint( objB, objA, pt, -col.axis, col.t, FeatureID(col.featureB, col.featureA)));
        }
    }
    for (ContactConstraint& c : newConstraints){
        c.init();
        c.warmStart();
        c.setUp(fixedDeltaTime);
    }

    const int solverIterations = 10;
    for (int i = 0; i < solverIterations; i++)
        for (ContactConstraint& c : newConstraints)
            c.solve();

    for (RigidBody3D* rb : rigidBodies)
        if (rb && rb->getVisible())
            rb->postPhysicsProcess();

    previousConstraints = newConstraints;
    newConstraints.clear();
    newConstraints.reserve(previousConstraints.size() * 2);
}

void Scene::render(float alpha){
    if(cameras.size() > 0){
        if(mainCamera == nullptr) mainCamera = *cameras.begin();
        for(RigidBody3D* rb : rigidBodies){
            if(rb != nullptr && rb->getVisible()){
                rb->interpolate(alpha);
            }
        }
        for(Mesh* m: meshes){
            if(m != nullptr && m->getVisible()){
                m->render(mainCamera);
            }
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

void Scene::remove(Node * node){
    std::vector<Node*>tempChildren = node->getChildren();
    for(Node* child: tempChildren){
        remove(child);
    }
    removeFromTree(node);
    delete node;
}

RayIntersection Scene::raycast(glm::vec3 const &origin, glm::vec3 const &direction, float const &length ) {
    std::clock_t start = std::clock();
    RayIntersection closestIntersection;
    closestIntersection.t = FLT_MAX;
    closestIntersection.intersectionExists = false;
    for (CollisionShape3D* c: colliders)
    {
        if(c != nullptr && c->getVisible()){
            RayIntersection newIntersection = c->raycast(origin, direction, length);
            if (newIntersection.intersectionExists && newIntersection.t < closestIntersection.t){
                closestIntersection = newIntersection;
            }
        }
    }
    //std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;
    return closestIntersection;
}
