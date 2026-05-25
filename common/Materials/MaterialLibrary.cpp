#include "MaterialLibrary.h"

#include "texture.h"
#include "common/Render/RenderSystem.hpp"
#include "common/Render/Shader.hpp"

MaterialLibrary::MaterialLibrary(RenderSystem* renderer)
    : renderer(renderer) {
}

MaterialLibrary::~MaterialLibrary() {
    for (Material* material : ownedMaterials) {
        delete material;
    }
}

Shader* MaterialLibrary::getStandardShader() {
    if (!renderer) return nullptr;
    return renderer->getOrCreateShader("../Shaders/vertex_shader.glsl", "../Shaders/fragment_shader.glsl");
}

Shader* MaterialLibrary::getPBRShader() {
    if (!renderer) return nullptr;
    return renderer->getOrCreateShader("../Shaders/vertex_shader_pbr.glsl", "../Shaders/fragment_shader_pbr.glsl");
}

Material* MaterialLibrary::getSharedUnlitGrassCube() {
    Material* cached = getShared("unlit_grass_cube");
    if (cached) return cached;

    Material* material = createUnlitGrassCube();
    setShared("unlit_grass_cube", material);
    return material;
}

Material* MaterialLibrary::getSharedUnlitRock() {
    Material* cached = getShared("unlit_rock");
    if (cached) return cached;

    Material* material = createUnlitRock();
    setShared("unlit_rock", material);
    return material;
}

Material* MaterialLibrary::getSharedUnlitSnowRock() {
    Material* cached = getShared("unlit_snow_rock");
    if (cached) return cached;

    Material* material = createUnlitSnowRock();
    setShared("unlit_snow_rock", material);
    return material;
}

TerrainMaterial* MaterialLibrary::getSharedClassicTerrain() {
    Material* cached = getShared("classic_terrain");
    if (cached) return static_cast<TerrainMaterial*>(cached);

    TerrainMaterial* material = createClassicTerrain();
    setShared("classic_terrain", material);
    return material;
}

PBRMaterial* MaterialLibrary::getSharedRustIron() {
    Material* cached = getShared("rust_iron");
    if (cached) return static_cast<PBRMaterial*>(cached);

    PBRMaterial* material = createRustIron();
    setShared("rust_iron", material);
    return material;
}

PBRMaterial* MaterialLibrary::getSharedGold() {
    Material* cached = getShared("gold");
    if (cached) return static_cast<PBRMaterial*>(cached);

    PBRMaterial* material = createGold();
    setShared("gold", material);
    return material;
}

PBRMaterial* MaterialLibrary::getSharedPBRGrass() {
    Material* cached = getShared("pbr_grass");
    if (cached) return static_cast<PBRMaterial*>(cached);

    PBRMaterial* material = createPBRGrass();
    setShared("pbr_grass", material);
    return material;
}

PBRMaterial* MaterialLibrary::getSharedPlastic() {
    Material* cached = getShared("plastic");
    if (cached) return static_cast<PBRMaterial*>(cached);

    PBRMaterial* material = createPlastic();
    setShared("plastic", material);
    return material;
}

PBRMaterial* MaterialLibrary::getSharedWall() {
    Material* cached = getShared("wall");
    if (cached) return static_cast<PBRMaterial*>(cached);

    PBRMaterial* material = createWall();
    setShared("wall", material);
    return material;
}

PBRMaterial* MaterialLibrary::getSharedPBRBlockTerrain() {
    Material* cached = getShared("pbr_block_terrain");
    if (cached) return static_cast<PBRMaterial*>(cached);

    PBRMaterial* material = new PBRMaterial(getPBRShader(), glm::vec3(0.45f, 0.55f, 0.38f), 0.0f, 0.98f, 0.85f);
    Texture blockAlbedo("../Resources/Textures/Environement/BlocTextures.png");
    blockAlbedo.setPixelArt(true);
    material->addTexture("albedoMap", blockAlbedo);
    remember("PBR Block Terrain", material);
    setShared("pbr_block_terrain", material);
    return material;
}

PBRMaterial* MaterialLibrary::getSharedPBRBlockAtlas() {
    return getSharedPBRBlockTerrain();
}

PBRMaterial* MaterialLibrary::getSharedPBRGrassCube() {
    Material* cached = getShared("pbr_grass_cube");
    if (cached) return static_cast<PBRMaterial*>(cached);

    PBRMaterial* material = new PBRMaterial(getPBRShader(), glm::vec3(1.0f), 0.0f, 0.98f, 0.85f);
    Texture albedo("../Resources/Textures/Environement/grassCubeTexSharp.png");
    albedo.setPixelArt(true);
    material->addTexture("albedoMap", albedo);
    remember("PBR Grass Cube", material);
    setShared("pbr_grass_cube", material);
    return material;
}

PBRMaterial* MaterialLibrary::getSharedPBRGrassBlock() {
    return getSharedPBRGrassCube();
}

Material* MaterialLibrary::createUnlitGrassCube() {
    Material* material = new Material(getStandardShader(), glm::vec3(1.0f));
    material->setLit(false);
    Texture albedo("../Resources/Textures/Environement/grassCubeTexSharp.png");
    albedo.setPixelArt(true);
    material->addTexture("texture0", albedo);
    return remember("Unlit Grass Cube", material);
}

Material* MaterialLibrary::createUnlitRock() {
    Material* material = new Material(getStandardShader(), glm::vec3(1.0f));
    material->setLit(false);
    material->addTexture("texture0", Texture("../Resources/Textures/Environement/rock.png"));
    return remember("Unlit Rock", material);
}

Material* MaterialLibrary::createUnlitSnowRock() {
    Material* material = new Material(getStandardShader(), glm::vec3(1.0f));
    material->setLit(false);
    material->addTexture("texture0", Texture("../Resources/Textures/Environement/snowrocks.png"));
    return remember("Unlit Snow Rock", material);
}

TerrainMaterial* MaterialLibrary::createClassicTerrain() {
    TerrainMaterial* material = new TerrainMaterial(glm::vec3(1.0f));
    material->shader = getStandardShader();
    material->setLit(false);
    material->addTexture("texture0", Texture("../Resources/Textures/Environement/grass.png"));
    material->addTexture("texture1", Texture("../Resources/Textures/Environement/rock.png"));
    material->addTexture("texture2", Texture("../Resources/Textures/Environement/snowrocks.png"));
    material->maxHeight = 20.0f;
    material->scale = 10.0f;
    return static_cast<TerrainMaterial*>(remember("Classic Terrain", material));
}

PBRMaterial* MaterialLibrary::createRustIron() {
    return createPBRTextureSet("Rust Iron", "../Resources/Textures/RustIron_pixel16", 1.0f, 0.5f, 1.0f, true);
}

PBRMaterial* MaterialLibrary::createGold() {
    return createPBRTextureSet("Gold", "../Resources/Textures/gold_pixel16", 1.0f, 0.35f, 1.0f, true);
}

PBRMaterial* MaterialLibrary::createPBRGrass() {
    return createPBRTextureSet("PBR Grass", "../Resources/Textures/grass_pixel16", 0.0f, 0.85f, 1.0f, true);
}

PBRMaterial* MaterialLibrary::createPlastic() {
    return createPBRTextureSet("Plastic", "../Resources/Textures/plastic_pixel16", 0.0f, 0.55f, 1.0f, true);
}

PBRMaterial* MaterialLibrary::createWall() {
    return createPBRTextureSet("Wall", "../Resources/Textures/wall_pixel16", 0.0f, 0.75f, 1.0f, true);
}

PBRMaterial* MaterialLibrary::createPBRTextureSet(const std::string& name, const std::string& folder, float metallic, float roughness, float ao, bool pixelArt) {
    PBRMaterial* material = new PBRMaterial(getPBRShader(), glm::vec3(1.0f), metallic, roughness, ao);

    Texture albedo(folder + "/albedo.png");
    Texture normal(folder + "/normal.png");
    Texture metallicMap(folder + "/metallic.png");
    Texture roughnessMap(folder + "/roughness.png");
    Texture aoMap(folder + "/ao.png");

    if (pixelArt) {
        albedo.setPixelArt(true);
        normal.setPixelArt(true);
        metallicMap.setPixelArt(true);
        roughnessMap.setPixelArt(true);
        aoMap.setPixelArt(true);
    }

    material->addTexture("albedoMap", albedo);
    material->addTexture("normalMap", normal);
    material->addTexture("metallicMap", metallicMap);
    material->addTexture("roughnessMap", roughnessMap);
    material->addTexture("aoMap", aoMap);
    return static_cast<PBRMaterial*>(remember(name, material));
}

PBRMaterial* MaterialLibrary::createPBR(glm::vec3 albedo, float metallic, float roughness, float ao) {
    PBRMaterial* material = new PBRMaterial(getPBRShader(), albedo, metallic, roughness, ao);
    return static_cast<PBRMaterial*>(remember("PBR Material", material));
}

PBRMaterial* MaterialLibrary::clonePBR(const PBRMaterial* source) {
    if (!source) return nullptr;

    PBRMaterial* material = new PBRMaterial(source->shader, source->albedo, source->metallic, source->roughness, source->ao);
    material->lit = source->lit;
    material->scale = source->scale;
    material->texmap = source->texmap;
    return static_cast<PBRMaterial*>(remember("PBR Material Copy", material));
}

Material* MaterialLibrary::getMaterial(const std::string& name) const {
    auto it = materialsByName.find(name);
    if (it == materialsByName.end()) return nullptr;
    return it->second;
}

void MaterialLibrary::renameMaterial(Material* material, const std::string& newName) {
    if (!material) return;

    for (NamedMaterial& entry : namedMaterials) {
        if (entry.material != material) continue;

        materialsByName.erase(entry.name);
        entry.name = makeUniqueName(newName);
        materialsByName[entry.name] = material;
        return;
    }
}

void MaterialLibrary::registerExternalMaterial(const std::string& name, Material* material) {
    if (!material) return;

    std::string uniqueName = makeUniqueName(name);
    namedMaterials.push_back({uniqueName, material});
    materialsByName[uniqueName] = material;
}

Material* MaterialLibrary::remember(const std::string& name, Material* material) {
    if (material) {
        ownedMaterials.push_back(material);
        registerExternalMaterial(name, material);
    }
    return material;
}

Material* MaterialLibrary::getShared(const std::string& name) {
    auto it = sharedMaterials.find(name);
    if (it == sharedMaterials.end()) return nullptr;
    return it->second;
}

void MaterialLibrary::setShared(const std::string& name, Material* material) {
    if (material) {
        sharedMaterials[name] = material;
    }
}

std::string MaterialLibrary::makeUniqueName(const std::string& name) const {
    std::string baseName = name.empty() ? "Material" : name;
    if (materialsByName.find(baseName) == materialsByName.end()) {
        return baseName;
    }

    int index = 2;
    while (true) {
        std::string candidate = baseName + " " + std::to_string(index);
        if (materialsByName.find(candidate) == materialsByName.end()) {
            return candidate;
        }
        index++;
    }
}
