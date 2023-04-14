#pragma once

#include <array>
#include <type_traits>
#include <cstring>
#include <cmath>

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
	using matrixType = Matrix<scal, M, N>;
	template <size_t _M, size_t _N>
	using matScal = Matrix<scal, _M, _N>;

	static_assert(std::is_arithmetic<scal>::value, "Matrix val is not an arithmetic type");
	static_assert(M != 0 && N != 0, "Invalid Matrix size");

	//		Exception class
	class MatrixException : public std::exception
	{
		public:
			MatrixException() : Msg("Matrix Error"){}
			MatrixException(std::string msg) : Msg(std::string("Matrix Error : ") + msg){}
			virtual const char *what(void) const throw()
			{
				return this->Msg.c_str();
			}

		private:
			std::string Msg;
	};

	public:
		Matrix(scal val = 0);
		Matrix(const matrixType& rhs){*this = rhs;}

		// Accessibility operators
		scal	operator[](size_t i) const;
		scal&	operator[](size_t i);
		scal	operator()(size_t i, size_t j) const;
		scal&	operator()(size_t i, size_t j);
		matrixType&	operator=(const matrixType& rhs);

		// Matrix arithmetic
		scal		dotProduct(const matrixType& rhs) const;
		matrixType	crossProduct(const matrixType& rhs) const;
		scal		euclidNorm() const;
		void		normalize();
};

// Basic arithmetic
template <typename scal, size_t M, size_t N>
Matrix<scal, M, N>	operator+(const Matrix<scal, M, N>& lhs, const Matrix<scal, M, N>& rhs);
template <typename scal, size_t M, size_t N>
Matrix<scal, M, N>	operator-(const Matrix<scal, M, N>& lhs, const Matrix<scal, M, N>& rhs);
template <typename scal, size_t M, size_t N>
Matrix<scal, M, N> operator*(scal val, const Matrix<scal, M, N>& mat);
template <typename scal, size_t M, size_t N>
Matrix<scal, M, N> operator*(const Matrix<scal, M, N>& mat, scal val);

// Matrix arithmetic
template <typename scal, size_t M, size_t N, size_t P>
Matrix<scal, M, P> operator*(const Matrix<scal, M, N>& lhs, const Matrix<scal, N, P>& rhs);
template <typename scal, size_t M, size_t N>
Matrix<scal, N, M> transpose(const Matrix<scal, M, N>& mat);

// Utils
template <typename scal, size_t M, size_t N>
std::ostream& operator<<(std::ostream& os, const Matrix<scal, M, N>& mat);

// ************************************************
// *************** Access operators ***************
// ************************************************
template <typename scal, size_t M, size_t N>
Matrix<scal, M, N>::Matrix(scal val)
{
	for (size_t i = 0; i < M; ++i)
		for (size_t j = 0; j < N; ++j)
			(*this)(i, j) = val;
}

template <typename scal, size_t M, size_t N>
scal	Matrix<scal, M, N>::operator[](size_t i) const
{
	if (N != 1)
		throw MatrixException("[] operator is for vector only");
	return (*this)(i, 0);
}

template <typename scal, size_t M, size_t N>
scal&	Matrix<scal, M, N>::operator[](size_t i)
{
	if (N != 1)
		throw MatrixException("[] operator is for vector only");
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

template <typename scal, size_t M, size_t N>
Matrix<scal, M, N>&	Matrix<scal, M, N>::operator=(const matrixType& rhs)
{
	for (size_t i = 0; i < M; ++i)
		for (size_t j = 0; j < N; ++j)
			(*this)(i, j) = rhs(i, j);
	return *this;
}

// ************************************************
// *************** Basic Arithmetic ***************
// ************************************************
template <typename scal, size_t M, size_t N>
Matrix<scal, M, N>	operator+(const Matrix<scal, M, N>& lhs, const Matrix<scal, M, N>& rhs)
{
	Matrix<scal, M, N> result(lhs);

	for (size_t i = 0; i < M; ++i)
		for (size_t j = 0; j < N; ++j)
			result(i, j) += rhs(i, j);

	return result;
}

template <typename scal, size_t M, size_t N>
Matrix<scal, M, N>	operator-(const Matrix<scal, M, N>& lhs, const Matrix<scal, M, N>& rhs)
{
	Matrix<scal, M, N> result(lhs);

	for (size_t i = 0; i < M; ++i)
		for (size_t j = 0; j < N; ++j)
			result(i, j) -= rhs(i, j);

	return result;
}

template <typename scal, size_t M, size_t N>
Matrix<scal, M, N> operator*(scal val, const Matrix<scal, M, N>& mat)
{
	Matrix<scal, M, N> result(mat);

	for (size_t i = 0; i < M; ++i)
		for (size_t j = 0; j < N; ++j)
			result(i, j) *= val;

	return result;
}

template <typename scal, size_t M, size_t N>
Matrix<scal, M, N> operator*(const Matrix<scal, M, N>& mat, scal val)
{
	return val * mat;
}

// ************************************************
// *************** Matrix Arithmetic **************
// ************************************************
template <typename scal, size_t M, size_t N>
scal	Matrix<scal, M, N>::dotProduct(const Matrix<scal, M, N>& rhs) const
{
	scal val;

	if (N != 1)
		throw MatrixException("Invalid matrix for dot product");
	for (size_t i = 0; i < M; ++i)
	{
		if (i == 0)
			val = (*this)[i] * rhs[i];
		else
			val += (*this)[i] * rhs[i];
	}
	return val;
}

template <typename scal, size_t M, size_t N>
Matrix<scal, M, N>	Matrix<scal, M, N>::crossProduct(const Matrix<scal, M, N>& rhs) const
{
	Matrix<scal, M, N>	result;

	if (M != 3 && N != 1)
		throw MatrixException("Invalid matrix for cross product");

	result[0] = (*this)[1] * rhs[2] - (*this)[2] * rhs[1];
	result[1] = (*this)[2] * rhs[0] - (*this)[0] * rhs[2];
	result[2] = (*this)[0] * rhs[1] - (*this)[1] * rhs[0];

	return result;
}

template <typename scal, size_t M, size_t N>
scal		Matrix<scal, M, N>::euclidNorm() const
{
	scal	result = 0;

	for (size_t i = 0; i < M; ++i)
		for (size_t j = 0; j < N; ++j)
			result += (*this)(i, j) * (*this)(i, j);

	return (std::sqrt(result));
}

template <typename scal, size_t M, size_t N>
void		Matrix<scal, M, N>::normalize()
{
	scal norm_1 = 1. / this->euclidNorm();
	*this = norm_1 * *this;
}


template <typename scal, size_t M, size_t N, size_t P>
Matrix<scal, M, P> operator*(const Matrix<scal, M, N>& lhs, const Matrix<scal, N, P>& rhs)
{
	Matrix<scal, M, P> mat;

	for (size_t i = 0; i < M; ++i)
		for (size_t j = 0; j < P; ++j)
			for (size_t k = 0; k < N; ++k)
				mat(i, j) += lhs(i, k) * rhs(k, j);

	return mat;
}

template <typename scal, size_t M, size_t N>
Matrix<scal, N, M> transpose(const Matrix<scal, M, N>& mat)
{
	Matrix<scal, N, M> transposed;

	for (size_t i = 0; i != M; ++i)
		for (size_t j = 0; j != N; ++j)
			transposed(j, i) = mat(i, j);

	return transposed;
}

// ************************************************
// ******************** Utils *********************
// ************************************************
template <typename scal, size_t M, size_t N>
std::ostream& operator<<(std::ostream& os, const Matrix<scal, M, N>& mat)
{
	for (size_t i = 0; i < M; ++i)
	{
		os << "[";
		for (size_t j = 0; j < N; ++j)
		{
			os << mat(i, j);
			if (j != N - 1)
				os << ", ";
		}
		os << "]";
		if (i != M - 1)
			os << std::endl;
	}
	return os;
}