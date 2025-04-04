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

void Scene::render_branch(Object* branch, maths::mat4f projectionMatrix, maths::mat4f viewMatrix, maths::mat4f parentToWorld)
{
	for (int childIndex = 0; childIndex < branch->children->count; childIndex++)
	{
		render_branch((branch->children)->operator[](childIndex), projectionMatrix, viewMatrix, parentToWorld * branch->local_to_world());
	}
	branch->render(projectionMatrix, viewMatrix * parentToWorld, 
		&ambientLight, activeLight->position, new Colour(1.0f, 1.0f, 1.0f));
}

Scene::Scene(Camera* activeCam, Light* ActiveLight, float ambientRed, float ambientGreen, float ambientBlue)
{
	activeCamera = activeCam;
	children = new ObjectList();
	children->add(ActiveLight);
	ambientLight = Colour(ambientRed, ambientGreen, ambientBlue);
	activeLight = ActiveLight;
}

void Scene::render()
{
	// calculate global matrix
	maths::mat4f projection = activeCamera->orthographic_matrix() * activeCamera->perspective_matrix();
	maths::mat4f view = maths::mat4f::stretch_z(-1.0f) * activeCamera->cameraspace_matrix();
	// navigate the tree, rendering each object
	for (int child = 0; child < children->count; child++)
	{
		render_branch(children->operator[](child), projection, view, maths::mat4f(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));
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
		mesh->shadingMode = flags[2];
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
		mesh->vertexCount = (edgesAddress - verticesAddress) / sizeof(float) / 3;
		mesh->edgeCount = (facesAddress - edgesAddress) / sizeof(int) / 2;
		mesh->faceCount = (fileSize - facesAddress) / (unsigned int)sizeof(int) / 3;

		// allocate memory for vertices, edges, faces
		Vertex* vertexData;
		int uniqueVertexCount = mesh->vertexCount;
		if (mesh->shadingMode == Mesh::MESH_SHADING_SMOOTH)
		{
			mesh->vertices = new Vertex[mesh->vertexCount];
			vertexData = mesh->vertices;
		}
		else
		{
			vertexData = new Vertex[mesh->vertexCount];
			mesh->vertexCount = mesh->faceCount * 3;
			mesh->vertices = new Vertex[mesh->vertexCount];
		}
		mesh->edges = new Edge[mesh->edgeCount];
		mesh->faces = new Face[mesh->faceCount];

		// fill the arrays with the info
		meshFile.seekg(verticesAddress, std::ios::beg);
		for (int vertex = 0; vertex < uniqueVertexCount; vertex++)
		{
			meshFile.read(reinterpret_cast<char*>(vertexData + vertex), sizeof(float) * 3);
		}
		meshFile.seekg(edgesAddress, std::ios::beg);
		meshFile.read(reinterpret_cast<char*>(mesh->edges), mesh->edgeCount * sizeof(int) * 2);
		meshFile.seekg(facesAddress, std::ios::beg);
		meshFile.read(reinterpret_cast<char*>(mesh->faces), mesh->faceCount * sizeof(int) * 3);

		// close file handler
		meshFile.close();

		// calculate normals
		for (unsigned int vertex = 0; vertex < mesh->vertexCount; vertex++)
		{
			mesh->vertices[vertex].normal = maths::vec3f(0, 0, 0);
		}
		for (unsigned int face = 0; face < mesh->faceCount; face++)
		{
			// get the 3 vertices of each face
			Vertex* vertex1 = &vertexData[mesh->faces[face].vertex1];
			Vertex* vertex2 = &vertexData[mesh->faces[face].vertex2];
			Vertex* vertex3 = &vertexData[mesh->faces[face].vertex3];

			// find cross product of two of the displacement vectors (find the normal of the face)
			maths::vec3f cross = maths::vec3f::cross(vertex3->position - vertex1->position, vertex2->position - vertex1->position);

			if (mesh->shadingMode == Mesh::MESH_SHADING_SMOOTH)
			{
				// if using smooth shading, add cross product to each vertex's normal
				vertex1->normal = vertex1->normal + cross;
				vertex2->normal = vertex2->normal + cross;
				vertex3->normal = vertex3->normal + cross;
			}
			else
			{
				// if using flat shading, copy vertex data into actual mesh, with cross product as the normal
				Vertex realVertex1 = { vertex1->position, cross };
				Vertex realVertex2 = { vertex2->position, cross };
				Vertex realVertex3 = { vertex3->position, cross };
				mesh->vertices[face * 3] = realVertex1;
				mesh->vertices[face * 3 + 1] = realVertex2;
				mesh->vertices[face * 3 + 2] = realVertex3;
				mesh->faces[face].vertex1 = face * 3;
				mesh->faces[face].vertex2 = face * 3 + 1;
				mesh->faces[face].vertex3 = face * 3 + 2;
			}
		}
		// normalise the normal vectors
		for (unsigned int vertex = 0; vertex < mesh->vertexCount; vertex++)
		{
			mesh->vertices[vertex].normal = maths::vec3f::normalise(mesh->vertices[vertex].normal);
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
	glBufferData(GL_ARRAY_BUFFER, mesh->vertexCount * sizeof(float) * 6, mesh->vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	if (mesh->faceCount != 0)
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->faceCount * sizeof(int) * 3, mesh->faces, GL_STATIC_DRAW);
	}
	else
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->edgeCount * sizeof(int) * 2, mesh->edges, GL_STATIC_DRAW);
	}
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
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
void Object::render(maths::mat4f projectionMatrix, maths::mat4f viewMatrix, 
	Colour* ambientLight, maths::vec3f lightPosition, Colour* lightColour)
{
	material->use(projectionMatrix, viewMatrix, local_to_world(), 
		ambientLight, lightPosition, lightColour);
	glBindVertexArray(VAO);
	if (mesh->faceCount != 0)
	{
		glDrawElements(GL_TRIANGLES, mesh->faceCount * 3, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawElements(GL_LINES, mesh->edgeCount * 2, GL_UNSIGNED_INT, 0);
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
Light
*/

Light::Light(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, Material* material) : Empty(Position, Rotation, Scale, material)
{

}
Light::Light(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, Material* material, const char* meshPath) : Empty(Position, Rotation, Scale, material, meshPath)
{

}

/*
PointLight
*/

PointLight::PointLight(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, Material* material) : Light(Position, Rotation, Scale, material, "models/pointlight.mdl")
{

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