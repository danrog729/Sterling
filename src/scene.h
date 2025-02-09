#ifndef STERLING_SCENE_H
#define STERLING_SCENE_H

#include "maths.h"

class Camera
{
public:
	maths::vec3f position;
	maths::unit_quaternion rotation;
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

#endif