#pragma once

#include "node3d.h"
#include "Mesh.hpp"
#include "../Materials/pbrmaterial.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>
#include <map>

class Model : public Node3d {
public:
    Model(std::string const &path, bool usePBR = false);
    ~Model();

    void loadModel(std::string const &path);

    bool isPBR;
    std::vector<Mesh*> meshes;
    std::string directory;

private:
    void processNode(aiNode *node, const aiScene *scene);
    Mesh* processMesh(aiMesh *mesh, const aiScene *scene);
    void loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, Material* material);

    std::map<std::string, Texture> textures_loaded;
};
