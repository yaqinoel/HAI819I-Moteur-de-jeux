#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
//#include <map>

class CollisionShape3D;

class ColliderIntersection
{
public:
    ColliderIntersection() {
        intersectionExist = false;
        t = INFINITY;
    }
    ColliderIntersection(CollisionShape3D* colliderA, CollisionShape3D* colliderB, float t, glm::vec3 axis, bool intersectionExist, std::vector<glm::vec3> contactPoints){
        this->colliderA = colliderA;
        this->colliderB = colliderB;
        this->t = t;
        this->axis = axis;
        this->intersectionExist = intersectionExist;
        this->contactPoints = contactPoints;
    }
    CollisionShape3D* colliderA = nullptr;
    CollisionShape3D* colliderB = nullptr;
    uint16_t featureA = 0;
    uint16_t featureB = 0;
    float t;
    glm::vec3 axis;
    bool intersectionExist;
    std::vector<glm::vec3> contactPoints = std::vector<glm::vec3>();
    ColliderIntersection inverse(){
        return ColliderIntersection(colliderB, colliderA, t, -axis, intersectionExist, contactPoints);
    }
};
