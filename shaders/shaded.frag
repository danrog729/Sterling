#version 460 core

struct PointLight
{
    vec3 colour;
    vec3 position;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};

struct Spotlight
{
    vec3 colour;
    vec3 position;
    vec3 direction;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
    float innerCutoff;
    float outerCutoff;
};

struct DirectionalLight
{
    vec3 colour;
    vec3 direction;
};

layout (std140, binding = 1) uniform Lights
{
    vec3 ambientLightColour;
    PointLight pointLights[16];
    Spotlight spotLights[16];
    DirectionalLight directionalLights[16];
    uint pointLightCount;
    uint spotlightCount;
    uint directionalLightCount;
};

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
struct Textures
{
    bool useAmbient;
    sampler2D ambient;
    bool useDiffuse;
    sampler2D diffuse;
    bool useSpecular;
    sampler2D specular;
};

in vec3 normal;
in vec3 fragPos;
in vec3 viewLightPos;
in vec2 TexCoord;

out vec4 FragColour;

uniform Material material;
uniform Textures textures;

vec3 pointLightContribution(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 spotlightContribution(Spotlight spotlight, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 directionalLightContribution(DirectionalLight directionalLight, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 ambientLight = ambientLightColour * material.ambient;

    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(-fragPos);

    vec3 result = ambientLight;

    for (int lightIndex = 0; lightIndex < pointLightCount; lightIndex++)
    {
        result += pointLightContribution(pointLights[lightIndex], norm, fragPos, viewDir);
    }
    for (int lightIndex = 0; lightIndex < spotlightCount; lightIndex++)
    {
        result += spotlightContribution(spotLights[lightIndex], norm, fragPos, viewDir);
    }
    for (int lightIndex = 0; lightIndex < directionalLightCount; lightIndex++)
    {
        result += directionalLightContribution(directionalLights[lightIndex], norm, fragPos, viewDir);
    }

    FragColour = vec4(result, 1.0);
}

vec3 pointLightContribution(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(pointLight.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float dist = length(pointLight.position - fragPos);
    float attenuation = 1.0 / (pointLight.constantAttenuation + pointLight.linearAttenuation * dist + pointLight.quadraticAttenuation * dist * dist);
    // combine
    vec3 diffuse;
    if (textures.useDiffuse) 
    {
        diffuse = pointLight.colour * diff * vec3(texture(textures.diffuse, TexCoord));
    }
    else
    {
        diffuse = pointLight.colour * diff * material.diffuse;
    }
    vec3 specular;
    if (textures.useSpecular)
    {
        specular = pointLight.colour * spec * vec3(texture(textures.specular, TexCoord));
    }
    else
    {
        specular = pointLight.colour * spec * material.specular;
    }
    return diffuse * attenuation + specular * attenuation;
}

vec3 spotlightContribution(Spotlight spotlight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(spotlight.position - fragPos);
    float costheta = dot(lightDir, normalize(-spotlight.direction));
    float epsilon = cos(spotlight.innerCutoff) - cos(spotlight.outerCutoff);
    float intensity = clamp((costheta - cos(spotlight.outerCutoff)) / epsilon, 0.0, 1.0);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float dist = length(spotlight.position - fragPos);
    float attenuation = 1.0 / (spotlight.constantAttenuation + spotlight.linearAttenuation * dist + spotlight.quadraticAttenuation * dist * dist);
    // combine
    vec3 diffuse;
    if (textures.useDiffuse) 
    {
        diffuse = spotlight.colour * diff * vec3(texture(textures.diffuse, TexCoord));
    }
    else
    {
        diffuse = spotlight.colour * diff * material.diffuse;
    }
    vec3 specular;
    if (textures.useSpecular)
    {
        specular = spotlight.colour * spec * vec3(texture(textures.specular, TexCoord));
    }
    else
    {
        specular = spotlight.colour * spec * material.specular;
    }
    return intensity * (diffuse * attenuation + specular * attenuation);
}

vec3 directionalLightContribution(DirectionalLight directionalLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(directionalLight.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine
    vec3 diffuse;
    if (textures.useDiffuse) 
    {
        diffuse = directionalLight.colour * diff * vec3(texture(textures.diffuse, TexCoord));
    }
    else
    {
        diffuse = directionalLight.colour * diff * material.diffuse;
    }
    vec3 specular;
    if (textures.useSpecular)
    {
        specular = directionalLight.colour * spec * vec3(texture(textures.specular, TexCoord));
    }
    else
    {
        specular = directionalLight.colour * spec * material.specular;
    }
    return diffuse + specular;
}