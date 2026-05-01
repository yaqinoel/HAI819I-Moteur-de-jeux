#include "Light.hpp"

class DirectionalLight : public Light{
public:
    DirectionalLight(glm::vec3 c, float i);
    virtual LightType getType() const override;
    glm::vec3 getDirection() const;
};


