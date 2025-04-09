#include "mesh.h"

#include "glad/glad.h"
#include <stddef.h>
#include <iostream>
#include <fstream>

Mesh::Mesh(const char* filePath)
{
	if (load_obj(filePath) == 0)
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
			// render wireframe
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
}

void Mesh::draw()
{
	glBindVertexArray(VAO);
	if (faces.size() != 0)
	{
		glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawElements(GL_LINES, edges.size() * 2, GL_UNSIGNED_INT, 0);
	}
}

int Mesh::load_obj(const char* path)
{
	std::ifstream file;
	file.exceptions(std::ifstream::badbit);
	try
	{
		// open file
		file.open(path);

		// read the data
		std::vector<maths::vec3f*> vertexPositions;
		std::vector<maths::vec3f*> vertexNormals;
		std::vector<maths::vec2f*> texCoords;
		char line[256];
		while (file.getline(line, 256))
		{
			// tokenise the line
			std::vector<char*> tokens;
			int index = -1;
			int startOfToken = 0;
			bool shouldContinue = true;
			do
			{
				index++;
				char character = line[index];
				switch (character)
				{
				case '\0':
					shouldContinue = false;
					[[__fallthrough]]
				case ' ':
				case '\n':
					// process token
					if (index != startOfToken)
					{
						line[index] = '\0';
						tokens.push_back(&line[startOfToken]);
					}
					startOfToken = index + 1;
				}
			} while (shouldContinue);

			// interpret the tokens
			if (strcmp(tokens[0], "v") == 0 && tokens.size() >= 4)
			{
				// defining new vertex
				maths::vec3f* candidate = new maths::vec3f();
				if (sscanf_s(tokens[1], "%f", &candidate->x) != 1 ||
					sscanf_s(tokens[2], "%f", &candidate->y) != 1 ||
					sscanf_s(tokens[3], "%f", &candidate->z) != 1)
				{
					std::cerr << "ERROR::MESH::UNABLE_TO_PARSE_VERTEX_POSITION_DATA\n";
					delete candidate;
				}
				else
				{
					vertexPositions.push_back(candidate);
				}
			}
			else if (strcmp(tokens[0], "vn") == 0 && tokens.size() >= 4)
			{
				// defining new vertex normal
				maths::vec3f* candidate = new maths::vec3f();
				if (sscanf_s(tokens[1], "%f", &candidate->x) != 1 ||
					sscanf_s(tokens[2], "%f", &candidate->y) != 1 ||
					sscanf_s(tokens[3], "%f", &candidate->z) != 1)
				{
					std::cerr << "ERROR::MESH::UNABLE_TO_PARSE_VERTEX_NORMAL_DATA\n";
					delete candidate;
				}
				else
				{
					vertexNormals.push_back(candidate);
				}
			}
			else if (strcmp(tokens[0], "vt") == 0 && tokens.size() >= 3)
			{
				// defining new texture coordinate
				maths::vec2f* candidate = new maths::vec2f();
				if (sscanf_s(tokens[1], "%f", &candidate->x) != 1 ||
					sscanf_s(tokens[2], "%f", &candidate->y) != 1)
				{
					std::cerr << "ERROR::MESH::UNABLE_TO_PARSE_TEXTURE_COORDINATE_DATA\n";
					delete candidate;
				}
				else
				{
					texCoords.push_back(candidate);
				}
			}
			else if (strcmp(tokens[0], "f") == 0 && tokens.size() >= 4)
			{
				// defining new face
				std::vector<int> points;
				for (int tokenIndex = 1; tokenIndex < tokens.size(); tokenIndex++)
				{
					// each token should be of the forms:
					// v (only vertex position is specified)
					// v/vt (vertex position and texture coordinates are specified)
					// v//vn (vertex position and normal is specified)
					// v/vt/vn (vertex position, texture coordinate and normal are specified)
					char* token = tokens[tokenIndex];
					unsigned int characterIndex = -1;
					unsigned int startOfInteger = 0;
					unsigned int slashCount = 0;
					int* point = (int*)malloc(3 * sizeof(int));
					bool shouldContinue = true;
					bool validPoint = true;
					do
					{
						characterIndex++;
						char character = token[characterIndex];
						switch (character)
						{
						case '\0':
							shouldContinue = false;
							[[__fallthrough]]
						case '/':
							// end of integer
							if (startOfInteger != characterIndex)
							{
								token[characterIndex] = '\0';
								int result = 0;
								if (slashCount > 2) // too many slashes
								{
									std::cerr << "ERROR::MESH::FACE_HAS_TOO_MANY_SLASHES\n";
									validPoint = false;
								}
								else if (sscanf_s(&token[startOfInteger], "%d", &point[slashCount]) != 1) // parse the integer if possible
								{
									std::cerr << "ERROR::MESH::UNABLE_TO_PARSE_FACE_DATA\n";
									validPoint = false;
								}
							}
							// increment slashcount for next time
							if (character == '/')
							{
								slashCount++;
							}
							startOfInteger = characterIndex + 1;
						}
					} while (shouldContinue);
					if (validPoint)
					{
						// add point to vertex array
						points.push_back(vertices.size());
						if (point[0] < 0)
						{
							point[0] += vertexPositions.size();
						}
						if (point[1] < 0)
						{
							point[1] += texCoords.size();
						}
						if (point[2] < 0)
						{
							point[2] += vertexNormals.size();
						}
						vertices.push_back(Vertex(*vertexPositions[point[0] - 1], *vertexNormals[point[2] - 1], *texCoords[point[1] - 1]));
					}
					free(point);
				}
				// triangulate the face and add each triangle to the list
				for (int pointIndex = 2; pointIndex < points.size(); pointIndex++)
				{
					// triangulate points[0], points[pointIndex - 1] and points[pointIndex]
					faces.push_back(Face(points[0], points[pointIndex - 1], points[pointIndex]));
				}
			}
		}

		// close file
		file.close();
		return 0;
	}
	catch (std::ifstream::failure e)
	{
		// Get the error code
		std::ios_base::iostate state = file.rdstate();

		// Check for specific error bits
		if (state & std::ios_base::eofbit)
		{
			std::cout << "End of file reached." << std::endl;
		}
		if (state & std::ios_base::failbit)
		{
			std::cout << "Non-fatal I/O error occurred." << std::endl;
		}
		if (state & std::ios_base::badbit)
		{
			std::cout << "Fatal I/O error occurred." << std::endl;
		}

		// Print system error message
		std::perror("Error: ");
		return -1;
	}
}