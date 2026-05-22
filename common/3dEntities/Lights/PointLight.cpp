
#include "PointLight.hpp"

PointLight::PointLight(glm::vec3 color, float intensity, float radius)
    : Light(color, intensity), radius(radius){}

LightType PointLight::getType() const {
    return LightType::Point;
}
