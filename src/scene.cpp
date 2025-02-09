#include "scene.h"
#include <math.h>

Camera::Camera(maths::vec3f Position, maths::unit_quaternion Rotation, float FOV, float NearClip, float FarClip, float AspectRatio)
{
	position = Position;
	rotation = Rotation;
	fov = FOV;
	nearClip = NearClip;
	farClip = FarClip;
	aspectRatio = AspectRatio;
}

maths::mat4f Camera::cameraspace_matrix()
{
	maths::mat4f locationMatrix = maths::mat4f(
		1.0f, 0.0f, 0.0f, -position.x,
		0.0f, 1.0f, 0.0f, -position.y,
		0.0f, 0.0f, 1.0f, -position.z,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	maths::mat4f rotationMatrix = rotation.to_rotation_matrix();
	return rotationMatrix * locationMatrix;
}

maths::mat4f Camera::orthographic_matrix()
{
	return maths::mat4f(
		1.0f / nearClip / tan(fov / 2.0f), 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f / nearClip / tan(fov / 2.0f) * aspectRatio, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f / (farClip - nearClip), -nearClip / (farClip - nearClip),
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

maths::mat4f Camera::perspective_matrix()
{
	return maths::mat4f(
		nearClip, 0.0f, 0.0f, 0.0f,
		0.0f, nearClip, 0.0f, 0.0f,
		0.0f, 0.0f, farClip + nearClip, -farClip * nearClip,
		0.0f, 0.0f, 1.0f, 0.0f
	);
}

maths::vec3f Camera::front()
{
	maths::vec4f vector4 = rotation.to_rotation_matrix() * maths::vec4f(0.0f, 0.0f, -1.0f, 0.0f);
	return maths::vec3f(vector4.x, vector4.y, vector4.z);
}

maths::vec3f Camera::back()
{
	maths::vec4f vector4 = rotation.to_rotation_matrix() * maths::vec4f(0.0f, 0.0f, 1.0f, 0.0f);
	return maths::vec3f(vector4.x, vector4.y, vector4.z);
}

maths::vec3f Camera::left()
{
	maths::vec4f vector4 = rotation.to_rotation_matrix() * maths::vec4f(-1.0f, 0.0f, 0.0f, 0.0f);
	return maths::vec3f(vector4.x, vector4.y, vector4.z);
}

maths::vec3f Camera::right()
{
	maths::vec4f vector4 = rotation.to_rotation_matrix() * maths::vec4f(1.0f, 0.0f, 0.0f, 0.0f);
	return maths::vec3f(vector4.x, vector4.y, vector4.z);
}

maths::vec3f Camera::up()
{
	maths::vec4f vector4 = rotation.to_rotation_matrix() * maths::vec4f(0.0f, 1.0f, 0.0f, 0.0f);
	return maths::vec3f(vector4.x, vector4.y, vector4.z);
}

maths::vec3f Camera::down()
{
	maths::vec4f vector4 = rotation.to_rotation_matrix() * maths::vec4f(0.0f, -1.0f, 0.0f, 0.0f);
	return maths::vec3f(vector4.x, vector4.y, vector4.z);
}