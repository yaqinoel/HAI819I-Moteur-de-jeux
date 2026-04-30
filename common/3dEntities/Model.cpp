#include "Model.hpp"
#include <iostream>

Model::Model(std::string const &path, bool usePBR) {
    this->isPBR = usePBR;
    loadModel(path);
}

Model::~Model() {
}

void Model::loadModel(std::string const &path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh* newMesh = processMesh(mesh, scene);
        meshes.push_back(newMesh);
        this->addChild(newMesh);
    }
    
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh* Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        // Positions
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        
        // Normals
        if (mesh->HasNormals()){
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
        
        // Texture Coordinates
        if(mesh->mTextureCoords[0]) {
            vertex.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            // Tangent
            if (mesh->HasTangentsAndBitangents()){
                vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
                vertex.bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
            }
        }
        else {
            vertex.texCoord = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // Indices
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        if (face.mNumIndices == 3) {
            triangles.push_back(Triangle(face.mIndices[0], face.mIndices[1], face.mIndices[2]));
        }
    }

    // Process Materials
    Material* finalMaterial = nullptr;
    
    if (this->isPBR) {
        PBRMaterial* pbrMat = new PBRMaterial();
        if(mesh->mMaterialIndex >= 0) {
            aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
            
            // Albedo / Diffuse
            loadMaterialTextures(aiMat, aiTextureType_BASE_COLOR, "albedoMap", pbrMat);
            if (pbrMat->texmap.find("albedoMap") == pbrMat->texmap.end()) {
                loadMaterialTextures(aiMat, aiTextureType_DIFFUSE, "albedoMap", pbrMat);
            }
            
            // Normal
            loadMaterialTextures(aiMat, aiTextureType_NORMALS, "normalMap", pbrMat);
            
            // Metallic
            loadMaterialTextures(aiMat, aiTextureType_METALNESS, "metallicMap", pbrMat);
            
            // Roughness
            loadMaterialTextures(aiMat, aiTextureType_DIFFUSE_ROUGHNESS, "roughnessMap", pbrMat);
            
            // AO
            loadMaterialTextures(aiMat, aiTextureType_AMBIENT_OCCLUSION, "aoMap", pbrMat);
            if (pbrMat->texmap.find("aoMap") == pbrMat->texmap.end()) {
                loadMaterialTextures(aiMat, aiTextureType_LIGHTMAP, "aoMap", pbrMat);
            }
        }
        finalMaterial = pbrMat;
    } else {
        Material* standardMat = new Material();
        if(mesh->mMaterialIndex >= 0) {
            aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
            
            loadMaterialTextures(aiMat, aiTextureType_DIFFUSE, "texture_diffuse", standardMat);
            loadMaterialTextures(aiMat, aiTextureType_SPECULAR, "texture_specular", standardMat);
        }
        finalMaterial = standardMat;
    }

    Mesh* engineMesh = new Mesh(vertices, triangles);
    engineMesh->setMaterial(finalMaterial);
    
    return engineMesh;
}

void Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, Material* material) {
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        
        std::string filename = std::string(str.C_Str());
        std::string path = directory + '/' + filename;
        
        if (textures_loaded.find(path) != textures_loaded.end()) {
            material->addTexture(typeName, textures_loaded[path]);
        } else {
            Texture newTexture(path);
            textures_loaded[path] = newTexture;
            material->addTexture(typeName, newTexture);
        }
    }
}
