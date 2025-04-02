#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 worldToScreen;

void main()
{
    gl_Position = worldToScreen * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
