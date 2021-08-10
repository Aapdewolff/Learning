#pragma once

#include <numeric>
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>

#include "gmath.h"

namespace gmath
{
	// Curiously Recurring Template Pattern for the base class of all vectors
	template<typename T, typename CRTP>
	class vector_base {
	public:
		CRTP& crtp() { return static_cast<CRTP&>(*this); }
		const CRTP& crtp() const { return static_cast<const CRTP&>(*this); }

		CRTP& operator=(const CRTP& vec)
		{
			std::copy(vec.data, vec.data + vec.size(), crtp().data);
			return crtp();
		}

		T& operator[](const size_t i)
		{
			return crtp().data[i];
		}

		const T& operator[](const size_t i) const
		{
			return crtp().data[i];
		}

		size_t size() const
		{
			return std::extent<decltype(CRTP::data)>::value;
		}

		T magnitude() const
		{
			T sum{};
			for (size_t i = 0; i < size(); i++)
				sum += crtp().data[i] * crtp().data[i];
			return gmath::sqrt<T>(sum);
		}

		T sqr_magnitude() const
		{
			T unsquared_magnitude = magnitude();
			return unsquared_magnitude * unsquared_magnitude;
		}

		void set_magnitude(const T& m)
		{
			normalize();
			if (m > 1.0)
				crtp() *= m;
		}

		CRTP normalized() const
		{
			CRTP vec_normalized{ crtp() };
			vec_normalized.normalize();
			return vec_normalized;
		}

		std::string to_string() const
		{
			std::stringstream ss;
			ss << crtp();
			return ss.str();
		}

		void normalize()
		{
			T m{ magnitude() };
			if (m > 0)
				crtp() /= m;
			else
				crtp().zero();
		}

		void zero()
		{
			std::fill(std::begin(crtp().data), std::end(crtp().data), T{});
		}

		void randomize(const T& min = 0.0, const T& max = 1.0)
		{
			for (size_t i = 0; i < size(); i++)
			{
				crtp().data[i] = gmath::random<T>(min, max);
			}
		}

		static T distance(const CRTP& a, const CRTP& b)
		{
			T sum{};
			for (size_t i = 0; i < a.size(); i++)
				sum += abs(a[i] - b[i]);
			return gmath::sqrt<T>(sum);
		}

		static CRTP lerp(const CRTP& a, const CRTP& b, const float& t)
		{
			CRTP result{};
			for (size_t i = 0; i < a.size(); i++)
				result[i] = gmath::lerp<T>(a[i], b[i], t);
			return result;
		}

		static CRTP lerp_unclamped(const CRTP& a, const CRTP& b, const float& t)
		{
			CRTP result{};
			for (size_t i = 0; i < a.size(); i++)
				result[i] = gmath::lerp_unclamped<T>(a[i], b[i], t);
			return result;
		}

		static CRTP clamp_magnitude(CRTP vec, const T& m)
		{
			if (vec.magnitude() > m)
				vec.set_magnitude(m);
			return vec;
		}

		static CRTP min(const CRTP& a, const CRTP& b)
		{
			CRTP result{};
			for (size_t i = 0; i < a.size(); i++)
				result[i] = gmath::min(a[i], b[i]);
			return result;
		}

		static CRTP max(const CRTP& a, const CRTP& b)
		{
			CRTP result{};
			for (size_t i = 0; i < a.size(); i++)
				result[i] = gmath::max(a[i], b[i]);
			return result;
		}

		static T dot(const CRTP& a, const CRTP& b)
		{
			T result{};
			for (size_t i = 0; i < a.size(); i++)
				result += a[i] * b[i];
			return result;
		}
	};

	/*
	* Template variants for different sizes of vectors.
	* Where vec2, vec3 and vec4 have xy(zw) properties for easy access.
	*/

	// Constructors and element properties for vectors with N > 4
	template <typename T, size_t N>
	struct vector : vector_base<T, vector<T, N>> {
		vector() = default;

		vector(T data[N])
		{
			std::copy(data, data + N, this->data);
		}

		vector(const vector<T, N>& vec)
		{
			std::copy(vec.data, vec.data + N, this->data);
		}

		union
		{
			T data[N];
			struct
			{
				T x, y, z, w;
			};
		};

		template<typename U>
		operator vector<U, N>() const
		{
			vector<U, N> vec{};
			for (size_t i = 0; i < N; i++)
			{
				vec[i] = static_cast<U>(data[i]);
			}
			return vec;
		}
	};

	// Constructors and element properties for vectors with N = 2
	template <typename T>
	struct vector<T, 2> : vector_base<T, vector<T, 2>> {
		vector() = default;

		vector(const T& x, const T& y)
			: x(x), y(y) {}

		vector(const vector<T, 2>& vec)
			: x(vec.x), y(vec.y) {}

		union
		{
			T data[2];
			struct
			{
				T x, y;
			};
		};

		template<typename U>
		operator vector<U, 2>() const
		{
			vector<U, 2> vec{};
			for (size_t i = 0; i < 2; i++)
			{
				vec[i] = static_cast<U>(data[i]);
			}
			return vec;
		}

		static inline const auto left() { return vector<T, 2>{ -1, 0 }; }
		static inline const auto right() { return vector<T, 2>{ 1, 0 }; }
		static inline const auto up() { return vector<T, 2>{ 0, 1 }; }
		static inline const auto down() { return vector<T, 2>{ 0, -1 }; }
	};

	// Constructors and element properties for vectors with N = 3
	template <typename T>
	struct vector<T, 3> : vector_base<T, vector<T, 3>> {
		vector() = default;

		vector(const T& x, const T& y, const T& z)
			: x(x), y(y), z(z) {}

		vector(const vector<T, 3>& vec)
			: x(vec.x), y(vec.y), z(vec.z) {}

		union
		{
			T data[3];
			struct
			{
				T x, y, z;
			};
		};

		template<typename U>
		operator vector<U, 3>() const
		{
			vector<U, 3> vec{};
			for (size_t i = 0; i < 3; i++)
			{
				vec[i] = static_cast<U>(data[i]);
			}
			return vec;
		}

		static inline const auto left() { return vector<T, 3>{ -1, 0, 0 }; }
		static inline const auto right() { return vector<T, 3>{ 1, 0, 0 }; }
		static inline const auto up() { return vector<T, 3>{ 0, 1, 0 }; }
		static inline const auto down() { return vector<T, 3>{ 0, -1, 0 }; }
		static inline const auto forward() { return vector<T, 3>{ 0, 0, 1 }; }
		static inline const auto back() { return vector<T, 3>{ 0, 0, -1 }; }
	};

	// Constructors and element properties for vectors with N = 4
	template <typename T>
	struct vector<T, 4> : vector_base<T, vector<T, 4>> {
		vector() = default;

		vector(const T& x, const T& y, const T& z, const T& w)
			: x(x), y(y), z(z), w(w) {}

		vector(const vector<T, 4>& vec)
			: x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}

		union
		{
			T data[4];
			struct
			{
				T x, y, z, w;
			};
		};

		template<typename U>
		operator vector<U, 4>() const
		{
			vector<U, 4> vec{};
			for (size_t i = 0; i < 4; i++)
			{
				vec[i] = static_cast<U>(data[i]);
			}
			return vec;
		}
	};

	/*
	* Cross type operators for vectors with the same size.
	* The return type is defined by the resulting type of element multiplication.
	*/

	template <typename T, typename U, size_t N>
	auto operator+(const vector<T, N>& a, const vector<U, N>& b)
		-> vector<decltype(a[0] * b[0]), N>
	{
		vector<decltype(a[0] * b[0]), N> result{};
		for (size_t i = 0; i < N; i++)
			result[i] = a[i] + b[i];
		return result;
	}

	template <typename T, typename U, size_t N>
	auto operator-(const vector<T, N>& a, const vector<U, N>& b)
		-> vector<decltype(a[0] * b[0]), N>
	{
		vector<decltype(a[0] * b[0]), N> result{};
		for (size_t i = 0; i < N; i++)
			result[i] = a[i] - b[i];
		return result;
	}

	template <typename T, typename U, size_t N>
	auto operator*(const vector<T, N>& a, const vector<U, N>& b)
		-> vector<decltype(a[0] * b[0]), N>
	{
		vector<decltype(a[0] * b[0]), N> result{};
		for (size_t i = 0; i < N; i++)
			result[i] = a[i] * b[i];
		return result;
	}

	template <typename T, typename U, size_t N>
	auto operator/(const vector<T, N>& a, const vector<U, N>& b)
		-> vector<decltype(a[0] * b[0]), N>
	{
		vector<decltype(a[0] * b[0]), N> result{};
		for (size_t i = 0; i < N; i++)
			result[i] = a[i] / b[i];
		return result;
	}

	template <typename T, typename U, size_t N>
	vector<T, N>& operator+=(vector<T, N>& a, const vector<U, N>& b)
	{
		for (size_t i = 0; i < N; i++)
			a[i] += b[i];
		return a;
	}

	template <typename T, typename U, size_t N>
	vector<T, N>& operator-=(vector<T, N>& a, const vector<U, N>& b)
	{
		for (size_t i = 0; i < N; i++)
			a[i] -= b[i];
		return a;
	}

	template <typename T, typename U, size_t N>
	vector<T, N>& operator*=(vector<T, N>& a, const vector<U, N>& b)
	{
		for (size_t i = 0; i < N; i++)
			a[i] *= b[i];
		return a;
	}

	template <typename T, typename U, size_t N>
	vector<T, N>& operator/=(vector<T, N>& a, const vector<U, N>& b)
	{
		for (size_t i = 0; i < N; i++)
			a[i] /= b[i];
		return a;
	}

	/*
	* Cross type operators for vectors with non-vectors.
	* The return type is defined by the resulting type of element multiplication.
	*/

	template <typename T, typename U, size_t N>
	auto operator+(const vector<T, N>& a, const U& b)
		-> vector<decltype(a[0] * b), N>
	{
		vector<decltype(a[0] * b), N> result{};
		for (size_t i = 0; i < N; i++)
			result[i] = a[i] + b;
		return result;
	}

	template <typename T, typename U, size_t N>
	auto operator-(const vector<T, N>& a, const U& b)
		-> vector<decltype(a[0] * b), N>
	{
		vector<decltype(a[0] * b), N> result{};
		for (size_t i = 0; i < N; i++)
			result[i] = a[i] - b;
		return result;
	}

	template <typename T, typename U, size_t N>
	auto operator*(const vector<T, N>& a, const U& b)
		-> vector<decltype(a[0] * b), N>
	{
		vector<decltype(a[0] * b), N> result{};
		for (size_t i = 0; i < N; i++)
			result[i] = a[i] * b;
		return result;
	}

	template <typename T, typename U, size_t N>
	auto operator/(const vector<T, N>& a, const U& b)
		-> vector<decltype(a[0] * b), N>
	{
		vector<decltype(a[0] * b), N> result{};
		for (size_t i = 0; i < N; i++)
			result[i] = a[i] / b;
		return result;
	}

	template <typename T, typename U, size_t N>
	vector<T, N>& operator+=(vector<T, N>& a, const U& b)
	{
		for (size_t i = 0; i < N; i++)
			a[i] += b;
		return a;
	}

	template <typename T, typename U, size_t N>
	vector<T, N>& operator-=(vector<T, N>& a, const U& b)
	{
		for (size_t i = 0; i < N; i++)
			a[i] -= b;
		return a;
	}

	template <typename T, typename U, size_t N>
	vector<T, N>& operator*=(vector<T, N>& a, const U& b)
	{
		for (size_t i = 0; i < N; i++)
			a[i] *= b;
		return a;
	}

	template <typename T, typename U, size_t N>
	vector<T, N>& operator/=(vector<T, N>& a, const U& b)
	{
		for (size_t i = 0; i < N; i++)
			a[i] /= b;
		return a;
	}

	/*
	* Vector comparison, greater and less than work by magnitude
	*/

	template <typename T, typename U, size_t N>
	bool operator==(const vector<T, N>& a, const vector<U, N>& b)
	{
		for (size_t i = 0; i < N; i++)
			if (static_cast<decltype(a[0] * b[0])>(a[i]) != static_cast<decltype(a[0] * b[0])>(b[i]))
				return false;
		return true;
	}

	template <typename T, typename U, size_t N>
	bool operator!=(const vector<T, N>& a, const vector<U, N>& b)
	{
		return !(a == b);
	}

	template <typename T, typename U, size_t N>
	bool operator>(const vector<T, N>& a, const vector<U, N>& b)
	{
		return static_cast<decltype(a[0] * b[0])>(a.magnitude()) > static_cast<decltype(a[0] * b[0])>(b.magnitude());
	}

	template <typename T, typename U, size_t N>
	bool operator<(const vector<T, N>& a, const vector<U, N>& b)
	{
		return !(a > b);
	}

	template <typename T, size_t N>
	std::ostream& operator<<(std::ostream& stream, const vector<T, N>& vec) {
		stream << "vec" << N << "(";
		for (size_t i = 0; i < N; i++)
		{
			stream << vec[i];
			if (i + 1 < N)
				stream << ", ";
		}
		stream << ")";
		return stream;
	}

	// Most commonly used vectors
	using vec2 = vector<float, 2>;
	using vec3 = vector<float, 3>;
	using vec4 = vector<float, 4>;
	using vec2_precise = vector<double, 2>;
	using vec3_precise = vector<double, 3>;
	using vec4_precise = vector<double, 4>;

	// Alias to differentiate gmath::vector from std::vector
	template<typename T, size_t N>
	using vec = vector<T, N>;
}