#pragma once

#include "RenderSystem.hpp"

class Camera;

class ForwardRenderSystem : public RenderSystem{
public:
    ForwardRenderSystem() = default;
    virtual ~ForwardRenderSystem() = default;
    virtual void render(Scene* scene) override;

protected:
    virtual void shadowMapPass(Scene* scene);
    virtual void colorPass(Scene* scene, Camera* camera);
};
