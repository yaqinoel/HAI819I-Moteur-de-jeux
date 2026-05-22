#include "collider3d.h"
#include "../3dEntities/Mesh.hpp"

Collider3D::Collider3D()
{
}
RayIntersection Collider3D::raycast(glm::vec3 const &origin, glm::vec3 const &direction, float const &length, uint64_t mask){

    if(shape != nullptr){
        RayIntersection intersection = shape->raycast(origin, direction, length, mask);
        if (intersection.intersectionExists && intersection.collider == nullptr) {
            intersection.collider = this;
        }
        return intersection;
    }
    else{
        std::cerr << "Error no shape associated with the Collider3D" << std::endl;
        RayIntersection closestIntersection;
        closestIntersection.t = FLT_MAX;
        closestIntersection.intersectionExists = false;
        return closestIntersection;
    }
}

void Collider3D::setShape(Shape* s){
    shape = s;
    s->collider = this;
    shape->setMass(mass);
}

bool Collider3D::computeAabb(PhysicsAabb& outAabb) const {
    return shape && shape->computeAabb(*this, outAabb);
}

void Collider3D::setDebug(bool b, Shader* debugShader) {
    debug = b;
    if(debugMesh != nullptr){
        debugMesh->setVisible(b);
    }
    else if(b == true){
        debugMesh = new Mesh(shape->getVertices(), shape->getTriangles());
        debugMesh->name = "debug mesh";
        Material* mat = new Material();
        mat->shader = debugShader;
        debugMesh->setMaterial(mat);
        instantiate(debugMesh, this);
    }
}
