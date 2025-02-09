#include "scene.h"
#include "glad/glad.h"

#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

/*
Object
*/

void Object::load_mesh(const char* path)
{
	// create the new mesh struct
	mesh = new Mesh();
	// retrieve the mesh data from filePath
	std::ifstream meshFile;
	// ensure ifstream objects can throw exceptions
	meshFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open file
		meshFile.open(path, std::ios::binary);

		// get length
		meshFile.seekg(0, std::ios::end);
		std::streamsize fileSize = meshFile.tellg();
		meshFile.seekg(0, std::ios::beg);

		// read flags
		char* flags = new char[4];
		meshFile.read(flags, 4);
		mesh->colouringMode = flags[3];

		// read addresses
		int verticesAddress, edgesAddress, facesAddress;
		meshFile.read(reinterpret_cast<char*>(&verticesAddress), sizeof(int));
		meshFile.read(reinterpret_cast<char*>(&edgesAddress), sizeof(int));
		meshFile.read(reinterpret_cast<char*>(&facesAddress), sizeof(int));

		// set counts
		mesh->vertexCount = (edgesAddress - verticesAddress) / sizeof(float);
		mesh->edgeRefCount = (facesAddress - edgesAddress) / sizeof(int);
		mesh->faceRefCount = (fileSize - facesAddress) / sizeof(int);

		// allocate memory for vertices, edges, faces
		mesh->vertices = new float[mesh->vertexCount];
		mesh->edges = new int[mesh->edgeRefCount];
		mesh->faces = new int[mesh->faceRefCount];

		// fill the arrays with the info
		meshFile.seekg(verticesAddress, std::ios::beg);
		meshFile.read(reinterpret_cast<char*>(mesh->vertices), mesh->vertexCount * sizeof(float));
		meshFile.seekg(edgesAddress, std::ios::beg);
		meshFile.read(reinterpret_cast<char*>(mesh->edges), mesh->edgeRefCount * sizeof(int));
		meshFile.seekg(facesAddress, std::ios::beg);
		meshFile.read(reinterpret_cast<char*>(mesh->faces), mesh->faceRefCount * sizeof(int));

		// close file handler
		meshFile.close();
	}
	catch (std::ifstream::failure e)
	{
		std::cerr << "ERROR::SHADER::CANNOT_READ_FILE\n" << e.what() << std::endl;
	}
}

void Object::generate_buffers()
{
	unsigned int VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertexCount * sizeof(float) * 3, mesh->vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	if (mesh->faceRefCount != 0)
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->faceRefCount * sizeof(int), mesh->faces, GL_STATIC_DRAW);
	}
	else
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->edgeRefCount * sizeof(int), mesh->edges, GL_STATIC_DRAW);
	}
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

Object::Object(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale)
{
	position = Position;
	rotation = Rotation;
	scale = Scale;
	mesh = NULL;
	VAO = 0;
}
Object::Object(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, const char* meshPath)
{
	position = Position;
	rotation = Rotation;
	scale = Scale;
	load_mesh(meshPath);
	generate_buffers();
}
maths::mat4f Object::local_to_world()
{
	maths::mat4f scaleMatrix = maths::mat4f(
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	maths::mat4f translationMatrix = maths::mat4f(
		1.0f, 0.0f, 0.0f, position.x,
		0.0f, 1.0f, 0.0f, position.y,
		0.0f, 0.0f, 1.0f, position.z,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	return translationMatrix * rotation.to_rotation_matrix() * scaleMatrix;
}
void Object::render()
{
	glBindVertexArray(VAO);
	if (mesh->faceRefCount != 0)
	{
		glDrawElements(GL_TRIANGLES, mesh->faceRefCount, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawElements(GL_LINES, mesh->edgeRefCount, GL_UNSIGNED_INT, 0);
	}
}

/*
Empty
*/

Empty::Empty(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale) : Object(Position, Rotation, Scale)
{

}
Empty::Empty(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, const char* meshPath) : Object(Position, Rotation, Scale, meshPath)
{

}

/*
Axes
*/

Axes::Axes(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale) : Empty(Position, Rotation, Scale, "models/axes.mdl")
{

}

/*
Camera
*/

Camera::Camera(maths::vec3f Position, maths::unit_quaternion Rotation, float FOV, float NearClip, float FarClip, float AspectRatio) : Empty(Position, Rotation, maths::vec3f(1.0f, 1.0f, 1.0f), "models/camera.mdl")
{
	fov = FOV;
	nearClip = NearClip;
	farClip = FarClip;
	aspectRatio = AspectRatio;
	scale = maths::vec3f(tanf(fov / 2.0f), tanf(fov / 2.0f) / aspectRatio, 1.0f);

}

maths::mat4f Camera::cameraspace_matrix()
{
	maths::mat4f locationMatrix = maths::mat4f(
		1.0f, 0.0f, 0.0f, -position.x,
		0.0f, 1.0f, 0.0f, -position.y,
		0.0f, 0.0f, 1.0f, -position.z,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	maths::mat4f rotationMatrix = rotation.conjugate().to_rotation_matrix();
	return rotationMatrix * locationMatrix;
}

maths::mat4f Camera::orthographic_matrix()
{
	return maths::mat4f(
		1.0f / (nearClip * tanf(fov / 2.0f)), 0.0f, 0.0f, 0.0f,
		0.0f, aspectRatio / (nearClip * tanf(fov / 2.0f)), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f / (farClip - nearClip), -nearClip / (farClip - nearClip),
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

maths::mat4f Camera::perspective_matrix()
{
	return maths::mat4f(
		nearClip, 0.0f, 0.0f, 0.0f,
		0.0f, nearClip, 0.0f, 0.0f,
		0.0f, 0.0f, farClip + nearClip, -farClip * nearClip,
		0.0f, 0.0f, 1.0f, 0.0f
	);
}

maths::vec3f Camera::front()
{
	maths::vec4f vector4 = rotation.to_rotation_matrix() * maths::vec4f(0.0f, 0.0f, -1.0f, 0.0f);
	return maths::vec3f(vector4.x, vector4.y, vector4.z);
}

maths::vec3f Camera::back()
{
	maths::vec4f vector4 = rotation.to_rotation_matrix() * maths::vec4f(0.0f, 0.0f, 1.0f, 0.0f);
	return maths::vec3f(vector4.x, vector4.y, vector4.z);
}

maths::vec3f Camera::left()
{
	maths::vec4f vector4 = rotation.to_rotation_matrix() * maths::vec4f(-1.0f, 0.0f, 0.0f, 0.0f);
	return maths::vec3f(vector4.x, vector4.y, vector4.z);
}

maths::vec3f Camera::right()
{
	maths::vec4f vector4 = rotation.to_rotation_matrix() * maths::vec4f(1.0f, 0.0f, 0.0f, 0.0f);
	return maths::vec3f(vector4.x, vector4.y, vector4.z);
}

maths::vec3f Camera::up()
{
	maths::vec4f vector4 = rotation.to_rotation_matrix() * maths::vec4f(0.0f, 1.0f, 0.0f, 0.0f);
	return maths::vec3f(vector4.x, vector4.y, vector4.z);
}

maths::vec3f Camera::down()
{
	maths::vec4f vector4 = rotation.to_rotation_matrix() * maths::vec4f(0.0f, -1.0f, 0.0f, 0.0f);
	return maths::vec3f(vector4.x, vector4.y, vector4.z);
}

/*
Model
*/

Model::Model(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, const char* meshPath) : Object(Position, Rotation, Scale, meshPath)
{

}

/*
Cube
*/

Cube::Cube(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale) : Model(Position, Rotation, Scale, "models/cube.mdl")
{

}

/*
Plane
*/

Plane::Plane(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale) : Model(Position, Rotation, Scale, "models/plane.mdl")
{

}