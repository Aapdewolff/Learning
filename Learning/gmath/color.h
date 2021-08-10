#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <sstream>
#include <format>
#include <limits>

#include "gmath.h"
#include "vec.h"

namespace gmath
{
	template<typename T>
	class color_base
	{
	public:
		color_base() = default;
		color_base(uint32_t hex)
		{
			set_hex(hex);
		}
		color_base(const T& r, const T& g, const T& b, const T& a)
			: r(r), g(g), b(b), a(a) {}
		color_base(const color_base<T>& c)
		{
			std::copy(c.data, c.data + 4, data);
		}

		color_base<T>& operator=(const color_base<T>& c)
		{
			std::copy(c.data, c.data + 4, data);
			return *this;
		}

		union
		{
			T data[4];
			struct
			{
				T r, g, b, a;
			};
		};

		T& operator[](const size_t i)
		{
			return data[i];
		}

		const T& operator[](const size_t i) const
		{
			return data[i];
		}

		void zero()
		{
			std::fill(std::begin(data), std::end(data), T{});
		}

		void randomize(const T& min = 0.0, const T& max = 1.0)
		{
			for (size_t i = 0; i < 4; i++)
			{
				data[i] = gmath::random<T>(min, max);
			}
		}

		void set_hex(uint32_t hex)
		{
			uint32_t mask{ 0xFF };
			uint8_t element{};

			size_t type_factor{ max_element_size() / static_cast<size_t>(std::numeric_limits<uint8_t>::max()) };

			element = static_cast<uint8_t>(hex & mask);
			a = element * type_factor;
			hex >>= 8;
			element = static_cast<uint8_t>(hex & mask);
			b = element * type_factor;
			hex >>= 8;
			element = static_cast<uint8_t>(hex & mask);
			g = element * type_factor;
			hex >>= 8;
			element = static_cast<uint8_t>(hex & mask);
			r = element * type_factor;
			hex >>= 8;
		}

		uint32_t get_hex() const
		{
			uint32_t hex{};
			uint32_t mask{ 0xFF };

			size_t type_factor{ max_element_size() / static_cast<size_t>(std::numeric_limits<uint8_t>::max()) };

			hex |= mask & (a / type_factor);
			hex <<= 8;
			hex |= mask & (b / type_factor);
			hex <<= 8;
			hex |= mask & (g / type_factor);
			hex <<= 8;
			hex |= mask & (r / type_factor);
			return hex;
		}

		size_t max_element_size() const
		{
			return static_cast<size_t>(std::numeric_limits<T>::max());
		}

		vec4 normalized() const
		{
			vec4 result{ static_cast<float>(r) / max_element_size() , static_cast<float>(g) / max_element_size(), static_cast<float>(b) / max_element_size(), static_cast<float>(a) / max_element_size() };
			return result;
		}

		vec4_precise normalized_precise() const
		{
			vec4_precise result{ static_cast<double>(r) / max_element_size() , static_cast<double>(g) / max_element_size(), static_cast<double>(b) / max_element_size(), static_cast<double>(a) / max_element_size() };
			return result;
		}

		std::string to_string() const
		{
			std::stringstream ss;
			ss << this;
			return ss.str();
		}

		color_base<T> grayscale() const
		{
			long avg{ (r + g + b) / 3 };
			T avg_t = static_cast<T>(avg);
			color_base<T> result{ avg_t, avg_t, avg_t, a };
			return result;
		}

		static color_base<T> lerp(const color_base<T>& a, const color_base<T>& b, const float& t)
		{
			color_base<T> result{};
			for (size_t i = 0; i < 4; i++)
				result[i] = gmath::lerp(a[i], b[i], t);
			return result;
		}

		static color_base<T> lerp_unclamped(const color_base<T>& a, const color_base<T>& b, const float& t)
		{
			color_base<T> result{};
			for (size_t i = 0; i < 4; i++)
				result[i] = gmath::lerp_unclamped(a[i], b[i], t);
			return result;
		}

		static inline const color_base<T> white() { return 0xFFFFFFFF; }
		static inline const color_base<T> silver() { return 0xC0C0C0FF; }
		static inline const color_base<T> gray() { return 0x808080FF; }
		static inline const color_base<T> black() { return 0x000000FF; }
		static inline const color_base<T> red() { return 0xFF0000FF; }
		static inline const color_base<T> maroon() { return 0x800000FF; }
		static inline const color_base<T> yellow() { return 0xFFFF00FF; }
		static inline const color_base<T> olive() { return 0x808000FF; }
		static inline const color_base<T> lime() { return 0x00FF00FF; }
		static inline const color_base<T> green() { return 0x008000FF; }
		static inline const color_base<T> aqua() { return 0x00FFFFFF; }
		static inline const color_base<T> teal() { return 0x008080FF; }
		static inline const color_base<T> blue() { return 0x0000FFFF; }
		static inline const color_base<T> navy() { return 0x000080FF; }
		static inline const color_base<T> fuchsia() { return 0xFF00FFFF; }
		static inline const color_base<T> purple() { return 0x800080FF; }
	};

	/*
	* Operators for colors with the same element type, cross element type is not supported as it makes no sense.
	*/

	template<typename T>
	color_base<T>& operator+=(color_base<T>& a, const color_base<T>& b)
	{
		for (size_t i = 0; i < 4; i++)
			a[i] = static_cast<T>(gmath::clamp(static_cast<int>(a[i] + b[i]), 0, static_cast<int>(std::numeric_limits<T>::max())));
		return a;
	}

	template<typename T>
	color_base<T>& operator-=(color_base<T>& a, const color_base<T>& b)
	{
		for (size_t i = 0; i < 4; i++)
			a[i] = static_cast<T>(gmath::clamp(static_cast<int>(a[i] - b[i]), 0, static_cast<int>(std::numeric_limits<T>::max())));
		return a;
	}

	template<typename T>
	color_base<T>& operator*=(color_base<T>& a, const color_base<T>& b)
	{
		for (size_t i = 0; i < 4; i++)
			a[i] = static_cast<T>(gmath::clamp(static_cast<int>(a[i] * b[i]), 0, static_cast<int>(std::numeric_limits<T>::max())));
		return a;
	}

	template<typename T>
	color_base<T>& operator/=(color_base<T>& a, const color_base<T>& b)
	{
		for (size_t i = 0; i < 4; i++)
			a[i] = static_cast<T>(gmath::clamp(static_cast<int>(a[i] / b[i]), 0, static_cast<int>(std::numeric_limits<T>::max())));
		return a;
	}

	template<typename T>
	color_base<T> operator+(const color_base<T>& a, const color_base<T>& b)
	{
		auto result{ a };
		result += b;
		return result;
	}

	template<typename T>
	color_base<T> operator-(const color_base<T>& a, const color_base<T>& b)
	{
		auto result{ a };
		result -= b;
		return result;
	}

	template<typename T>
	color_base<T> operator*(const color_base<T>& a, const color_base<T>& b)
	{
		auto result{ a };
		result *= b;
		return result;
	}

	template<typename T>
	color_base<T> operator/(const color_base<T>& a, const color_base<T>& b)
	{
		auto result{ a };
		result /= b;
		return result;
	}

	/*
	* Color comparison, greater and less than work by average rgb intensity (a is ignored)
	*/

	template<typename T>
	bool operator==(const color_base<T>& a, const color_base<T>& b)
	{
		for (size_t i = 0; i < 4; i++)
			if (a[i] != b[i])
				return false;
		return true;
	}

	template<typename T>
	bool operator!=(const color_base<T>& a, const color_base<T>& b)
	{
		return !(a == b);
	}

	template<typename T>
	bool operator>(const color_base<T>& a, const color_base<T>& b)
	{
		return a.grayscale()[0] > b.grayscale()[0];
	}

	template<typename T>
	bool operator<(const color_base<T>& a, const color_base<T>& b)
	{
		return !(a > b);
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& stream, const color_base<T>& c)
	{
		return (stream << std::format("color({}, {}, {}, {})", static_cast<int>(c.r), static_cast<int>(c.g), static_cast<int>(c.b), static_cast<int>(c.a)));
	}

	// Most commonly used color formats
	using color = color_base<uint8_t>;
	using color16 = color_base<uint16_t>;
	using color32 = color_base<uint32_t>;
	using color64 = color_base<uint64_t>;
}