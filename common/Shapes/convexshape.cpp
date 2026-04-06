#include "convexshape.h"

ConvexShape::ConvexShape()
{

}

void ConvexShape::InitMesh(std::vector<Vertex> vertices, std::vector<Triangle> triangles){
    for(Vertex v:vertices){
        this->vertices.push_back(v.position);
        if(radius*radius < glm::dot(v.position, v.position)){
            radius = glm::length(v.position);
            inertia += glm::dot(v.position, v.position) * glm::mat3(1.0f) - glm::outerProduct(v.position, v.position);
        }
    }
    for(Triangle t:triangles){
        this->triangles.push_back(glm::vec3(t[0], t[1], t[2]));
    }
    initTree();
    inertia *= mass/vertices.size();
}

void ConvexShape::initTree(){
    device = rtcNewDevice(nullptr);
    scene  = rtcNewScene(device);

    geometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

    struct EmbreeVertex {
        float x, y, z;
    };

    struct EmbreeTriangle {
        unsigned int v0, v1, v2;
    };

    EmbreeVertex* verts = (EmbreeVertex*) rtcSetNewGeometryBuffer(
        geometry,
        RTC_BUFFER_TYPE_VERTEX,
        0,
        RTC_FORMAT_FLOAT3,
        sizeof(EmbreeVertex),
        vertices.size()
        );

    for (size_t i = 0; i < vertices.size(); i++) {
        verts[i].x = vertices[i].x;
        verts[i].y = vertices[i].y;
        verts[i].z = vertices[i].z;
    }

    EmbreeTriangle* tris = (EmbreeTriangle*) rtcSetNewGeometryBuffer(
        geometry,
        RTC_BUFFER_TYPE_INDEX,
        0,
        RTC_FORMAT_UINT3,
        sizeof(EmbreeTriangle),
        triangles.size()
        );

    for (size_t i = 0; i < triangles.size(); i++) {
        tris[i].v0 = triangles[i][0];
        tris[i].v1 = triangles[i][1];
        tris[i].v2 = triangles[i][2];
    }

    rtcCommitGeometry(geometry);
    rtcAttachGeometry(scene, geometry);
    rtcReleaseGeometry(geometry);

    rtcCommitScene(scene);
}

RayIntersection ConvexShape::raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length)
{
    RayIntersection intersection;
    intersection.intersectionExists = false;

    RTCRayHit rayhit{};

    glm::vec3 dir = glm::normalize(direction);
    glm::vec3 globalPos = collider->getGlobalPosition();
    rayhit.ray.org_x = origin.x - globalPos.x;
    rayhit.ray.org_y = origin.y - globalPos.y;
    rayhit.ray.org_z = origin.z - globalPos.z;

    rayhit.ray.dir_x = dir.x;
    rayhit.ray.dir_y = dir.y;
    rayhit.ray.dir_z = dir.z;

    rayhit.ray.tnear = 0.0f;
    rayhit.ray.tfar  = length;

    rayhit.ray.time = 0.0f;
    rayhit.ray.mask = 0xFFFFFFFF;
    rayhit.ray.flags = 0;

    rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    rtcIntersect1(scene, &rayhit);

    if(rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID)
    {
        intersection.intersectionExists = true;
        intersection.t = rayhit.ray.tfar;

        intersection.point = origin + dir * intersection.t;

        intersection.normal = glm::normalize(glm::vec3(
            rayhit.hit.Ng_x,
            rayhit.hit.Ng_y,
            rayhit.hit.Ng_z
            ));

        intersection.collider = collider;
    }

    return intersection;
}
