#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
//#include <map>

class CollisionShape3D;

class ColliderIntersection
{
public:
    CollisionShape3D* collider = nullptr;
    float t;
    glm::vec3 axis;
    bool intersectionExist;
    std::vector<glm::vec3> contactPoints = std::vector<glm::vec3>();
    //std::map<CollisionShape3D*, float> collidionList = std::map<CollisionShape3D*, float>();
    void RaySceneIntersection() {
        intersectionExist = false;
        t = FLT_MAX;
    }
};
