#include "meshkdtree.h"
#include "../3dEntities/Mesh.hpp"



bool meshKdTree::point_inside(glm::vec3 o, glm::vec3 s, glm::vec3 p){
    for(int i = 0; i < 3; i ++){
        if(p[i] < o[i] || p[i] > o[i] + s[i]){
            return false;
        }
    }
    return true;
}

bool meshKdTree::triangleBoxOverlap(const glm::vec3& boxOrigin, const glm::vec3& boxSize, int triangle)
{
    glm::vec3 boxHalfSize = boxSize/2.0f;
    glm::vec3 boxCenter = boxOrigin+boxHalfSize;

    glm::vec3 v0 = mesh->vertices[mesh->triangles[triangle][0]].position;
    glm::vec3 v1 = mesh->vertices[mesh->triangles[triangle][1]].position;
    glm::vec3 v2 = mesh->vertices[mesh->triangles[triangle][2]].position;
    glm::vec3 tv0 = v0 - boxCenter;
    glm::vec3 tv1 = v1 - boxCenter;
    glm::vec3 tv2 = v2 - boxCenter;

    glm::vec3 e0 = tv1 - tv0;
    glm::vec3 e1 = tv2 - tv1;
    glm::vec3 e2 = tv0 - tv2;

    float min, max, p0, p1, p2, rad;

    // 1) Test 9 axes (edge × axis)
    // X-axis
    rad = boxHalfSize[1] * fabs(e0[2]) + boxHalfSize[2] * fabs(e0[1]);
    p0 = tv0[1] * e0[2] - tv0[2] * e0[1];
    p2 = tv2[1] * e0[2] - tv2[2] * e0[1];
    if (std::min(p0, p2) > rad || std::max(p0, p2) < -rad) return false;

    rad = boxHalfSize[1] * fabs(e1[2]) + boxHalfSize[2] * fabs(e1[1]);
    p0 = tv0[1] * e1[2] - tv0[2] * e1[1];
    p1 = tv1[1] * e1[2] - tv1[2] * e1[1];
    if (std::min(p0, p1) > rad || std::max(p0, p1) < -rad) return false;

    rad = boxHalfSize[1] * fabs(e2[2]) + boxHalfSize[2] * fabs(e2[1]);
    p1 = tv1[1] * e2[2] - tv1[2] * e2[1];
    p2 = tv2[1] * e2[2] - tv2[2] * e2[1];
    if (std::min(p1, p2) > rad || std::max(p1, p2) < -rad) return false;

    // Y-axis
    rad = boxHalfSize[0] * fabs(e0[2]) + boxHalfSize[2] * fabs(e0[0]);
    p0 = tv0[2] * e0[0] - tv0[0] * e0[2];
    p2 = tv2[2] * e0[0] - tv2[0] * e0[2];
    if (std::min(p0, p2) > rad || std::max(p0, p2) < -rad) return false;

    rad = boxHalfSize[0] * fabs(e1[2]) + boxHalfSize[2] * fabs(e1[0]);
    p0 = tv0[2] * e1[0] - tv0[0] * e1[2];
    p1 = tv1[2] * e1[0] - tv1[0] * e1[2];
    if (std::min(p0, p1) > rad || std::max(p0, p1) < -rad) return false;

    rad = boxHalfSize[0] * fabs(e2[2]) + boxHalfSize[2] * fabs(e2[0]);
    p1 = tv1[2] * e2[0] - tv1[0] * e2[2];
    p2 = tv2[2] * e2[0] - tv2[0] * e2[2];
    if (std::min(p1, p2) > rad || std::max(p1, p2) < -rad) return false;

    // Z-axis
    rad = boxHalfSize[0] * fabs(e0[1]) + boxHalfSize[1] * fabs(e0[0]);
    p0 = tv0[0] * e0[1] - tv0[1] * e0[0];
    p2 = tv2[0] * e0[1] - tv2[1] * e0[0];
    if (std::min(p0, p2) > rad || std::max(p0, p2) < -rad) return false;

    rad = boxHalfSize[0] * fabs(e1[1]) + boxHalfSize[1] * fabs(e1[0]);
    p0 = tv0[0] * e1[1] - tv0[1] * e1[0];
    p1 = tv1[0] * e1[1] - tv1[1] * e1[0];
    if (std::min(p0, p1) > rad || std::max(p0, p1) < -rad) return false;

    rad = boxHalfSize[0] * fabs(e2[1]) + boxHalfSize[1] * fabs(e2[0]);
    p1 = tv1[0] * e2[1] - tv1[1] * e2[0];
    p2 = tv2[0] * e2[1] - tv2[1] * e2[0];
    if (std::min(p1, p2) > rad || std::max(p1, p2) < -rad) return false;

    // 2) Test AABB axes
    FINDMINMAX(tv0[0], tv1[0], tv2[0], min, max);
    if (min > boxHalfSize[0] || max < -boxHalfSize[0]) return false;

    FINDMINMAX(tv0[1], tv1[1], tv2[1], min, max);
    if (min > boxHalfSize[1] || max < -boxHalfSize[1]) return false;

    FINDMINMAX(tv0[2], tv1[2], tv2[2], min, max);
    if (min > boxHalfSize[2] || max < -boxHalfSize[2]) return false;

    // 3) Test triangle normal
    glm::vec3 normal = glm::cross(e0, e1);
    float d = glm::dot(normal, tv0);
    rad = boxHalfSize[0] * fabs(normal[0]) +
          boxHalfSize[1] * fabs(normal[1]) +
          boxHalfSize[2] * fabs(normal[2]);
    if (fabs(d) > rad) return false;

    return true; // No separating axis found
}

void meshKdTree::insertTriangle(std::vector<int> &triangles, int triangle, int separation_direction){
    for(int i = 0; i < (int)triangles.size(); i ++){
        if(mesh->getBarycentre(triangles[i])[separation_direction] > mesh->getBarycentre(triangle)[separation_direction]){
            triangles.insert(triangles.begin() + i, triangle);
            return;
        }
    }
    triangles.push_back(triangle);
    return;
}

meshKdTree::meshKdTree(std::vector<int> triangles, Mesh* mesh, const glm::vec3 boxOrigin, const glm::vec3 boxSize, int depth, int separation_direction){
    child0 = nullptr;
    child1 = nullptr;
    this->mesh = mesh;
    this->triangles = triangles;
    this->boxOrigin = boxOrigin;
    this->boxSize = boxSize;
    this->depth = depth;
    std::vector<int> triangles_inside_sorted;
    for(int t: triangles){
        std::cout << t <<std::endl;
        if(point_inside(boxOrigin, boxSize, mesh->getBarycentre(t))){
            triangles_inside_sorted.push_back(t);
        }
    }
    std::sort(triangles.begin(), triangles.end(),
              [mesh = this->mesh, separation_direction](int a, int b)
              {
                  return mesh->getBarycentre(a)[separation_direction] <
                         mesh->getBarycentre(b)[separation_direction];
              });
    if(triangles_inside_sorted.size() > 8 && depth > 0){
        //if there are more than 1 barycenter in the box, we create the two children
        std::vector<int> c0_triangles = std::vector<int>();
        std::vector<int> c1_triangles = std::vector<int>();
        glm::vec3 centerPosition = mesh->getBarycentre(triangles_inside_sorted[triangles_inside_sorted.size()/2]);
        float centerRatio = (centerPosition[separation_direction]-boxOrigin[separation_direction])/(boxSize[separation_direction]);
        glm::vec3 c0_origin = glm::vec3(boxOrigin);
        glm::vec3 c0_size = glm::vec3(boxSize);
        c0_size[separation_direction] = boxSize[separation_direction]*centerRatio;
        glm::vec3 c1_origin = glm::vec3(boxOrigin);
        glm::vec3 c1_size = glm::vec3(boxSize);
        c1_size[separation_direction] = boxSize[separation_direction]*(1-centerRatio);
        c1_origin[separation_direction] += (boxSize[separation_direction] - c1_size[separation_direction]);
        for(int t: triangles){
            if(triangleBoxOverlap(c0_origin, c0_size, t)){
                c0_triangles.push_back(t);
            }
            if(triangleBoxOverlap(c1_origin, c1_size, t)){
                c1_triangles.push_back(t);
            }
        }
        int newSeparation = (separation_direction+1)%3;
        child0 = new meshKdTree(c0_triangles, mesh, c0_origin, c0_size, depth-1, newSeparation);
        child1 = new meshKdTree(c1_triangles, mesh, c1_origin, c1_size, depth-1, newSeparation);
    }
}

bool meshKdTree::rayIntersectsBox(glm::vec3 const &origin, glm::vec3 const &direction, float const &length, float &tNear, float &tFar){
    tNear = -std::numeric_limits<float>::infinity();
    tFar  =  std::numeric_limits<float>::infinity();
    glm::vec3 boxMax = boxOrigin + boxSize;
    for (int i = 0; i < 3; i++) {
        if (direction[i] == 0.0f) {
            if (origin[i] < origin[i] || origin[i] > boxMax[i])
                return false;
        } else {
            float t1 = (boxOrigin[i] - origin[i]) / direction[i];
            float t2 = (boxMax[i] - origin[i]) / direction[i];
            if (t1 > t2) std::swap(t1, t2);
            tNear = std::max(tNear, t1);
            tFar  = std::min(tFar, t2);

            if (tNear > tFar) {
                return false;
            }
            if (tFar < 0) {
                return false;
            }
            if(tFar > length){
                return false;
            }
        }
    }
    return true;
}

RayIntersection meshKdTree::rayThroughTree(glm::vec3 const &origin, glm::vec3 const &direction, float const &length){
    RayIntersection closestIntersection;
    closestIntersection.t = FLT_MAX;
    closestIntersection.intersectionExists = false;
    if(child0 == nullptr || child1 == nullptr){
        for(int i = 0; i < (int)triangles.size(); i ++){
            RayIntersection newIntersection = mesh->intersectTriangle(triangles[i], origin, direction, length);
            if (newIntersection.intersectionExists && newIntersection.t < closestIntersection.t && glm::dot(newIntersection.normal, direction) < 0){
                closestIntersection = newIntersection;
            }
        }
    }
    else{
        float t0Near, t1Near, t0Far, t1Far;
        bool hit0 = child0->rayIntersectsBox(origin, direction, length, t0Near, t0Far);
        bool hit1 = child1->rayIntersectsBox(origin, direction, length, t1Near, t1Far);
        if (!hit0 && !hit1){
            return closestIntersection;
        }

        meshKdTree* first = nullptr;
        meshKdTree* second = nullptr;
        float secondNear = 0.0f;

        if (hit0 && hit1) {
            if (t0Near < t1Near) {
                first = child0;
                second = child1;
                secondNear = t1Near;
            } else {
                first = child1;
                second = child0;
                secondNear = t0Near;
            }
        }
        else if (hit0) {
            first = child0;
        }
        else {
            first = child1;
        }

        closestIntersection = first->rayThroughTree(origin, direction, length);

        if (second && (!closestIntersection.intersectionExists || secondNear < closestIntersection.t)){
            RayIntersection other = second->rayThroughTree(origin, direction, length);
            if (other.intersectionExists && other.t < closestIntersection.t) {
                closestIntersection = other;
            }
        }
    }
    return closestIntersection;
}
