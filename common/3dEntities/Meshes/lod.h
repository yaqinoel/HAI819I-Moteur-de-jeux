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
    void setCam(Camera const * const cam){this->cam = cam; camDistance = glm::l2Norm(cam->position-position);}
    void addLOD(Mesh* me, float di, float ma=5);
    RayIntersection intersect(glm::vec3 const &origin, glm::vec3 const &direction, float const &length );
private:
    float camDistance;
    Camera const* cam;
    float size;
    int currentLOD = 0;
    std::vector<mesh_distance> lod_meshes = std::vector<mesh_distance>();
};

#endif // LOD_H
