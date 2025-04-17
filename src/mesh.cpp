#include "mesh.h"

#include "glad/glad.h"
#include <stddef.h>
#include <iostream>
#include <fstream>

MeshPrimitive::MeshPrimitive()
{
	VAO = 0;
	vertices = std::vector<Vertex>(0);
	edges = std::vector<Edge>(0);
	faces = std::vector<Face>(0);
	materialIndex = 0;
}

void MeshPrimitive::setup()
{
	glGenVertexArrays(1, &VAO);
	unsigned int VBO, EBO;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	if (faces.size() == 0)
	{
		// render wireframe or single points
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(Edge), &edges[0], GL_STATIC_DRAW);
	}
	else
	{
		// render faces
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(Face), &faces[0], GL_STATIC_DRAW);
	}

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));

	glBindVertexArray(0);
}

void MeshPrimitive::draw()
{
	// assume the shader has already been set up with the uniforms etc.

	glBindVertexArray(VAO);
	if (faces.size() != 0)
	{
		glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);
	}
	else if (edges.size() != 0)
	{
		glDrawElements(GL_LINES, edges.size() * 2, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawElements(GL_POINTS, vertices.size(), GL_UNSIGNED_INT, 0);
	}
}