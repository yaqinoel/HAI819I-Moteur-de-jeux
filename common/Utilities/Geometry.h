#pragma once

#include <vector>
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

    inline Vertex () {
        position = glm::vec3(0);
        normal = glm::vec3(0);
        texCoord = glm::vec2(0);
    }
    inline Vertex (const Vertex & v) {
        position = v.position;
        normal = v.normal;
        texCoord = v.texCoord;
    }
    inline Vertex (glm::vec3 p, glm::vec3 n, glm::vec2 tc) {
        position = p;
        normal = glm::normalize(n);
        texCoord = tc;
    }
    inline Vertex (glm::vec3 p, glm::vec2 tc) {
        position = p;
        normal = glm::normalize(p);
        texCoord = tc;
    }
    inline Vertex (glm::vec3 p, glm::vec3 n) {
        position = p;
        normal = n;
        texCoord = glm::vec2(0);
    }
    inline Vertex (glm::vec3 p) {
        position = p;
        normal = glm::normalize(p);
        texCoord = glm::vec2(0);
    }
};
