#pragma once

#include <array>
#include <type_traits>
#include <cstring>


template <typename scal, size_t M, size_t N>
class Matrix;

template <typename scal, size_t M>
using Vector = Matrix<scal, M, 1>;

using Vector3d = Vector<double, 3>;
using Matrix3d = Matrix<double, 3, 3>;

using Vector4d = Vector<double, 4>;
using Matrix4d = Matrix<double, 4, 4>;

// ************************************************
// ***************** Matrix Class *****************
// ************************************************
template <typename scal, size_t M, size_t N>
class Matrix : public std::array<std::array<scal, N>, M>
{
	using arrayType = std::array<std::array<scal, N>, M>;
	template <size_t _M, size_t _N>
	using matScal = Matrix<scal, _M, _N>;

	static_assert(std::is_arithmetic<scal>::value, "Matrix val is not an arithmetic type");
	static_assert(M != 0 && N != 0, "Invalid Matrix size");

	public:
		Matrix(scal val = 0)
		{
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					(*this)(i, j) = val;
		}

		template <size_t P>
		matScal<M, P> operator*(const matScal<N, P>& mat);

		scal	operator[](size_t i) const;
		scal&	operator[](size_t i);
		scal	operator()(size_t i, size_t j) const;
		scal&	operator()(size_t i, size_t j);

};

template <typename scal, size_t M, size_t N>
template <size_t P>
Matrix<scal, M, P> Matrix<scal, M, N>::operator*(const Matrix<scal, N, P>& rhs)
{
	matScal<M, P> mat;

	for (size_t i = 0; i < M; ++i)
		for (size_t j = 0; j < P; ++j)
			for (size_t k = 0; k < N; ++k)
				mat(i, j) += (*this)(i, k) * rhs(k, j);

	return (mat);
}


// ************************************************
// *************** Access operators ***************
// ************************************************
template <typename scal, size_t M, size_t N>
scal	Matrix<scal, M, N>::operator[](size_t i) const
{
	return (*this)(i, 0);
}

template <typename scal, size_t M, size_t N>
scal&	Matrix<scal, M, N>::operator[](size_t i)
{
	return (*this)(i, 0);
}

template <typename scal, size_t M, size_t N>
scal	Matrix<scal, M, N>::operator()(size_t i, size_t j) const
{
	return (*this).at(i).at(j);
}

template <typename scal, size_t M, size_t N>
scal&	Matrix<scal, M, N>::operator()(size_t i, size_t j)
{
	return (*this).at(i).at(j);
}