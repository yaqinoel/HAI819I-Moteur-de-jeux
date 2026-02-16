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
   
#include "Mesh.hpp"
#include <glm/gtx/string_cast.hpp>
#include <fstream>

using namespace std;

void Mesh::clear () {
    clearTopology ();
    clearGeometry ();
}

void Mesh::clearGeometry () {
    vertices.clear ();
}

void Mesh::clearTopology () {
    triangles.clear ();
}

void Mesh::openOFF(const std::string &filename, unsigned int normWeight) {
    vertices = std::vector<Vertex>() ;
    triangles = std::vector<Triangle>() ;

    ifstream in (filename.c_str ());
    if (!in)
        exit (EXIT_FAILURE);
    string offString;
    unsigned int sizeV, sizeT, tmp;
    in >> offString >> sizeV >> sizeT >> tmp;

    vertices.resize(sizeV);
    for( int v = 0 ; v < sizeV ; ++v )
    {
        glm::vec3 vertex;
        in >> vertex.x >> vertex.y >> vertex.z;
        if( std::isnan(vertex.x) )
            vertex.x = 0.0;
        if( std::isnan(vertex.y) )
            vertex.y = 0.0;
        if( std::isnan(vertex.z) )
            vertex.z = 0.0;
        vertices[v].position = vertex;
        vertices[v].normal = vertex;
        vertices[v].texCoord = glm::vec2(0, 0);
    }
    int s;
    for (unsigned int i = 0; i < sizeT; i++) {
        in >> s;
        unsigned int v[3];
        for (unsigned int j = 0; j < 3; j++){
            in >> v[j];
        }
        triangles.push_back (Triangle (v[0], v[1], v[2]));
    }
    in.close ();

    glm::vec3 center;
    float radius;
    //Vertex::scaleToUnitBox (vertices, center, radius);
    recomputeSmoothVertexNormals (normWeight);
}

void Mesh::computeTriangleNormals (vector<glm::vec3> & triangleNormals) {
    for(Triangle it : triangles){
        glm::vec3 e01 (vertices[it[1]].position - vertices[it[0]].position);
        glm::vec3 e02 (vertices[it[2]].position - vertices[it[0]].position);
        glm::vec3 n (glm::cross (e01, e02));
        n = glm::normalize(n);
        triangleNormals.push_back (n);
    }
}

void Mesh::recomputeSmoothVertexNormals (unsigned int normWeight) {
    vector<glm::vec3> triangleNormals;
    computeTriangleNormals (triangleNormals);
    for(int i = 0; i < vertices.size(); i ++){
        vertices[i].normal = glm::vec3 (0.0, 0.0, 0.0);
    }
    vector<glm::vec3>::iterator itNormal = triangleNormals.begin ();
    for(Triangle it : triangles){
        for (unsigned int  j = 0; j < 3; j++) {
            Vertex & vj = vertices[it[j]];
            float w = 1.0; // uniform weights
            glm::vec3 e0 = vertices[it[(j+1)%3]].position - vj.position;
            glm::vec3 e1 = vertices[it[(j+2)%3]].position - vj.position;
            if (normWeight == 1) { // area weight
                w = glm::cross (e0, e1).length () / 2.0;
            } else if (normWeight == 2) { // angle weight
                e0 = glm::normalize(e0);
                e1 = glm::normalize(e1);
                w = (2.0 - (glm::dot (e0, e1) + 1.0)) / 2.0;
            } 
            if (w <= 0.0)
                continue;
            vj.normal = (vj.normal + (*itNormal) * w);
        }
        itNormal ++;
    }
    for(int i = 0; i < vertices.size(); i ++){
        vertices[i].normal = glm::normalize(vertices[i].normal);
    }
}
void Mesh::synchronize() const {

    if (_synchronized){
        unsynchronize();
    }

    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    // buffer contenant tous les sommets
    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // buffer contenant tous les indices
    glGenBuffers(1, &_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(Triangle), &triangles[0] , GL_STATIC_DRAW);

    int attributeIndex = 0;
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glVertexAttribPointer(attributeIndex, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(attributeIndex);

    attributeIndex = 1;
    glVertexAttribPointer(attributeIndex, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(attributeIndex);

    attributeIndex = 2;
    glVertexAttribPointer(attributeIndex, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(attributeIndex);
    _synchronized = true;
}

void Mesh::unsynchronize() const {
    glDeleteBuffers(1, &_VBO);
    glDeleteBuffers(1, &_EBO);
    glDeleteBuffers(1, &_UV);
    glDeleteBuffers(1, &_NORMALS);
    glDeleteProgram(shaderPID);
    glDeleteVertexArrays(1, &_VAO);

    _synchronized = false;
}


void Mesh::setMaterial(Material material) {
    this->material = material;
}

void Mesh::setShader(std::string vertex_shader, std::string fragment_shader){
    shaderPID = LoadShaders(vertex_shader.c_str(), fragment_shader.c_str());
    modelMatrixUniform = glGetUniformLocation(shaderPID, "model");
    viewMatrixUniform = glGetUniformLocation(shaderPID, "view");
    projectionMatrixUniform = glGetUniformLocation(shaderPID, "projection");
    viewUniform = glGetUniformLocation(shaderPID, "viewVector");
}

void Mesh::render(const Camera* camera) const{
    if (vertices.empty()) return;
    if (!_synchronized){
        synchronize();
    }
    else{
        glUseProgram(shaderPID);
    }
    glBindVertexArray(_VAO);


    material.render(shaderPID);
    glm::mat4 model = globalMatrix();
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat4 View = camera->getViewMatrix();
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(View));

    glm::mat4 Projection = camera->getProjectionMatrix();
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, glm::value_ptr(Projection));

    glUniform3fv(viewUniform, 1, glm::value_ptr(camera->forward()));

    glDrawElements(GL_TRIANGLES, triangles.size()*3, GL_UNSIGNED_INT, (void*)0 );

    glUseProgram(0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    for(Node * c : children)c->render(camera);
}
