#include "node3d.h"

Node3d::Node3d() : Node()
{

}

void Node3d::unDirty() const{
    localMatrix = glm::translate(glm::mat4(1.0f), localPosition)*glm::mat4_cast(localRotation)*glm::scale(glm::mat4(1.0f), scale);
    if(parent != nullptr){
        globalMatrix = parent->getGlobalMatrix()*localMatrix;
        globalRotation = ((Node3d*)parent)->globalRotation * localRotation;
        globalPosition = glm::vec3(globalMatrix[3]);
    }
    else{
        globalMatrix = localMatrix;
        globalRotation = localRotation;
        globalPosition = localPosition;
    }
    dirty = false;
}

void Node3d::unInverseDirty() const{
    inverseGlobalMatrix = glm::inverse(globalMatrix);
    inverseDirty = false;
}

glm::mat4 Node3d::getInverseGlobalMatrix() const{
    if(inverseDirty)unInverseDirty();
    return inverseGlobalMatrix;
}

glm::mat4 Node3d::getGlobalMatrix() const{
    if(dirty)unDirty();
    return globalMatrix;
}

glm::mat4 Node3d::getLocalMatrix() const{
    if(dirty)unDirty();
    return localMatrix;
}

glm::vec3 Node3d::getGlobalPosition() const{
    if(dirty)unDirty();
    return globalPosition;
}

glm::quat Node3d::getGlobalRotation() const{
    if(dirty)unDirty();
    return globalRotation;
}

glm::vec3 Node3d::getLocalPosition() const {
    if(dirty)unDirty();
    return localPosition;
}
glm::quat Node3d::getLocalRotation() const {
    if(dirty)unDirty();
    return localRotation;
}

void Node3d::setParent(Node* p){
    glm::mat4 oldGlobal = getGlobalMatrix();

    Node::setParent(p);

    if(parent != nullptr){
        glm::mat4 invParent = glm::inverse(((Node3d*)parent)->getGlobalMatrix());
        localMatrix = invParent * oldGlobal;
        decompose(localMatrix, localPosition, localRotation, scale);
    } else {
        decompose(oldGlobal, localPosition, localRotation, scale);
    }

    markDirty();
}

void Node3d::addChild(Node* c){
    Node::addChild(c);
}

void Node3d::setGlobalPosition(const glm::vec3  &globPos){
    if(parent != nullptr){
        glm::mat4 invParent = parent->getInverseGlobalMatrix();
        localPosition = glm::vec3(invParent * glm::vec4(globPos, 1.0f));
    }
    else{
        localPosition = globPos;
    }
    markDirty();
}
void Node3d::setGlobalRotation(const glm::quat& globRot){
    glm::quat normalizedGlobal = glm::normalize(globRot);
    if(parent != nullptr){
        glm::quat parentGlobalRot = ((Node3d*)parent)->getGlobalRotation();
        glm::quat invParentRot = glm::inverse(parentGlobalRot);
        localRotation = glm::normalize(invParentRot * normalizedGlobal);
    }
    else{
        localRotation = normalizedGlobal;
    }
    markDirty();
}
void Node3d::setForward(const glm::vec3& forw)
{
    glm::vec3 f = glm::normalize(forw);
    if (glm::length2(f) < 0.000001f) return;
    glm::vec3 r = glm::normalize(glm::cross(UP, f));
    glm::vec3 u = glm::cross(f, r);
    glm::mat3 rotMatrix(r, u, f);
    setLocalRotation(glm::normalize(glm::quat_cast(rotMatrix)));
    markDirty();
}
void Node3d::setRight(const glm::vec3& right)
{
    glm::vec3 r = glm::normalize(-right);
    if (glm::length2(r) < 0.000001f) return;
    glm::vec3 f = glm::normalize(glm::cross(UP, r));
    glm::vec3 u = glm::cross(f, r);
    glm::mat3 rotMatrix(r, u, f);
    setLocalRotation(glm::normalize(glm::quat_cast(rotMatrix)));
    markDirty();
}

void Node3d::decompose(const glm::mat4& m, glm::vec3& position, glm::quat& rotation, glm::vec3& scale){
    position = glm::vec3(m[3]);

    glm::vec3 col0 = glm::vec3(m[0]);
    glm::vec3 col1 = glm::vec3(m[1]);
    glm::vec3 col2 = glm::vec3(m[2]);

    scale.x = glm::length(col0);
    scale.y = glm::length(col1);
    scale.z = glm::length(col2);

    if (scale.x < 1e-8f || scale.y < 1e-8f || scale.z < 1e-8f) {
        rotation = glm::quat(1, 0, 0, 0);
        return;
    }

    glm::mat3 rotMatrix;
    rotMatrix[0] = col0 / scale.x;
    rotMatrix[1] = col1 / scale.y;
    rotMatrix[2] = col2 / scale.z;

    if (glm::determinant(rotMatrix) < 0.0f) {
        scale = -scale;
        rotMatrix[0] = -rotMatrix[0];
        rotMatrix[1] = -rotMatrix[1];
        rotMatrix[2] = -rotMatrix[2];
    }

    rotation = glm::normalize(glm::quat_cast(rotMatrix));
}
