#version 460 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 normal;
in vec3 fragPos;
in vec3 viewLightPos;

out vec4 FragColour;

uniform Material material;
uniform Light light;

void main()
{
    float specularStrength = 0.5;

    vec3 ambientLight = light.ambient * material.ambient;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(viewLightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseLight = light.diffuse * diff * material.diffuse;

    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specularLight = light.specular * material.specular * spec;

    vec3 result = ambientLight + diffuseLight + specularLight;
    FragColour = vec4(result, 1.0);
}