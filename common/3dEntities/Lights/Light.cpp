#include "Light.hpp"

Light::Light(glm::vec3 color, float intensity) {
    this->color = color;
    this->intensity = intensity;
}

glm::vec3 Light::getRadiance() const {
    return color * intensity;
}

bool Light::getActive() const{
    return isActive;
}

void Light::setActive(bool active) {
    isActive = active;
}
