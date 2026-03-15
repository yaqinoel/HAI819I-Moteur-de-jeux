#pragma once
#include <cmath>
#include <glm/glm.hpp>

// Fade function (quintic)
inline glm::vec2 fade(const glm::vec2 &t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

// Linear interpolation
inline float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

// Hash function to generate pseudo-random gradients
inline glm::vec2 randomGradient(int ix, int iy) {
    int w = 8 * 32;
    int a = ix, b = iy;
    int hash = (a * 1836311903) ^ (b * 2971215073);
    hash = (hash >> 13) ^ hash;
    float angle = float(hash & 255) / 256.0f * 6.28318530718f; // 0..2pi
    return glm::vec2(cos(angle), sin(angle));
}

// 2D Perlin noise
float perlin2D(const glm::vec2 &p) {
    glm::ivec2 ip = glm::ivec2(floor(p.x), floor(p.y));
    glm::vec2 fp = glm::fract(p);

    // Gradients at corners
    glm::vec2 g00 = randomGradient(ip.x, ip.y);
    glm::vec2 g10 = randomGradient(ip.x + 1, ip.y);
    glm::vec2 g01 = randomGradient(ip.x, ip.y + 1);
    glm::vec2 g11 = randomGradient(ip.x + 1, ip.y + 1);

    // Vectors to corners
    glm::vec2 d00 = fp - glm::vec2(0.0f, 0.0f);
    glm::vec2 d10 = fp - glm::vec2(1.0f, 0.0f);
    glm::vec2 d01 = fp - glm::vec2(0.0f, 1.0f);
    glm::vec2 d11 = fp - glm::vec2(1.0f, 1.0f);

    // Dot products
    float v00 = glm::dot(g00, d00);
    float v10 = glm::dot(g10, d10);
    float v01 = glm::dot(g01, d01);
    float v11 = glm::dot(g11, d11);

    glm::vec2 f = fade(fp);

    float x1 = lerp(v00, v10, f.x);
    float x2 = lerp(v01, v11, f.x);

    // Normalize to 0..1
    return lerp(x1, x2, f.y) * 0.5f + 0.5f;
}
