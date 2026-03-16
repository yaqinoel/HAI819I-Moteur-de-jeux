#include "node.h"


Node::Node()
{
    children = std::vector<Node*>();
    parent = nullptr;
}

Node::Node(Node* parent){
    children = std::vector<Node*>();
    this->parent = parent;

}

std::vector<Node*>& Node::getChildren(){
    return children;
}

void Node::addChild(Node* c){
    //std::cout << c->name << " has as parent " << name << std::endl;
    if(c == nullptr){
        std::cerr << " Error : nullptr as an added child" << std::endl;
        return;
    }
    c->setParent(this);
}

bool Node::hasChild(Node* node){
    return std::find(children.begin(), children.end(), node)!= children.end();
}

bool Node::hasParent(){
    return parent!=nullptr;
}

void Node::addChildren(std::vector<Node*> c){
    for (Node* n : c) {
        children.push_back(n);
        n->setParent(this);
    }
}

Node* Node::getParent() const{
    return parent;
}

void Node::setParent(Node* p){
    // std::cout << p->name << " has as a child " << name << std::endl;
    Node* self = this;
    if(parent != p){
        if(parent != nullptr) parent->removeChild(self);
        parent = p;
        if(parent != nullptr) parent->children.push_back(this);
    }
}

void Node::removeChild(Node* c){
    children.erase(std::remove(children.begin(), children.end(), c), children.end());
    if(c->getParent() != nullptr){
        c->removeParent();
    }
}

void Node::removeParent(){
    if(parent == nullptr) return;

    Node* temp_parent = parent;
    parent = nullptr;
    temp_parent->removeChild(this);
}

void Node::process(float deltaTime){for(Node* n : children)n->process(deltaTime);};
