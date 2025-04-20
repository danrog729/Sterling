#include "scene.h"

#include <fstream>
#include <iostream>

#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj/fast_obj.h"

/*
Path Dictionary
*/

PathDictionary::PathDictionary()
{
	count = 0;
	root = NULL;
	traversalResult = NULL;
	traversalResultParent = NULL;
	addLeft = false;
}

void PathDictionary::traverse(const char* target)
{
	// traverse the tree to find the given path, and return the indices if found, NULL if not
	traversalResultParent = root;
	traversalResult = root;
	bool shouldContinue = true;
	while (traversalResult != NULL && shouldContinue)
	{
		// traverse the BST
		int difference = strcmp(traversalResult->path, target);
		if (difference == 0)
		{
			// strings are equal, return this
			shouldContinue = false;
		}
		else if (difference > 0)
		{
			// current is greater than path we're looking for, traverse left
			traversalResultParent = traversalResult;
			traversalResult = traversalResult->leftChild;
			addLeft = true;
		}
		else
		{
			// current is less than the path we're looking for, traverse right
			traversalResultParent = traversalResult;
			traversalResult = traversalResult->rightChild;
			addLeft = false;
		}
	}
}

int PathDictionary::get_entry(const char* path)
{
	// traverse the tree to find the given path, and return the index if found, -1 if not
	traverse(path);
	if (traversalResult == NULL)
	{
		return -1;
	}
	else
	{
		return traversalResult->index;
	}
}

void PathDictionary::add_entry(const char* path, int index)
{
	if (root == NULL)
	{
		root = new PathDictionaryEntry();
		root->path = path;
		root->index = index;
		root->leftChild = NULL;
		root->rightChild = NULL;
	}
	else
	{
		traverse(path);
		traversalResult = new PathDictionaryEntry();
		traversalResult->path = path;
		traversalResult->index = index;
		traversalResult->leftChild = NULL;
		traversalResult->rightChild = NULL;
		if (addLeft)
		{
			traversalResultParent->leftChild = traversalResult;
		}
		else
		{
			traversalResultParent->rightChild = traversalResult;
		}
	}
}

/*
Scene
*/

void Scene::update_ambient_lights()
{
	backgroundColour = maths::vec3f(0.0f, 0.0f, 0.0f);
	bool needsUpdating = false;
	for (int lightIndex = 0; lightIndex < ambientLights.size(); lightIndex++)
	{
		backgroundColour = backgroundColour + ambientLights[lightIndex]->colour();
		needsUpdating |= ambientLights[lightIndex]->isDirty();
		ambientLights[lightIndex]->clean();
	}
	backgroundColour = backgroundColour / ambientLights.size();
	if (needsUpdating)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 12, &backgroundColour);
	}
}

void Scene::update_point_lights(maths::mat4f viewMatrix, bool updateLightPositions)
{
	bool needsUpdating = false;
	for (int lightIndex = 0; lightIndex < pointLights.size() && lightIndex < 16; lightIndex++)
	{
		needsUpdating |= pointLights[lightIndex]->isDirty();
		pointLights[lightIndex]->add_to_uniform_buffer(16 + lightIndex * 48, viewMatrix, updateLightPositions);
	}
	if (needsUpdating)
	{
		unsigned int minimum = pointLights.size();
		if (minimum > 16)
		{
			minimum = 16;
		}
		glBufferSubData(GL_UNIFORM_BUFFER, 2320, 4, &minimum);
	}
}

void Scene::update_spotlights(maths::mat4f viewMatrix, bool updateLightPositions)
{
	bool needsUpdating = false;
	for (int lightIndex = 0; lightIndex < spotlights.size() && lightIndex < 16; lightIndex++)
	{
		needsUpdating |= spotlights[lightIndex]->isDirty();
		spotlights[lightIndex]->add_to_uniform_buffer(784 + lightIndex * 64, viewMatrix, updateLightPositions);
	}
	if (needsUpdating)
	{
		unsigned int minimum = spotlights.size();
		if (minimum > 16)
		{
			minimum = 16;
		}
		glBufferSubData(GL_UNIFORM_BUFFER, 2324, 4, &minimum);
	}
}

void Scene::update_directional_lights(maths::mat4f viewMatrix, bool updateLightPositions)
{
	bool needsUpdating = false;
	for (int lightIndex = 0; lightIndex < directionalLights.size() && lightIndex < 16; lightIndex++)
	{
		needsUpdating |= directionalLights[lightIndex]->isDirty();
		directionalLights[lightIndex]->add_to_uniform_buffer(1808 + lightIndex * 64, viewMatrix, updateLightPositions);
	}
	if (needsUpdating)
	{
		unsigned int minimum = directionalLights.size();
		if (minimum > 16)
		{
			minimum = 16;
		}
		glBufferSubData(GL_UNIFORM_BUFFER, 2328, 4, &minimum);
	}
}

Scene::Scene()
{
	meshDictionary = PathDictionary();
	materialDictionary = PathDictionary();
	activeCamera = NULL;

	glGenBuffers(1, &lightBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, lightBuffer);
	glBufferData(GL_UNIFORM_BUFFER, 2336, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightBuffer);
}

void Scene::load_model_from_file(const char* filepath)
{
	// create buffers to hold vertex position, normal and texture coordinate data
	std::vector<maths::vec3f> vertexPositions;
	std::vector<maths::vec3f> vertexNormals;
	std::vector<maths::vec2f> texCoords;

	// create the new objMesh struct
	Mesh* mesh = new Mesh();
	// retrieve the objMesh data from filePath
	std::ifstream meshFile;
	// ensure ifstream objects can throw exceptions
	meshFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open file
		meshFile.open(filepath, std::ios::binary);

		// get length
		meshFile.seekg(0, std::ios::end);
		std::streamsize fileSize = meshFile.tellg();
		meshFile.seekg(0, std::ios::beg);

		// read format
		char* format = new char[4];
		meshFile.read(format, 4);

		// read material file name length
		unsigned int materialFileNameLength;
		meshFile.read((char*)&materialFileNameLength, 4);

		// read material file name
		char* materialFileName = (char*)malloc(materialFileNameLength + 1);
		if (materialFileName == NULL)
		{
			std::cerr << "ERROR::MESH::OUT_OF_MEMORY\n";
			return;
		}
		materialFileName[materialFileNameLength] = '\0';
		meshFile.read(materialFileName, materialFileNameLength);
		free(materialFileName);

		// read primitive count
		unsigned int primitiveCount;
		meshFile.read((char*)&primitiveCount, 4);

		for (unsigned int primitiveIndex = 0; primitiveIndex < primitiveCount; primitiveIndex++)
		{
			MeshPrimitive* primitive = new MeshPrimitive();
			mesh->primitives.push_back(primitive);

			// read material index
			unsigned int materialIndex;
			meshFile.read((char*)&materialIndex, 4);
			primitive->materialIndex = materialIndex;

			// read rendering mode
			char renderingMode;
			meshFile.read((char*)&renderingMode, 1);

			// read vertex count
			unsigned int vertexCount;
			meshFile.read((char*)&vertexCount, 4);

			// read vertex normal count
			unsigned int vertexNormalCount;
			meshFile.read((char*)&vertexNormalCount, 4);

			// read texture coordinate count
			unsigned int texCoordCount;
			meshFile.read((char*)&texCoordCount, 4);

			// read texture coordinate count
			unsigned int edgeFaceCount;
			meshFile.read((char*)&edgeFaceCount, 4);

			// read vertex data
			for (unsigned int vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
			{
				maths::vec3f position;
				meshFile.read((char*)&position, 12);
				vertexPositions.push_back(position);
			}

			// read vertex normal data
			for (unsigned int vertexNormalIndex = 0; vertexNormalIndex < vertexNormalCount; vertexNormalIndex++)
			{
				maths::vec3f normal;
				meshFile.read((char*)&normal, 12);
				vertexNormals.push_back(normal);
			}

			// read texture coordinate data
			for (unsigned int texCoordIndex = 0; texCoordIndex < texCoordCount; texCoordIndex++)
			{
				maths::vec2f texCoord;
				meshFile.read((char*)&texCoord, 8);
				texCoords.push_back(texCoord);
			}
			
			// read edge/face data
			if (renderingMode == 0x00) // points
			{
				for (unsigned int vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
				{
					primitive->vertices.push_back(Vertex(vertexPositions[vertexIndex], maths::vec3f(0, 0, 0), maths::vec2f(0, 0)));
				}
			}
			else if (renderingMode == 0x01) // edge
			{
				for (unsigned int edgeIndex = 0; edgeIndex < edgeFaceCount; edgeIndex++)
				{
					// read edge index 1
					unsigned int edgeIndex1;
					meshFile.read((char*)&edgeIndex1, 4);
					unsigned int realEdgeIndex1 = primitive->vertices.size();
					primitive->vertices.push_back(Vertex(vertexPositions[edgeIndex1], maths::vec3f(0, 0, 0), maths::vec2f(0, 0)));

					// read edge index 2
					unsigned int edgeIndex2;
					meshFile.read((char*)&edgeIndex2, 4);
					unsigned int realEdgeIndex2 = primitive->vertices.size();
					primitive->vertices.push_back(Vertex(vertexPositions[edgeIndex2], maths::vec3f(0, 0, 0), maths::vec2f(0, 0)));

					// add the edges to the edge list
					primitive->edges.push_back(Edge(realEdgeIndex1, realEdgeIndex2));
				}
			}
			else if (renderingMode == 0x02) // face
			{
				for (unsigned int faceIndex = 0; faceIndex < edgeFaceCount; faceIndex++)
				{
					// read face index 1
					unsigned int positionIndex1;
					unsigned int normalIndex1;
					unsigned int texCoordIndex1;
					meshFile.read((char*)&positionIndex1, 4);
					meshFile.read((char*)&normalIndex1, 4);
					meshFile.read((char*)&texCoordIndex1, 4);
					unsigned int faceIndex1 = primitive->vertices.size();
					primitive->vertices.push_back(Vertex(vertexPositions[positionIndex1], vertexNormals[normalIndex1], texCoords[texCoordIndex1]));

					// read face index 2
					unsigned int positionIndex2;
					unsigned int normalIndex2;
					unsigned int texCoordIndex2;
					meshFile.read((char*)&positionIndex2, 4);
					meshFile.read((char*)&normalIndex2, 4);
					meshFile.read((char*)&texCoordIndex2, 4);
					unsigned int faceIndex2 = primitive->vertices.size();
					primitive->vertices.push_back(Vertex(vertexPositions[positionIndex2], vertexNormals[normalIndex2], texCoords[texCoordIndex2]));

					// read face index 3
					unsigned int positionIndex3;
					unsigned int normalIndex3;
					unsigned int texCoordIndex3;
					meshFile.read((char*)&positionIndex3, 4);
					meshFile.read((char*)&normalIndex3, 4);
					meshFile.read((char*)&texCoordIndex3, 4);
					unsigned int faceIndex3 = primitive->vertices.size();
					primitive->vertices.push_back(Vertex(vertexPositions[positionIndex3], vertexNormals[normalIndex3], texCoords[texCoordIndex3]));

					// add the face to the face list
					primitive->faces.push_back(Face(faceIndex1, faceIndex2, faceIndex3));
				}
			}
			primitive->setup();
		}
		meshes.push_back(mesh);
	}
	catch (std::ifstream::failure e)
	{
		std::cerr << "ERROR::MESH::CANNOT_READ_FILE\n" << e.what() << std::endl;
	}
}

int Scene::load_model_from_obj(const char* filepath)
{
	fastObjMesh* objMesh = fast_obj_read(filepath);

	// copy into my own mesh objects

	// create a mesh
	Mesh* mesh = new Mesh();
	meshes.push_back(mesh);

	// create a objMesh primitive for each material
	for (int primitiveIndex = 0; primitiveIndex < objMesh->material_count; primitiveIndex++)
	{
		MeshPrimitive* primitive = new MeshPrimitive();
		mesh->primitives.push_back(primitive);

		// setup the material
		fastObjMaterial* objMaterial = &objMesh->materials[primitiveIndex];
		int materialIndex = materialDictionary.get_entry(objMaterial->name);
		if (materialIndex == -1)
		{
			// material does not yet exist
			Material* material = new Material("shaders/shaded.vert", "shaders/shaded.frag");
			materialIndex = materials.size();
			materialDictionary.add_entry(objMaterial->name, materialIndex);
			materials.push_back(material);
			
			// set properties
			material->ambientColour = maths::vec3f(objMaterial->Ka[0], objMaterial->Ka[1], objMaterial->Ka[2]);
			material->diffuseColour = maths::vec3f(objMaterial->Kd[0], objMaterial->Kd[1], objMaterial->Kd[2]);
			material->specularColour = maths::vec3f(objMaterial->Ks[0], objMaterial->Ks[1], objMaterial->Ks[2]);
			material->emissionColour = maths::vec3f(objMaterial->Ke[0], objMaterial->Ke[1], objMaterial->Ke[2]);
			material->transmittanceColour = maths::vec3f(objMaterial->Kt[0], objMaterial->Kt[1], objMaterial->Kt[2]);
			material->shininess = objMaterial->Ns;
			material->indexOfRefraction = objMaterial->Ni;
			material->transmissionFilter = maths::vec3f(objMaterial->Tf[0], objMaterial->Tf[1], objMaterial->Tf[2]);
			material->dissolve = objMaterial->d;

			// set textures
			if (objMaterial->map_Ka != 0)
			{
				material->ambientMap = new Texture2D(objMesh->textures[objMaterial->map_Ka].name);
			}
			if (objMaterial->map_Kd != 0)
			{
				material->diffuseMap = new Texture2D(objMesh->textures[objMaterial->map_Kd].name);
			}
			if (objMaterial->map_Ks != 0)
			{
				material->specularMap = new Texture2D(objMesh->textures[objMaterial->map_Ks].name);
			}
			if (objMaterial->map_Ke != 0)
			{
				material->emissionMap = new Texture2D(objMesh->textures[objMaterial->map_Ke].name);
			}
			if (objMaterial->map_Kt != 0)
			{
				material->transmittanceMap = new Texture2D(objMesh->textures[objMaterial->map_Kt].name);
			}
			if (objMaterial->map_Ns != 0)
			{
				material->shininessMap = new Texture2D(objMesh->textures[objMaterial->map_Ns].name);
			}
			if (objMaterial->map_Ni != 0)
			{
				material->indexOfRefractionMap = new Texture2D(objMesh->textures[objMaterial->map_Ni].name);
			}
			if (objMaterial->map_d != 0)
			{
				material->dissolveMap = new Texture2D(objMesh->textures[objMaterial->map_d].name);
			}
			if (objMaterial->map_bump != 0)
			{
				material->bumpMap = new Texture2D(objMesh->textures[objMaterial->map_bump].name);
			}
		}
		primitive->materialIndex = materialIndex;
	}

	// loop through all of the faces
	unsigned int indexIndex = 0;
	for (int faceIndex = 0; faceIndex < objMesh->face_count; faceIndex++)
	{
		unsigned int primitiveIndex = objMesh->face_materials[faceIndex];
		unsigned int startingPosition = mesh->primitives[primitiveIndex]->vertices.size();

		// add each vertex to the associated primitive
		for (int vertexIndex = 0; vertexIndex < objMesh->face_vertices[faceIndex]; vertexIndex++)
		{
			unsigned int positionIndex = objMesh->indices[indexIndex].p;
			unsigned int normalIndex = objMesh->indices[indexIndex].n;
			unsigned int texCoordIndex = objMesh->indices[indexIndex].t;
			indexIndex++;

			Vertex vertex = Vertex(
				maths::vec3f(
					objMesh->positions[positionIndex * 3],
					objMesh->positions[positionIndex * 3 + 1],
					objMesh->positions[positionIndex * 3 + 2]
				),
				maths::vec3f(
					objMesh->normals[normalIndex * 3],
					objMesh->normals[normalIndex * 3 + 1],
					objMesh->normals[normalIndex * 3 + 2]
				),
				maths::vec2f(
					objMesh->texcoords[texCoordIndex * 2],
					objMesh->texcoords[texCoordIndex * 2 + 1]
				)
			);
			mesh->primitives[primitiveIndex]->vertices.push_back(vertex);
		}

		// triangulate the face and add it to the associated primitive
		for (int triangle = 2; triangle < objMesh->face_vertices[faceIndex]; triangle++)
		{
			// triangulate between the first vertex, the vertex before the current, and the current
			mesh->primitives[primitiveIndex]->faces.push_back(
				Face(
					startingPosition,
					startingPosition + triangle - 1,
					startingPosition + triangle
				)
			);
		}
	}

	// setup the primitives
	for (int primitiveIndex = 0; primitiveIndex < objMesh->material_count; primitiveIndex++)
	{
		mesh->primitives[primitiveIndex]->setup();
	}

	fast_obj_destroy(objMesh);

	return meshes.size() - 1;
}

int Scene::load_mesh(const char* filePath)
{
	// check if the mesh is already in the dictionary
	int index = meshDictionary.get_entry(filePath);
	if (index == -1)
	{
		// load mesh
		index = load_model_from_obj(filePath);
		// add to dictionary
		meshDictionary.add_entry(filePath, index);
	}
	// else mesh already exists
	return index;
}

void Scene::add_object(Object* object)
{
	children.push_back(object);
	object->scene = this;
	object->parent = NULL;
}

void Scene::render()
{
	if (activeCamera != NULL)
	{
		// update the projection and view matrix buffers if they need to be updated
		activeCamera->projection_matrix();
		maths::mat4f viewMatrix = activeCamera->view_matrix();
		bool updateLightPositions = activeCamera->transformation.changedOnLastAccess();
		// update the light buffers if they need to be updated
		glBindBuffer(GL_UNIFORM_BUFFER, lightBuffer);
		update_ambient_lights();
		update_point_lights(viewMatrix, updateLightPositions);
		update_spotlights(viewMatrix, updateLightPositions);
		update_directional_lights(viewMatrix, updateLightPositions);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		// render background
		glClearColor(backgroundColour.x, backgroundColour.y, backgroundColour.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// render the objects in the scene
		for (int childIndex = 0; childIndex < children.size(); childIndex++)
		{
			children[childIndex]->render();
		}
	}
}