#version 460 core

out vec4 FragColour;

uniform vec4 colour;
uniform vec4 ambientLight;

void main()
{
    FragColour = colour * ambientLight;
}