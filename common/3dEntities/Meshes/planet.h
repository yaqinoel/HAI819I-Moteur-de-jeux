#pragma once



#include "../Mesh.hpp"

class Planet : public Mesh
{
public:
    Planet(){}
    void process(float deltaTime) override;
    float daySpeed;
    float yearSpeed;
};
