#include "materials.h"

Colour::Colour()
{
	red = 0;
	green = 0;
	blue = 0;
}
Colour::Colour(float R, float G, float B)
{
	red = R;
	green = G;
	blue = B;
}

Material::Material(const char* vertexShaderPath, const char* fragmentShaderPath, float red, float green, float blue)
{
	shader = new Shader(vertexShaderPath, fragmentShaderPath);
	colour = Colour(red, green, blue);
}

void Material::use(maths::mat4f cameraMatrix, Colour* ambientLight)
{
	shader->use();
	shader->setMat4f("worldToScreen", cameraMatrix);
}

UnlitMaterial::UnlitMaterial(float red, float green, float blue) : Material("shaders/unlit.vert", "shaders/unlit.frag", red, green, blue)
{

}

void UnlitMaterial::use(maths::mat4f cameraMatrix, Colour* ambientLight)
{
	shader->use();
	shader->setMat4f("worldToScreen", cameraMatrix);
	shader->setVec4f("colour", colour.red, colour.green, colour.blue, 1.0f);
}

ShadedMaterial::ShadedMaterial(float red, float green, float blue) : Material("shaders/shaded.vert", "shaders/shaded.frag", red, green, blue)
{

}

void ShadedMaterial::use(maths::mat4f cameraMatrix, Colour* ambientLight)
{
	shader->use();
	shader->setMat4f("worldToScreen", cameraMatrix);
	shader->setVec4f("colour", colour.red, colour.green, colour.blue, 1.0f);
	shader->setVec4f("ambientLight", ambientLight->red, ambientLight->green, ambientLight->blue, 1.0f);
}