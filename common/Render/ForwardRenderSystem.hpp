#pragma once

#include "RenderSystem.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class Camera;
class DirectionalLight;
class PointLight;
class Shader;

class ForwardRenderSystem : public RenderSystem{
public:
    ForwardRenderSystem() = default;
    virtual ~ForwardRenderSystem();
    virtual void render(Scene* scene) override;

protected:
    virtual void shadowMapPass(Scene* scene);
    virtual void colorPass(Scene* scene, Camera* camera);

private:
    static const int MaxPointShadowMaps = 4;

    void initializeShadowMap();
    void initializePointShadowMaps();
    DirectionalLight* findShadowDirectionalLight(Scene* scene) const;
    std::vector<PointLight*> collectShadowPointLights(Scene* scene) const;

    GLuint shadowFBO = 0;
    GLuint shadowMap = 0;
    int shadowMapSize = 2048;
    bool hasShadowMap = false;
    glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
    Shader* shadowDepthShader = nullptr;

    GLuint pointShadowFBO = 0;
    GLuint pointShadowMaps[MaxPointShadowMaps] = {0};
    int pointShadowMapSize = 1024;
    int pointShadowCount = 0;
    glm::vec3 pointShadowPositions[MaxPointShadowMaps] = {glm::vec3(0.0f)};
    float pointShadowFarPlanes[MaxPointShadowMaps] = {1.0f};
    Shader* pointShadowDepthShader = nullptr;
};
