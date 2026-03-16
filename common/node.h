

#pragma once

#include <set>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

class Camera;

class Node
{
public:
    Node();
    Node(Node* parent);
    virtual ~Node(){
        //std::cout <<name << " start destruction with " <<children.size() << " children" <<std::endl;
        removeParent();
        std::vector<Node*>tempChildren = children;
        for(Node * c : tempChildren){
            //std::cout <<name << " destroy children " << c->name<<std::endl;
            if(c != nullptr) {
                delete(c);
            }
        }
        //std::cout <<name << " destroyed"<<std::endl;
    }
    std::vector<Node*>& getChildren();
    void addChild(Node* c);
    void addChildren(std::vector<Node*> c);
    void removeChild(Node* c);
    void removeParent();
    bool hasChild(Node* node);
    bool hasParent();
    Node* getParent() const;
    void setParent(Node* p);
    virtual void process(float deltaTime) =0;
    virtual glm::mat4 globalMatrix() const=0;
    virtual void render(const Camera* camera) const=0;
    bool visible = true;
    std::string name;

protected:
    std::vector<Node*> children;
    Node* parent;
};
