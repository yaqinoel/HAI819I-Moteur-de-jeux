#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
in mat3 TBN;

// material parameters
struct PBRMaterial {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
};
uniform PBRMaterial pbrMaterial;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;
uniform int useIBL;
uniform int debugIBLMode;

// has_ variables from our material.cpp
uniform int has_albedoMap;
uniform int has_normalMap;
uniform int has_metallicMap;
uniform int has_roughnessMap;
uniform int has_aoMap;

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform int pointLightCount;

uniform vec3 viewVector;
uniform vec3 camPos;

const float PI = 3.14159265359;

vec3 getAlbedo() { return has_albedoMap == 1 ? pow(texture(albedoMap, TexCoords).rgb, vec3(2.2)) : pbrMaterial.albedo; }
float getMetallic() { return has_metallicMap == 1 ? texture(metallicMap, TexCoords).r : pbrMaterial.metallic; }
float getRoughness() { return has_roughnessMap == 1 ? texture(roughnessMap, TexCoords).r : pbrMaterial.roughness; }
float getAO() { return has_aoMap == 1 ? texture(aoMap, TexCoords).r : pbrMaterial.ao; }
vec3 getNormal() {
    if(has_normalMap == 1) {
        vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;
        return normalize(TBN * tangentNormal);
    }
    return normalize(Normal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0000001); // prevent divide by zero
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{		
    vec3 albedo     = getAlbedo();
    float metallic  = getMetallic();
    float roughness = getRoughness();
    float ao        = getAO();
    vec3 N          = getNormal();

    vec3 V = normalize(camPos - WorldPos);

    if (debugIBLMode == 1) {
        FragColor = vec4(normalize(N) * 0.5 + 0.5, 1.0);
        return;
    }
    if (debugIBLMode == 2) {
        FragColor = vec4(vec3(roughness), 1.0);
        return;
    }
    if (debugIBLMode == 3) {
        vec3 R = reflect(-V, N);
        FragColor = vec4(textureLod(prefilterMap, R, roughness * 4.0).rgb, 1.0);
        return;
    }
    if (debugIBLMode == 4) {
        float NdotV = max(dot(N, V), 0.0);
        vec2 envBRDF = texture(brdfLUT, vec2(NdotV, roughness)).rg;
        FragColor = vec4(envBRDF.r, envBRDF.g, 0.0, 1.0);
        return;
    }
    if (debugIBLMode == 5) {
        vec3 irradiance = texture(irradianceMap, N).rgb;
        FragColor = vec4(irradiance * albedo, 1.0);
        return;
    }

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    for(int i = 0; i < pointLightCount; ++i)
    {
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        float NdotL = max(dot(N, L), 0.0);

        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    vec3 ambient = vec3(0.03) * albedo * ao;
    if (useIBL == 1) {
        vec3 R = reflect(-V, N);
        float NdotV = max(dot(N, V), 0.0);

        vec3 F = fresnelSchlickRoughness(NdotV, F0, roughness);
        vec3 kS = F;
        vec3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;

        vec3 irradiance = texture(irradianceMap, N).rgb;
        vec3 diffuse = irradiance * albedo;

        const float MAX_REFLECTION_LOD = 4.0;
        vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
        vec2 envBRDF = texture(brdfLUT, vec2(NdotV, roughness)).rg;
        vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

        ambient = (kD * diffuse + specular) * ao;
    }
    
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}
