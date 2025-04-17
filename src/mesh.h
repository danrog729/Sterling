#ifndef STERLING_MESH_H
#define STERLING_MESH_H

#include "maths.h"
#include <vector>

struct Vertex
{
public:
	maths::vec3f position;
	maths::vec3f normal;
	maths::vec2f textureCoords;

	Vertex(maths::vec3f Position, maths::vec3f Normal, maths::vec2f TextureCoords)
	{
		position = Position;
		normal = Normal;
		textureCoords = TextureCoords;
	}

	Vertex()
	{
		position = maths::vec3f(0, 0, 0);
		normal = maths::vec3f(0, 0, 0);
		textureCoords = maths::vec2f(0, 0);
	}
};

struct Edge
{
public:
	unsigned int vertex1;
	unsigned int vertex2;

	Edge(unsigned int v1, unsigned int v2)
	{
		vertex1 = v1;
		vertex2 = v2;
	}

	Edge()
	{
		vertex1 = 0;
		vertex2 = 0;
	}
};

struct Face
{
public:
	unsigned int vertex1;
	unsigned int vertex2;
	unsigned int vertex3;

	Face(unsigned int v1, unsigned int v2, unsigned int v3)
	{
		vertex1 = v1;
		vertex2 = v2;
		vertex3 = v3;
	}

	Face()
	{
		vertex1 = 0;
		vertex2 = 0;
		vertex3 = 0;
	}
};

class MeshPrimitive
{
private:
	unsigned int VAO;
public:
	std::vector<Vertex> vertices;
	std::vector<Edge> edges;
	std::vector<Face> faces;
	unsigned int materialIndex;

	MeshPrimitive();
	void setup();
	void draw();
};

class Mesh
{
public:
	std::vector<MeshPrimitive*> primitives;
};

#endif