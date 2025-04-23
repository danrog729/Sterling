#ifndef STERLING_PRIMITIVES_H
#define STERLING_PRIMITIVES_H

#include "object.h"
#include "scene.h"

namespace primitives
{
	/// <summary>
	/// Create a cube object
	/// </summary>
	/// <param name="scene">The scene to add the mesh to</param>
	/// <returns>A pointer to the created object</returns>
	Object* cube(Scene* scene, const char* name);
	/// <summary>
	/// Create a plane object
	/// </summary>
	/// <param name="scene">The scene to add the mesh to</param>
	/// <returns>A pointer to the created object</returns>
	Object* plane(Scene* scene);
}

#endif