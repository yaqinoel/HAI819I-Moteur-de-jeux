#pragma once



#include "../Mesh.hpp"

class Planet : public Mesh
{
public:
    Planet(Planet* parent, Node* root);
    Planet(Node* parent);
    Planet():Mesh(){}
    void setParentDistance(float d) {setLocalPosition(parentEmpty->forward()*d);}
    void process(float deltaTime) override;
    float daySpeed;
    float yearSpeed;
    Planet* parentPlanet = nullptr;
    Node3d* parentEmpty = nullptr;
};
