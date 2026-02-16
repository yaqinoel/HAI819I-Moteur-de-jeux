#include "node3d.h"

Node3d::Node3d()
{

}

glm::mat4 Node3d::globalMatrix() const{
    if(parent != nullptr){
        return parent->globalMatrix()*localMatrix();
    }
    return localMatrix();
}
