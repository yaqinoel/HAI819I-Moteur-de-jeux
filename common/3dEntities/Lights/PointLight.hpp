#pragma once
#include "Light.hpp"

class PointLight : public Light {
public:
    float radius;

    PointLight(glm::vec3 color, float intensity, float radius);
    virtual LightType getType() const override;
};
