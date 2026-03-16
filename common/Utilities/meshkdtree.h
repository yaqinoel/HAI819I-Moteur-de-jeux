#ifndef MESHKDTREE_H
#define MESHKDTREE_H

#include "glm/detail/type_vec.hpp"
#include "rayintersection.h"
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#define FINDMINMAX(x0, x1, x2, min, max) \
min = max = x0; \
    if (x1 < min) min = x1; \
    if (x1 > max) max = x1; \
    if (x2 < min) min = x2; \
    if (x2 > max) max = x2;

class Mesh;

class meshKdTree
{
public:
    meshKdTree* child0;
    meshKdTree* child1;
    glm::vec3 boxOrigin;
    glm::vec3 boxSize;
    std::vector<int> triangles;
    Mesh* mesh;
    int depth;
    ~meshKdTree()
    {
        delete child0;
        delete child1;
    }
    bool point_inside(glm::vec3 o, glm::vec3 s, glm::vec3 p);

    /*******
    test if a triangle is overlapping with a box using the separating axis theorem
    *******/
    bool triangleBoxOverlap(const glm::vec3& boxOrigin, const glm::vec3& boxSize, int triangle);


    /*******
    insert a triangle in an array of pointer to triangles, in a way that makes the array sorted on one direction
    *******/
    void insertTriangle(std::vector<int> &triangles, int triangle, int separation_direction);

    meshKdTree(){
        child0 = nullptr;
        child1 = nullptr;
    }

    meshKdTree(std::vector<int> triangles, Mesh *mesh, const glm::vec3 origin, const glm::vec3 size, int depth = 50, int separation_direction = 0);

    /*******
    test if a ray goes through a box and give the tNear and tFar values
    *******/
    bool rayIntersectsBox(glm::vec3 const &origin, glm::vec3 const &direction, float const &length, float &tNear, float &tFar);

    /*******
    gives the nearest intersection of the ray through the box
    *******/
    RayIntersection rayThroughTree(glm::vec3 const &origin, glm::vec3 const &direction, float const &length);

    void printSelf(){
        for(int i = 0; i < 50-depth; i ++){
            std::cout << "  ";
        }
        std::cout << "depth : " << 50-depth << " triangles nbr : " <<triangles.size() << std::endl;
    }
};

#endif // MESHKDTREE_H
