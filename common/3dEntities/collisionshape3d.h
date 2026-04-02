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
    void intersect(CollisionShape3D* other);
    std::vector<ColliderIntersection> collisions = std::vector<ColliderIntersection>();
private:
    Shape *shape = nullptr;
    bool debug = false;
    Mesh *debugMesh = nullptr;
};

