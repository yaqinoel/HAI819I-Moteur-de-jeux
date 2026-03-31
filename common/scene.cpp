#include "scene.h"
#include "node.h"
#include "3dEntities/Mesh.hpp"
#include "3dEntities/camera.h"
#include "3dEntities/rigidbody3d.h"
#include "3dEntities/collisionshape3d.h"
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
    addToTree(node);
    parent->addChild(node);
}


void Scene::addToTree(Node* node){
    nodes.insert(node);
    if (Mesh* m = dynamic_cast<Mesh*>(node)) {
        meshes.insert(m);
    }
    if (Camera* c = dynamic_cast<Camera*>(node)) {
        cameras.insert(c);
    }
    if (RigidBody3D* r = dynamic_cast<RigidBody3D*>(node)) {
        rigidBodies.insert(r);
    }
    if (CollisionShape3D* c = dynamic_cast<CollisionShape3D*>(node)) {
        colliders.insert(c);
    }
    for(Node* child: node->getChildren()){
        addToTree(child);
    }
}

void Scene::instantiate(Node* node){
    instantiate(node, root);
}

void Scene::process(float deltaTime){
    for(Node* n : nodes){
        if(n != nullptr && n->getVisible()){
            n->process(deltaTime);
        }
    }
}

void Scene::physicsProcess(float fixedDeltaTime){
    for(RigidBody3D* rb : rigidBodies){
        if(rb != nullptr && rb->getVisible()){
            rb->physicsProcess(fixedDeltaTime);
        }
    }
    for(RigidBody3D* rb : rigidBodies){
        if(rb != nullptr && rb->getVisible()){
            rb->postPhysicsProcess(fixedDeltaTime);
        }
    }
}

void Scene::render(){
    if(cameras.size() > 0){
        if(mainCamera == nullptr) mainCamera = *cameras.begin();
        for(Mesh* m: meshes){
            if(m != nullptr && m->getVisible()){
                m->render(mainCamera);
            }
        }
    }
}

void Scene::removeFromTree(Node* node){
    nodes.erase(node);
    if (Mesh* m = dynamic_cast<Mesh*>(node)) {
        meshes.erase(m);
    }
    if (Camera* c = dynamic_cast<Camera*>(node)) {
        cameras.erase(c);
    }
    if (RigidBody3D* r = dynamic_cast<RigidBody3D*>(node)) {
        rigidBodies.erase(r);
    }
    if (CollisionShape3D* c = dynamic_cast<CollisionShape3D*>(node)) {
        colliders.erase(c);
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
