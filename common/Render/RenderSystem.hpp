#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <common/GUI/gui.h>

class Shader;
class Scene;
class Light;

class RenderSystem {
public:
    RenderSystem() = default;
    virtual ~RenderSystem();
    Shader* getOrCreateShader(const std::string& vPath, const std::string& fPath);
    virtual void render(Scene* scene) = 0;
    void renderGUI(Scene* scene);
    GUI *gui;

protected:
    std::unordered_map<std::string, Shader*> m_ShaderLibrary;
    void uploadLights(Shader* shader, const std::vector<Light*>& lights);
};
