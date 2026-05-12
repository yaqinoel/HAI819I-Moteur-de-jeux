#pragma once

#include "Shader.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class Camera;

class IBLEnvironment {
public:
    explicit IBLEnvironment(const std::string& hdrPath);
    ~IBLEnvironment();

    bool initialize();
    bool isReady() const { return ready; }
    void bindIrradianceMap(GLuint slot) const;
    void bindPrefilterMap(GLuint slot) const;
    void bindBRDFLUT(GLuint slot) const;
    void renderSkybox(const Camera* camera);

private:
    std::string hdrPath;
    bool ready = false;

    GLuint hdrTexture = 0;
    GLuint envCubemap = 0;
    GLuint irradianceMap = 0;
    GLuint prefilterMap = 0;
    GLuint brdfLUTTexture = 0;
    GLuint captureFBO = 0;
    GLuint captureRBO = 0;
    GLuint cubeVAO = 0;
    GLuint cubeVBO = 0;
    GLuint quadVAO = 0;
    GLuint quadVBO = 0;

    Shader* equirectangularShader = nullptr;
    Shader* irradianceShader = nullptr;
    Shader* prefilterShader = nullptr;
    Shader* brdfShader = nullptr;
    Shader* skyboxShader = nullptr;

    GLuint loadHDRTexture();
    void createCaptureFramebuffer(int size);
    void createEnvironmentCubemap();
    void createIrradianceMap();
    void createPrefilterMap();
    void createBRDFLUTTexture();
    void renderCube();
    void renderQuad();
    void deleteResources();
};
