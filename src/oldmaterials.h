#ifndef STERLING_OLDMATERIALS_H
#define STERLING_OLDMATERIALS_H

#include "shaders.h"
#include "maths.h"

class Colour
{
public:
	float red;
	float green;
	float blue;

	Colour();
	Colour(float R, float G, float B);
};

class OldMaterial
{
protected:
	Shader* shader;
public:
	Colour colour;

	OldMaterial(const char* vertexShaderPath, const char* fragmentShaderPath, float red, float green, float blue);
	virtual void use(maths::mat4f projectionMatrix, maths::mat4f viewMatrix, maths::mat4f modelMatrix,
		Colour* ambientLight, maths::vec3f lightPos, Colour* lightColour);
};

class UnlitMaterial : public OldMaterial
{
public:
	UnlitMaterial(float red, float green, float blue);
	void use(maths::mat4f projectionMatrix, maths::mat4f viewMatrix, maths::mat4f modelMatrix,
		Colour* ambientLight, maths::vec3f lightPos, Colour* lightColour);
};

class ShadedMaterial : public OldMaterial
{
public:
	float shininess;
	ShadedMaterial(float red, float green, float blue, float shininess);
	void use(maths::mat4f projectionMatrix, maths::mat4f viewMatrix, maths::mat4f modelMatrix,
		Colour* ambientLight, maths::vec3f lightPos, Colour* lightColour);
};

#endif