#include "shape.h"
#include "cube.h"
#include "voxelshape.h"

Shape::Shape()
{

}

RayIntersection Shape::raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length, uint64_t layers){
    if((layers & collider->collisionLayers) == 0) return RayIntersection();
    else return this->raycast(origin, direction, length);
}

std::vector<ColliderIntersection> Shape::intersect(Shape* shape, bool calculatePoints){
    if(glm::length(shape->collider->getGlobalPosition()-collider->getGlobalPosition()) < radius+shape->radius){
        if(shape->type == CUBE){
            Cube* c = static_cast<Cube*>(shape);
            if(c){
                return intersectCube(c, calculatePoints);
            }
        }
        else if(shape->type == VOXEL){
            VoxelShape* v = static_cast<VoxelShape*>(shape);
            if(v){
                return intersectVoxel(v, calculatePoints);
            }
        }
    }
    return std::vector<ColliderIntersection>();
}




//this algorithm takes a polygon and a plane, and clip every vertices of the polygon so that they are under or on the plane
std::vector<glm::vec3> Shape::ClipPolygonToPlane(const std::vector<glm::vec3>& polygon, const glm::vec3& planeNormal, const glm::vec3& planePoint) {
    std::vector<glm::vec3> output;
    short int outputSize = 0;
    if(polygon.empty()) return output;

    for (size_t i = 0; i < polygon.size(); i++) {
        //the algorithm goes from edge to edge by going from vertex to vertex in a linear order
        glm::vec3 currentVertex  = polygon[i];
        glm::vec3 previousVertex = polygon[(i + polygon.size() - 1) % polygon.size()];

        //calculates the signed distance to the plane
        float currentDistance  = glm::dot(currentVertex - planePoint, planeNormal);
        float previousDistance = glm::dot(previousVertex - planePoint, planeNormal);

        if (currentDistance <= 0 && previousDistance <= 0) {
            output.push_back(currentVertex);
            outputSize ++;
        }
        else if (previousDistance <= 0 && currentDistance > 0) {
            // edge goes from inside to outside
            float t = previousDistance / (previousDistance - currentDistance);
            output.push_back(previousVertex + t * (currentVertex - previousVertex));
            outputSize ++;
        }
        else if (previousDistance > 0 && currentDistance <= 0) {
            // edge goes from outside to inside
            float t = previousDistance / (previousDistance - currentDistance);
            output.push_back(previousVertex + t * (currentVertex - previousVertex));
            output.push_back(currentVertex);
            outputSize +=2;
        }
        if(outputSize >= 4){
            return output;
        }
    }

    return output;
}
std::vector<glm::vec3> Shape::clipPolygon(const std::vector<glm::vec3>& polygon, const std::vector<glm::vec3>& planeNormals, const std::vector<glm::vec3>& planePoints) {
    std::vector<glm::vec3> result = polygon;
    for (size_t i = 0; i < planeNormals.size(); i++) {
        result = ClipPolygonToPlane(result, planeNormals[i], planePoints[i]);
        if (result.empty()) return result;
    }
    return result;
}

std::vector<glm::vec3> Shape::projectToPlane(const std::vector<glm::vec3>& points, const glm::vec3& planeNormal, const glm::vec3& planePoint) {
    std::vector<glm::vec3> projected;
    for (auto& p : points) {
        glm::vec3 proj = p - planeNormal * glm::dot(p - planePoint, planeNormal);
        projected.push_back(proj);
    }
    return projected;
}
