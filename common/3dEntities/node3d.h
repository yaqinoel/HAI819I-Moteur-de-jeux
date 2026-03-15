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
    glm::vec3 position = glm::vec3(0);
    glm::vec3 globalPosition() const {return glm::vec3(globalMatrix()[3]);}
    glm::vec3 scale = glm::vec3(1);
    glm::quat rotation = glm::quat();
    glm::vec3 eulerAngles() const{return glm::eulerAngles(rotation);}
    glm::vec3 forward() const{return rotation*FORWARD;}
    glm::vec3 backwards() const{return rotation*BACKWARDS;}
    glm::vec3 up() const{return rotation*UP;}
    glm::vec3 down() const{return rotation*DOWN;}
    glm::vec3 right() const{return rotation*RIGHT;}
    glm::vec3 left() const{return rotation*LEFT;}
    void process(float deltaTime) override {for(Node * c : children)c->process(deltaTime);}
    void render(const Camera* camera) const override {for(Node * c : children)c->render(camera);}
    glm::mat4 localMatrix() const{return glm::translate(glm::mat4(1.0f), position)*glm::mat4_cast(rotation)*glm::scale(glm::mat4(1.0f), scale);}
    void Rotate(const float angleRadians, const glm::vec3& axis) { glm::quat delta = glm::angleAxis(angleRadians, glm::normalize(axis)); rotation = glm::normalize(rotation * delta);}
    void SetRotation(const glm::vec3 eulerRotation){rotation = glm::normalize(glm::quat(glm::radians(eulerRotation)));}
    void SetForward(const glm::vec3& forward)
    {
        glm::vec3 f = glm::normalize(forward);
        if (glm::length2(f) < 0.000001f) return;
        glm::vec3 r = glm::normalize(glm::cross(UP, f));
        glm::mat3 rotMatrix(r, glm::cross(f, r), f);
        rotation = glm::normalize(glm::quat_cast(rotMatrix));
    }
    void SetRight(const glm::vec3& right)
    {
        glm::vec3 r = glm::normalize(-right);
        if (glm::length2(r) < 0.000001f) return;
        glm::vec3 f = glm::normalize(glm::cross(UP, r));
        glm::vec3 u = glm::cross(f, r);
        glm::mat3 rotMatrix(r, u, f);
        rotation = glm::normalize(glm::quat_cast(rotMatrix));
    }
    glm::mat4 globalMatrix() const override;
    void Translate(const glm::vec3 translation){position += translation;}
};
#endif
