#ifndef STERLING_MATERIALS_H
#define STERLING_MATERIALS_H

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

class Material
{
protected:
	Shader* shader;
public:
	Colour colour;

	Material(const char* vertexShaderPath, const char* fragmentShaderPath, float red, float green, float blue);
	virtual void use(maths::mat4f cameraMatrix, maths::mat4f modelMatrix, Colour* ambientLight);
};

class UnlitMaterial : public Material
{
public:
	UnlitMaterial(float red, float green, float blue);
	void use(maths::mat4f cameraMatrix, maths::mat4f modelMatrix, Colour* ambientLight);
};

class ShadedSmoothMaterial : public Material
{
public:
	ShadedSmoothMaterial(float red, float green, float blue);
	void use(maths::mat4f cameraMatrix, maths::mat4f modelMatrix, Colour* ambientLight);
};

class ShadedFlatMaterial : public Material
{
public:
	ShadedFlatMaterial(float red, float green, float blue);
	void use(maths::mat4f cameraMatrix, maths::mat4f modelMatrix, Colour* ambientLight);
};

#endif