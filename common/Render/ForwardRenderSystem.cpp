#include "ForwardRenderSystem.hpp"
#include "IBLEnvironment.hpp"
#include "Shader.hpp"
#include "common/scene.h"
#include "common/3dEntities/Mesh.hpp"
#include "common/3dEntities/Lights/Light.hpp"
#include "common/3dEntities/Lights/DirectionalLight.hpp"
#include "common/3dEntities/Lights/PointLight.hpp"
#include "common/Materials/material.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

constexpr int DEBUG_IBL_MODE = 0;

ForwardRenderSystem::~ForwardRenderSystem() {
    if (shadowMap != 0) glDeleteTextures(1, &shadowMap);
    if (shadowFBO != 0) glDeleteFramebuffers(1, &shadowFBO);
    glDeleteTextures(MaxPointShadowMaps, pointShadowMaps);
    if (dummyPointShadowMap != 0) glDeleteTextures(1, &dummyPointShadowMap);
    if (pointShadowFBO != 0) glDeleteFramebuffers(1, &pointShadowFBO);
}

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
    hasShadowMap = false;
    pointShadowCount = 0;
    if (!scene) return;

    DirectionalLight* shadowLight = findShadowDirectionalLight(scene);

    GLint oldViewport[4];
    GLint oldCullFaceMode = GL_BACK;
    GLfloat oldPolygonOffsetFactor = 0.0f;
    GLfloat oldPolygonOffsetUnits = 0.0f;
    GLboolean cullEnabled = glIsEnabled(GL_CULL_FACE);
    GLboolean polygonOffsetEnabled = glIsEnabled(GL_POLYGON_OFFSET_FILL);
    glGetIntegerv(GL_VIEWPORT, oldViewport);
    glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
    glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &oldPolygonOffsetFactor);
    glGetFloatv(GL_POLYGON_OFFSET_UNITS, &oldPolygonOffsetUnits);

    if (shadowLight) {
        initializeShadowMap();
        if (shadowFBO != 0 && shadowMap != 0 && shadowDepthShader) {
            glm::vec3 lightDirection = glm::normalize(shadowLight->getDirection());
            if (glm::length(lightDirection) < 0.0001f) {
                lightDirection = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.3f));
            }

            glm::vec3 sceneMin(0.0f);
            glm::vec3 sceneMax(0.0f);
            bool hasMesh = false;
            for (Mesh* mesh : scene->meshes) {
                if (!mesh || !mesh->getVisible() || !mesh->meshDisplay) continue;

                glm::vec3 position = mesh->getGlobalPosition();
                if (!hasMesh) {
                    sceneMin = position;
                    sceneMax = position;
                    hasMesh = true;
                } else {
                    sceneMin = glm::min(sceneMin, position);
                    sceneMax = glm::max(sceneMax, position);
                }
            }

            glm::vec3 sceneCenter = hasMesh ? (sceneMin + sceneMax) * 0.5f : glm::vec3(0.0f);
            float orthoSize = shadowLight->shadowOrthoSize;
            float nearPlane = shadowLight->shadowNearPlane;
            float farPlane = shadowLight->shadowFarPlane;

            glm::vec3 up = std::abs(glm::dot(lightDirection, glm::vec3(0.0f, 1.0f, 0.0f))) > 0.9f
                ? glm::vec3(0.0f, 0.0f, 1.0f)
                : glm::vec3(0.0f, 1.0f, 0.0f);

            glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);
            glm::mat4 lightView = glm::lookAt(sceneCenter - lightDirection * (farPlane * 0.5f), sceneCenter, up);
            lightSpaceMatrix = lightProjection * lightView;

            glViewport(0, 0, shadowMapSize, shadowMapSize);
            glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
            glClear(GL_DEPTH_BUFFER_BIT);

            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(2.0f, 4.0f);

            shadowDepthShader->use();
            shadowDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
            for (Mesh* mesh : scene->meshes) {
                if (!mesh || !mesh->getVisible() || !mesh->meshDisplay) continue;

                shadowDepthShader->setMat4("model", mesh->getGlobalMatrix());
                mesh->drawOnly();
            }

            hasShadowMap = true;
        }
    }

    std::vector<PointLight*> pointLights = collectShadowPointLights(scene);
    if (!pointLights.empty()) {
        initializePointShadowMaps();
        if (pointShadowFBO != 0 && pointShadowDepthShader) {
            glViewport(0, 0, pointShadowMapSize, pointShadowMapSize);
            glBindFramebuffer(GL_FRAMEBUFFER, pointShadowFBO);

            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);

            pointShadowDepthShader->use();
            for (int lightIndex = 0; lightIndex < static_cast<int>(pointLights.size()); ++lightIndex) {
                PointLight* light = pointLights[lightIndex];
                glm::vec3 lightPos = light->getGlobalPosition();
                float nearPlane = light->shadowNearPlane;
                float farPlane = light->shadowFarPlane;
                glm::mat4 shadowProjection = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);
                glm::mat4 shadowTransforms[] = {
                    shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                    shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                    shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
                    shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
                    shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                    shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
                };

                pointShadowPositions[lightIndex] = lightPos;
                pointShadowFarPlanes[lightIndex] = farPlane;
                pointShadowDepthShader->setVec3("lightPos", lightPos);
                pointShadowDepthShader->setFloat("farPlane", farPlane);

                for (unsigned int face = 0; face < 6; ++face) {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, pointShadowMaps[lightIndex], 0);
                    glClear(GL_DEPTH_BUFFER_BIT);
                    pointShadowDepthShader->setMat4("shadowMatrix", shadowTransforms[face]);

                    for (Mesh* mesh : scene->meshes) {
                        if (!mesh || !mesh->getVisible() || !mesh->meshDisplay) continue;

                        pointShadowDepthShader->setMat4("model", mesh->getGlobalMatrix());
                        mesh->drawOnly();
                    }
                }
            }

            pointShadowCount = static_cast<int>(pointLights.size());
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);
    if (cullEnabled) {
        glEnable(GL_CULL_FACE);
        glCullFace(oldCullFaceMode);
    } else {
        glDisable(GL_CULL_FACE);
    }
    if (polygonOffsetEnabled) {
        glEnable(GL_POLYGON_OFFSET_FILL);
    } else {
        glDisable(GL_POLYGON_OFFSET_FILL);
    }
    glPolygonOffset(oldPolygonOffsetFactor, oldPolygonOffsetUnits);

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
        if (!m || !m->getVisible() || !m->meshDisplay || !m->material || !m->material->shader) continue;

        Shader* currentShader = m->material->shader;

        if (currentShader != lastShader) {
            currentShader->use();

            uploadLights(currentShader, lights);
            currentShader->setMat4("view", camera->getViewMatrix());
            currentShader->setMat4("projection", camera->getProjectionMatrix());
            currentShader->setVec3("camPos", camera->getGlobalPosition());
            currentShader->setInt("debugIBLMode", DEBUG_IBL_MODE);
            currentShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
            currentShader->setInt("useShadowMap", hasShadowMap ? 1 : 0);
            currentShader->setInt("shadowMap", 8);
            if (hasShadowMap) {
                glActiveTexture(GL_TEXTURE8);
                glBindTexture(GL_TEXTURE_2D, shadowMap);
            }
            initializeDummyPointShadowMap();
            currentShader->setInt("pointShadowCount", pointShadowCount);
            for (int i = 0; i < MaxPointShadowMaps; ++i) {
                int slot = 9 + i;
                currentShader->setInt("pointShadowMaps[" + std::to_string(i) + "]", slot);
                currentShader->setVec3("pointShadowPositions[" + std::to_string(i) + "]", pointShadowPositions[i]);
                currentShader->setFloat("pointShadowFarPlanes[" + std::to_string(i) + "]", pointShadowFarPlanes[i]);
                glActiveTexture(GL_TEXTURE0 + slot);
                if (i < pointShadowCount) {
                    glBindTexture(GL_TEXTURE_CUBE_MAP, pointShadowMaps[i]);
                } else {
                    glBindTexture(GL_TEXTURE_CUBE_MAP, dummyPointShadowMap);
                }
            }

            lastShader = currentShader;
        }

        if (m->material->isPBR()) {
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

            currentShader->setInt("has_albedoMap", 0);
            currentShader->setInt("has_normalMap", 0);
            currentShader->setInt("has_metallicMap", 0);
            currentShader->setInt("has_roughnessMap", 0);
            currentShader->setInt("has_aoMap", 0);
        } else {
            currentShader->setInt("useIBL", 0);
        }

        m->material->bind();
        currentShader->setMat4("model", m->getGlobalMatrix());
        m->drawOnly();
    }
}

void ForwardRenderSystem::initializePointShadowMaps() {
    if (pointShadowFBO == 0) {
        glGenFramebuffers(1, &pointShadowFBO);
    }
    if (!pointShadowDepthShader) {
        pointShadowDepthShader = getOrCreateShader("../Shaders/point_shadow_depth.vert", "../Shaders/point_shadow_depth.frag");
    }

    for (int i = 0; i < MaxPointShadowMaps; ++i) {
        if (pointShadowMaps[i] != 0) continue;

        glGenTextures(1, &pointShadowMaps[i]);
        glBindTexture(GL_TEXTURE_CUBE_MAP, pointShadowMaps[i]);
        for (unsigned int face = 0; face < 6; ++face) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_DEPTH_COMPONENT,
                         pointShadowMapSize, pointShadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, pointShadowFBO);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ForwardRenderSystem::initializeDummyPointShadowMap() {
    if (dummyPointShadowMap != 0) return;

    float farDepth = 1.0f;
    glGenTextures(1, &dummyPointShadowMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, dummyPointShadowMap);
    for (unsigned int face = 0; face < 6; ++face) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_DEPTH_COMPONENT,
                     1, 1, 0, GL_DEPTH_COMPONENT, GL_FLOAT, &farDepth);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void ForwardRenderSystem::initializeShadowMap() {
    if (shadowFBO != 0 && shadowMap != 0) return;

    shadowDepthShader = getOrCreateShader("../Shaders/shadow_depth.vert", "../Shaders/shadow_depth.frag");

    glGenFramebuffers(1, &shadowFBO);
    glGenTextures(1, &shadowMap);

    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapSize, shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "[Shadow] Failed to create shadow framebuffer." << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

DirectionalLight* ForwardRenderSystem::findShadowDirectionalLight(Scene* scene) const {
    if (!scene) return nullptr;

    for (Light* light : scene->lights) {
        if (!light || !light->getActive() || !light->getVisible()) continue;
        if (!light->castShadow) continue;
        if (light->getType() == LightType::Directional) {
            return static_cast<DirectionalLight*>(light);
        }
    }

    return nullptr;
}

std::vector<PointLight*> ForwardRenderSystem::collectShadowPointLights(Scene* scene) const {
    std::vector<PointLight*> pointLights;
    if (!scene) return pointLights;

    for (Light* light : scene->lights) {
        if (!light || !light->getActive() || !light->getVisible()) continue;
        if (!light->castShadow) continue;
        if (light->getType() != LightType::Point) continue;

        pointLights.push_back(static_cast<PointLight*>(light));
        if (static_cast<int>(pointLights.size()) >= MaxPointShadowMaps) break;
    }

    return pointLights;
}
