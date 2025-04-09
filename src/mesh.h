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
};

struct Edge
{
public:
	unsigned int vertex1;
	unsigned int vertex2;
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
};

class Mesh
{
private:
	std::vector<Vertex> vertices;
	std::vector<Edge> edges;
	std::vector<Face> faces;
	unsigned int VAO;

	int load_obj(const char* path);

public:
	Mesh(const char* filePath);
	void draw();
};

#endif