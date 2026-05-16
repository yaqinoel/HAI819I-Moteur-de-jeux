#pragma once

#include "RenderSystem.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>

class Camera;
class DirectionalLight;
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
    void initializeShadowMap();
    DirectionalLight* findShadowDirectionalLight(Scene* scene) const;

    GLuint shadowFBO = 0;
    GLuint shadowMap = 0;
    int shadowMapSize = 2048;
    bool hasShadowMap = false;
    glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
    Shader* shadowDepthShader = nullptr;
};
