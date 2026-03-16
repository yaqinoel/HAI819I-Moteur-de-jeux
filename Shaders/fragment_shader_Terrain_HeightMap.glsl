#version 330 core

// Ouput data
out vec4 color;
in vec3 vertices_position_fragment;
in vec3 vertices_normals_fragment;
in vec2 vertices_texCoord_fragment;
uniform sampler2D heightMap;
uniform float frequency;
uniform float maxHeight;
uniform float scale;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;

uniform mat4 model;
uniform mat4 view;


void main(){
    vec4 c;
    float h = vertices_position_fragment.y/maxHeight;
    if(h < 0.5){
        float d0 = h*2;
        float d1 = (0.5-h)*2;
        c = d0 * texture(texture1, vertices_texCoord_fragment/scale) + d1 * texture(texture0, vertices_texCoord_fragment/scale);
    }
    else{
        float d0 = (h-0.5)*2;
        float d1 = (1-h)*2;
        c = d0 * texture(texture2, vertices_texCoord_fragment/scale) + d1 * texture(texture1, vertices_texCoord_fragment/scale);
    }

    color = c;
}
