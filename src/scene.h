#ifndef STERLING_SCENE_H
#define STERLING_SCENE_H

#include <vector>
#include "mesh.h"
#include "material.h"
#include "object.h"

class Object;
class Camera;

#include "fast_obj/fast_obj.h"

class PathDictionary
{
private:
	struct PathDictionaryEntry
	{
		const char* path;
		int index;
		PathDictionaryEntry* leftChild;
		PathDictionaryEntry* rightChild;
	};
	PathDictionaryEntry* root;
	PathDictionaryEntry* traversalResult;
	PathDictionaryEntry* traversalResultParent;
	bool addLeft;

	/// <summary>
	/// Traverse the tree to find a specific target. If the target exists: traversalResult is that node, if not, traversalResult is where it would need to be added
	/// </summary>
	/// <param name="target">The target node</param>
	void traverse(const char* target);

public:
	unsigned int count;

	/// <summary>
	/// Initialise the path dictionary
	/// </summary>
	PathDictionary();
	/// <summary>
	/// Return the mesh index that corresponds to a given file path, -1 if entry does not exist
	/// </summary>
	/// <param name="path">The path to find the indices for</param>
	/// <returns>The mesh index, -1 if entry does not exist</returns>
	int get_entry(const char* path);
	/// <summary>
	/// Add an entry into the dictionary
	/// </summary>
	/// <param name="path">The file path of the new entry</param>
	/// <param name="index">The index into the associated list</param>
	void add_entry(const char* path, int index);
};

/// <summary>
/// Represents a 3D environment, with objects, lights, cameras etc.
/// </summary>
class Scene
{
public:
	/// <summary>
	/// List of all of the meshes in the scene.
	/// </summary>
	std::vector<Mesh*> meshes;
	/// <summary>
	/// List of all of the materials in the scene.
	/// </summary>
	std::vector<Material*> materials;
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
	Camera* activeCamera;
	/// <summary>
	/// List of all of the lights in the scene
	/// </summary>
	// std::vector<Light> lights;
	/// <summary>
	/// List of all of the highest objects in the scene hierarchy
	/// </summary>
	std::vector<Object*> children;

	/// <summary>
	/// Load a mesh from a .mesh file and add it to the mesh list
	/// </summary>
	/// <param name="filepath">The path to the file to open</param>
	void load_model_from_file(const char* filepath);
	/// <summary>
	/// Load a mesh from a .obj file and add it to the mesh list
	/// </summary>
	/// <param name="filepath">The path to the file to open</param>
	/// <returns>The index into the mesh list</returns>
	int load_model_from_obj(const char* filepath);

	/// <summary>
	/// Initialise the mesh/material lists/dictionaries, the lights list and the object tree
	/// </summary>
	Scene();
	/// <summary>
	/// Ask the scene to load a .mesh file. Returns an index for the mesh in the mesh list.
	/// </summary>
	/// <param name="filePath">The relative path to the .mesh file</param>
	/// <returns>An index into the mesh list</returns>
	int load_mesh(const char* filePath);
	/// <summary>
	/// Add an object to the scene's children
	/// </summary>
	/// <param name="object">The object to add</param>
	void add_object(Object* object);
	/// <summary>
	/// Render the scene
	/// </summary>
	void render();
};

#endif