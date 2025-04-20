#include "material.h"

Material::Material(const char* vertexShader, const char* fragmentShader)
{
	// set up the shader
	shader = new Shader(vertexShader, fragmentShader);

	ambientColour = maths::vec3f(0, 0, 0);
	diffuseColour = maths::vec3f(0, 0, 0);
	specularColour = maths::vec3f(0, 0, 0);
	emissionColour = maths::vec3f(0, 0, 0);
	transmittanceColour = maths::vec3f(0, 0, 0);
	shininess = 1;
	indexOfRefraction = 0;
	transmissionFilter = maths::vec3f(0, 0, 0);
	dissolve = 0;

	ambientMap = NULL;
	diffuseMap = NULL;
	specularMap = NULL;
	emissionMap = NULL;
	transmittanceMap = NULL;
	shininessMap = NULL;
	indexOfRefractionMap = NULL;
	dissolveMap = NULL;
	bumpMap = NULL;
}

void Material::use(maths::mat4f model)
{
	shader->use();
	shader->setMat4f("model", model);

	shader->setVec3f("material.ambient", ambientColour.x, ambientColour.y, ambientColour.z);
	shader->setVec3f("material.diffuse", diffuseColour.x, diffuseColour.y, diffuseColour.z);
	shader->setVec3f("material.specular", specularColour.x, specularColour.y, specularColour.z);
	shader->setFloat("material.shininess", shininess);

	if (ambientMap != NULL)
	{
		shader->setBool("textures.useAmbient", true);
		shader->setInt("textures.ambient", 0);
		glActiveTexture(GL_TEXTURE0);
		ambientMap->use();
	}
	if (diffuseMap != NULL)
	{
		shader->setBool("textures.useDiffuse", true);
		shader->setInt("textures.diffuse", 1);
		glActiveTexture(GL_TEXTURE1);
		diffuseMap->use();
	}
	if (specularMap != NULL)
	{
		shader->setBool("textures.useSpecular", true);
		shader->setInt("textures.specular", 2);
		glActiveTexture(GL_TEXTURE2);
		specularMap->use();
	}
}