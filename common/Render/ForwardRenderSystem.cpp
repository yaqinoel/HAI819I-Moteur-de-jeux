#include "ForwardRenderSystem.hpp"
#include "IBLEnvironment.hpp"
#include "common/scene.h"
#include "common/3dEntities/Mesh.hpp"
#include <algorithm>

constexpr int DEBUG_IBL_MODE = 0;

void ForwardRenderSystem::render(Scene* scene) {
    if (!scene || !scene->mainCamera) return;
    if (scene->iblEnvironment && !scene->iblEnvironment->isReady()) {
        scene->iblEnvironment->initialize();
    }

    shadowMapPass(scene);
    colorPass(scene, scene->mainCamera);
    if (scene->iblEnvironment && scene->iblEnvironment->isReady()) {
        scene->iblEnvironment->renderSkybox(scene->mainCamera);
    }
}

void ForwardRenderSystem::shadowMapPass(Scene* scene) {
    // TODO Shadow Mapping Pass
}

void ForwardRenderSystem::colorPass(Scene* scene, Camera* camera) {
    auto& meshes = scene->meshes;
    auto& lights = scene->lights;

    std::sort(meshes.begin(), meshes.end(), [](Mesh* a, Mesh* b) {
        if (!a->material || !b->material) return false;
        return a->material->shader < b->material->shader;
    });

    Shader* lastShader = nullptr;

    for (Mesh* m : meshes) {
        if (!m || !m->getVisible() || !m->meshDisplay || !m->material) continue;

        Shader* currentShader = m->material->shader;

        if (currentShader != lastShader) {
            currentShader->use();

            uploadLights(currentShader, lights);
            if (scene->iblEnvironment && scene->iblEnvironment->isReady()) {
                scene->iblEnvironment->bindIrradianceMap(5);
                scene->iblEnvironment->bindPrefilterMap(6);
                scene->iblEnvironment->bindBRDFLUT(7);
                currentShader->setInt("irradianceMap", 5);
                currentShader->setInt("prefilterMap", 6);
                currentShader->setInt("brdfLUT", 7);
                currentShader->setInt("useIBL", 1);
            } else {
                currentShader->setInt("useIBL", 0);
            }
            currentShader->setMat4("view", camera->getViewMatrix());
            currentShader->setMat4("projection", camera->getProjectionMatrix());
            currentShader->setVec3("camPos", camera->getGlobalPosition());
            currentShader->setInt("debugIBLMode", DEBUG_IBL_MODE);

            lastShader = currentShader;
        }

        currentShader->setInt("has_albedoMap", 0);
        currentShader->setInt("has_normalMap", 0);
        currentShader->setInt("has_metallicMap", 0);
        currentShader->setInt("has_roughnessMap", 0);
        currentShader->setInt("has_aoMap", 0);
        m->material->bind();
        currentShader->setMat4("model", m->getGlobalMatrix());
        m->drawOnly();
    }
}
