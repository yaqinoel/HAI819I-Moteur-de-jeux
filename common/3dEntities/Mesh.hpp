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
#include <embree4/rtcore.h>
#include "common/Utilities/rayintersection.h"
#include "../Utilities/Geometry.h"

#include "../Materials/material.h"
#include "camera.h"


class Mesh : public Node3d{
public:
    inline Mesh () : Node3d() {}
    inline Mesh (const std::vector<Vertex> & v) : vertices (v), Node3d(){}
    inline Mesh (const std::vector<Vertex> & v, const std::vector<Triangle> & t) : vertices (v), triangles (t), Node3d() {}
    Mesh (const std::vector<glm::vec3> & v, const std::vector<glm::ivec3> & t);
    inline Mesh (const Mesh & mesh) : vertices (mesh.vertices), triangles (mesh.triangles), Node3d() {}
    inline Mesh (Node*p) {setParent(p);}
    virtual ~Mesh(){
        if (_synchronized){
            unsynchronize();
        }
    }
    std::vector<Vertex> & getVertices () { return vertices; }
    const std::vector<Vertex> & getVertices () const { return vertices; }
    std::vector<Triangle> & getTriangles () { return triangles; }
    const std::vector<Triangle> & getTriangles () const { return triangles; }
    void setVertices(std::vector<glm::vec3> vec);
    void setTriangles(std::vector<glm::ivec3> vec);
    void openOFF(const std::string &filename, unsigned int normWeight = 1);
    void openOBJ(const std::string &filename);
    void drawOnly() const;
    void clear ();
    void clearGeometry ();
    void clearTopology ();
    void recomputeSmoothVertexNormals (unsigned int weight);
    void computeTriangleNormals (std::vector<glm::vec3> & triangleNormals);
    void setMaterial(Material* material);
    bool meshDisplay = true;
    virtual void setUniforms() const {}
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    Material* material = nullptr;
    std::map<std::string, Texture> mymap;
    glm::vec3 getBarycentre(const int triangleIndex);

    RayIntersection intersect( glm::vec3 const &origin, glm::vec3 const &direction, float const &length);
    RayIntersection intersectTriangle(glm::vec3 const &origin, glm::vec3 const &direction, float const &length);
    //virtual void process(float deltaTime) override {if(name == "knight mesh"){std::cout << glm::to_string(globalPosition()) << std::endl;}}


protected:
    void unsynchronize() const;
    void synchronize() const;
    int LOD = 0;

    mutable bool _synchronized = false;
    mutable GLuint _VBO;
    mutable GLuint _VAO;
    mutable GLuint _EBO;
};

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
