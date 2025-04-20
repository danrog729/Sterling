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
	/// Destroy the object
	/// </summary>
	virtual ~Object();

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

	~Camera() override;

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

class Light : public Object
{
protected:
	maths::vec3f _colour;
	bool _isDirty;

public:
	/// <summary>
	/// Represents the colour of the light
	/// </summary>
	maths::vec3f colour();
	void colour(maths::vec3f newColour);

	bool isDirty();

	Light(Scene* scene);
	virtual ~Light() override;

	// Forcibly clear the isDirty flag
	void clean();
};

class AmbientLight : public Light
{
public:
	AmbientLight(Scene* scene);
	~AmbientLight() override;
};

class PointLight : public Light
{
private:
	float _constantAttenuation;
	float _linearAttenuation;
	float _quadraticAttenuation;
public:
	/// <summary>
	/// Controls the attenuation of the light
	/// </summary>
	float constantAttenuation();
	void constantAttenuation(float newValue);
	float linearAttenuation();
	void linearAttenuation(float newValue);
	float quadraticAttenuation();
	void quadraticAttenuation(float newValue);

	PointLight(Scene* scene);
	~PointLight() override;

	/// <summary>
	/// Add this light's information to the currently bound uniform buffer, at a specified offset
	/// </summary>
	/// <param name="offset">The offset to add the light's information at</param>
	/// <param name="viewSpaceMatrix">The matrix to transform light positions into the camera's view space</param>
	/// <param name="forcePositionUpdate">Force the light positions to be updated, because the viewspace matrix has changed</param>
	void add_to_uniform_buffer(unsigned int offset, maths::mat4f viewSpaceMatrix, bool forcePositionUpdate);
};

class Spotlight : public Light
{
private:
	float _constantAttenuation;
	float _linearAttenuation;
	float _quadraticAttenuation;
	float _innerCutoff;
	float _outerCutoff;
public:
	/// <summary>
	/// Controls the cutoff angles of the spotlight from the direction of the spotlight (in radians)
	/// </summary>
	float constantAttenuation();
	void constantAttenuation(float newValue);
	float linearAttenuation();
	void linearAttenuation(float newValue);
	float quadraticAttenuation();
	void quadraticAttenuation(float newValue);
	float innerCutoff();
	void innerCutoff(float newValue);
	float outerCutoff();
	void outerCutoff(float newValue);

	Spotlight(Scene* scene);
	~Spotlight() override;

	/// <summary>
	/// Add this light's information to the currently bound uniform buffer, at a specified offset
	/// </summary>
	/// <param name="offset">The offset to add the light's information at</param>
	/// <param name="viewSpaceMatrix">The matrix to transform light positions into the camera's view space</param>
	/// <param name="forcePositionUpdate">Force the light positions to be updated, because the viewspace matrix has changed</param>
	void add_to_uniform_buffer(unsigned int offset, maths::mat4f viewSpaceMatrix, bool forcePositionUpdate);
};

class DirectionalLight : public Light
{
public:
	DirectionalLight(Scene* scene);
	~DirectionalLight() override;

	/// <summary>
	/// Add this light's information to the currently bound uniform buffer, at a specified offset
	/// </summary>
	/// <param name="offset">The offset to add the light's information at</param>
	/// <param name="viewSpaceMatrix">The matrix to transform light positions into the camera's view space</param>
	/// <param name="forcePositionUpdate">Force the light positions to be updated, because the viewspace matrix has changed</param>
	void add_to_uniform_buffer(unsigned int offset, maths::mat4f viewSpaceMatrix, bool forcePositionUpdate);
};

#endif