// --------------------------------------------------------------------------
// gMini,
// a minimal Glut/OpenGL app to extend                              
//
// Copyright(C) 2007-2009                
// Tamy Boubekeur
//                                                                            
// All rights reserved.                                                       
//                                                                            
// This program is free software; you can redistribute it and/or modify       
// it under the terms of the GNU General Public License as published by       
// the Free Software Foundation; either version 2 of the License, or          
// (at your option) any later version.                                        
//                                                                            
// This program is distributed in the hope that it will be useful,            
// but WITHOUT ANY WARRANTY; without even the implied warranty of             
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              
// GNU General Public License (http://www.gnu.org/licenses/gpl.txt)           
// for more details.                                                          
//                                                                          
// --------------------------------------------------------------------------

#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <common/shader.hpp>
#include "common/Utilities/rayintersection.h"

#include "../Materials/material.h"
#include "camera.h"

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


class Mesh : public Node3d{
public:
    inline Mesh () : Node3d() {}
    inline Mesh (const std::vector<Vertex> & v) : vertices (v), Node3d(){}
    inline Mesh (const std::vector<Vertex> & v, const std::vector<Triangle> & t) : vertices (v), triangles (t), Node3d() {}
    inline Mesh (const Mesh & mesh) : vertices (mesh.vertices), triangles (mesh.triangles), Node3d() {}
    virtual ~Mesh(){
        if (_synchronized){
            unsynchronize();
        }
    }
    std::vector<Vertex> & getVertices () { return vertices; }
    const std::vector<Vertex> & getVertices () const { return vertices; }
    std::vector<Triangle> & getTriangles () { return triangles; }
    const std::vector<Triangle> & getTriangles () const { return triangles; }
    void openOFF(const std::string &filename, unsigned int normWeight = 1);
    void openOBJ(const std::string &filename);
    void render(const Camera* camera) const override;
    void setShader(std::string vertex_shader, std::string fragment_shader);
    void clear ();
    void clearGeometry ();
    void clearTopology ();
    void recomputeSmoothVertexNormals (unsigned int weight);
    void computeTriangleNormals (std::vector<glm::vec3> & triangleNormals);
    void setMaterial(Material* material);
    virtual void setUniforms() const {}
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    Material* material;
    std::map<std::string, Texture> mymap;

    RayIntersection intersect( glm::vec3 const &origin, glm::vec3 const &direction, glm::vec3 const &length);
    RayIntersection intersectTriangle(int const &triangleIndex, glm::vec3 const &origin, glm::vec3 const &direction, glm::vec3 const &length);


protected:
    void unsynchronize() const;
    void synchronize() const;
    int LOD = 0;

    mutable bool _synchronized = false;
    mutable GLuint _VBO;
    mutable GLuint _VAO;
    mutable GLuint _EBO;

    mutable GLuint shaderPID;
    mutable GLuint viewUniform;
    mutable GLuint modelMatrixUniform;
    mutable GLuint viewMatrixUniform;
    mutable GLuint projectionMatrixUniform;

};

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
