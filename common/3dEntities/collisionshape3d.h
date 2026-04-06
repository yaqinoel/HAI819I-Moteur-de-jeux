#pragma once

#include "node3d.h"
#include "../Shapes/shape.h"

class CollisionShape3D : public Node3d
{
public:
    CollisionShape3D();
    RayIntersection raycast(glm::vec3 const &origin, glm::vec3 const &direction, float const &length);
    bool active = true;
    void SetShape(Shape* s);
    void setDebug(bool b);
    ColliderIntersection intersect(CollisionShape3D* other);
    glm::mat3 getInertia() {return shape->inertia;}
    std::vector<ColliderIntersection> collisions = std::vector<ColliderIntersection>();
    RigidBody3D *rb = nullptr;
    float mass = 1000;
private:
    Shape *shape = nullptr;
    bool debug = false;
    Mesh *debugMesh = nullptr;
};

