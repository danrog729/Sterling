#ifndef STERLING_SCENE_H
#define STERLING_SCENE_H

#include "maths.h"

struct Mesh
{
public:
	const static char MESH_COLOURING_NONE = 0;
	const static char MESH_COLOURING_VERTEX = 1;
	const static char MESH_COLOURING_EDGE = 2;
	const static char MESH_COLOURING_FACE = 3;

	float* vertices;
	unsigned int vertexCount;
	int* edges;
	unsigned int edgeRefCount;
	int* faces;
	unsigned int faceRefCount;
	char colouringMode;
};

class Object
{
private:
	unsigned int VAO;

	void load_mesh(const char* path);
	void generate_buffers();
protected:
	Mesh* mesh;
public:
	maths::vec3f position;
	maths::unit_quaternion rotation;
	maths::vec3f scale;

	Object(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale);
	Object(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, const char* meshPath);

	maths::mat4f local_to_world();

	void render();
};

class Empty : public Object
{
public:
	Empty(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale);
	Empty(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, const char* meshPath);
};

class Axes : public Empty
{
public:
	Axes(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale);
};

class Camera : public Empty
{
public:
	float fov;
	float nearClip;
	float farClip;
	float aspectRatio;

	Camera(maths::vec3f Position, maths::unit_quaternion Rotation, float FOV, float NearClip, float FarClip, float AspectRatio);

	maths::mat4f cameraspace_matrix();
	maths::mat4f orthographic_matrix();
	maths::mat4f perspective_matrix();

	maths::vec3f front();
	maths::vec3f back();
	maths::vec3f left();
	maths::vec3f right();
	maths::vec3f up();
	maths::vec3f down();
};

class Model : public Object
{
public:
	Model(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, const char* meshPath);
};

class Cube : public Model
{
public:
	Cube(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale);
};

class Plane : public Model
{
public:
	Plane(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale);
};

#endif