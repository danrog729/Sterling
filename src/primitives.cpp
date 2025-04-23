#include "primitives.h"
#include "mesh.h"
#include "material.h"

namespace primitives
{
	Object* cube(Scene* scene, const char* name)
	{
		MeshPrimitive* primitive = new MeshPrimitive();

		primitive->vertices.push_back(Vertex(maths::vec3f(-1, 1, -1), maths::vec3f(0, 1, 0), maths::vec2f(0.375, 0)));
		primitive->vertices.push_back(Vertex(maths::vec3f(-1, 1, 1), maths::vec3f(0, 1, 0), maths::vec2f(0.625, 0)));
		primitive->vertices.push_back(Vertex(maths::vec3f(1, 1, 1), maths::vec3f(0, 1, 0), maths::vec2f(0.625, 0.25)));
		primitive->vertices.push_back(Vertex(maths::vec3f(1, 1, -1), maths::vec3f(0, 1, 0), maths::vec2f(0.375, 0.25)));
		primitive->faces.push_back(Face(0, 1, 2));
		primitive->faces.push_back(Face(0, 2, 3));
		primitive->vertices.push_back(Vertex(maths::vec3f(1, 1, -1), maths::vec3f(1, 0, 0), maths::vec2f(0.375, 0.25)));
		primitive->vertices.push_back(Vertex(maths::vec3f(1, 1, 1), maths::vec3f(1, 0, 0), maths::vec2f(0.625, 0.25)));
		primitive->vertices.push_back(Vertex(maths::vec3f(1, -1, 1), maths::vec3f(1, 0, 0), maths::vec2f(0.625, 0.25)));
		primitive->vertices.push_back(Vertex(maths::vec3f(1, -1, -1), maths::vec3f(1, 0, 0), maths::vec2f(0.375, 0.5)));
		primitive->faces.push_back(Face(4, 5, 6));
		primitive->faces.push_back(Face(4, 6, 7));
		primitive->vertices.push_back(Vertex(maths::vec3f(1, -1, -1), maths::vec3f(0, -1, 0), maths::vec2f(0.375, 0.5)));
		primitive->vertices.push_back(Vertex(maths::vec3f(1, -1, 1), maths::vec3f(0, -1, 0), maths::vec2f(0.625, 0.5)));
		primitive->vertices.push_back(Vertex(maths::vec3f(-1, -1, 1), maths::vec3f(0, -1, 0), maths::vec2f(0.625, 0.75)));
		primitive->vertices.push_back(Vertex(maths::vec3f(-1, -1, -1), maths::vec3f(0, -1, 0), maths::vec2f(0.375, 0.75)));
		primitive->faces.push_back(Face(8, 9, 10));
		primitive->faces.push_back(Face(8, 10, 11));
		primitive->vertices.push_back(Vertex(maths::vec3f(-1, -1, -1), maths::vec3f(-1, 0, 0), maths::vec2f(0.375, 0.75)));
		primitive->vertices.push_back(Vertex(maths::vec3f(-1, -1, 1), maths::vec3f(-1, 0, 0), maths::vec2f(0.625, 0.75)));
		primitive->vertices.push_back(Vertex(maths::vec3f(-1, 1, 1), maths::vec3f(-1, 0, 0), maths::vec2f(0.625, 1)));
		primitive->vertices.push_back(Vertex(maths::vec3f(-1, 1, -1), maths::vec3f(-1, 0, 0), maths::vec2f(0.375, 1)));
		primitive->faces.push_back(Face(12, 13, 14));
		primitive->faces.push_back(Face(12, 14, 15));
		primitive->vertices.push_back(Vertex(maths::vec3f(1, 1, -1), maths::vec3f(0, 0, -1), maths::vec2f(0.125, 0.5)));
		primitive->vertices.push_back(Vertex(maths::vec3f(1, -1, -1), maths::vec3f(0, 0, -1), maths::vec2f(0.375, 0.5)));
		primitive->vertices.push_back(Vertex(maths::vec3f(-1, -1, -1), maths::vec3f(0, 0, -1), maths::vec2f(0.375, 0.75)));
		primitive->vertices.push_back(Vertex(maths::vec3f(-1, 1, -1), maths::vec3f(0, 0, -1), maths::vec2f(0.125, 0.75)));
		primitive->faces.push_back(Face(16, 17, 18));
		primitive->faces.push_back(Face(16, 18, 19));
		primitive->vertices.push_back(Vertex(maths::vec3f(1, -1, 1), maths::vec3f(0, 0, 1), maths::vec2f(0.625, 0.5)));
		primitive->vertices.push_back(Vertex(maths::vec3f(1, 1, 1), maths::vec3f(0, 0, 1), maths::vec2f(0.875, 0.5)));
		primitive->vertices.push_back(Vertex(maths::vec3f(-1, 1, 1), maths::vec3f(0, 0, 1), maths::vec2f(0.875, 0.75)));
		primitive->vertices.push_back(Vertex(maths::vec3f(-1, -1, 1), maths::vec3f(0, 0, 1), maths::vec2f(0.625, 0.75)));
		primitive->faces.push_back(Face(20, 21, 22));
		primitive->faces.push_back(Face(20, 22, 23));
		
		primitive->setup();

		Mesh* mesh = new Mesh();
		mesh->primitives.push_back(primitive);
		scene->meshes.push_back(mesh);

		Object* object = new Object(scene, name);
		object->mesh = scene->meshes.size() - 1;
		object->hasMesh = true;

		Material* material = new Material("shaders/shaded.vert", "shaders/shaded.frag");
		material->ambientColour = maths::vec3f(0, 0, 0);
		material->diffuseColour = maths::vec3f(0.8, 0.8, 0.8);
		material->specularColour = maths::vec3f(0.8, 0.8, 0.8);
		material->shininess = 32;
		scene->materials.push_back(material);
		primitive->materialIndex = scene->materials.size() - 1;

		return object;
	}
}