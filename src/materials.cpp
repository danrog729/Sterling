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

void Material::use(maths::mat4f cameraMatrix, maths::mat4f modelMatrix, Colour* ambientLight)
{
	shader->use();
	shader->setMat4f("worldToScreen", cameraMatrix);
	shader->setMat4f("modelToWorld", modelMatrix);
}

UnlitMaterial::UnlitMaterial(float red, float green, float blue) : Material("shaders/unlit.vert", "shaders/unlit.frag", red, green, blue)
{

}

void UnlitMaterial::use(maths::mat4f cameraMatrix, maths::mat4f modelMatrix, Colour* ambientLight)
{
	shader->use();
	shader->setMat4f("worldToScreen", cameraMatrix);
	shader->setMat4f("modelToWorld", modelMatrix);
	shader->setVec3f("colour", colour.red, colour.green, colour.blue);
}

ShadedSmoothMaterial::ShadedSmoothMaterial(float red, float green, float blue) : Material("shaders/shadedSmooth.vert", "shaders/shadedSmooth.frag", red, green, blue)
{

}

void ShadedSmoothMaterial::use(maths::mat4f cameraMatrix, maths::mat4f modelMatrix, Colour* ambientLight)
{
	shader->use();
	shader->setMat4f("worldToScreen", cameraMatrix);
	shader->setMat4f("modelToWorld", modelMatrix);
	shader->setVec3f("colour", colour.red, colour.green, colour.blue);
	shader->setVec3f("ambientLight", ambientLight->red, ambientLight->green, ambientLight->blue);
}

ShadedFlatMaterial::ShadedFlatMaterial(float red, float green, float blue) : Material("shaders/shadedFlat.vert", "shaders/shadedFlat.frag", red, green, blue)
{

}

void ShadedFlatMaterial::use(maths::mat4f cameraMatrix, maths::mat4f modelMatrix, Colour* ambientLight)
{
	shader->use();
	shader->setMat4f("worldToScreen", cameraMatrix);
	shader->setMat4f("modelToWorld", modelMatrix);
	shader->setVec3f("colour", colour.red, colour.green, colour.blue);
	shader->setVec3f("ambientLight", ambientLight->red, ambientLight->green, ambientLight->blue);
}