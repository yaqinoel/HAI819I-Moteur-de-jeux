

#pragma once

#include <set>
#include <glm/glm.hpp>

class Camera;

class Node
{
public:
    Node();
    Node(Node* parent);
    std::set<Node*> getChildren() const;
    void addChild(Node* c);
    void addChildren(std::set<Node*> c);
    void removeChild(Node* c);
    void removeParent();
    Node* getParent() const;
    void setParent(Node* p);
    virtual void process(float deltaTime) =0;
    virtual glm::mat4 globalMatrix() const=0;
    virtual void render(const Camera* camera) const=0;

protected:
    std::set<Node*> children;
    Node* parent;
};
