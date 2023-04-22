
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
//! Warning : rotAxis MUST be normalized
template<typename scal>
Matrix<scal, 4, 4>	Rotation(scal angle, const Matrix<scal, 3, 1>& rotAxis)
{
	scal	cosAng = cos(angle * M_PI / 180.);
	scal	sinAng = sin(angle * M_PI / 180.);
	Matrix<scal, 3, 3> result3d = cosAng * Matrix<scal, 3, 3>::Identity();

	result3d += (1 - cosAng) * outerProduct(rotAxis, rotAxis);
	result3d += sinAng * SkewSymmetricCrossProduct(rotAxis);

	auto result = Matrix<scal, 4,4>::Identity();
	for (size_t i = 0; i < 3; ++i)
		for (size_t j = 0; j < 3; ++j)
			result(i,j) = result3d(i,j);

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
	result(1,0) = rightAxis[1];
	result(2,0) = rightAxis[2];

	result(0,1) = upAxis[0];
	result(1,1) = upAxis[1];
	result(2,1) = upAxis[2];

	result(0,2) = forward[0];
	result(1,2) = forward[1];
	result(2,2) = forward[2];

	result(3,0) = -rightAxis.dotProduct(eye);
	result(3,1) = -upAxis.dotProduct(eye);
	result(3,2) = -forward.dotProduct(eye);

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
	result(2, 3) = - 1;
	result(3, 2) = (-2 * zFar * zNear) / (zFar - zNear);

	return result;
}

// ************************************************
// ********************** Utilities ***************
// ************************************************
template<typename scal, size_t M, size_t N>
Matrix<scal, M, N>	outerProduct(const Matrix<scal,M,1>& m, const Matrix<scal,N,1>& n)
{
	Matrix<scal,M,N> result;

	std::cout << "start of " << __FUNCTION__ << std::endl;
	std::cout << M << " " << N << std::endl;
	std::cout << "result : " << result << std::endl;
	for (size_t i = 0; i < M; ++i)
		for (size_t j = 0; j < N; ++j)
			result(i,j) = m[i] * n[j];
	std::cout << "end of " << __FUNCTION__ << std::endl;

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



