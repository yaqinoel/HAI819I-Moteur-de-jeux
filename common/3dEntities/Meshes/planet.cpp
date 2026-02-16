#include "planet.h"

void Planet::process(float deltaTime){
    Node::process(deltaTime);
    Rotate(deltaTime*daySpeed, up());
    if(parent != nullptr){
        Node3d* p = static_cast<Node3d*>(parent);
        p->Rotate(yearSpeed*deltaTime, p->up());
    }
}
