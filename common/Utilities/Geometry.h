#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <array>

struct Triangle {
    inline Triangle () {
        v[0] = v[1] = v[2] = 0;
    }
    inline Triangle (const Triangle & t) {
        v[0] = t.v[0];   v[1] = t.v[1];   v[2] = t.v[2];
    }
    inline Triangle (unsigned int v0, unsigned int v1, unsigned int v2) {
        v[0] = v0;   v[1] = v1;   v[2] = v2;
    }
    inline Triangle (glm::ivec3 vec) {
        v[0] = vec[0];   v[1] = vec[1];   v[2] = vec[2];
    }
    unsigned int & operator [] (unsigned int iv) { return v[iv]; }
    unsigned int operator [] (unsigned int iv) const { return v[iv]; }
    inline Triangle & operator = (const Triangle & t) {
        v[0] = t.v[0];   v[1] = t.v[1];   v[2] = t.v[2];
        return (*this);
    }
    // membres :
    std::array<unsigned int, 3> v;
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 tangent;
    glm::vec3 bitangent;

    inline Vertex () {
        position = glm::vec3(0);
        normal = glm::vec3(0);
        texCoord = glm::vec2(0);
        tangent = glm::vec3(0);
        bitangent = glm::vec3(0);
    }
    inline Vertex (const Vertex & v) {
        position = v.position;
        normal = v.normal;
        texCoord = v.texCoord;
        tangent = v.tangent;
        bitangent = v.bitangent;
    }
    inline Vertex (glm::vec3 p, glm::vec3 n, glm::vec2 tc) {
        position = p;
        normal = glm::normalize(n);
        texCoord = tc;
        tangent = glm::vec3(0);
        bitangent = glm::vec3(0);
    }
    inline Vertex (glm::vec3 p, glm::vec2 tc) {
        position = p;
        normal = glm::normalize(p);
        texCoord = tc;
        tangent = glm::vec3(0);
        bitangent = glm::vec3(0);
    }
    inline Vertex (glm::vec3 p, glm::vec3 n) {
        position = p;
        normal = n;
        texCoord = glm::vec2(0);
        tangent = glm::vec3(0);
        bitangent = glm::vec3(0);
    }
    inline Vertex (glm::vec3 p) {
        position = p;
        normal = glm::normalize(p);
        texCoord = glm::vec2(0);
        tangent = glm::vec3(0);
        bitangent = glm::vec3(0);
    }
};

static glm::vec3 projectPointOnPlane(const glm::vec3& point, const glm::vec3& planePoint, const glm::vec3& planeNormal){
    glm::vec3 n = glm::normalize(planeNormal);
    float distance = glm::dot(point - planePoint, n);
    return point - distance * n;
}
static glm::vec3 projectPointOnNormalizedPlane(const glm::vec3& point, const glm::vec3& planePoint, const glm::vec3& planeNormal){
    float distance = glm::dot(point - planePoint, planeNormal);
    return point - distance * planeNormal;
}

static glm::vec3 projectPointOnNormalizedLine(const glm::vec3& point, const glm::vec3& direction){
    return glm::dot(point, direction) * direction;
}
static glm::vec3 projectPointOnLine(const glm::vec3& point, const glm::vec3& direction){
    float t = glm::dot(point, direction) / glm::dot(direction, direction);
    return t * direction;
}

static glm::vec3 projectPointOnNormalizedLine(const glm::vec3& point, const glm::vec3& origin, const glm::vec3& direction){
    float t = glm::dot(point - origin, direction);
    return origin + t * direction;
}
static glm::vec3 projectPointOnLine(const glm::vec3& point, const glm::vec3& origin, const glm::vec3& direction){
    float t = glm::dot(point - origin, direction) / glm::dot(direction, direction);
    return origin + t * direction;
}

