#include "planet.h"

Planet::Planet(Node* parent)  : Mesh() {
    setParent(parent);
}

Planet::Planet(Planet* parentPlanet, Node* root) : Mesh(){
    parentEmpty = new Node3d();
    this->parentPlanet = parentPlanet;
    parentEmpty->setParent(root);
    setParent(parentEmpty);
}

void Planet::process(float deltaTime){
    Node::process(deltaTime);
    Rotate(deltaTime*daySpeed, UP);
    if(parentEmpty != nullptr && parentPlanet != nullptr){
        parentEmpty->Rotate(yearSpeed*deltaTime, parentEmpty->up());
        parentEmpty->position = parentPlanet->globalPosition();
    }
}
