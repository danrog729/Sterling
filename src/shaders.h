#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	// The unique shader program ID, for use with OpenGL functions
	unsigned int ID;

	/// <summary>
	/// Reads the shader files, compiles them and links them into a shader program.
	/// </summary>
	/// <param name="vertexPath">: The path of the vertex shader to compile and link</param>
	/// <param name="fragmentPath">: The path of the fragment shader to compile and link</param>
	Shader(const char* vertexPath, const char* fragmentPath);
	/// <summary>
	/// Binds the shader
	/// </summary>
	void use() const;
	/// <summary>
	/// Sets a Boolean inside the shader
	/// </summary>
	/// <param name="name">: The identifier of the bool inside the shader code</param>
	/// <param name="value">: The value to give the bool</param>
	void setBool(const std::string& name, bool value) const;
	/// <summary>
	/// Sets an integer inside the shader
	/// </summary>
	/// <param name="name">: The identifier of the int inside the shader code</param>
	/// <param name="value">: The value to give the int</param>
	void setInt(const std::string& name, int value) const;
	/// <summary>
	/// Sets a float inside the shader
	/// </summary>
	/// <param name="name">: The identifier of the float inside the shader code</param>
	/// <param name="value">: The value to give the float</param>
	void setFloat(const std::string& name, float value) const;
	/// <summary>
	/// Sets a vector4 of floats inside the shader
	/// </summary>
	/// <param name="name">: The identifier of the vector4 inside the shader code</param>
	/// <param name="value">: The values to give the vector4</param>
	void setVec4f(const std::string& name, float x, float y, float z, float w) const;
};

#endif