#include "scene.h"
#include "node.h"
#include "3dEntities/Mesh.hpp"
#include "3dEntities/camera.h"
#include "Physics/rigidbody3d.h"
#include "Physics/collider3d.h"
#include <ctime>

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
    for(Node* child: node->getChildren()){
        addToTree(child);
    }
}

void Scene::instantiate(Node* node){
    instantiate(node, root);
}

void Scene::process(float deltaTime){
    inputManager->UpdateInputs();
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
void Scene::physicsProcess() {
    physicsWorld.step(rigidBodies, colliders, fixedDeltaTime);
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
    if (Collider3D* c = dynamic_cast<Collider3D*>(node)) {
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

RayIntersection Scene::raycast(glm::vec3 const &origin, glm::vec3 const &direction, float const &length, uint64_t mask) {
    std::clock_t start = std::clock();
    RayIntersection closestIntersection;
    closestIntersection.t = FLT_MAX;
    closestIntersection.intersectionExists = false;
    for (Collider3D* c: colliders)
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
