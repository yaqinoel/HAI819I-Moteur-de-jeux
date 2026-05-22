#pragma once

#include "node3d.h"
#include "../Shapes/shape.h"

class Shader;

class CollisionShape3D : public Node3d
{
public:
    CollisionShape3D();
    RayIntersection raycast(glm::vec3 const &origin, glm::vec3 const &direction, float const &length, uint64_t mask);
    bool active = true;
    void SetShape(Shape* s);
    void setDebug(bool b, Shader* debugShader = nullptr);
    std::vector<ColliderIntersection> intersect(CollisionShape3D* other);
    glm::mat3 getInertia() {return shape->inertia;}
    std::vector<ColliderIntersection> collisions = std::vector<ColliderIntersection>();
    RigidBody3D *rb = nullptr;
    float mass = 1000;


    uint64_t collisionLayers = 1ULL;
    uint64_t collisionDetectionLayers = 1ULL;

    void addCollisionLayer(int l){collisionLayers |= l;}
    void addCollisionDetectionLayers(int l){collisionDetectionLayers |= l;}
    void removeCollisionLayer(int l){collisionLayers &= ~(1ULL << l);}
    void removeCollisionDetectionLayer(int l){collisionDetectionLayers &= ~(1ULL << l);}

private:
    Shape *shape = nullptr;
    bool debug = false;
    Mesh *debugMesh = nullptr;
};
