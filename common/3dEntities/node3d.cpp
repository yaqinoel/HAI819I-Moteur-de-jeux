#include "node3d.h"

Node3d::Node3d() : Node()
{
}

glm::mat4 Node3d::globalMatrix() const{
    if(parent != nullptr){
        return parent->globalMatrix()*localMatrix();
    }
    return localMatrix();
}

void Node3d::setParent(Node* p){
    Node::setParent(p);
    position -= ((Node3d*)p)->globalPosition();
    //std::cout << glm::to_string(position) << std::endl;
}

void Node3d::addChild(Node* c){
    Node::addChild(c);
    //((Node3d*)c)->position -= position;
    //std::cout << name << std::endl;
    //std::cout << glm::to_string(position) << std::endl;
}
