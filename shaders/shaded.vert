#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 texCoord;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec3 normal;
out vec3 fragPos;
out vec3 viewLightPos;
out vec2 TexCoord;

uniform mat4 model;
uniform Light light;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vec3 modelNormal = mat3(transpose(inverse(model))) * aNormal;
    normal = mat3(view) * modelNormal;
    viewLightPos = vec3(view * vec4(light.position, 1.0));
    fragPos = vec3(view * model * vec4(aPos, 1.0));
    TexCoord = texCoord;
}
