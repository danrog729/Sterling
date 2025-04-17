#ifndef STERLING_MATERIAL_H
#define STERLING_MATERIAL_H

#include "maths.h"
#include "textures.h"
#include "shaders.h"

class Material
{
private:
	Shader* shader;

public:
	maths::vec3f ambientColour;
	maths::vec3f diffuseColour;
	maths::vec3f specularColour;
	maths::vec3f emissionColour;
	maths::vec3f transmittanceColour;
	float shininess;
	float indexOfRefraction;
	maths::vec3f transmissionFilter;
	float dissolve;
	
	Texture2D* ambientMap;
	Texture2D* diffuseMap;
	Texture2D* specularMap;
	Texture2D* emissionMap;
	Texture2D* transmittanceMap;
	Texture2D* shininessMap;
	Texture2D* indexOfRefractionMap;
	Texture2D* dissolveMap;
	Texture2D* bumpMap;

	/// <summary>
	/// Initialise the material, setting up the shader
	/// </summary>
	/// <param name="vertexShader">The path to the vertex shader to use</param>
	/// <param name="fragmentShader">The path to the fragment shader to use</param>
	Material(const char* vertexShader, const char* fragmentShader);

	/// <summary>
	/// Set OpenGL to use this material for any future rendering calls
	/// </summary>
	/// <param name="model">The matrix for the object's local space -> world space</param>
	void use(maths::mat4f model);
};

#endif