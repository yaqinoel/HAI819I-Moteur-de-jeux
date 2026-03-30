#include "cube.h"

Cube::Cube(float size_x, float size_y, float size_z){
    this->size_x = size_x;
    this->size_y = size_y;
    this->size_z = size_z;

    vertices.resize(8);
    triangles.resize(12);

    vertices[0] = glm::vec3(-0.5*size_x, -0.5*size_y, 0.5*size_z);
    vertices[1] = glm::vec3(-0.5*size_x, 0.5*size_y, 0.5*size_z);
    vertices[2] = glm::vec3(-0.5*size_x, -0.5*size_y, -0.5*size_z);
    vertices[3] = glm::vec3(-0.5*size_x, 0.5*size_y, -0.5*size_z);
    vertices[4] = glm::vec3(0.5*size_x, -0.5*size_y, 0.5*size_z);
    vertices[5] = glm::vec3(0.5*size_x, 0.5*size_y, 0.5*size_z);
    vertices[6] = glm::vec3(0.5*size_x, -0.5*size_y, -0.5*size_z);
    vertices[7] = glm::vec3(0.5*size_x, 0.5*size_y, -0.5*size_z);
    radius = glm::length(vertices[0]);
    triangles[0] = glm::ivec3(1, 2, 0);
    triangles[1] = glm::ivec3(1, 3, 2);
    triangles[2] = glm::ivec3(3, 6, 2);
    triangles[3] = glm::ivec3(7, 4, 6);
    triangles[4] = glm::ivec3(3, 5, 7);
    triangles[5] = glm::ivec3(3, 7, 6);
    triangles[6] = glm::ivec3(6, 0, 2);
    triangles[7] = glm::ivec3(6, 4, 0);
    triangles[8] = glm::ivec3(5, 0, 4);
    triangles[9] = glm::ivec3(7, 5, 4);
    triangles[10] = glm::ivec3(5, 1, 0);
    triangles[11] = glm::ivec3(3, 1, 5);
}




RayIntersection Cube::raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length){
    RayIntersection intersection;
    intersection.intersectionExists = false;
    for(int i = 0; i < 12; i += 2){
        glm::vec3 pointA = vertices[triangles[i][0]];
        glm::vec3 pointB = vertices[triangles[i][1]];
        glm::vec3 pointC = vertices[triangles[i][2]];
        glm::vec3 normal = glm::cross(pointB-pointA, pointC-pointA);
        double denominator = glm::dot(direction, normal);
        if(denominator < 0){
            float t = (glm::dot(pointA-origin, normal))/denominator;
            glm::vec3 point = origin + direction * t;
            float distance1 = glm::dot(point - pointA, pointB)/glm::length2(pointB);
            float distance2 = glm::dot(point - pointA, pointC)/glm::length2(pointC);
            float distance = (point-origin).length();
            if(distance1 > 0. && distance1 < 1. && distance2 > 0. && distance2 < 1. && distance <= length && distance < intersection.t){
                intersection.intersectionExists = true;
                intersection.t = distance;
                intersection.point = point;
                intersection.normal = normal;
            }
        }
    }
    return intersection;
}
