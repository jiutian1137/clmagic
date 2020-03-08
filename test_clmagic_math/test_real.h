#pragma once
#include <iostream>
#include "../src/clmagic/math/real.h"

inline void test_rational() {
	using namespace::clmagic;

	std::cout << "sizeof(float_bit) = " << sizeof(IEEE754_float) << std::endl;
	std::cout << IEEE754_float::mask_sign() << std::endl;
	std::cout << IEEE754_float::mask_exponent() << std::endl;
	std::cout << IEEE754_float::mask_significant() << std::endl;

	IEEE754_float _Tf = 2.f;
	std::cout << _Tf << std::endl;
	std::cout << rational(_Tf) << std::endl;
	for (float f = 4.f; f <= 100.f; f += 1.f) {
		std::cout << (f / 3.f) << ", " << rational(IEEE754_double(f / 3.f)) << std::endl;
	}


	std::cout << rational(1, 3) * rational(3, 1) << std::endl;
	std::cout << rational(2, 6) - rational(1, 6) << std::endl;
	std::cout << rational(2, 6) + rational(5, 6) << std::endl;
	std::cout << rational(2, 6) + rational(5, 13) << std::endl;
	std::cout << rational(100, 1) / rational(50, 1) << std::endl;
	std::cout << (rational(2, 3) < rational(4, 7)) << std::endl;
	std::cout << (rational(2, 3) < rational(7, 10)) << std::endl;

	auto _1m = meter<float>(1);
	auto _1km = kilometer<float>(_1m);
	std::cout << "1m: " << _1m << "\t=" << _1km << "km" << std::endl;

	auto _Pi = degree<float>(180.f);
	std::cout << "Pi = " << _Pi << "deg = " << radian<float>(_Pi) << "rad" << std::endl;

	auto _10length = meter<vector3<float, 0>>(vector3<float, 0>(10.f));
	std::cout << centimeter<vector3<float, 0>>(_10length) << std::endl;

}