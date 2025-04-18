#ifndef STERLING_OBJECT_H
#define STERLING_OBJECT_H

#include "maths.h"
#include "scene.h"
#include "mesh.h"
#include "vector"

class Scene;

struct Transformation
{
private:
	maths::mat4f _transformationMatrix;
	maths::mat4f _inverseMatrix;
	maths::vec3f _position;
	maths::unit_quaternion _rotation;
	maths::vec3f _scale;
	bool isDirty;
	bool isInverseDirty;
	bool _changedOnLastAccess;

public:
	// constructors
	Transformation();

	// getters, setters
	maths::mat4f transformationMatrix();
	maths::mat4f inverseMatrix();
	bool changedOnLastAccess();

	maths::vec3f position();
	void position(maths::vec3f newPosition);

	maths::unit_quaternion rotation();
	void rotation(maths::unit_quaternion newRotation);

	maths::vec3f scale();
	void scale(maths::vec3f newScale);

	// Get an axis in object space
	maths::vec3f rotate_vector(maths::vec3f vector);
	maths::vec3f up();
	maths::vec3f down();
	maths::vec3f left();
	maths::vec3f right();
	maths::vec3f forward();
	maths::vec3f backward();
};

class Object
{
private:
	bool hasMesh;

public:
	/// <summary>
	/// Index into the scene's mesh list for which mesh this object should include
	/// </summary>
	int mesh;
	/// <summary>
	/// The position, rotation and scale of the object
	/// </summary>
	Transformation transformation;
	/// <summary>
	/// List of child objects, these inherit this object's transformation
	/// </summary>
	std::vector<Object*> children;
	/// <summary>
	/// The parent of this object. Its transformation is inherited in this object.
	/// </summary>
	Object* parent;
	/// <summary>
	/// The scene this object is a part of
	/// </summary>
	Scene* scene;

	/// <summary>
	/// Create a new object with no mesh
	/// </summary>
	/// <param name="scene">The scene to add the object to</param>
	Object(Scene* scene);
	/// <summary>
	/// Create a new object
	/// </summary>
	/// <param name="filepath">The filepath to the mesh for this object to have</param>
	/// <param name="scene">The scene to add the object to</param>
	Object(const char* filepath, Scene* scene);
	/// <summary>
	/// Give this object a child
	/// </summary>
	/// <param name="child">The object to add as a child</param>
	void add_child(Object* child);
	/// <summary>
	/// Get the matrix to transform local space to absolute world space. Combines the matrices of all parent objects.
	/// </summary>
	/// <returns></returns>
	maths::mat4f get_global_matrix();
	/// <summary>
	/// Render the object
	/// </summary>
	void render();
};

class Camera : public Object
{
private:
	float _fov;
	float _nearClip;
	float _farClip;
	float _aspectRatio;

	maths::mat4f _projection;
	bool projectionMatrixDirty;

	unsigned int matrixBuffer;

public:
	float fov();
	void fov(float newFOV);

	float nearClip();
	void nearClip(float newNearClip);

	float farClip();
	void farClip(float newFarClip);

	float aspectRatio();
	void aspectRatio(float newAspectRatio);

	Camera(Scene* scene);

	/// <summary>
	/// Return the matrix to transform viewspace points into screenspace points
	/// </summary>
	/// <returns>The matrix</returns>
	maths::mat4f projection_matrix();
	/// <summary>
	/// Returns the matrix to transform worldspace points into viewspace points
	/// </summary>
	/// <returns>The matrix</returns>
	maths::mat4f view_matrix();
};

#endif