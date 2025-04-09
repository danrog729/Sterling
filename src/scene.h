#ifndef STERLING_SCENE_H
#define STERLING_SCENE_H

#include <vector>
#include "mesh.h"

class PathDictionary
{
private:
	struct PathDictionaryEntry
	{
		char* path;
		std::vector<int> indices;
		PathDictionaryEntry* leftChild;
		PathDictionaryEntry* rightChild;
	};
	PathDictionaryEntry* root;

public:
	unsigned int count;

	/// <summary>
	/// Initialise the path dictionary
	/// </summary>
	PathDictionary();
	/// <summary>
	/// Return the mesh indices that correspond to a given .obj file path, NULL if entry does not exist
	/// </summary>
	/// <param name="path">The path to find the indices for</param>
	/// <returns>Pointer to vector that contains the mesh indices, NULL if entry does not exist</returns>
	std::vector<int>* get_entry(const char* path);
};

/// <summary>
/// Represents a 3D environment, with objects, lights, cameras etc.
/// </summary>
class Scene
{
private:
	/// <summary>
	/// List of all of the meshes in the scene.
	/// </summary>
	std::vector<Mesh> meshes;
	/// <summary>
	/// List of all of the materials in the scene.
	/// </summary>
	// std::vector<Material> materials;
	/// <summary>
	/// Binary search tree containing the paths to all of the .obj files already loaded, and indexes to the meshes they define
	/// </summary>
	PathDictionary meshDictionary;
	/// <summary>
	/// Binary search tree containing the paths to all of the .mtl files already loaded, and indexes to the materials they define
	/// </summary>
	PathDictionary materialDictionary;
	/// <summary>
	/// The camera that scenes should be rendered from the perspective of
	/// </summary>
	// Camera activeCamera;
	/// <summary>
	/// List of all of the lights in the scene
	/// </summary>
	// std::vector<Light> lights;
	/// <summary>
	/// List of all of the highest objects in the scene hierarchy
	/// </summary>
	// std::vector<Object> children;
public:
	/// <summary>
	/// Initialise the mesh/material lists/dictionaries, the lights list and the object tree
	/// </summary>
	Scene();
	/// <summary>
	/// Ask the scene to load a .obj file. Returns a list of indices for the meshes in the mesh list.
	/// </summary>
	/// <param name="filePath">The relative path to the .obj file</param>
	/// <returns>Vector containing indices into the mesh list.</returns>
	std::vector<int> load_mesh(const char* filePath);
};

#endif