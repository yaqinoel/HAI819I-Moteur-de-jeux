#include "SpotLight.hpp"

SpotLight::SpotLight(glm::vec3 c, float i, float r, float innerAngle, float outerAngle)
    : Light(c, i), radius(r)
{
    cutOff = glm::cos(glm::radians(innerAngle));
    outerCutOff = glm::cos(glm::radians(outerAngle));
}

LightType SpotLight::getType() const {
    return LightType::Spot;
}

glm::vec3 SpotLight::getDirection() const {
    return this->forward();
}