#include "ForwardRenderSystem.hpp"
#include "common/scene.h"
#include "common/3dEntities/Mesh.hpp"
#include <algorithm>

void ForwardRenderSystem::render(Scene* scene) {
    if (!scene || !scene->mainCamera) return;
    shadowMapPass(scene);
    colorPass(scene, scene->mainCamera);
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
            currentShader->setMat4("view", camera->getViewMatrix());
            currentShader->setMat4("projection", camera->getProjectionMatrix());
            currentShader->setVec3("camPos", camera->getGlobalPosition());

            lastShader = currentShader;
        }

        m->material->bind();
        currentShader->setMat4("model", m->getGlobalMatrix());
        m->drawOnly();
    }
}