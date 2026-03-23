#include "lod.h"

LOD::LOD() {}

void LOD::process(float deltaTime){
    Node::process(deltaTime);
    camDistance = glm::l2Norm(cam->position-position);
    int newLOD = 0;
    while(newLOD < lod_meshes.size() && lod_meshes[newLOD].distance < camDistance){
        newLOD++;
    }
    if(currentLOD != newLOD){
        if(newLOD < lod_meshes.size()){
            if(currentLOD < lod_meshes.size()) {
                lod_meshes[currentLOD].mesh->visible = false;
            }
            lod_meshes[newLOD].mesh->visible = true;
        }
        else{
            if(currentLOD < lod_meshes.size()) lod_meshes[currentLOD].mesh->visible = false;
        }
        currentLOD = newLOD;
    }
    else if(currentLOD < lod_meshes.size()){
        lod_meshes[currentLOD].mesh->visible = true;
    }
}

RayIntersection LOD::intersect(glm::vec3 const &origin, glm::vec3 const &direction, float const &length ) {
    if(currentLOD < lod_meshes.size()){
        return lod_meshes[currentLOD].mesh->intersect(origin, direction, length);
    }
    else{
        RayIntersection intersection;
        intersection.intersectionExists = false;
        return intersection;
    }
}

void LOD::addLOD(Mesh* me, float di, float ma){
    int i = 0;
    while(i < lod_meshes.size() && lod_meshes[i].distance < di){
        i++;
    }
    if(lod_meshes.size() > 0) {
        me->visible = false;
    }
    lod_meshes.insert(lod_meshes.begin()+i, mesh_distance(me, di, ma));
    addChild(me);
}
