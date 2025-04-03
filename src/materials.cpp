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

void Material::use(maths::mat4f projectionMatrix, maths::mat4f viewMatrix, maths::mat4f modelMatrix,
	Colour* ambientLight, maths::vec3f lightPos, Colour* lightColour)
{
	shader->use();
	shader->setMat4f("model", modelMatrix);
	shader->setMat4f("view", viewMatrix);
	shader->setMat4f("projection", projectionMatrix);
}

UnlitMaterial::UnlitMaterial(float red, float green, float blue) : Material("shaders/unlit.vert", "shaders/unlit.frag", red, green, blue)
{

}

void UnlitMaterial::use(maths::mat4f projectionMatrix, maths::mat4f viewMatrix, maths::mat4f modelMatrix,
	Colour* ambientLight, maths::vec3f lightPos, Colour* lightColour)
{
	shader->use();
	shader->setMat4f("model", modelMatrix);
	shader->setMat4f("view", viewMatrix);
	shader->setMat4f("projection", projectionMatrix);
	shader->setVec3f("colour", colour.red, colour.green, colour.blue);
}

ShadedMaterial::ShadedMaterial(float red, float green, float blue) : Material("shaders/shaded.vert", "shaders/shaded.frag", red, green, blue)
{

}

void ShadedMaterial::use(
	maths::mat4f projectionMatrix, maths::mat4f viewMatrix, maths::mat4f modelMatrix, 
	Colour* ambientLight, maths::vec3f lightPos, Colour* lightColour)
{
	shader->use();
	shader->setMat4f("model", modelMatrix);
	shader->setMat4f("view", viewMatrix);
	shader->setMat4f("projection", projectionMatrix);
	shader->setVec3f("ambientLight", ambientLight->red, ambientLight->green, ambientLight->blue);
	shader->setVec3f("lightPosition", lightPos.x, lightPos.y, lightPos.z);
	shader->setVec3f("lightColour", lightColour->red, lightColour->green, lightColour->blue);
	shader->setVec3f("colour", colour.red, colour.green, colour.blue);
}