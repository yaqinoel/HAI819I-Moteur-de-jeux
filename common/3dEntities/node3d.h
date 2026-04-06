#ifndef NODE3D
#define NODE3D

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../node.h"

#define FORWARD glm::vec3(0, 0, 1)
#define BACKWARDS glm::vec3(0, 0, -1)
#define UP glm::vec3(0, 1, 0)
#define DOWN glm::vec3(0, -1, 0)
#define RIGHT glm::vec3(-1, 0, 0)
#define LEFT glm::vec3(1, 0, 0)

class Node3d : public Node
{
public:
    Node3d();
    virtual ~Node3d(){};
    glm::mat4 getInverseGlobalMatrix() const;
    glm::mat4 getGlobalMatrix() const override;
    virtual glm::vec3 getGlobalPosition() const;
    virtual glm::quat getGlobalRotation() const;
    glm::mat4 getLocalMatrix() const;
    glm::vec3 getLocalPosition() const;
    glm::quat getLocalRotation() const;
    glm::vec3 getScale() const {return scale;}
    glm::vec3 eulerAngles() const {return glm::eulerAngles(localRotation);}
    glm::vec3 forward() const {return getGlobalRotation()*FORWARD;}
    glm::vec3 backwards() const {return getGlobalRotation()*BACKWARDS;}
    glm::vec3 up() const {return getGlobalRotation()*UP;}
    glm::vec3 down() const {return getGlobalRotation()*DOWN;}
    glm::vec3 right() const {return getGlobalRotation()*RIGHT;}
    glm::vec3 left() const {return getGlobalRotation()*LEFT;}

    glm::vec3 localToWorld(glm::vec3 p){return glm::vec3(getGlobalMatrix() * glm::vec4(p, 1.0f));}
    glm::vec3 worldToLocal(glm::vec3 p){return glm::vec3(getInverseGlobalMatrix()*glm::vec4(p, 1.0f));}

    void addChild(Node* c) override;
    void setParent(Node* p) override;
    void Rotate(const float angleRadians, const glm::vec3& axis) { glm::quat delta = glm::angleAxis(angleRadians, glm::normalize(axis)); localRotation = glm::normalize(localRotation * delta);markDirty();}
    virtual void setGlobalPosition(const glm::vec3  &globPos);
    virtual void setGlobalRotation(const glm::quat  &globRot);
    virtual void setLocalPosition(const glm::vec3 pos) override {localPosition = pos; markDirty();};
    void SetLocalRotation(const glm::vec3 eulerRotation){localRotation = glm::normalize(glm::quat(glm::radians(eulerRotation))); markDirty();}
    void SetLocalRotation(const glm::quat rotation){localRotation = glm::normalize(rotation); markDirty();}
    void SetForward(const glm::vec3& forward);
    void SetRight(const glm::vec3& right);
    void setScale(const glm::vec3& newScale){scale = newScale; markDirty();}
    virtual void Translate(const glm::vec3 translation){localPosition += translation; markDirty();}
    void markDirty() override {inverseDirty = true; Node::markDirty();}

    void unDirty() const override;
    void unInverseDirty() const;
private:
    mutable glm::vec3 localPosition = glm::vec3(0);
    mutable glm::vec3 globalPosition = glm::vec3(0);
    mutable glm::quat localRotation = glm::quat();
    mutable glm::quat globalRotation = glm::quat();
    mutable glm::mat4 localMatrix = glm::mat4(1);
    mutable glm::mat4 globalMatrix = glm::mat4(1);
    mutable glm::mat4 inverseGlobalMatrix = glm::mat4(1);
    glm::vec3 scale = glm::vec3(1);
    void decompose(const glm::mat4& m, glm::vec3& position, glm::quat& rotation, glm::vec3& scale);
    mutable bool inverseDirty = true;
};
#endif
