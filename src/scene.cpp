#include "scene.h"
#include "glad/glad.h"

#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

/*
Scene
*/

void Scene::render_branch(Object* branch, maths::mat4f parentToScreen)
{
	for (int childIndex = 0; childIndex < branch->children->count; childIndex++)
	{
		render_branch((branch->children)->operator[](childIndex), parentToScreen * branch->local_to_world());
	}
	branch->render(parentToScreen, &ambientLight);
}

Scene::Scene(Camera* activeCam, float ambientRed, float ambientGreen, float ambientBlue)
{
	activeCamera = activeCam;
	children = new ObjectList();
	ambientLight = Colour(ambientRed, ambientGreen, ambientBlue);
}

void Scene::render()
{
	// calculate global matrix
	maths::mat4f worldToScreen = activeCamera->orthographic_matrix() * activeCamera->perspective_matrix() * maths::mat4f::stretch_z(-1.0f) * activeCamera->cameraspace_matrix();
	// navigate the tree, rendering each object
	for (int child = 0; child < children->count; child++)
	{
		render_branch(children->operator[](child), worldToScreen);
	}
}

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
		unsigned int verticesAddress = 0x10, edgesAddress = 0x10, facesAddress = 0x10;
		meshFile.read(reinterpret_cast<char*>(&verticesAddress), sizeof(unsigned int));
		meshFile.read(reinterpret_cast<char*>(&edgesAddress), sizeof(unsigned int));
		meshFile.read(reinterpret_cast<char*>(&facesAddress), sizeof(unsigned int));

		// validate addresses
		if (verticesAddress > edgesAddress || edgesAddress > facesAddress || verticesAddress > facesAddress)
		{
			throw "ERROR::MESH::FILE_NOT_VALID::Vertices must come before edges and edges must come before faces.";
		}

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

		// calculate normals
		mesh->normals = new float[mesh->vertexCount];
		for (unsigned int edge = 0; edge + 1 < mesh->edgeRefCount; edge += 2)
		{
			int vertex1 = mesh->edges[edge];
			int vertex2 = mesh->edges[edge + 1];
			// find 
		}
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

Object::Object(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, Material* material)
{
	position = Position;
	rotation = Rotation;
	scale = Scale;
	this->material = material;
	children = new ObjectList();
	mesh = NULL;
	VAO = 0;
}
Object::Object(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, Material* material, const char* meshPath)
{
	position = Position;
	rotation = Rotation;
	scale = Scale;
	this->material = material;
	children = new ObjectList();
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
void Object::render(maths::mat4f worldToScreenMatrix, Colour* ambientLight)
{
	material->use(worldToScreenMatrix * local_to_world(), ambientLight);
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

Empty::Empty(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, Material* material) : Object(Position, Rotation, Scale, material)
{

}
Empty::Empty(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, Material* material, const char* meshPath) : Object(Position, Rotation, Scale, material, meshPath)
{

}

/*
Axes
*/

Axes::Axes(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, Material* material) : Empty(Position, Rotation, Scale, material, "models/axes.mdl")
{

}

/*
Camera
*/

Camera::Camera(maths::vec3f Position, maths::unit_quaternion Rotation, Material* material, float FOV, float NearClip, float FarClip, float AspectRatio) : Empty(Position, Rotation, maths::vec3f(1.0f, 1.0f, 1.0f), material, "models/camera.mdl")
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

Model::Model(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, Material* material, const char* meshPath) : Object(Position, Rotation, Scale, material, meshPath)
{

}

/*
Cube
*/

Cube::Cube(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, Material* material) : Model(Position, Rotation, Scale, material, "models/cube.mdl")
{

}

/*
Plane
*/

Plane::Plane(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, Material* material) : Model(Position, Rotation, Scale, material, "models/plane.mdl")
{

}

/*
ObjectList
*/

ObjectList::ObjectList()
{
	count = 0;
	capacity = 4;
	data = (Object**)malloc(capacity * sizeof(Object*));
	if (data == NULL)
	{
		throw "ERROR::SCENE::OBJECTLIST::NOT_ENOUGH_MEMORY";
	}
}

Object* ObjectList::operator[](int index)
{
	// return a pointer to the object at this index
	return data[index];
}

void ObjectList::add(Object* object)
{
	if (count + 1 > capacity)
	{
		// reallocate memory and copy everything
		capacity *= 2;
		Object** newData = (Object**)malloc(capacity * sizeof(Object*));
		if (newData == NULL)
		{
			throw "ERROR::SCENE::OBJECTLIST::NOT_ENOUGH_MEMORY";
		}
		for (int index = 0; index < count; index++)
		{
			newData[index] = data[index];
		}
		free(data);
		data = newData;
	}
	// add the new item
	data[count] = object;
	count++;
}

void ObjectList::insert(Object* object, int index)
{
	if (count + 1 > capacity)
	{
		// reallocate memory and copy everything, inserting the new one where required
		capacity *= 2;
		Object** newData = (Object**)malloc(capacity * sizeof(Object*));
		if (newData == NULL)
		{
			throw "ERROR::SCENE::OBJECTLIST::NOT_ENOUGH_MEMORY";
		}
		int insertedNewOne = 0;
		for (int copyIndex = 0; copyIndex < count; copyIndex++)
		{
			if (copyIndex == index)
			{
				// add the new one
				newData[copyIndex] = object;
				insertedNewOne = 1;
			}
			newData[copyIndex + insertedNewOne] = data[copyIndex];
		}
		free(data);
		data = newData;
	}
	else
	{
		// move the old ones over and insert the new one
		for (int copyIndex = index; copyIndex < count; copyIndex++)
		{
			data[copyIndex + 1] = data[copyIndex];
		}
		data[index] = object;
	}
	count++;
}

void ObjectList::remove_at(int index)
{
	// copy all of the ones after the target down
	for (int copyIndex = index; copyIndex < count - 1; copyIndex++)
	{
		data[copyIndex] = data[copyIndex + 1];
	}
	count--;
}

void ObjectList::remove(Object* object)
{
	// find the object to delete
	int index = 0;
	bool found = false;
	while (!found)
	{
		if (data[index] == object)
		{
			found = true;
		}
		else
		{
			index++;
		}
	}
	// copy all the next ones to the one before
	for (int copyIndex = index; copyIndex < count - 1; copyIndex++)
	{
		data[copyIndex] = data[copyIndex + 1];
	}
	count--;
}