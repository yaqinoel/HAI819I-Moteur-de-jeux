#pragma once

#include "shape.h"
#include "../Utilities/Geometry.h"
#include <embree4/rtcore.h>
#include "../3dEntities/collisionshape3d.h"

class MeshShape : public Shape
{
public:
    MeshShape();
    virtual ~MeshShape(){
        if(scene) rtcReleaseScene(scene);
        if(device) rtcReleaseDevice(device);
    }
    void InitMesh(std::vector<Vertex> vertices, std::vector<Triangle> triangles);

    void initTree();
    virtual RayIntersection raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length) override ;
private:
    RTCDevice device = nullptr;
    RTCScene scene = nullptr;
    RTCGeometry geometry = nullptr;
};


