#include <iostream>
#include <format>

#include "gmath/vec.h"
#include "gmath/matrix.h"
#include "gmath/color.h"
#include "gmath/gmath.h"

int main()
{
	{
		gmath::mat4_precise a{};
		gmath::mat4_precise b{};
		gmath::mat4 c{};

		a.randomize();
		b = gmath::mat4_precise::inverse(a);
		c = gmath::mat4::translation(gmath::vec3{ 1.0, 5.5, 3.2 });

		std::cout << a << std::endl;
		std::cout << b << std::endl;
		std::cout << c << std::endl;
	}

	std::cout << std::endl;

	{
		gmath::vec3 a{ 1.0, 2.0, 5.5 };
		gmath::vec<int, 3> b = a; // gmath::vec3 = gmath::vector<float, 3>, this means all floating precision will be casted away and all values are rounded down

		std::cout << a << std::endl;
		std::cout << b << std::endl;
	}

	std::cout << std::endl;

	{
		gmath::color a{ 0xFF501FFF }; // gmath::color uses uint32_t so 4 * 8 bits
		gmath::color16 b{ 0x05FF06FF }; // gmath::color16 uses uint64_t so 4 * 16 bits, 16 bits per element therefore the name color16

		uint32_t hex_a = a.get_hex(); // get_hex() always returns uint32_t because this results in the readable format of 0xFFFFFFFF
		uint32_t hex_b = b.get_hex();

		std::cout << "a: " << a << std::endl;
		std::cout << std::format("{:#X}", hex_a) << std::endl;
		std::cout << "b: " << b << std::endl; // note that the rgba max values are now the maximum of 16 bits which is NOT 255 but 65536
		std::cout << std::format("{:#X}", hex_b) << std::endl;
		std::cout << "a normalized: " << a.normalized() << std::endl; // these varying max values of each element can be converted when calling normalized()
		std::cout << "b normalized: " << b.normalized() << std::endl; // returns rgba between 0 and 1
	}
}