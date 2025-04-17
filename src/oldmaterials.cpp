#include "oldmaterials.h"

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

OldMaterial::OldMaterial(const char* vertexShaderPath, const char* fragmentShaderPath, float red, float green, float blue)
{
	shader = new Shader(vertexShaderPath, fragmentShaderPath);
	colour = Colour(red, green, blue);
}

void OldMaterial::use(maths::mat4f projectionMatrix, maths::mat4f viewMatrix, maths::mat4f modelMatrix,
	Colour* ambientLight, maths::vec3f lightPos, Colour* lightColour)
{
	shader->use();
	shader->setMat4f("model", modelMatrix);
	shader->setMat4f("view", viewMatrix);
	shader->setMat4f("projection", projectionMatrix);
}

UnlitMaterial::UnlitMaterial(float red, float green, float blue) : OldMaterial("shaders/unlit.vert", "shaders/unlit.frag", red, green, blue)
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

ShadedMaterial::ShadedMaterial(float red, float green, float blue, float shininess) : OldMaterial("shaders/shaded.vert", "shaders/shaded.frag", red, green, blue)
{
	this->shininess = shininess;
}

void ShadedMaterial::use(
	maths::mat4f projectionMatrix, maths::mat4f viewMatrix, maths::mat4f modelMatrix, 
	Colour* ambientLight, maths::vec3f lightPos, Colour* lightColour)
{
	shader->use();
	shader->setMat4f("model", modelMatrix);
	shader->setMat4f("view", viewMatrix);
	shader->setMat4f("projection", projectionMatrix);

	shader->setVec3f("material.ambient", colour.red, colour.green, colour.blue);
	shader->setVec3f("material.diffuse", colour.red, colour.green, colour.blue);
	shader->setVec3f("material.specular", colour.red, colour.green, colour.blue);
	shader->setFloat("material.shininess", shininess);

	shader->setVec3f("light.ambient", ambientLight->red, ambientLight->green, ambientLight->blue);
	shader->setVec3f("light.position", lightPos.x, lightPos.y, lightPos.z);
	shader->setVec3f("light.diffuse", lightColour->red, lightColour->green, lightColour->blue);
	shader->setVec3f("light.specular", lightColour->red, lightColour->green, lightColour->blue);
}