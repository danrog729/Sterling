#ifndef STERLING_MATHS_H
#define STERLING_MATHS_H

namespace maths
{
	const float PI = 3.141592653589f;

	struct vec2f
	{
		float x, y;

		vec2f()
		{
			x = 0; y = 0;
		}
		vec2f(float X, float Y)
		{
			x = X;
			y = Y;
		}
	};

	struct vec3f
	{
		float x, y, z;

		vec3f()
		{
			x = 0; y = 0; z = 0;
		}
		vec3f(float X, float Y, float Z)
		{
			x = X;
			y = Y;
			z = Z;
		}

		static float dot(const vec3f vector1, const vec3f vector2);
		static vec3f cross(const vec3f vector1, const vec3f vector2);
		static vec3f normalise(const vec3f vector);

		vec3f operator*(float value);
		vec3f operator/(float value);
		vec3f operator+(const vec3f& vector);
		vec3f operator-(const vec3f& vector);
	};

	struct vec4f
	{
		float x, y, z, w;

		vec4f(float X, float Y, float Z, float W)
		{
			x = X;
			y = Y;
			z = Z;
			w = W;
		}

		static float dot(const vec4f vector1, const vec4f vector2);

		vec4f operator*(float value);
		vec4f operator+(const vec4f& vector);
		vec4f operator-(const vec4f& vector);
	};

	struct mat4f
	{
		float m11, m12, m13, m14;
		float m21, m22, m23, m24;
		float m31, m32, m33, m34;
		float m41, m42, m43, m44;

		mat4f();

		mat4f(
			float m11, float m12, float m13, float m14,
			float m21, float m22, float m23, float m24,
			float m31, float m32, float m33, float m34,
			float m41, float m42, float m43, float m44
		);

		/*
		Operator Overloads
		*/

		mat4f operator+();
		mat4f operator-();

		mat4f operator+(const mat4f& matrix);
		mat4f operator-(const mat4f& matrix);

		mat4f operator*(const mat4f& matrix);

		vec4f operator*(const vec4f& vector);

		static mat4f stretch_z(float stretch);

		static mat4f transpose(const mat4f matrix);
	};

	struct unit_quaternion
	{
		float r, i, j, k;

		unit_quaternion()
		{
			r = 1; i = 0; j = 0; k = 0;
		}
		unit_quaternion(float R, float I, float J, float K)
		{
			r = R;
			i = I;
			j = J;
			k = K;
		}

		unit_quaternion operator*(const unit_quaternion& quat);

		static unit_quaternion from_axis_angle(maths::vec3f axis, float angle);

		static unit_quaternion from_euler_angles(maths::vec3f angles);

		mat4f to_rotation_matrix();

		vec3f to_euler_angles();

		unit_quaternion normalise();

		unit_quaternion conjugate();
	};
}

#endif