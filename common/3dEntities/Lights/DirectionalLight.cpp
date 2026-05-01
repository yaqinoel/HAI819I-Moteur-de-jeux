#include "DirectionalLight.hpp"

DirectionalLight::DirectionalLight(glm::vec3 c = glm::vec3(1.0f), float i = 1.0f)
    : Light(c, i) {}

LightType DirectionalLight::getType() const {
    return LightType::Directional;
}
glm::vec3 DirectionalLight::getDirection() const {
    return this->forward();
}