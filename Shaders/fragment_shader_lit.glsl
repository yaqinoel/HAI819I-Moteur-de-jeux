#version 330 core

// Ouput data
out vec4 color;
in vec3 vertices_position_fragment;
in vec3 vertices_normals_fragment;
in vec2 vertices_texCoord_fragment;

uniform mat4 model;
uniform mat4 view;

struct Material {
    vec3 albedo;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

uniform Material material;


void main()
{
    color = vec4(material.albedo.xyz, 1);
}
