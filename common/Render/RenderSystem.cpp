
#include "RenderSystem.hpp"
#include "Shader.hpp"
#include "common/3dEntities/Lights/Light.hpp"
#include "common/3dEntities/Lights/PointLight.hpp"
#include "common/3dEntities/Lights/DirectionalLight.hpp"

constexpr int MAX_PBR_POINT_LIGHTS = 4;
constexpr int MAX_PBR_DIRECTIONAL_LIGHTS = 1;

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
    if (!shader) return;

    int pointLightCount = 0;
    int directionalLightCount = 0;
    for (Light* light : lights) {
        if (!light || !light->getActive() || !light->getVisible()) continue;

        if (light->getType() == LightType::Point && pointLightCount < MAX_PBR_POINT_LIGHTS) {
            shader->setVec3("lightPositions[" + std::to_string(pointLightCount) + "]", light->getGlobalPosition());
            shader->setVec3("lightColors[" + std::to_string(pointLightCount) + "]", light->getRadiance());
            pointLightCount++;
        }

        if (light->getType() == LightType::Directional && directionalLightCount < MAX_PBR_DIRECTIONAL_LIGHTS) {
            DirectionalLight* directionalLight = static_cast<DirectionalLight*>(light);
            shader->setVec3("directionalLightDirections[" + std::to_string(directionalLightCount) + "]", directionalLight->getDirection());
            shader->setVec3("directionalLightColors[" + std::to_string(directionalLightCount) + "]", directionalLight->getRadiance());
            directionalLightCount++;
        }
    }

    for (int i = pointLightCount; i < MAX_PBR_POINT_LIGHTS; ++i) {
        shader->setVec3("lightPositions[" + std::to_string(i) + "]", glm::vec3(0.0f));
        shader->setVec3("lightColors[" + std::to_string(i) + "]", glm::vec3(0.0f));
    }
    for (int i = directionalLightCount; i < MAX_PBR_DIRECTIONAL_LIGHTS; ++i) {
        shader->setVec3("directionalLightDirections[" + std::to_string(i) + "]", glm::vec3(0.0f, -1.0f, 0.0f));
        shader->setVec3("directionalLightColors[" + std::to_string(i) + "]", glm::vec3(0.0f));
    }

    shader->setInt("pointLightCount", pointLightCount);
    shader->setInt("directionalLightCount", directionalLightCount);
}
