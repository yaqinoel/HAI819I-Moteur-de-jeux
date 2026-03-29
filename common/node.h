

#pragma once

#include <set>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <glm/gtx/string_cast.hpp>
#include "scene.h"


class Camera;

class Node
{
public:
    Node();
    Node(Node* parent);
    virtual ~Node(){}
    std::vector<Node*>& getChildren();
    virtual void addChild(Node* c);
    void addChildren(std::vector<Node*> c);
    void removeChild(Node* c);
    void removeParent();
    bool hasChild(Node* node);
    bool hasParent();
    Node* getParent() const;
    virtual void setParent(Node* p);
    virtual void process(float deltaTime){};
    virtual glm::mat4 globalMatrix() const=0;
    std::string name;
    inline static Scene* scene;
    void instantiate(Node* node, Node* parent);
    bool getVisible() const {if(!visible) return false; else if(parent) return parent->getVisible(); else return true;}
    void setVisible(bool b){visible = b;}

protected:
    std::vector<Node*> children;
    Node* parent;
private:
    bool visible = true;
};
