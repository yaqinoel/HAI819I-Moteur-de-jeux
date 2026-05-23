#include "voxelshape.h"
#include "../Collision/physicsgeometry.h"
#include "../collider3d.h"
#include <glm/gtx/norm.hpp>
#include <cmath>
#include <limits>

VoxelShape::VoxelShape() {
    type = VOXEL;
}

bool VoxelShape::isSolid(int x, int y, int z) const {
    if (x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= depth)
        return false;
    return get(x, y, z) != 0;
}

glm::ivec3 VoxelShape::worldToCell(const glm::vec3& worldPoint) const {
    return worldToCell(*collider, worldPoint);
}

glm::ivec3 VoxelShape::worldToCell(const Collider3D& collider, const glm::vec3& worldPoint) const {
    glm::vec3 localPoint = glm::vec3(collider.getInverseGlobalMatrix() * glm::vec4(worldPoint, 1.0f));
    glm::vec3 local = (localPoint - glm::vec3(voxelSize * 0.5f)) / voxelSize;
    return glm::ivec3(glm::floor(local));
}

glm::vec3 VoxelShape::cellLocalMin(int x, int y, int z) const {
    return glm::vec3(x, y, z) * voxelSize + glm::vec3(voxelSize * 0.5f);
}

glm::vec3 VoxelShape::cellLocalMax(int x, int y, int z) const {
    return cellLocalMin(x, y, z) + glm::vec3(voxelSize);
}

glm::vec3 VoxelShape::cellMin(int x, int y, int z) const {
    return glm::vec3(collider->getGlobalMatrix() * glm::vec4(cellLocalMin(x, y, z), 1.0f));
}

glm::vec3 VoxelShape::cellMax(int x, int y, int z) const {
    return glm::vec3(collider->getGlobalMatrix() * glm::vec4(cellLocalMax(x, y, z), 1.0f));
}

OrientedBox VoxelShape::cellWorldBox(const Collider3D& collider, int x, int y, int z) const {
    OrientedBox box;
    glm::mat4 model = collider.getGlobalMatrix();
    glm::vec3 localMin = cellLocalMin(x, y, z);
    glm::vec3 localMax = cellLocalMax(x, y, z);
    glm::vec3 localCenter = (localMin + localMax) * 0.5f;

    box.collider = const_cast<Collider3D*>(&collider);
    box.body = collider.rb;
    box.center = glm::vec3(model * glm::vec4(localCenter, 1.0f));
    box.half = glm::vec3(voxelSize * 0.5f);

    glm::vec3 xAxis = glm::vec3(model * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
    glm::vec3 yAxis = glm::vec3(model * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
    glm::vec3 zAxis = glm::vec3(model * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));

    box.axis[0] = glm::length2(xAxis) > 1e-8f ? glm::normalize(xAxis) : glm::vec3(1.0f, 0.0f, 0.0f);
    box.axis[1] = glm::length2(yAxis) > 1e-8f ? glm::normalize(yAxis) : glm::vec3(0.0f, 1.0f, 0.0f);
    box.axis[2] = glm::length2(zAxis) > 1e-8f ? glm::normalize(zAxis) : glm::vec3(0.0f, 0.0f, 1.0f);
    return box;
}

bool VoxelShape::cellWorldAabb(const Collider3D& collider, int x, int y, int z, PhysicsAabb& outAabb) const {
    if (!isSolid(x, y, z))
        return false;

    OrientedBox box = cellWorldBox(collider, x, y, z);
    outAabb.min = glm::vec3(std::numeric_limits<float>::infinity());
    outAabb.max = glm::vec3(-std::numeric_limits<float>::infinity());
    for (const glm::vec3& corner : boxCorners(box)) {
        outAabb.min = glm::min(outAabb.min, corner);
        outAabb.max = glm::max(outAabb.max, corner);
    }
    return isFiniteVec3(outAabb.min) && isFiniteVec3(outAabb.max);
}

void VoxelShape::forEachSolidCell(const std::function<void(int, int, int)>& callback) const {
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            for (int z = 0; z < depth; ++z) {
                if (isSolid(x, y, z))
                    callback(x, y, z);
            }
        }
    }
}

int VoxelShape::solidCellCount() const {
    int count = 0;
    forEachSolidCell([&](int, int, int) {
        count++;
    });
    return count;
}

void VoxelShape::recomputeInertia() {
    int solidCells = solidCellCount();
    if (solidCells <= 0 || width <= 0 || height <= 0 || depth <= 0) {
        localCenterOfMass = glm::vec3(0.0f);
        inertia = glm::mat3(0.0f);
        return;
    }

    glm::vec3 centerSum(0.0f);
    forEachSolidCell([&](int x, int y, int z) {
        centerSum += (cellLocalMin(x, y, z) + cellLocalMax(x, y, z)) * 0.5f;
    });
    localCenterOfMass = centerSum / static_cast<float>(solidCells);

    float cellMass = mass / static_cast<float>(solidCells);
    float cellInertiaValue = cellMass * (voxelSize * voxelSize + voxelSize * voxelSize) / 12.0f;
    glm::mat3 cellInertia(0.0f);
    cellInertia[0][0] = cellInertiaValue;
    cellInertia[1][1] = cellInertiaValue;
    cellInertia[2][2] = cellInertiaValue;

    inertia = glm::mat3(0.0f);
    forEachSolidCell([&](int x, int y, int z) {
        glm::vec3 cellCenter = (cellLocalMin(x, y, z) + cellLocalMax(x, y, z)) * 0.5f;
        glm::vec3 offset = cellCenter - localCenterOfMass;
        glm::mat3 parallelAxis = cellMass * (glm::dot(offset, offset) * glm::mat3(1.0f) - glm::outerProduct(offset, offset));
        inertia += cellInertia + parallelAxis;
    });
}

bool VoxelShape::computeAabb(const Collider3D& collider, PhysicsAabb& outAabb) const {
    if (width <= 0 || height <= 0 || depth <= 0)
        return false;

    glm::vec3 localMin = glm::vec3(voxelSize * 0.5f);
    glm::vec3 localMax = localMin + glm::vec3(width, height, depth) * voxelSize;
    glm::mat4 model = collider.getGlobalMatrix();

    outAabb.min = glm::vec3(std::numeric_limits<float>::infinity());
    outAabb.max = glm::vec3(-std::numeric_limits<float>::infinity());

    for (int x = 0; x <= 1; ++x) {
        for (int y = 0; y <= 1; ++y) {
            for (int z = 0; z <= 1; ++z) {
                glm::vec3 localCorner(
                    x == 0 ? localMin.x : localMax.x,
                    y == 0 ? localMin.y : localMax.y,
                    z == 0 ? localMin.z : localMax.z
                );
                glm::vec3 worldCorner = glm::vec3(model * glm::vec4(localCorner, 1.0f));
                outAabb.min = glm::min(outAabb.min, worldCorner);
                outAabb.max = glm::max(outAabb.max, worldCorner);
            }
        }
    }

    return isFiniteVec3(outAabb.min) && isFiniteVec3(outAabb.max);
}

void VoxelShape::InitMesh(int width, int height, int depth, std::vector<unsigned short int> voxelData){
    this->width = width;
    this->height = height;
    this->depth = depth;
    radius = glm::length(glm::vec3(width, height, depth));
    voxels.resize(voxelData.size());
    for(int i = 0; i < voxelData.size(); i++){
        voxels[i] = voxelData[i] > 0;
    }
    recomputeInertia();
}


RayIntersection VoxelShape::raycast(glm::vec3 const &origin, glm::vec3 const &direction, float const &length)
{

    RayIntersection intersection;
    intersection.intersectionExists = false;
    if(direction == glm::vec3(0) || length <= 0) return intersection;
    return intersection;
}

void VoxelShape::setCell(int x, int y, int z, uint8_t v) {
    if (x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= depth)
        return;
    voxels[index(x, y, z)] = v;
}
