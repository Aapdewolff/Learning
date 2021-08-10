#pragma once

#include <math.h>
#include <intrin.h>
#include <xmmintrin.h>
#include <pmmintrin.h>
#include <stdint.h>
#include <chrono>
#include <random>

#define E						2.71828182845904523536   // e
#define LOG2E					1.44269504088896340736   // log2(e)
#define LOG10E					0.434294481903251827651  // log10(e)
#define LN2						0.693147180559945309417  // ln(2)
#define LN10					2.30258509299404568402   // ln(10)
#define PI						3.14159265358979323846   // pi
#define TWO_PI					6.283185307179586476925  // 2*pi
#define THREEHALF_PI			10.99557428756427633461  // 3.5*pi
#define HALF_PI					1.57079632679489661923   // pi/2
#define QUARTER_PI				0.785398163397448309616  // pi/4
#define INVERSED_PI				0.318309886183790671538  // 1/pi
#define INVERSED_TWO_PI			0.159154943091895335768  // 1/(2*pi)
#define TWO_DIVIDED_BY_PI		0.636619772367581343076  // 2/pi
#define TWO_DIVIDED_BY_SQRTPI	1.12837916709551257390   // 2/sqrt(pi)
#define SQRT2					1.41421356237309504880   // sqrt(2)
#define INVERSED_SQRT2			0.707106781186547524401  // 1/sqrt(2)

namespace gmath
{
	template<typename T>
	T deg_to_rad(const T& degrees)
	{
		return degrees * (PI / 180.0);
	}

	template<typename T>
	T rad_to_deg(const T& radians)
	{
		return radians * (180.0 / PI);
	}

	template<typename T>
	T min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	T max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	template<typename T>
	T clamp(const T& value, const T& min, const T& max)
	{
		return value < min ? min : (value > max ? max : value);
	}

	template<typename T>
	T clamp01(const T& value)
	{
		return value < 0.0 ? 0.0 : (value > 1.0 ? 1.0 : value);
	}

	template<typename T>
	T lerp(const T& a, const T& b, const float& t)
	{
		float c{ clamp01<float>(t) };
		return a * (1.0 - c) + b * c;
	}

	template<typename T>
	T lerp_unclamped(const T& a, const T& b, const float& t)
	{
		return a * (1 - t) + b * t;
	}

	template<typename T>
	T wrap(const T& value, const T& min, const T& max)
	{
		return min + fmod(value - min, max - min);
	}

	template<typename T>
	T sin(T x)
	{
		const double A = 0.00735246819687011731341356165096815;
		const double B = -0.16528911397014738207016302002888890;
		const double C = 0.99969198629596757779830113868360584;

		int32_t k;
		double x2;

		k = round(INVERSED_PI * x);
		x -= k * PI;
		x2 = static_cast<double>(x) * x;
		x = x * (C + x2 * (B + A * x2));
		if (k % 2)
			x = -x;

		return x;
	}

	template<typename T>
	T cos(T x)
	{
		return sin<T>(HALF_PI - x);
	}

	template<typename T>
	T atan(T x)
	{
		return QUARTER_PI * x - x * (fabs(x) - 1.0) * (0.2447 + 0.0663 * fabs(x));
	}

	template<typename T>
	T atan2(const T& y, const T& x)
	{
		if (fabs(x) > fabs(y))
		{
			T at = atan<T>(y / x);
			if (x > 0.0)
				return at;
			else
				return y > 0.0 ? at + PI : at - PI;
		}
		else
		{
			T at = atan<T>(x / y);
			if (x > 0.0)
				return y > 0.0 ? HALF_PI - at : -HALF_PI - at;
			else
				return y > 0.0 ? HALF_PI + at : -HALF_PI + at;
		}
	}

	template<typename T>
	T sqrt(T x)
	{
		static int csr = 0;
		if (!csr)
			csr = _mm_getcsr() | 0x8040;
		_mm_setcsr(csr);
		return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x))) * x;
	}

	int32_t round(double x)
	{
		const double magic = 6755399441055744.0;

		union
		{
			double d;

			struct
			{
				int32_t lw;
				int32_t hw;
			};
		} fast_trunc;

		fast_trunc.d = x;
		fast_trunc.d += magic;

		return fast_trunc.lw;
	}

	template<typename T = float>
	T random(const T& min = 0.0, const T& max = 1.0)
	{
		static std::random_device rd;
		static std::mt19937 gen;
		static bool gen_init{};
		if (!gen_init)
		{
			std::mt19937::result_type seed = rd() ^ (
				static_cast<std::mt19937::result_type>(
					std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()) +
				static_cast<std::mt19937::result_type>(
					std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()));

			gen.seed(seed);
			gen_init = true;
		}

		return static_cast<T>(min + static_cast <float> (gen()) / (static_cast <float> (std::mt19937::max() / (max - min))));
	}
}