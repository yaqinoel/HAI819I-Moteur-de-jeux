
#include "RenderSystem.hpp"
#include "Shader.hpp"
#include "common/3dEntities/Lights/Light.hpp"

RenderSystem::~RenderSystem() {
    for (auto& pair : m_ShaderLibrary) {
        delete pair.second;
    }
    m_ShaderLibrary.clear();
}

Shader* RenderSystem::getOrCreateShader(const std::string& vPath, const std::string& fPath){
    std::string key = vPath + "|" + fPath;
    if (m_ShaderLibrary.find(key) != m_ShaderLibrary.end()) {
        return m_ShaderLibrary[key];
    }
    Shader* newShader = new Shader(vPath.c_str(), fPath.c_str());
    m_ShaderLibrary[key] = newShader;
    return newShader;
}

void RenderSystem::uploadLights(Shader* shader, const std::vector<Light*>& lights) {
    // TODO upload all lights data for a shader
}