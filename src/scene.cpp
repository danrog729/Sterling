#include "scene.h"

/*
Path Dictionary
*/

PathDictionary::PathDictionary()
{
	count = 0;
	root = NULL;
}

std::vector<int>* PathDictionary::get_entry(const char* path)
{
	// traverse the tree to find the given path, and return the indices if found, NULL if not
	PathDictionaryEntry* parent = root;
	PathDictionaryEntry* current = root;
	bool shouldContinue = true;
	std::vector<int>* result = NULL;
	while (current != NULL && shouldContinue)
	{
		// traverse the BST
		int difference = strcmp(current->path, path);
		if (difference == 0)
		{
			// strings are equal, return this
			shouldContinue = false;
			result = &current->indices;
		}
		else if (difference > 0)
		{
			// current is greater than path we're looking for, traverse left
			parent = current;
			current = current->leftChild;
		}
		else
		{
			// current is less than the path we're looking for, traverse right
			parent = current;
			current = current->rightChild;
		}
	}
	return result;
}

/*
Scene
*/

Scene::Scene()
{
	meshDictionary = PathDictionary();
	materialDictionary = PathDictionary();
}

std::vector<int> Scene::load_mesh(const char* filePath)
{
	// check if the mesh is already in the dictionary
	std::vector<int>* indices = meshDictionary.get_entry(filePath);
	std::vector<int> result;
	if (indices != NULL)
	{
		// mesh already exists
		result = *indices;
	}
	else
	{
		// load mesh
		// add to mesh list
		// add to dictionary
	}
	return result;
}