#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertices_position_modelspace;
layout(location = 1) in vec3 vertices_normals;
layout(location = 2) in vec2 vertices_texCoord;

//TODO create uniform transformations matrices Model View Projection
// Values that stay constant for the whole mesh.

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vertices_position_fragment;
out vec3 vertices_normals_fragment;
out vec2 vertices_texCoord_fragment;

void main(){

        vertices_position_fragment = vec3(model * vec4(vertices_position_modelspace, 1.0));
        vertices_normals_fragment = mat3(transpose(inverse(model))) * vertices_normals;
        vertices_texCoord_fragment = vertices_texCoord;

        // TODO : Output position of the vertex, in clip space : MVP * position
        //gl_Position = vec4(vertices_position_modelspace,1) * model;
        gl_Position = projection * view * model * vec4(vertices_position_modelspace,1);
}

