#pragma once

#include "../3dEntities/node3d.h"
#include "Shapes/shape.h"

class Mesh;
class RigidBody3D;
class Shader;

class Collider3D : public Node3d
{
public:
    Collider3D();
    RayIntersection raycast(glm::vec3 const &origin, glm::vec3 const &direction, float const &length, uint64_t mask);
    bool active = true;
    void setShape(Shape* s);
    void SetShape(Shape* s) { setShape(s); }
    Shape* getShape() const { return shape; }
    bool computeAabb(PhysicsAabb& outAabb) const;
    void setDebug(bool b, Shader* debugShader = nullptr);
    glm::mat3 getInertia() {return shape->inertia;}
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
