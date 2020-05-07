//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
// Apache Licene 2.0
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_calculation_real_REAL_h_
#define clmagic_calculation_real_REAL_h_
#include <cmath>
#include <cassert>
#include <cstdint>

namespace clmagic {
#if defined(__CLMAGIC_REAL_AS_FLOAT__) || !defined(__CLMAGIC_REAL_AS_DOUBLE__)
	using real_t = float;
#elif defined(__CLMAGIC_REAL_AS_DOUBLE__)
	using real_t = double;
#endif

	constexpr float constexpr_trunc(float _Real) {
		return (static_cast<float>(static_cast<int>(_Real)));
	}

	constexpr double constexpr_trunc(double _Real) {
		return (constexpr_trunc(static_cast<float>(_Real)));
	}

	constexpr float constexpr_fract(float _Real) {
		return (_Real - constexpr_trunc(_Real));
	}

	constexpr double constexpr_fract(double _Real) {
		return (_Real - constexpr_trunc(_Real));
	}

	constexpr float constexpr_floor(float _Real) {
		return (static_cast<float>(static_cast<int>(_Real)));
	}

	constexpr double constexpr_floor(double _Real) {
		return (static_cast<double>(static_cast<int>(_Real)));
	}

	constexpr int constexpr_mod(int _Real, int _Mod) {// _Real % _Mod
		return (_Real % _Mod);
	}

	constexpr float constexpr_mod(float _Real, float _Mod) {// _Real % _Mod
		return (_Real - constexpr_floor(_Real / _Mod) * _Mod);
	}

	constexpr double constexpr_mod(double _Real, double _Mod) {// _Real % _Mod
		return (_Real - constexpr_floor(_Real / _Mod) * _Mod);
	}


	constexpr double constant_Pi = static_cast<double>(3.14159);
	constexpr double constant_e = static_cast<double>(2.71828);

	/* - - - - - - - numeric calculate - - - - - - - - - */
	using _CSTD abs;
	using _CSTD trunc;
	using _CSTD floor;
	using _CSTD round;

	template<typename _Ty = void>
	struct _Real_modulus {
		_Ty operator()(const _Ty& a, const _Ty& b) const {
			return (a % b);
		}
	};

	template<>
	struct _Real_modulus<float> {
		float operator()(float a, float b) const {
			return _CSTD fmod(a, b);
		}
	};

	template<>
	struct _Real_modulus<double> {
		double operator()(double a, double b) const {
			return _CSTD fmod(a, b);
		}
	};

	template<>
	struct _Real_modulus<void> {
		template<typename _Ty>
		auto operator()(_Ty& a, _Ty&& b) -> decltype(_Real_modulus<_Ty>()(a, b)) const {
			return _Real_modulus<_Ty>()(a, b);
		}
	};
	
	using _CSTD fmod;

	using _CSTD pow;
	using _CSTD sqrt;
	using _CSTD cbrt;

	inline float  invsqrt(float  _X) {
		float   xhalf = 0.5f * _X;
		int32_t i = *reinterpret_cast<int32_t*>(&_X);/* get bits for floating value */
		i = 0x5f3759df - (i >> 1);         /* gives initial guess y0 */
		_X = *reinterpret_cast<float*>(&i); /* convert bits back to float */
		_X = _X * (1.5f - xhalf * _X * _X); /* Newton step, repeating increases accuracy */
		return _X;
	}
	inline double invsqrt(double _X) {
		double   xhalf = 0.5 * _X;
		uint64_t i = *reinterpret_cast<uint64_t*>(&_X);
		i = 0x5fe6ec85e7de30daL - (i >> 1);
		_X = *reinterpret_cast<double*>(&i);
		_X = _X * (1.5 - xhalf * _X * _X);
		return _X;
	}
	inline float  invcbrt(float  _X) { return (1.f / cbrt(_X)); }
	inline double invcbrt(double _X) { return (1.0 / cbrt(_X)); }

	// log(e)(x)
	using _CSTD log;
	using _CSTD log2;
	using _CSTD log10;

	using _CSTD exp;
	using _CSTD exp2;
	inline int32_t exp2(int32_t _X) { return (1 << _X); }
	inline int64_t exp2(int64_t _X) { return (int64_t(1) << _X); }

	inline float   exp10(float  _X) { return pow(10.f, _X); }
	inline double  exp10(double _X) { return pow(10.0, _X); }

	// log(e)(_X+1)
	using _CSTD log1p;
	// exp(x)-1
	using _CSTD expm1;
	using _CSTD erf;
	using _CSTD erfc;

	// random: [0.0, 1.0]
	inline float  randf() { return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)); }
	inline double randd() { return (static_cast<double>(rand()) / static_cast<double>(RAND_MAX)); }

	// random: [_Min, _Max]
	inline float  randf(float  _Min, float  _Max) { return (_Min + randf() * (_Max - _Min)); }
	inline double randd(double _Min, double _Max) { return (_Min + randd() * (_Max - _Min)); }


	/* - - - - - - - geometry - - - - - - - - - */
	// sqrt(a² + b²)
	using _CSTD hypot;
	using _CSTD sin;
	using _CSTD cos;
	using _CSTD tan;
	using _CSTD asin;
	using _CSTD acos;
	using _CSTD atan;
	using _CSTD atan2;
	/*<formula>
		_Ty a = any_value
		sin(a)*sin(a) + cos(a)*cos(a) = 1
		
		sin(a + 2Pi*k) = sin(a)
		cos(a + 2Pi*k) = cos(a)
		tan(a + 2Pi*k) = tan(a)
		cot(a + 2Pi*k) = cot(a)

		sin(a + Pi) = -sin(a)
		cos(a + Pi) = -cos(a)
		tan(a + Pi) =  tan(a)
		cot(a + Pi) =  cot(a)

		sin(-a) = -sin(a)
		cos(-a) =  cos(a)
		tan(-a) = -tan(a)
		cot(-a) = -cot(a)

		sin(a + Pi/2) =  cos(a)
		cos(a + Pi/2) = -sin(a)
		tan(a + Pi/2) = -cot(a)
		cot(a + Pi/2) = -tan(a)
		
		_Ty b = any_value
		sin(a + b) = sin(a)*cos(b) + cos(a)*sin(b)
		sin(a - b) = sin(a)*cos(b) - cos(a)*sin(b)
		cos(a + b) = cos(a)*cos(b) - sin(a)*sin(b)
		cos(a - b) = cos(a)*cos(b) + sin(a)*sin(b)
		tan(a + b) = ( tan(a)+tan(b) ) / ( 1 - tan(a)*tan(b) )
		tan(a - b) = ( tan(a)-tan(b) ) / ( 1 + tan(a)*tan(b) )
	</formula>*/

	using _CSTD sinh;
	using _CSTD cosh;
	using _CSTD tanh;
	using _CSTD asinh;
	using _CSTD acosh;
	using _CSTD atanh;

	inline float dot(const float* v0, const float* v1, size_t N) {
		float s = v0[0] * v1[0];
		for (size_t i = 1; i != N; ++i) {
			s += v0[i] * v1[i];
		}
		return (s);
	}

	inline double dot(const double* v0, const double* v1, size_t N) {
		double s = v0[0] * v1[0];
		for (size_t i = 1; i != N; ++i) {
			s += v0[i] * v1[i];
		}
		return (s);
	}

	inline void cross3(const float* v0, const float* v1, float* dst) {
		dst[0] = v0[1] * v1[2] - v0[2] * v1[1];
		dst[1] = v0[2] * v1[0] - v0[0] * v1[2];
		dst[2] = v0[0] * v1[1] - v0[1] * v1[0];
	}

	inline void cross3(const double* v0, const double* v1, double* dst) {
		dst[0] = v0[1] * v1[2] - v0[2] * v1[1];
		dst[1] = v0[2] * v1[0] - v0[0] * v1[2];
		dst[2] = v0[0] * v1[1] - v0[1] * v1[0];
	}

#define _Real_wrapper_operator( CNAME, VAR )                             \
	CNAME  operator+ (CNAME _Right) const { return (VAR + _Right.VAR); } \
	CNAME  operator- (CNAME _Right) const { return (VAR - _Right.VAR); } \
	CNAME  operator* (CNAME _Right) const { return (VAR * _Right.VAR); } \
	CNAME  operator/ (CNAME _Right) const { return (VAR / _Right.VAR); } \
	CNAME  operator% (CNAME _Right) const { return (::clmagic::_Real_modulus<>()(VAR, _Right.VAR)); } \
	CNAME& operator+=(CNAME _Right) { *this = *this + _Right; return *this; } \
	CNAME& operator-=(CNAME _Right) { *this = *this + _Right; return *this; } \
	CNAME& operator*=(CNAME _Right) { *this = *this + _Right; return *this; } \
	CNAME& operator/=(CNAME _Right) { *this = *this + _Right; return *this; } \
	CNAME& operator%=(CNAME _Right) { *this = *this + _Right; return *this; }

}// namespace clmagic

#endif