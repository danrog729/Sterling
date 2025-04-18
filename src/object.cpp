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
	_fov = maths::PI / 2;
	_nearClip = 0.1;
	_farClip = 100;
	_aspectRatio = 4.0f / 3.0f;
	projectionMatrixDirty = true;

	glGenBuffers(1, &matrixBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, matrixBuffer);
	glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, matrixBuffer);
	projection_matrix();
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
	maths::mat4f view = transformation.inverseMatrix();
	if (transformation.changedOnLastAccess())
	{
		// update the GPU buffer
		maths::mat4f transposed = maths::mat4f::transpose(maths::mat4f::stretch_z(-1.0f) * view);
		glBindBuffer(GL_UNIFORM_BUFFER, matrixBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &transposed);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	return view;
}