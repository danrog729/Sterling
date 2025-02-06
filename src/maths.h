#ifndef STERLING_MATHS_H
#define STERLING_MATHS_H

namespace maths
{
	struct vec3f
	{
		float x;
		float y;
		float z;
	};

	struct vec4f
	{
		float x;
		float y;
		float z;
		float w;
	};

	struct mat4f
	{
		float m11; float m12; float m13; float m14;
		float m21; float m22; float m23; float m24;
		float m31; float m32; float m33; float m34;
		float m41; float m42; float m43; float m44;

		// Constructor
		mat4f(
			float m11, float m12, float m13, float m14,
			float m21, float m22, float m23, float m24,
			float m31, float m32, float m33, float m34,
			float m41, float m42, float m43, float m44
		);

		// Unary operators
		mat4f operator+(mat4f &matrix);
		mat4f operator-(mat4f &matrix);
	};
}

#endif