
#pragma once

#include <cmath>

#include "Matrix.hpp"

// Object transformation
template<typename scal>
Matrix<scal, 4, 4> Translation(scal x, scal y, scal z);
template<typename scal>
Matrix<scal, 4, 4>	Rotation(scal angle, const Matrix<scal,3,1>& rotAxis);

// Camera transformation
template<typename scal>
Matrix<scal, 4, 4>	ViewMatrix(const Vector3<scal>& eye, const Vector3<scal>& center, const Vector3<scal>& headUp);
template<typename scal>
Matrix<scal, 4, 4>	PerspectiveProj(scal fov, scal screenRatio, scal zNear, scal zFar);

// Utils
template<typename scal, size_t M, size_t N>
Matrix<scal, M, N>	outerProduct(const Matrix<scal,M,1>& m, const Matrix<scal,N,1>& n);
template<typename scal>
Matrix<scal, 3, 3>	SkewSymmetricCrossProduct(const Matrix<scal,3,1>& vec);

// ************************************************
// **************** Object Transformation *********
// ************************************************
template<typename scal>
Matrix<scal, 4, 4> Translation(scal x, scal y, scal z)
{
	Matrix<scal, 4, 4>	result = Matrix<scal, 4, 4>::Identity();

	result(0, 3) = x;
	result(1, 3) = y;
	result(2, 3) = z;

	return result;
}

// Angle in degree
template<typename scal>
Matrix<scal, 4, 4>	Rx(scal angle)
{
	Matrix<scal,4,4> result = Matrix<scal, 4,4>::Identity();

	scal cosAngle = cos(angle * M_PI / 180.);
	scal sinAngle = sin(angle * M_PI / 180.);

	result(1,1) = cosAngle;
	result(2,2) = cosAngle;
	result(1,2) = -1. * sinAngle;
	result(2,1) = sinAngle;

	return result;
}

// Angle in degree
template<typename scal>
Matrix<scal, 4, 4>	Ry(scal angle)
{
	Matrix<scal,4,4> result = Matrix<scal, 4,4>::Identity();

	scal cosAngle = cos(angle * M_PI / 180.);
	scal sinAngle = sin(angle * M_PI / 180.);

	result(0,0) = cosAngle;
	result(2,2) = cosAngle;
	result(2,0) = -1. * sinAngle;
	result(0,2) = sinAngle;

	return result;
}

// Angle in degree
template<typename scal>
Matrix<scal, 4, 4>	Rz(scal angle)
{
	Matrix<scal,4,4> result = Matrix<scal, 4,4>::Identity();

	scal cosAngle = cos(angle * M_PI / 180.);
	scal sinAngle = sin(angle * M_PI / 180.);

	result(0,0) = cosAngle;
	result(1,1) = cosAngle;
	result(0,1) = -1. * sinAngle;
	result(1,0) = sinAngle;

	return result;
}

// ************************************************
// ************* Projection Transformation ********
// ************************************************
template<typename scal>
Matrix<scal, 4, 4>	ViewMatrix(const Vector3<scal>& eye, const Vector3<scal>& center, const Vector3<scal>& headUp)
{
	Vector3<scal> forward = (eye - center).normalize();
	Vector3<scal> rightAxis = headUp.crossProduct(forward).normalize();
	Vector3<scal> upAxis = forward.crossProduct(rightAxis);
	Matrix<scal,4,4> result = Matrix<scal,4,4>::Identity();

	result(0,0) = rightAxis[0];
	result(0,1) = rightAxis[1];
	result(0,2) = rightAxis[2];
	result(1,0) = upAxis[0];
	result(1,1) = upAxis[1];
	result(1,2) = upAxis[2];
	result(2,0) = forward[0];
	result(2,1) = forward[1];
	result(2,2) = forward[2];
	result(0,3) = -rightAxis.dotProduct(eye);
	result(1,3) = -upAxis.dotProduct(eye);
	result(2,3) = -forward.dotProduct(eye);

	return result;
}

// fov in degree
template<typename scal>
Matrix<scal, 4, 4>	PerspectiveProj(scal fov, scal screenRatio, scal zNear, scal zFar)
{
	scal tanHalfFov = tan(fov * M_PI / 360);
	Matrix<scal,4,4>	result;

	result(0, 0) = 1 / (screenRatio * tanHalfFov);
	result(1, 1) = 1 / tanHalfFov;
	result(2, 2) = - (zFar + zNear) / (zFar - zNear);
	result(3, 2) = - 1;
	result(2, 3) = (-2 * zFar * zNear) / (zFar - zNear);

	return result;
}

// ************************************************
// ********************** Utilities ***************
// ************************************************
template<typename scal, size_t M, size_t N>
Matrix<scal, M, N>	outerProduct(const Matrix<scal,M,1>& m, const Matrix<scal,N,1>& n)
{
	Matrix<scal,M,N> result;

	for (size_t i = 0; i < M; ++i)
		for (size_t j = 0; j < N; ++j)
			result(i,j) = m[i] * n[j];

	return result;
}

// https://en.wikipedia.org/wiki/Skew-symmetric_matrix#Cross_product
// | 0  -uz  uy|
// | uz  0  -ux|
// |-uy  ux  0 |
template<typename scal>
Matrix<scal, 3, 3>	SkewSymmetricCrossProduct(const Matrix<scal,3,1>& vec)
{
	Matrix<scal, 3, 3>	result;

	result(1,0) = vec[2];
	result(2,0) = -1 * vec[1];
	result(0,1) = -1 * vec[2];
	result(2,1) = vec[0];
	result(0,2) = vec[1];
	result(0,2) = -1 * vec[0];

	return result;
}



