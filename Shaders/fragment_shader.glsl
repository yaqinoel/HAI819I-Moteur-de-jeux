#version 330 core

// Ouput data
out vec4 color;
in vec3 vertices_position_fragment;
in vec3 vertices_normals_fragment;
in vec2 vertices_texCoord_fragment;

uniform sampler2D texture0;
uniform int hasTexture;
uniform int lit;

uniform mat4 model;
uniform mat4 view;

struct Material {
    vec3 albedo;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

uniform Material material;

const vec3 light_position = vec3(0.0, 0.0, 0.0);
const vec4 light_color_power = vec4(1.0, 1.0, 1.0, 10.0);

void main()
{
    vec3 N = normalize(vertices_normals_fragment);

    // Camera position from inverse view matrix
    vec3 cameraPos = vec3(inverse(view)[3]);
    vec3 V = normalize(cameraPos - vertices_position_fragment);

    // Ambient term
    vec3 ambient = material.ambient * material.albedo;

    // Diffuse and specular
    vec3 L = normalize(light_position - vertices_position_fragment);
    float distance = sqrt(length(light_position - vertices_position_fragment));

    // Diffuse
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = material.diffuse * NdotL * material.albedo;

    // Specular
    vec3 H = normalize(L + V);
    float NdotH = max(dot(N, H), 0.0);
    float spec = pow(NdotH, material.shininess);
    vec3 specular = material.specular * spec * light_color_power.rgb;

    // Attenuation
    float attenuation = light_color_power.a / (distance * distance);

    vec3 litColor = ambient + (diffuse + specular) * light_color_power.rgb * attenuation;

    // Combine with texture
    vec4 textureColor = hasTexture != 0 ? texture(texture0, vertices_texCoord_fragment) : vec4(1.0);
    if(lit == 1) color = vec4(litColor * textureColor.rgb, textureColor.a);
    else color = hasTexture != 0 ? textureColor : vec4(material.albedo, 1.0);
}
