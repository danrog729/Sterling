#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 normal;
out vec3 fragPos;
out vec3 viewLightPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPosition;
uniform vec3 lightColour;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vec3 modelNormal = mat3(transpose(inverse(model))) * aNormal;
    normal = mat3(view) * modelNormal;
    viewLightPos = vec3(view * vec4(lightPosition, 1.0));
    fragPos = vec3(view * model * vec4(aPos, 1.0));
}
