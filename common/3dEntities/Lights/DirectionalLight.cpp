#include "DirectionalLight.hpp"

DirectionalLight::DirectionalLight(glm::vec3 c, float i)
    : Light(c, i) {}

LightType DirectionalLight::getType() const {
    return LightType::Directional;
}
glm::vec3 DirectionalLight::getDirection() const {
    return this->forward();
}