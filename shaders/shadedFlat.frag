#version 460 core

in vec3 normal;

out vec4 FragColour;
in vec3 fragPos;

uniform vec3 colour;
uniform vec3 ambientLight;

void main()
{
    vec3 lightPos = vec3(2,5,5);
    vec3 lightColour = vec3(1,1,1);

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseLight = diff * lightColour;

    vec3 result = (ambientLight + diffuseLight) * colour;
    FragColour = vec4(result, 1.0);
}