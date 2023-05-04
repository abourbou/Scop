
#pragma once

#include <cmath>

#include "Matrix.hpp"

// Object transformation

template<typename scal>
Matrix<scal, 4, 4> Translation(scal x, scal y, scal z);
template<typename scal>
Matrix<scal, 4, 4>	Rx(scal angle);
template<typename scal>
Matrix<scal, 4, 4>	Ry(scal angle);
template<typename scal>
Matrix<scal, 4, 4>	Rz(scal angle);

// Camera transformation
template<typename scal>
Matrix<scal, 4, 4>	ViewMatrix(const Vector3<scal>& eye, const Vector3<scal>& center, const Vector3<scal>& headUp);
template<typename scal>
Matrix<scal, 4, 4>	PerspectiveProj(scal fov, scal screenRatio, scal zNear, scal zFar);

// Utils
template<typename scal, size_t N>
Matrix<scal, 4, N>	CartToHomog(const Matrix<scal, 3, N>& mat);

template<typename scal, size_t N>
Matrix<scal, 3, N>	HomogToCart(const Matrix<scal, 4, N>& mat);

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
// *********************** Utils ******************
// ************************************************
template<typename scal, size_t N>
Matrix<scal, 4, N>	CartToHomog(const Matrix<scal, 3, N>& mat)
{
	Matrix<scal, 4, N> result;

	for (size_t i = 0; i < 3; i++)
		for (size_t j = 0; j < N; j++)
			result(i,j) = mat(i,j);
	result(3, N - 1) = 1;

	return result;
}

template<typename scal, size_t N>
Matrix<scal, 3, N>	HomogToCart(const Matrix<scal, 4, N>& mat)
{
	Matrix<scal, 3, N> result;
	scal w_1 = 1. / mat(3, N - 1);

	for (size_t i = 0; i < 3; i++)
		for (size_t j = 0; j < N; j++)
			result(i,j) = mat(i,j) * w_1;

	return result;
}
