#include "Light.hpp"

class SpotLight : public Light {
public:
    float radius;
    float cutOff;
    float outerCutOff;

    SpotLight(glm::vec3 c, float i, float r, float innerAngle, float outerAngle);
    virtual LightType getType() const override;
    glm::vec3 getDirection() const;
};


