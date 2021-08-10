#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <format>

#include "vec.h"
#include "gmath.h"

namespace gmath
{
	template<typename T, size_t N, size_t M>
	class matrix
	{
	public:
		matrix() : elements{} {};
		matrix(T elements[N * M])
		{
			std::copy(elements, elements + N * M, this->elements);
		}
		matrix(const matrix<T, N, M>& mat)
		{
			std::copy(mat.elements, mat.elements + N * M, elements);
		}
		matrix(const T& diagonal)
			: elements{}
		{
			for (size_t i = 0; i < N; i++)
			{
				for (size_t j = 0; j < M; j++)
				{
					if (i == j)
						rows[i][j] = diagonal;
				}
			}
		}

		template<typename U>
		operator matrix<U, N, M>() const
		{
			matrix<U, N, M> mat{};
			for (size_t i = 0; i < size(); i++)
			{
				mat[i] = static_cast<U>(elements[i]);
			}
			return mat;
		}

		matrix<T, N, M>& operator=(const matrix<T, N, M>& mat)
		{
			std::copy(mat.elements, mat.elements + N * M, elements);
			return *this;
		}

		union
		{
			T elements[N * M];
			vec<T, M> rows[N];
		};

		T& operator[](const size_t i)
		{
			return elements[i];
		}

		const T& operator[](const size_t i) const
		{
			return elements[i];
		}

		void zero()
		{
			std::fill(std::begin(elements), std::end(elements), T{});
		}

		void randomize(const T& min = 0.0, const T& max = 1.0)
		{
			for (size_t i = 0; i < size(); i++)
			{
				elements[i] = gmath::random<T>(min, max);
			}
		}

		size_t size() const
		{
			return N * M;
		}

		std::string to_string() const
		{
			std::stringstream ss;
			ss << this;
			return ss.str();
		}

		matrix<T, M, N> transpose() const
		{
			matrix<T, M, N> result{};
			for (size_t i = 0; i < N; i++)
			{
				for (size_t j = 0; j < M; j++)
				{
					result.rows[j][i] = rows[i][j];
				}
			}
			return result;
		}

		static matrix<T, N, M> identity()
		{
			matrix<T, N, M> mat{ 1.0 };
			return mat;
		}

		static matrix<T, 4, 4> orthographic(const T& left, const T& right, const T& bottom, const T& top, const T& near, const T& far)
		{
			matrix<T, 4, 4> result{ 1.0 };
			result.elements[0 + 0 * 4] = 2.0 / (right - left);
			result.elements[1 + 1 * 4] = 2.0 / (top - bottom);
			result.elements[2 + 2 * 4] = 2.0 / (near - far);
			result.elements[0 + 3 * 4] = (left + right) / (left - right);
			result.elements[1 + 3 * 4] = (bottom + top) / (bottom - top);
			result.elements[2 + 3 * 4] = (far + near) / (far - near);
			return result;
		}

		static matrix<T, 4, 4> perspective(const T& fov, const T& aspectRatio, const T& near, const T& far)
		{
			matrix<T, 4, 4> result{ 1.0 };
			T q = 1.0 / tan(deg_to_rad<T>(0.5 * fov));
			T a = q / aspectRatio;
			T b = (near + far) / (near - far);
			T c = (2.0 * near * far) / (near - far);

			result.elements[0 + 0 * 4] = a;
			result.elements[1 + 1 * 4] = q;
			result.elements[2 + 2 * 4] = b;
			result.elements[3 + 2 * 4] = -1.0;
			result.elements[2 + 3 * 4] = c;
			return result;
		}

		static matrix<T, 4, 4> translation(const vec<T, 3>& translation)
		{
			matrix<T, 4, 4> result{ 1.0 };
			result.elements[0 + 3 * 4] = translation.x;
			result.elements[1 + 3 * 4] = translation.y;
			result.elements[2 + 3 * 4] = translation.z;
			return result;
		}

		static matrix<T, 4, 4> rotation(const T& angle, const vec<T, 3>& axis)
		{
			matrix<T, 4, 4> result{ 1.0 };
			T r = deg_to_rad(angle);
			T c = gmath::cos(r);
			T s = gmath::sin(r);
			T omc = 1.0 - c;
			result.elements[0 + 0 * 4] = axis.x * omc + c;
			result.elements[1 + 0 * 4] = axis.y * axis.x * omc + axis.z * s;
			result.elements[2 + 0 * 4] = axis.z * axis.x * omc - axis.y * s;
			result.elements[0 + 1 * 4] = axis.x * axis.y * omc - axis.z * s;
			result.elements[1 + 1 * 4] = axis.y * omc + c;
			result.elements[2 + 1 * 4] = axis.y * axis.z * omc + axis.x * s;
			result.elements[0 + 2 * 4] = axis.x * axis.z * omc + axis.y * s;
			result.elements[1 + 2 * 4] = axis.y * axis.z * omc - axis.x * s;
			result.elements[2 + 2 * 4] = axis.z * omc + c;
			return result;
		}

		static matrix<T, 4, 4> scale(const vec<T, 3>& scale) {
			matrix<T, 4, 4> result{ 1.0 };
			result.elements[0 + 0 * 4] = scale.x;
			result.elements[1 + 1 * 4] = scale.y;
			result.elements[2 + 2 * 4] = scale.z;
			return result;
		}

		static matrix<T, 4, 4> inverse(matrix<T, 4, 4>& mat)
		{
			matrix<T, 4, 4> result{};

			result.elements[0] = mat.elements[5] * mat.elements[10] * mat.elements[15] -
				mat.elements[5] * mat.elements[11] * mat.elements[14] -
				mat.elements[9] * mat.elements[6] * mat.elements[15] +
				mat.elements[9] * mat.elements[7] * mat.elements[14] +
				mat.elements[13] * mat.elements[6] * mat.elements[11] -
				mat.elements[13] * mat.elements[7] * mat.elements[10];

			result.elements[4] = -mat.elements[4] * mat.elements[10] * mat.elements[15] +
				mat.elements[4] * mat.elements[11] * mat.elements[14] +
				mat.elements[8] * mat.elements[6] * mat.elements[15] -
				mat.elements[8] * mat.elements[7] * mat.elements[14] -
				mat.elements[12] * mat.elements[6] * mat.elements[11] +
				mat.elements[12] * mat.elements[7] * mat.elements[10];

			result.elements[8] = mat.elements[4] * mat.elements[9] * mat.elements[15] -
				mat.elements[4] * mat.elements[11] * mat.elements[13] -
				mat.elements[8] * mat.elements[5] * mat.elements[15] +
				mat.elements[8] * mat.elements[7] * mat.elements[13] +
				mat.elements[12] * mat.elements[5] * mat.elements[11] -
				mat.elements[12] * mat.elements[7] * mat.elements[9];

			result.elements[12] = -mat.elements[4] * mat.elements[9] * mat.elements[14] +
				mat.elements[4] * mat.elements[10] * mat.elements[13] +
				mat.elements[8] * mat.elements[5] * mat.elements[14] -
				mat.elements[8] * mat.elements[6] * mat.elements[13] -
				mat.elements[12] * mat.elements[5] * mat.elements[10] +
				mat.elements[12] * mat.elements[6] * mat.elements[9];

			result.elements[1] = -mat.elements[1] * mat.elements[10] * mat.elements[15] +
				mat.elements[1] * mat.elements[11] * mat.elements[14] +
				mat.elements[9] * mat.elements[2] * mat.elements[15] -
				mat.elements[9] * mat.elements[3] * mat.elements[14] -
				mat.elements[13] * mat.elements[2] * mat.elements[11] +
				mat.elements[13] * mat.elements[3] * mat.elements[10];

			result.elements[5] = mat.elements[0] * mat.elements[10] * mat.elements[15] -
				mat.elements[0] * mat.elements[11] * mat.elements[14] -
				mat.elements[8] * mat.elements[2] * mat.elements[15] +
				mat.elements[8] * mat.elements[3] * mat.elements[14] +
				mat.elements[12] * mat.elements[2] * mat.elements[11] -
				mat.elements[12] * mat.elements[3] * mat.elements[10];

			result.elements[9] = -mat.elements[0] * mat.elements[9] * mat.elements[15] +
				mat.elements[0] * mat.elements[11] * mat.elements[13] +
				mat.elements[8] * mat.elements[1] * mat.elements[15] -
				mat.elements[8] * mat.elements[3] * mat.elements[13] -
				mat.elements[12] * mat.elements[1] * mat.elements[11] +
				mat.elements[12] * mat.elements[3] * mat.elements[9];

			result.elements[13] = mat.elements[0] * mat.elements[9] * mat.elements[14] -
				mat.elements[0] * mat.elements[10] * mat.elements[13] -
				mat.elements[8] * mat.elements[1] * mat.elements[14] +
				mat.elements[8] * mat.elements[2] * mat.elements[13] +
				mat.elements[12] * mat.elements[1] * mat.elements[10] -
				mat.elements[12] * mat.elements[2] * mat.elements[9];

			result.elements[2] = mat.elements[1] * mat.elements[6] * mat.elements[15] -
				mat.elements[1] * mat.elements[7] * mat.elements[14] -
				mat.elements[5] * mat.elements[2] * mat.elements[15] +
				mat.elements[5] * mat.elements[3] * mat.elements[14] +
				mat.elements[13] * mat.elements[2] * mat.elements[7] -
				mat.elements[13] * mat.elements[3] * mat.elements[6];

			result.elements[6] = -mat.elements[0] * mat.elements[6] * mat.elements[15] +
				mat.elements[0] * mat.elements[7] * mat.elements[14] +
				mat.elements[4] * mat.elements[2] * mat.elements[15] -
				mat.elements[4] * mat.elements[3] * mat.elements[14] -
				mat.elements[12] * mat.elements[2] * mat.elements[7] +
				mat.elements[12] * mat.elements[3] * mat.elements[6];

			result.elements[10] = mat.elements[0] * mat.elements[5] * mat.elements[15] -
				mat.elements[0] * mat.elements[7] * mat.elements[13] -
				mat.elements[4] * mat.elements[1] * mat.elements[15] +
				mat.elements[4] * mat.elements[3] * mat.elements[13] +
				mat.elements[12] * mat.elements[1] * mat.elements[7] -
				mat.elements[12] * mat.elements[3] * mat.elements[5];

			result.elements[14] = -mat.elements[0] * mat.elements[5] * mat.elements[14] +
				mat.elements[0] * mat.elements[6] * mat.elements[13] +
				mat.elements[4] * mat.elements[1] * mat.elements[14] -
				mat.elements[4] * mat.elements[2] * mat.elements[13] -
				mat.elements[12] * mat.elements[1] * mat.elements[6] +
				mat.elements[12] * mat.elements[2] * mat.elements[5];

			result.elements[3] = -mat.elements[1] * mat.elements[6] * mat.elements[11] +
				mat.elements[1] * mat.elements[7] * mat.elements[10] +
				mat.elements[5] * mat.elements[2] * mat.elements[11] -
				mat.elements[5] * mat.elements[3] * mat.elements[10] -
				mat.elements[9] * mat.elements[2] * mat.elements[7] +
				mat.elements[9] * mat.elements[3] * mat.elements[6];

			result.elements[7] = mat.elements[0] * mat.elements[6] * mat.elements[11] -
				mat.elements[0] * mat.elements[7] * mat.elements[10] -
				mat.elements[4] * mat.elements[2] * mat.elements[11] +
				mat.elements[4] * mat.elements[3] * mat.elements[10] +
				mat.elements[8] * mat.elements[2] * mat.elements[7] -
				mat.elements[8] * mat.elements[3] * mat.elements[6];

			result.elements[11] = -mat.elements[0] * mat.elements[5] * mat.elements[11] +
				mat.elements[0] * mat.elements[7] * mat.elements[9] +
				mat.elements[4] * mat.elements[1] * mat.elements[11] -
				mat.elements[4] * mat.elements[3] * mat.elements[9] -
				mat.elements[8] * mat.elements[1] * mat.elements[7] +
				mat.elements[8] * mat.elements[3] * mat.elements[5];

			result.elements[15] = mat.elements[0] * mat.elements[5] * mat.elements[10] -
				mat.elements[0] * mat.elements[6] * mat.elements[9] -
				mat.elements[4] * mat.elements[1] * mat.elements[10] +
				mat.elements[4] * mat.elements[2] * mat.elements[9] +
				mat.elements[8] * mat.elements[1] * mat.elements[6] -
				mat.elements[8] * mat.elements[2] * mat.elements[5];

			T determinant = mat.elements[0] * result.elements[0] + mat.elements[1] * result.elements[4] + mat.elements[2] * result.elements[8] + mat.elements[3] * result.elements[12];
			determinant = 1.0 / determinant;
			result *= determinant;

			return result;
		}
	};

	template<typename T, typename U, size_t N, size_t M>
	auto operator*(const matrix<T, N, M>& a, const matrix<U, M, N>& b)
		-> matrix<decltype(a[0] * b[0]), N, N>
	{
		matrix<decltype(a[0] * b[0]), N, N> result{};
		for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < N; j++)
			{
				T sum{};
				for (size_t k = 0; k < M; k++)
				{
					sum += a.rows[i][k] * b.rows[k][j];
				}
				result.rows[i][j] = sum;
			}
		}
		return result;
	}

	template<typename T, typename U, size_t N, size_t M>
	auto operator*=(matrix<T, N, M>& a, const U& b)
		-> matrix<decltype(a[0] * b), N, M>&
	{
		for (size_t i = 0; i < a.size(); i++)
		{
			a[i] *= b;
		}
		return a;
	}

	template<typename T, typename U, size_t N, size_t M>
	auto operator*(const matrix<T, N, M>& a, const U& b)
		-> matrix<decltype(a[0] * b), N, M>
	{
		matrix<decltype(a[0] * b), N, M> result{ a };
		result *= b;
		return result;
	}

	template<typename T, typename U, size_t N, size_t M>
	auto operator/=(matrix<T, N, M>& a, const U& b)
		-> matrix<decltype(a[0] * b), N, M>&
	{
		for (size_t i = 0; i < a.size(); i++)
		{
			a[i] /= b;
		}
		return a;
	}

	template<typename T, typename U, size_t N, size_t M>
	auto operator/(const matrix<T, N, M>& a, const U& b)
		-> matrix<decltype(a[0] * b), N, M>
	{
		matrix<decltype(a[0] * b), N, M> result{ a };
		result /= b;
		return result;
	}

	template <typename T, typename U, size_t N, size_t M>
	bool operator==(const matrix<T, N, M>& a, const matrix<U, N, M>& b)
	{
		for (size_t i = 0; i < N * M; i++)
			if (static_cast<decltype(a[0] * b[0])>(a[i]) != static_cast<decltype(a[0] * b[0])>(b[i]))
				return false;
		return true;
	}

	template <typename T, typename U, size_t N, size_t M>
	bool operator!=(const matrix<T, N, M>& a, const matrix<U, N, M>& b)
	{
		return !(a == b);
	}

	template<typename T, size_t N, size_t M>
	std::ostream& operator<<(std::ostream& stream, const matrix<T, N, M>& mat)
	{
		stream << "matrix" << N << "x" << M << "\n";
		for (size_t i = 0; i < N; i++)
		{
			stream << "(";
			for (size_t j = 0; j < M; j++)
			{
				stream << std::format("{:3}", mat.rows[i][j]);
				if (j + 1 < M)
					stream << ", ";
			}
			stream << ")";
			if (i + 1 < N)
				stream << "\n";
		}
		return stream;
	}

	// Most commonly used matrix
	using mat4 = matrix<float, 4, 4>;
	using mat4_precise = matrix<double, 4, 4>;

	// Template alias for readability
	template<size_t N, size_t M>
	using mat = matrix<float, N, M>;
	template<size_t N, size_t M>
	using mat_precise = matrix<double, N, M>;
}