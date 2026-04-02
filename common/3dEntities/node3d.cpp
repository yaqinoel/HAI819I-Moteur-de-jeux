#include "node3d.h"

Node3d::Node3d() : Node()
{

}

void Node3d::unDirty() const{
    localMatrix = glm::translate(glm::mat4(1.0f), localPosition)*glm::mat4_cast(localRotation)*glm::scale(glm::mat4(1.0f), scale);
    if(parent != nullptr){
        globalMatrix = parent->getGlobalMatrix()*localMatrix;
    }
    else{
        globalMatrix = localMatrix;
    }
    dirty = false;
}

glm::mat4 Node3d::getGlobalMatrix() const{
    if(dirty){
        unDirty();
    }
    return globalMatrix;
}

glm::mat4 Node3d::getLocalMatrix() const{
    if(dirty){
        unDirty();
    }
    return localMatrix;
}

glm::vec3 Node3d::getGlobalPosition() const{
    if(dirty){
        unDirty();
        globalPosition = glm::vec3(globalMatrix[3]);
    }
    return globalPosition;
}

glm::quat Node3d::getGlobalRotation() const{
    if(dirty){
        unDirty();
        globalRotation = glm::quat_cast(extractRotation(globalMatrix));
    }
    return globalRotation;
}

void Node3d::setParent(Node* p){
    Node::setParent(p);
    localPosition -= ((Node3d*)p)->getGlobalPosition();
    markDirty();
}

void Node3d::addChild(Node* c){
    Node::addChild(c);
}

void Node3d::setGlobalPosition(const glm::vec3  &globPos){
    if(parent != nullptr){
        glm::mat4 parentGlobal = parent->getGlobalMatrix();
        glm::mat4 invParent = glm::inverse(parentGlobal);
        localPosition = glm::vec3(invParent * glm::vec4(globPos, 1.0f));
        markDirty();
        return;
    }
    localPosition = globPos;
    markDirty();
}
void Node3d::setGlobalRotation(const glm::quat& globRot){
    glm::quat normalizedGlobal = glm::normalize(globRot);
    if(parent != nullptr){
        glm::quat parentGlobalRot = ((Node3d*)parent)->getGlobalRotation();
        glm::quat invParentRot = glm::inverse(parentGlobalRot);
        localRotation = glm::normalize(invParentRot * normalizedGlobal);
        markDirty();
        return;
    }
    localRotation = normalizedGlobal;
    markDirty();
}
void Node3d::SetForward(const glm::vec3& forward)
{
    glm::vec3 f = glm::normalize(forward);
    if (glm::length2(f) < 0.000001f) return;
    glm::vec3 r = glm::normalize(glm::cross(UP, f));
    glm::mat3 rotMatrix(r, glm::cross(f, r), f);
    localRotation = glm::normalize(glm::quat_cast(rotMatrix));
    markDirty();
}
void Node3d::SetRight(const glm::vec3& right)
{
    glm::vec3 r = glm::normalize(-right);
    if (glm::length2(r) < 0.000001f) return;
    glm::vec3 f = glm::normalize(glm::cross(UP, r));
    glm::vec3 u = glm::cross(f, r);
    glm::mat3 rotMatrix(r, u, f);
    localRotation = glm::normalize(glm::quat_cast(rotMatrix));
    markDirty();
}
