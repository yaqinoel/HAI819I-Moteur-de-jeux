#include "Light.hpp"

Light::Light(glm::vec3 color, float intensity) {
    this->color = color;
    this->intensity = intensity;
}

glm::vec3 Light::getRadiance() {
    return color * intensity;
}
