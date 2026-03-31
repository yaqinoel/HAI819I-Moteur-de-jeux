#include "lod.h"

LOD::LOD() {}

void LOD::process(float deltaTime){
    Node::process(deltaTime);
    if(scene->mainCamera != nullptr){
        camDistance = glm::l2Norm(scene->mainCamera->position-position);
        int newLOD = 0;
        while(newLOD < lod_meshes.size() && lod_meshes[newLOD].distance < camDistance){
            newLOD++;
        }
        if(currentLOD != newLOD){
            if(newLOD < lod_meshes.size()){
                if(currentLOD < lod_meshes.size()) {
                    lod_meshes[currentLOD].mesh->setVisible(false);
                }
                lod_meshes[newLOD].mesh->setVisible(true);
            }
            else{
                if(currentLOD < lod_meshes.size()) lod_meshes[currentLOD].mesh->setVisible(false);
            }
            currentLOD = newLOD;
        }
        else if(currentLOD < lod_meshes.size()){
            lod_meshes[currentLOD].mesh->setVisible(true);
        }
    }
    else{
        for(mesh_distance md : lod_meshes){
            md.mesh->setVisible(false);
        }
    }
}

void LOD::addLOD(Mesh* me, float di, float ma){
    int i = 0;
    while(i < lod_meshes.size() && lod_meshes[i].distance < di){
        i++;
    }
    if(lod_meshes.size() > 0) {
        me->setVisible(false);
    }
    lod_meshes.insert(lod_meshes.begin()+i, mesh_distance(me, di, ma));
    instantiate(me, this);
}
