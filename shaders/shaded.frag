#version 460 core

in vec3 normal;
in vec3 fragPos;
in vec3 viewLightPos;

out vec4 FragColour;

uniform vec3 colour;
uniform vec3 ambientLight;
uniform vec3 lightColour;

void main()
{
    float specularStrength = 0.5;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(viewLightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseLight = diff * lightColour;

    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColour;

    vec3 result = (ambientLight + diffuseLight + specular) * colour;
    FragColour = vec4(result, 1.0);
}