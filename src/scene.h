#ifndef STERLING_SCENE_H
#define STERLING_SCENE_H

#include "maths.h"
#include "shaders.h"
#include "materials.h"

class Scene;
class ObjectList;
struct Mesh;
class Object;
class Empty;
class Camera;
class Axes;
class Model;
class Cube;
class Plane;

class Scene
{
private:
	void render_branch(Object* branch, maths::mat4f parentToScreen);
public:
	Camera* activeCamera;
	ObjectList* children;
	Colour ambientLight;

	Scene(Camera* activeCam, float ambientRed, float ambientGreen, float ambientBlue);

	void render();
};

class ObjectList
{
private:
	Object** data;
public:
	int count;
	int capacity;

	ObjectList();

	Object* operator[](int index);

	/// <summary>
	/// Add an object to the end of the list
	/// </summary>
	/// <param name="object">The object to add</param>
	void add(Object* object);
	/// <summary>
	/// Insert an object at a specific index
	/// </summary>
	/// <param name="object">The object to insert</param>
	/// <param name="index">The index to insert it at</param>
	void insert(Object* object, int index);

	/// <summary>
	/// Remove an object at a specific index
	/// </summary>
	/// <param name="index">The index to remove</param>
	void remove_at(int index);
	/// <summary>
	/// Remove an object from the list
	/// </summary>
	/// <param name="object">The object to remove</param>
	void remove(Object* object);
};

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
	float* normals;
};

class Object
{
private:
	unsigned int VAO;
	Material* material;

	void load_mesh(const char* path);
	void generate_buffers();
protected:
	Mesh* mesh;
public:
	maths::vec3f position;
	maths::unit_quaternion rotation;
	maths::vec3f scale;
	ObjectList* children;

	Object(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, Material* shader);
	Object(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, Material* shader, const char* meshPath);

	maths::mat4f local_to_world();

	void render(maths::mat4f worldToScreenMatrix, Colour* ambientLight);
};

class Empty : public Object
{
public:
	Empty(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, Material* shader);
	Empty(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, Material* shader, const char* meshPath);
};

class Axes : public Empty
{
public:
	Axes(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, Material* shader);
};

class Camera : public Empty
{
public:
	float fov;
	float nearClip;
	float farClip;
	float aspectRatio;

	Camera(maths::vec3f Position, maths::unit_quaternion Rotation, Material* shader, float FOV, float NearClip, float FarClip, float AspectRatio);

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
	Model(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, Material* shader, const char* meshPath);
};

class Cube : public Model
{
public:
	Cube(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, Material* shader);
};

class Plane : public Model
{
public:
	Plane(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, Material* shader);
};

#endif