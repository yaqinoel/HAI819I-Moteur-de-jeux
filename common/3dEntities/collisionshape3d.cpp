#include "collisionshape3d.h"
#include "../3dEntities/Mesh.hpp"

CollisionShape3D::CollisionShape3D()
{
}
RayIntersection CollisionShape3D::raycast(glm::vec3 const &origin, glm::vec3 const &direction, float const &length, uint64_t mask){
    if(shape != nullptr){
        return shape->raycast(origin, direction, length, mask);
    }
    else{
        std::cerr << "Error no shape associated with the CollisionShape" << std::endl;
        RayIntersection closestIntersection;
        closestIntersection.t = FLT_MAX;
        closestIntersection.intersectionExists = false;
        return closestIntersection;
    }
}

void CollisionShape3D::SetShape(Shape* s){
    shape = s;
    s->collider = this;
    shape->setMass(mass);
}

bool CollisionShape3D::computeAabb(PhysicsAabb& outAabb) const {
    return shape && shape->computeAabb(*this, outAabb);
}

void CollisionShape3D::setDebug(bool b) {
    debug = b;
    if(debugMesh != nullptr){
        debugMesh->setVisible(b);
    }
    else if(b == true){
        debugMesh = new Mesh(shape->getVertices(), shape->getTriangles());
        debugMesh->name = "debug mesh";
        Material* mat = new Material();
        debugMesh->setShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader_shape.glsl");
        debugMesh->setMaterial(mat);
        instantiate(debugMesh, this);
    }
}
