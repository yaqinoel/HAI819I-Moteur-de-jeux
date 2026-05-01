#pragma once
#include "common/3dEntities/node3d.h"

enum class LightType {
    Directional,
    Point,
    Spot
};

class Light : public Node3d{
public:
    glm::vec3 color;
    float intensity;
    bool isActive = true;

    Light(glm::vec3 color, float intensity);
    glm::vec3 getRadiance() const;
    virtual LightType getType() const = 0;
    bool getActive() const;
    void setActive(bool bActive);
};

