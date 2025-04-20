#include "object.h"

Transformation::Transformation()
{
	_transformationMatrix = maths::mat4f(
		1, 0, 0, 0, 
		0, 1, 0, 0, 
		0, 0, 1, 0, 
		0, 0, 0, 1
	);
	_inverseMatrix = maths::mat4f(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
	_position = maths::vec3f(0, 0, 0);
	_rotation = maths::unit_quaternion(1, 0, 0, 0);
	_scale = maths::vec3f(1, 1, 1);
	isDirty = false;
	isInverseDirty = false;
	_changedOnLastAccess = true;
}

maths::mat4f Transformation::transformationMatrix()
{
	if (isDirty)
	{
		isDirty = false;
		_changedOnLastAccess = true;
		maths::mat4f translationMatrix = maths::mat4f(
			1, 0, 0, _position.x,
			0, 1, 0, _position.y,
			0, 0, 1, _position.z,
			0, 0, 0, 1
		);
		maths::mat4f scaleMatrix = maths::mat4f(
			_scale.x, 0, 0, 0,
			0, _scale.y, 0, 0,
			0, 0, _scale.z, 0,
			0, 0, 0, 1
		);
		_transformationMatrix = translationMatrix * _rotation.to_rotation_matrix() * scaleMatrix;
	}
	else
	{
		_changedOnLastAccess = false;
	}
	return _transformationMatrix;
}

maths::mat4f Transformation::inverseMatrix()
{
	if (isInverseDirty)
	{
		isInverseDirty = false;
		_changedOnLastAccess = true;
		maths::mat4f translationMatrix = maths::mat4f(
			1, 0, 0, -_position.x,
			0, 1, 0, -_position.y,
			0, 0, 1, -_position.z,
			0, 0, 0, 1
		);
		maths::mat4f scaleMatrix = maths::mat4f(
			1 / _scale.x, 0, 0, 0,
			0, 1 / _scale.y, 0, 0,
			0, 0, 1 / _scale.z, 0,
			0, 0, 0, 1
		);
		_inverseMatrix = scaleMatrix * _rotation.conjugate().to_rotation_matrix() * translationMatrix;
	}
	else
	{
		_changedOnLastAccess = false;
	}
	return _inverseMatrix;
}

bool Transformation::changedOnLastAccess()
{
	return _changedOnLastAccess;
}

maths::vec3f Transformation::position()
{
	return _position;
}
void Transformation::position(maths::vec3f newPosition)
{
	_position = newPosition;
	isDirty = true;
	isInverseDirty = true;
	_changedOnLastAccess = true;
}

maths::unit_quaternion Transformation::rotation()
{
	return _rotation;
}
void Transformation::rotation(maths::unit_quaternion newRotation)
{
	_rotation = newRotation;
	isDirty = true;
	isInverseDirty = true;
	_changedOnLastAccess = true;
}

maths::vec3f Transformation::scale()
{
	return _scale;
}
void Transformation::scale(maths::vec3f newScale)
{
	_scale = newScale;
	isDirty = true;
	isInverseDirty = true;
	_changedOnLastAccess = true;
}

maths::vec3f Transformation::rotate_vector(maths::vec3f vector)
{
	maths::vec4f result = _rotation.to_rotation_matrix() * maths::vec4f(vector.x, vector.y, vector.z, 0);
	return maths::vec3f(result.x, result.y, result.z);
}
maths::vec3f Transformation::up()
{
	return rotate_vector(maths::vec3f(0, 0, 1));
}
maths::vec3f Transformation::down()
{
	return rotate_vector(maths::vec3f(0, 0, -1));
}
maths::vec3f Transformation::left()
{
	return rotate_vector(maths::vec3f(-1, 0, 0));
}
maths::vec3f Transformation::right()
{
	return rotate_vector(maths::vec3f(1, 0, 0));
}
maths::vec3f Transformation::forward()
{
	return rotate_vector(maths::vec3f(0, 1, 0));
}
maths::vec3f Transformation::backward()
{
	return rotate_vector(maths::vec3f(0, -1, 0));
}

Object::Object(Scene* scene)
{
	this->scene = scene;
	parent = NULL;
	hasMesh = false;
	mesh = 0;
}

Object::Object(const char* filepath, Scene* scene)
{
	this->scene = scene;
	mesh = scene->load_mesh(filepath);
	parent = NULL;
	hasMesh = true;
}

Object::~Object()
{
	std::vector<Object*>* parentsList;
	if (parent == NULL)
	{
		parentsList = &scene->children;
	}
	else
	{
		parentsList = &parent->children;
	}
	// remove from parent's child list
	bool found = false;
	for (int childIndex = 0; childIndex < parentsList->size() && !found; childIndex++)
	{
		if ((*parentsList)[childIndex] == this)
		{
			found = true;
			parentsList->erase(parentsList->begin() + childIndex);
		}
	}
	// add all of the object's children to the parent's child list
	for (int childIndex = 0; childIndex < children.size(); childIndex++)
	{
		parentsList->push_back(children[childIndex]);
	}
}

void Object::add_child(Object* child)
{
	children.push_back(child);
	child->parent = this;
	child->scene = scene;
}

maths::mat4f Object::get_global_matrix()
{
	if (parent == NULL)
	{
		return transformation.transformationMatrix();
	}
	else
	{
		return parent->transformation.transformationMatrix() * transformation.transformationMatrix();
	}
}

void Object::render()
{
	if (hasMesh)
	{
		for (int primitiveIndex = 0; primitiveIndex < scene->meshes[mesh]->primitives.size(); primitiveIndex++)
		{
			scene->materials[scene->meshes[mesh]->primitives[primitiveIndex]->materialIndex]->use(get_global_matrix());
			scene->meshes[mesh]->primitives[primitiveIndex]->draw();
		}
	}
	for (int childIndex = 0; childIndex < children.size(); childIndex++)
	{
		children[childIndex]->render();
	}
}

float Camera::fov()
{
	return _fov;
}
void Camera::fov(float newFOV)
{
	projectionMatrixDirty = true;
	_fov = newFOV;
}

float Camera::nearClip()
{
	return _nearClip;
}
void Camera::nearClip(float newNearClip)
{
	projectionMatrixDirty = true;
	_nearClip = newNearClip;
}

float Camera::farClip()
{
	return _farClip;
}
void Camera::farClip(float newFarClip)
{
	projectionMatrixDirty = true;
	_farClip = newFarClip;
}

float Camera::aspectRatio()
{
	return _aspectRatio;
}
void Camera::aspectRatio(float newAspectRatio)
{
	projectionMatrixDirty = true;
	_aspectRatio = newAspectRatio;
}

Camera::Camera(Scene* newScene) : Object(newScene)
{
	_fov = maths::PI / 2.0f;
	_nearClip = 0.1f;
	_farClip = 100.0f;
	_aspectRatio = 4.0f / 3.0f;
	projectionMatrixDirty = true;

	glGenBuffers(1, &matrixBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, matrixBuffer);
	glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, matrixBuffer);
	projection_matrix();
}

Camera::~Camera()
{
	// remove the scene's reference to the active camera if this is the active camera
	if (scene->activeCamera == this)
	{
		scene->activeCamera = NULL;
	}
}

maths::mat4f Camera::projection_matrix()
{
	if (projectionMatrixDirty)
	{
		projectionMatrixDirty = false;
		maths::mat4f perspective = maths::mat4f(
			_nearClip, 0.0f, 0.0f, 0.0f,
			0.0f, _nearClip, 0.0f, 0.0f,
			0.0f, 0.0f, _farClip + _nearClip, -_farClip * _nearClip,
			0.0f, 0.0f, 1.0f, 0.0f
		);

		maths::mat4f orthographic = maths::mat4f(
			1.0f / (_nearClip * tanf(_fov / 2.0f)), 0.0f, 0.0f, 0.0f,
			0.0f, _aspectRatio / (_nearClip * tanf(_fov / 2.0f)), 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f / (_farClip - _nearClip), -_nearClip / (_farClip - _nearClip),
			0.0f, 0.0f, 0.0f, 1.0f
		);

		_projection = orthographic * perspective;
		maths::mat4f transposed = maths::mat4f::transpose(_projection);

		glBindBuffer(GL_UNIFORM_BUFFER, matrixBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &transposed);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	return _projection;
}

maths::mat4f Camera::view_matrix()
{
	maths::mat4f view = maths::mat4f::stretch_z(-1.0f) * transformation.inverseMatrix();
	if (transformation.changedOnLastAccess())
	{
		// update the GPU buffer
		maths::mat4f transposed = maths::mat4f::transpose(view);
		glBindBuffer(GL_UNIFORM_BUFFER, matrixBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &transposed);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	return view;
}

Light::Light(Scene* scene) : Object(scene)
{
	_colour = maths::vec3f(1.0f, 1.0f, 1.0f);
	_isDirty = true;
}
Light::~Light()
{
	
}
maths::vec3f Light::colour()
{
	return _colour;
}
void Light::colour(maths::vec3f newColour)
{
	_colour = newColour;
	_isDirty = true;
}
bool Light::isDirty()
{
	return _isDirty || transformation.changedOnLastAccess();
}
void Light::clean()
{
	_isDirty = false;
	transformation.transformationMatrix();
}

AmbientLight::AmbientLight(Scene* scene) : Light(scene)
{
	scene->ambientLights.push_back(this);
}
AmbientLight::~AmbientLight()
{
	// remove self from scene's ambient light list
	bool found = false;
	for (int childIndex = 0; childIndex < scene->ambientLights.size() && !found; childIndex++)
	{
		if (scene->ambientLights[childIndex] == this)
		{
			found = true;
			scene->ambientLights.erase(scene->ambientLights.begin() + childIndex);
		}
	}
}

PointLight::PointLight(Scene* scene) : Light(scene)
{
	_constantAttenuation = 1.0f;
	_linearAttenuation = 0.07f;
	_quadraticAttenuation = 0.017f;
	scene->pointLights.push_back(this);
}
PointLight::~PointLight()
{
	// remove self from scene's point light list
	bool found = false;
	for (int childIndex = 0; childIndex < scene->pointLights.size() && !found; childIndex++)
	{
		if (scene->pointLights[childIndex] == this)
		{
			found = true;
			scene->pointLights.erase(scene->pointLights.begin() + childIndex);
		}
	}
}
float PointLight::constantAttenuation()
{
	return _constantAttenuation;
}
void PointLight::constantAttenuation(float newValue)
{
	_constantAttenuation = newValue;
	_isDirty = true;
}
float PointLight::linearAttenuation()
{
	return _linearAttenuation;
}
void PointLight::linearAttenuation(float newValue)
{
	_linearAttenuation = newValue;
	_isDirty = true;
}
float PointLight::quadraticAttenuation()
{
	return _quadraticAttenuation;
}
void PointLight::quadraticAttenuation(float newValue)
{
	_quadraticAttenuation = newValue;
	_isDirty = true;
}
void PointLight::add_to_uniform_buffer(unsigned int offset, maths::mat4f viewSpaceMatrix, bool forcePositionUpdate)
{
	if (forcePositionUpdate || _isDirty)
	{
		maths::vec3f position = transformation.position();
		maths::vec4f transformed = viewSpaceMatrix * maths::vec4f(position.x, position.y, position.z, 1.0f);
		position = maths::vec3f(transformed.x, transformed.y, transformed.z);
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 16, 12, &position);
	}
	if (_isDirty)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, offset, 12, &_colour);
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 28, 4, &_constantAttenuation);
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 32, 4, &_linearAttenuation);
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 36, 4, &_quadraticAttenuation);
	}
	_isDirty = false;
}

Spotlight::Spotlight(Scene* scene) : Light(scene)
{
	_constantAttenuation = 1.0f;
	_linearAttenuation = 0.07f;
	_quadraticAttenuation = 0.017f;
	_innerCutoff = 0.45f;
	_outerCutoff = maths::PI / 6;
	scene->spotlights.push_back(this);
}
Spotlight::~Spotlight()
{
	// remove self from scene's spotlight list
	bool found = false;
	for (int childIndex = 0; childIndex < scene->spotlights.size() && !found; childIndex++)
	{
		if (scene->spotlights[childIndex] == this)
		{
			found = true;
			scene->spotlights.erase(scene->spotlights.begin() + childIndex);
		}
	}
}
float Spotlight::constantAttenuation()
{
	return _constantAttenuation;
}
void Spotlight::constantAttenuation(float newValue)
{
	_constantAttenuation = newValue;
	_isDirty = true;
}
float Spotlight::linearAttenuation()
{
	return _linearAttenuation;
}
void Spotlight::linearAttenuation(float newValue)
{
	_linearAttenuation = newValue;
	_isDirty = true;
}
float Spotlight::quadraticAttenuation()
{
	return _quadraticAttenuation;
}
void Spotlight::quadraticAttenuation(float newValue)
{
	_quadraticAttenuation = newValue;
	_isDirty = true;
}
float Spotlight::innerCutoff()
{
	return _innerCutoff;
}
void Spotlight::innerCutoff(float newValue)
{
	_innerCutoff = newValue;
	_isDirty = true;
}
float Spotlight::outerCutoff()
{
	return _outerCutoff;
}
void Spotlight::outerCutoff(float newValue)
{
	_outerCutoff = newValue;
	_isDirty = true;
}
void Spotlight::add_to_uniform_buffer(unsigned int offset, maths::mat4f viewSpaceMatrix, bool forcePositionUpdate)
{
	if (forcePositionUpdate || _isDirty)
	{
		maths::vec3f position = transformation.position();
		maths::vec4f transformed = viewSpaceMatrix * maths::vec4f(position.x, position.y, position.z, 1.0f);
		position = maths::vec3f(transformed.x, transformed.y, transformed.z);
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 16, 12, &position);

		transformed = viewSpaceMatrix * transformation.transformationMatrix() * maths::vec4f(0.0f, 0.0f, -1.0f, 0.0f);
		maths::vec3f direction = maths::vec3f(transformed.x, transformed.y, transformed.z);
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 32, 12, &direction);
	}
	if (_isDirty)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, offset, 12, &_colour);
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 44, 4, &_constantAttenuation);
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 48, 4, &_linearAttenuation);
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 52, 4, &_quadraticAttenuation);
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 56, 4, &_innerCutoff);
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 60, 4, &_outerCutoff);
	}
	_isDirty = false;
}

DirectionalLight::DirectionalLight(Scene* scene) : Light(scene)
{
	scene->directionalLights.push_back(this);
}
DirectionalLight::~DirectionalLight()
{
	// remove self from scene's directional light list
	bool found = false;
	for (int childIndex = 0; childIndex < scene->directionalLights.size() && !found; childIndex++)
	{
		if (scene->directionalLights[childIndex] == this)
		{
			found = true;
			scene->directionalLights.erase(scene->directionalLights.begin() + childIndex);
		}
	}
}
void DirectionalLight::add_to_uniform_buffer(unsigned int offset, maths::mat4f viewSpaceMatrix, bool forcePositionUpdate)
{
	if (forcePositionUpdate || _isDirty)
	{
		maths::vec4f transformed = viewSpaceMatrix * transformation.transformationMatrix() * maths::vec4f(0.0f, 0.0f, -1.0f, 0.0f);
		maths::vec3f direction = maths::vec3f(transformed.x, transformed.y, transformed.z);
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 16, 12, &direction);
	}
	if (_isDirty)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, offset, 12, &_colour);
	}
	_isDirty = false;
}