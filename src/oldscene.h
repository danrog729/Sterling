#ifndef STERLING_OLDSCENE_H
#define STERLING_OLDSCENE_H

#include "maths.h"
#include "shaders.h"
#include "oldmaterials.h"

class OldScene;
class ObjectList;
struct OldVertex;
struct OldMesh;
class OldObject;
class Empty;
class OldCamera;
class Axes;
class AmbientLight;
class PointLight;
class Model;
class Cube;
class Plane;

class OldScene
{
private:
	void render_branch(OldObject* branch, maths::mat4f projectionMatrix, maths::mat4f viewMatrix, maths::mat4f parentToWorld);
public:
	OldCamera* activeCamera;
	AmbientLight* activeLight;
	ObjectList* children;
	Colour ambientLight;

	OldScene(OldCamera* activeCam, AmbientLight* ActiveLight, float ambientRed, float ambientGreen, float ambientBlue);

	void render();
};

class ObjectList
{
private:
	OldObject** data;
public:
	int count;
	int capacity;

	ObjectList();

	OldObject* operator[](int index);

	/// <summary>
	/// Add an object to the end of the list
	/// </summary>
	/// <param name="object">The object to add</param>
	void add(OldObject* object);
	/// <summary>
	/// Insert an object at a specific index
	/// </summary>
	/// <param name="object">The object to insert</param>
	/// <param name="index">The index to insert it at</param>
	void insert(OldObject* object, int index);

	/// <summary>
	/// Remove an object at a specific index
	/// </summary>
	/// <param name="index">The index to remove</param>
	void remove_at(int index);
	/// <summary>
	/// Remove an object from the list
	/// </summary>
	/// <param name="object">The object to remove</param>
	void remove(OldObject* object);
};

struct OldVertex
{
public:
	maths::vec3f position;
	maths::vec3f normal;
};
struct OldEdge
{
public:
	int vertex1;
	int vertex2;
};
struct OldFace
{
public:
	int vertex1;
	int vertex2;
	int vertex3;
};

struct OldMesh
{
public:
	const static char MESH_COLOURING_NONE = 0;
	const static char MESH_COLOURING_VERTEX = 1;
	const static char MESH_COLOURING_EDGE = 2;
	const static char MESH_COLOURING_FACE = 3;

	const static char MESH_SHADING_SMOOTH = 0;
	const static char MESH_SHADING_FLAT = 1;

	OldVertex* vertices;
	unsigned int vertexCount;
	OldEdge* edges;
	unsigned int edgeCount;
	OldFace* faces;
	unsigned int faceCount;
	char colouringMode;
	char shadingMode;
};

class OldObject
{
private:
	unsigned int VAO;
	OldMaterial* material;

	void load_mesh(const char* path);
	void generate_buffers();
protected:
	OldMesh* mesh;
public:
	maths::vec3f position;
	maths::unit_quaternion rotation;
	maths::vec3f scale;
	ObjectList* children;

	OldObject(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, OldMaterial* shader);
	OldObject(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, OldMaterial* shader, const char* meshPath);

	maths::mat4f local_to_world();

	void render(maths::mat4f projectionMatrix, maths::mat4f viewMatrix,
		Colour* ambientLight, maths::vec3f lightPosition, Colour* lightColour);
};

class Empty : public OldObject
{
public:
	Empty(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, OldMaterial* shader);
	Empty(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, OldMaterial* shader, const char* meshPath);
};

class Axes : public Empty
{
public:
	Axes(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, OldMaterial* shader);
};

class OldCamera : public Empty
{
public:
	float fov;
	float nearClip;
	float farClip;
	float aspectRatio;

	OldCamera(maths::vec3f Position, maths::unit_quaternion Rotation, OldMaterial* shader, float FOV, float NearClip, float FarClip, float AspectRatio);

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

class AmbientLight : public Empty
{
public:
	AmbientLight(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, OldMaterial* material);
	AmbientLight(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, OldMaterial* shader, const char* meshPath);
};

class PointLight : public AmbientLight
{
public:
	PointLight(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, OldMaterial* material);
};

class Model : public OldObject
{
public:
	Model(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, OldMaterial* shader, const char* meshPath);
};

class Cube : public Model
{
public:
	Cube(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, OldMaterial* shader);
};

class Plane : public Model
{
public:
	Plane(maths::vec3f Position, maths::unit_quaternion Rotation, maths::vec3f Scale, OldMaterial* shader);
};

#endif