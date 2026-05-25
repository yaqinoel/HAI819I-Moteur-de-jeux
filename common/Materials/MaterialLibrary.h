#pragma once

#include "material.h"
#include "pbrmaterial.h"
#include "terrainmaterial.h"

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

class RenderSystem;
class Shader;

struct NamedMaterial {
    std::string name;
    Material* material = nullptr;
};

class MaterialLibrary {
public:
    explicit MaterialLibrary(RenderSystem* renderer);
    ~MaterialLibrary();

    Shader* getStandardShader();
    Shader* getPBRShader();

    Material* getSharedUnlitGrassCube();
    Material* getSharedUnlitRock();
    Material* getSharedUnlitSnowRock();
    TerrainMaterial* getSharedClassicTerrain();
    PBRMaterial* getSharedRustIron();
    PBRMaterial* getSharedGold();
    PBRMaterial* getSharedPBRGrass();
    PBRMaterial* getSharedPlastic();
    PBRMaterial* getSharedWall();
    PBRMaterial* getSharedPBRBlockTerrain();
    PBRMaterial* getSharedPBRBlockAtlas();
    PBRMaterial* getSharedPBRGrassCube();
    PBRMaterial* getSharedPBRGrassBlock();

    Material* createUnlitGrassCube();
    Material* createUnlitRock();
    Material* createUnlitSnowRock();
    TerrainMaterial* createClassicTerrain();
    PBRMaterial* createRustIron();
    PBRMaterial* createGold();
    PBRMaterial* createPBRGrass();
    PBRMaterial* createPlastic();
    PBRMaterial* createWall();
    PBRMaterial* createPBRTextureSet(const std::string& name, const std::string& folder, float metallic, float roughness, float ao, bool pixelArt = false);
    PBRMaterial* createPBR(glm::vec3 albedo, float metallic, float roughness, float ao);
    PBRMaterial* clonePBR(const PBRMaterial* source);

    Material* getMaterial(const std::string& name) const;
    const std::vector<NamedMaterial>& getMaterials() const { return namedMaterials; }
    void renameMaterial(Material* material, const std::string& newName);
    void registerExternalMaterial(const std::string& name, Material* material);

private:
    RenderSystem* renderer = nullptr;
    std::unordered_map<std::string, Material*> sharedMaterials;
    std::unordered_map<std::string, Material*> materialsByName;
    std::vector<Material*> ownedMaterials;
    std::vector<NamedMaterial> namedMaterials;

    Material* remember(const std::string& name, Material* material);
    Material* getShared(const std::string& name);
    void setShared(const std::string& name, Material* material);
    std::string makeUniqueName(const std::string& name) const;
};
