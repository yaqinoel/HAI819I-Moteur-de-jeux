#version 330 core

// Ouput data
out vec4 color;
in vec3 vertices_position_fragment;
in vec3 vertices_normals_fragment;
in vec2 vertices_texCoord_fragment;

uniform float frequency;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;

uniform mat4 model;
uniform mat4 view;

vec2 fade(vec2 t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

// Random gradient vector
vec2 randomGradient(int ix, int iy) {
    int w = 8 * 32;
    int s = w / 2;
    int a = ix, b = iy;
    int hash = (a * 1836311903) ^ (b * 2971215073);
    hash = (hash >> 13) ^ hash;
    float angle = float(hash & 255) / 256.0 * 6.28318530718; // 0..2π
    return vec2(cos(angle), sin(angle));
}

// Perlin noise
float perlin(vec2 p) {
    ivec2 ip = ivec2(floor(p));
    vec2 fp = fract(p);

    // gradients at corners
    vec2 g00 = randomGradient(ip.x, ip.y);
    vec2 g10 = randomGradient(ip.x + 1, ip.y);
    vec2 g01 = randomGradient(ip.x, ip.y + 1);
    vec2 g11 = randomGradient(ip.x + 1, ip.y + 1);

    // vectors to corners
    vec2 d00 = fp - vec2(0.0, 0.0);
    vec2 d10 = fp - vec2(1.0, 0.0);
    vec2 d01 = fp - vec2(0.0, 1.0);
    vec2 d11 = fp - vec2(1.0, 1.0);

    // dot products
    float v00 = dot(g00, d00);
    float v10 = dot(g10, d10);
    float v01 = dot(g01, d01);
    float v11 = dot(g11, d11);

    vec2 f = fade(fp);
    float x1 = lerp(v00, v10, f.x);
    float x2 = lerp(v01, v11, f.x);
    return lerp(x1, x2, f.y) * 0.5 + 0.5; // normalize to 0..1
}

void main(){
    vec4 c;
    float h = perlin(vertices_texCoord_fragment * frequency);
    if(h < 0.5){
        float d0 = h*2;
        float d1 = (0.5-h)*2;
        c = d0 * texture(texture1, vertices_texCoord_fragment) + d1 * texture(texture0, vertices_texCoord_fragment);
    }
    else{
        float d0 = (h-0.5)*2;
        float d1 = (1-h)*2;
        c = d0 * texture(texture2, vertices_texCoord_fragment) + d1 * texture(texture1, vertices_texCoord_fragment);
    }

    color = c;
}
