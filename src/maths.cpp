#include "maths.h"

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

maths::mat4f maths::mat4f::operator+(mat4f &matrix)
{
	return matrix;
}

maths::mat4f maths::mat4f::operator-(mat4f &matrix)
{
	return mat4f(
		-matrix.m11, -matrix.m12, -matrix.m13, -matrix.m14,
		-matrix.m21, -matrix.m22, -matrix.m23, -matrix.m24,
		-matrix.m31, -matrix.m32, -matrix.m33, -matrix.m34,
		-matrix.m41, -matrix.m42, -matrix.m43, -matrix.m44
	);
}