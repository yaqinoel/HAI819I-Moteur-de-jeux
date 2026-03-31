#ifndef LOD_H
#define LOD_H

#include <common/3dEntities/Mesh.hpp>


struct mesh_distance{
    Mesh* mesh;
    float distance;
    float margin;
    mesh_distance(Mesh* me, float di, float ma){
        mesh = me;
        distance = di;
        margin = ma;
    }
};

class LOD : public Mesh
{
public:
    LOD();
    void process(float deltaTime) override;
    void addLOD(Mesh* me, float di, float ma=5);
private:
    float camDistance;
    float size;
    int currentLOD = 0;
    std::vector<mesh_distance> lod_meshes = std::vector<mesh_distance>();
};

#endif // LOD_H
