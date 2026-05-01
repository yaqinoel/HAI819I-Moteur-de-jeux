#include "Light.hpp"

class DirectionalLight : public Light{
public:
    DirectionalLight(glm::vec3 c = glm::vec3(1.0f), float i = 1.0f);
    virtual LightType getType() const override;
    glm::vec3 getDirection() const;
};


