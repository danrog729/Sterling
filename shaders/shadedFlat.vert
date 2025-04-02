#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 normal;
out vec3 fragPos;

uniform mat4 modelToWorld;
uniform mat4 worldToScreen;

void main()
{
    gl_Position = worldToScreen * modelToWorld * vec4(aPos, 1.0);
    normal = aNormal;
    fragPos = vec3(modelToWorld * vec4(aPos, 1.0));
}
