#include "voxelshape.h"

VoxelShape::VoxelShape() {
    type = VOXEL;
}

bool VoxelShape::isSolid(int x, int y, int z) const {
    if (x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= depth)
        return false;
    return get(x, y, z) != 0;
}

glm::ivec3 VoxelShape::worldToCell(const glm::vec3& worldPoint) const {
    glm::vec3 local = (worldPoint - collider->getGlobalPosition() - glm::vec3(voxelSize * 0.5f)) / voxelSize;
    return glm::ivec3(glm::floor(local));
}

glm::vec3 VoxelShape::cellMin(int x, int y, int z) const {
    return collider->getGlobalPosition() + glm::vec3(x, y, z) * voxelSize + glm::vec3(voxelSize * 0.5f);
}

glm::vec3 VoxelShape::cellMax(int x, int y, int z) const {
    return cellMin(x, y, z) + glm::vec3(voxelSize);
}

bool VoxelShape::computeAabb(const Collider3D& collider, PhysicsAabb& outAabb) const {
    glm::vec3 origin = collider.getGlobalPosition() + glm::vec3(voxelSize * 0.5f);
    outAabb.min = origin;
    outAabb.max = origin + glm::vec3(width, height, depth) * voxelSize;
    return width > 0 && height > 0 && depth > 0;
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
}
void VoxelShape::InitMeshFromTerrain(int width, int height, int depth, std::vector<unsigned short int> voxelData){
    this->width = width-2;
    this->height = height-2;
    this->depth = depth-2;
    radius = glm::length(glm::vec3(width, height, depth));
    voxels.resize(width*height*depth);

    for (int x = 1; x < width-1; x++) {
        for (int y = 1; y < height-1; y++) {
            for (int z = 1; z < depth-1; z++) {
                setCell(x-1, y-1, z-1, voxelData[x*width*height+ y*height+ z]);
            }
        }
    }
}


RayIntersection VoxelShape::raycast(glm::vec3 const &origin, glm::vec3 const &direction, float const &length)
{

    RayIntersection intersection;
    intersection.intersectionExists = false;
    if(direction == glm::vec3(0) || length <= 0) return intersection;

    glm::vec3 dir = glm::normalize(direction);
    glm::vec3 globalPos = collider->getGlobalPosition();

    // Transform ray into local voxel space
    glm::vec3 localOrigin = (origin - (globalPos + glm::vec3(voxelSize/2.0))) / voxelSize;

    // Current voxel cell
    int x = (int)std::floor(localOrigin.x);
    int y = (int)std::floor(localOrigin.y);
    int z = (int)std::floor(localOrigin.z);

    // Step direction per axis
    int stepX = (dir.x >= 0) ? 1 : -1;
    int stepY = (dir.y >= 0) ? 1 : -1;
    int stepZ = (dir.z >= 0) ? 1 : -1;

    // How far along the ray we must travel to cross one voxel boundary per axis
    float tDeltaX = (dir.x != 0) ? std::abs(voxelSize / dir.x) : INFINITY;
    float tDeltaY = (dir.y != 0) ? std::abs(voxelSize / dir.y) : INFINITY;
    float tDeltaZ = (dir.z != 0) ? std::abs(voxelSize / dir.z) : INFINITY;

    // Distance to first voxel boundary from origin
    auto tBoundary = [](float origin, float dir, int step) -> float {
        if (dir == 0.0f) return INFINITY;
        float boundary;
        if (step > 0) {
            boundary = std::floor(origin);
            if (origin > boundary) boundary += 1.0f;
        } else {
            boundary = std::ceil(origin);
            if (origin < boundary) boundary -= 1.0f;
        }

        return std::abs((boundary - origin) / dir);
    };

    float tMaxX = tBoundary(localOrigin.x, dir.x, stepX);
    float tMaxY = tBoundary(localOrigin.y, dir.y, stepY);
    float tMaxZ = tBoundary(localOrigin.z, dir.z, stepZ);

    float t = 0.0f;
    int face = -1; // 0=X, 1=Y, 2=Z — tracks which face was entered last

    while (t <= length)
    {
        // Check bounds
        if (x >= 0 && x < width && y >= 0 && y < height && z >= 0 && z < depth)
        {
            if (get(x, y, z) != 0 && t <= length)
            {
                intersection.intersectionExists = true;
                intersection.t = t;
                intersection.point = origin + dir * t;
                intersection.collider = collider;

                // Normal points opposite to the entered face
                glm::vec3 normal(0.0f);
                if      (face == 0) normal.x = -stepX;
                else if (face == 1) normal.y = -stepY;
                else if (face == 2) normal.z = -stepZ;
                intersection.normal = normal;

                return intersection;
            }
        }
        else
        {
            bool pastX = (x < 0 && stepX < 0) || (x >= width  && stepX > 0);
            bool pastY = (y < 0 && stepY < 0) || (y >= height && stepY > 0);
            bool pastZ = (z < 0 && stepZ < 0) || (z >= depth  && stepZ > 0);
            if (pastX || pastY || pastZ) break;
        }

        if (tMaxX < tMaxY && tMaxX < tMaxZ)
        {
            t = tMaxX;
            tMaxX += tDeltaX;
            x += stepX;
            face = 0;
        }
        else if (tMaxY < tMaxZ)
        {
            t = tMaxY;
            tMaxY += tDeltaY;
            y += stepY;
            face = 1;
        }
        else
        {
            t = tMaxZ;
            tMaxZ += tDeltaZ;
            z += stepZ;
            face = 2;
        }
    }
    return intersection;
}
