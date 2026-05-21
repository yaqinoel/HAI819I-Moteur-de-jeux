#include "sphere.h"

Sphere::Sphere(float rad){
    type = SPHERE;
    radius = rad;
    int stacks = 20;
    int slices = 20;

    vertices.clear();
    triangles.clear();

    for (int i = 0; i <= stacks; ++i)
    {
        float v = float(i) / stacks;
        float phi = v * M_PI;

        for (int j = 0; j <= slices; ++j)
        {
            float u = float(j) / slices;
            float theta = u * 2.0f * M_PI;

            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);

            vertices.emplace_back(x, y, z);
        }
    }

    for (int i = 0; i < stacks; ++i)
    {
        for (int j = 0; j < slices; ++j)
        {
            int row1 = i * (slices + 1);
            int row2 = (i + 1) * (slices + 1);

            int a = row1 + j;
            int b = row1 + j + 1;
            int c = row2 + j;
            int d = row2 + j + 1;

            triangles.emplace_back(a, c, b);
            triangles.emplace_back(b, c, d);
        }
    }
    inertia = glm::mat3(mass*rad*rad*2.0/5.0);
}

bool Sphere::computeAabb(const Collider3D& collider, PhysicsAabb& outAabb) const {
    glm::vec3 center = collider.getGlobalPosition();
    glm::vec3 extents(radius);
    outAabb.min = center - extents;
    outAabb.max = center + extents;
    return radius >= 0.0f;
}




RayIntersection Sphere::raycast( glm::vec3 const &origin, glm::vec3 const &direction, float const &length){
    RayIntersection intersection;
    intersection.intersectionExists = false;

    glm::vec3 oc = origin - collider->getGlobalPosition();

    float a = glm::dot(direction, direction);
    float b = 2.0f * glm::dot(oc, direction);
    float c = glm::dot(oc, oc) - radius * radius;

    float discriminant = b*b - 4*a*c;

    if (discriminant < 0.0f)
        return intersection;

    float sqrtD = sqrt(discriminant);

    float t1 = (-b - sqrtD) / (2*a);
    float t2 = (-b + sqrtD) / (2*a);

    float t = std::min(t1, t2);

    if (t < 0.0f)
        t = std::max(t1, t2);

    if (t < 0.0f || t > length)
        return intersection;

    intersection.intersectionExists = true;
    intersection.t = t;
    intersection.point = origin + t * direction;
    intersection.normal = glm::normalize(intersection.point - collider->getGlobalPosition());

    return intersection;
}
