#include "maths.h"
#include <math.h>
#include <iostream>

/*
Vector (3 float)
*/

float maths::vec3f::dot(const vec3f vector1, const vec3f vector2)
{
	return
		vector1.x * vector2.x +
		vector1.y * vector2.y +
		vector1.z * vector2.z;
}

maths::vec3f maths::vec3f::cross(const vec3f vector1, const vec3f vector2)
{
	return maths::vec3f(
		vector1.y * vector2.z - vector1.z * vector2.y,
		vector1.z * vector2.x - vector1.x * vector2.z,
		vector1.x * vector2.y - vector1.y * vector2.x
	);
}

maths::vec3f maths::vec3f::normalise(const vec3f vector)
{
	float magnitude = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	return maths::vec3f(vector.x / magnitude, vector.y / magnitude, vector.z / magnitude);
}

maths::vec3f maths::vec3f::operator*(float value)
{
	return maths::vec3f(
		x * value,
		y * value,
		z * value
	);
}

maths::vec3f maths::vec3f::operator/(float value)
{
	return maths::vec3f(
		x / value,
		y / value,
		z / value
	);
}

maths::vec3f maths::vec3f::operator+(const vec3f& vector)
{
	return maths::vec3f(
		x + vector.x,
		y + vector.y,
		z + vector.z
	);
}

maths::vec3f maths::vec3f::operator-(const vec3f& vector)
{
	return maths::vec3f(
		x - vector.x,
		y - vector.y,
		z - vector.z
	);
}

/*
Vector (4 float)
*/

float maths::vec4f::dot(const vec4f vector1, const vec4f vector2)
{
	return
		vector1.x * vector2.x +
		vector1.y * vector2.y +
		vector1.z * vector2.z +
		vector1.w * vector2.w;
}

maths::vec4f maths::vec4f::operator*(float value)
{
	return maths::vec4f(
		x * value,
		y * value,
		z * value,
		w * value
	);
}

maths::vec4f maths::vec4f::operator+(const vec4f& vector)
{
	return maths::vec4f(
		x + vector.x,
		y + vector.y,
		z + vector.z,
		w + vector.w
	);
}

maths::vec4f maths::vec4f::operator-(const vec4f& vector)
{
	return maths::vec4f(
		x - vector.x,
		y - vector.y,
		z - vector.z,
		w - vector.w
	);
}

/*
Matrix (4x4 float)
*/

maths::mat4f::mat4f()
{
	m11 = 1; m12 = 0; m13 = 0; m14 = 0;
	m21 = 0; m22 = 1; m23 = 0; m24 = 0;
	m31 = 0; m32 = 0; m33 = 1; m34 = 0;
	m41 = 0; m42 = 0; m43 = 0; m44 = 1;
}

maths::mat4f::mat4f(
	float M11, float M12, float M13, float M14,
	float M21, float M22, float M23, float M24,
	float M31, float M32, float M33, float M34,
	float M41, float M42, float M43, float M44
)
{
	m11 = M11; m12 = M12; m13 = M13; m14 = M14;
	m21 = M21; m22 = M22; m23 = M23; m24 = M24;
	m31 = M31; m32 = M32; m33 = M33; m34 = M34;
	m41 = M41; m42 = M42; m43 = M43; m44 = M44;
}

maths::mat4f maths::mat4f::operator+()
{
	return *this;
}

maths::mat4f maths::mat4f::operator-()
{
	return mat4f(
		-m11, -m12, -m13, -m14,
		-m21, -m22, -m23, -m24,
		-m31, -m32, -m33, -m34,
		-m41, -m42, -m43, -m44
	);
}

maths::mat4f maths::mat4f::operator+(const mat4f& matrix)
{
	return mat4f(
		m11 + matrix.m11, m12 + matrix.m12, m13 + matrix.m13, m14 + matrix.m14,
		m21 + matrix.m21, m22 + matrix.m22, m23 + matrix.m23, m24 + matrix.m24,
		m31 + matrix.m31, m32 + matrix.m32, m33 + matrix.m33, m34 + matrix.m34,
		m41 + matrix.m41, m42 + matrix.m42, m43 + matrix.m43, m44 + matrix.m44
	);
}

maths::mat4f maths::mat4f::operator-(const mat4f &matrix)
{
	return mat4f(
		m11 - matrix.m11, m12 - matrix.m12, m13 - matrix.m13, m14 - matrix.m14,
		m21 - matrix.m21, m22 - matrix.m22, m23 - matrix.m23, m24 - matrix.m24,
		m31 - matrix.m31, m32 - matrix.m32, m33 - matrix.m33, m34 - matrix.m34,
		m41 - matrix.m41, m42 - matrix.m42, m43 - matrix.m43, m44 - matrix.m44
	);
}

maths::mat4f maths::mat4f::operator*(const mat4f& matrix)
{
	return mat4f(
		m11 * matrix.m11 + m12 * matrix.m21 + m13 * matrix.m31 + m14 * matrix.m41,
		m11 * matrix.m12 + m12 * matrix.m22 + m13 * matrix.m32 + m14 * matrix.m42,
		m11 * matrix.m13 + m12 * matrix.m23 + m13 * matrix.m33 + m14 * matrix.m43,
		m11 * matrix.m14 + m12 * matrix.m24 + m13 * matrix.m34 + m14 * matrix.m44,

		m21 * matrix.m11 + m22 * matrix.m21 + m23 * matrix.m31 + m24 * matrix.m41,
		m21 * matrix.m12 + m22 * matrix.m22 + m23 * matrix.m32 + m24 * matrix.m42,
		m21 * matrix.m13 + m22 * matrix.m23 + m23 * matrix.m33 + m24 * matrix.m43,
		m21 * matrix.m14 + m22 * matrix.m24 + m23 * matrix.m34 + m24 * matrix.m44,

		m31 * matrix.m11 + m32 * matrix.m21 + m33 * matrix.m31 + m34 * matrix.m41,
		m31 * matrix.m12 + m32 * matrix.m22 + m33 * matrix.m32 + m34 * matrix.m42,
		m31 * matrix.m13 + m32 * matrix.m23 + m33 * matrix.m33 + m34 * matrix.m43,
		m31 * matrix.m14 + m32 * matrix.m24 + m33 * matrix.m34 + m34 * matrix.m44,

		m41 * matrix.m11 + m42 * matrix.m21 + m43 * matrix.m31 + m44 * matrix.m41,
		m41 * matrix.m12 + m42 * matrix.m22 + m43 * matrix.m32 + m44 * matrix.m42,
		m41 * matrix.m13 + m42 * matrix.m23 + m43 * matrix.m33 + m44 * matrix.m43,
		m41 * matrix.m14 + m42 * matrix.m24 + m43 * matrix.m34 + m44 * matrix.m44
	);
}

maths::vec4f maths::mat4f::operator*(const vec4f& vector)
{
	return vec4f(
		m11 * vector.x + m12 * vector.y + m13 * vector.z + m14 * vector.w,
		m21 * vector.x + m22 * vector.y + m23 * vector.z + m24 * vector.w,
		m31 * vector.x + m32 * vector.y + m33 * vector.z + m34 * vector.w,
		m41 * vector.x + m42 * vector.y + m43 * vector.z + m44 * vector.w
	);
}

maths::mat4f maths::mat4f::stretch_z(float stretch)
{
	return maths::mat4f(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, stretch, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

maths::mat4f maths::mat4f::transpose(maths::mat4f matrix)
{
	return maths::mat4f(
		matrix.m11, matrix.m21, matrix.m31, matrix.m41,
		matrix.m12, matrix.m22, matrix.m32, matrix.m42,
		matrix.m13, matrix.m23, matrix.m33, matrix.m43,
		matrix.m14, matrix.m24, matrix.m34, matrix.m44
	);
}

/*
Quaternion
*/

maths::unit_quaternion maths::unit_quaternion::operator*(const unit_quaternion& quat)
{
	return unit_quaternion(
		r * quat.r - i * quat.i - j * quat.j - k * quat.k,
		r * quat.i + i * quat.r + j * quat.k - k * quat.j,
		r * quat.j - i * quat.k + j * quat.r + k * quat.i,
		r * quat.k + i * quat.j - j * quat.i + k * quat.r
	);
}

maths::unit_quaternion maths::unit_quaternion::from_axis_angle(maths::vec3f axis, float angle)
{
	return unit_quaternion(cosf(angle / 2), axis.x * sinf(angle / 2), axis.y * sinf(angle / 2), axis.z * sinf(angle / 2)).normalise();
}

maths::mat4f maths::unit_quaternion::to_rotation_matrix()
{
	return mat4f(
		1 - 2 * (j * j + k * k), 2 * (i * j - k * r), 2 * (i * k + j * r), 0,
		2 * (i * j + k * r), 1 - 2 * (i * i + k * k), 2 * (j * k - i * r), 0,
		2 * (i * k - j * r), 2 * (j * k + i * r), 1 - 2 * (i * i + j * j), 0,
		0, 0, 0, 1
	);
}

maths::unit_quaternion maths::unit_quaternion::normalise()
{
	float magnitude = sqrtf(r * r + i * i + j * j + k * k);
	return unit_quaternion(r / magnitude, i / magnitude, j / magnitude, k / magnitude);
}

maths::unit_quaternion maths::unit_quaternion::conjugate()
{
	return unit_quaternion(r, -i, -j, -k);
}