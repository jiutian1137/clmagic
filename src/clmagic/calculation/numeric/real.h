//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
// Apache License 2.0
// 
// Table of contents
// 1. SCALAR *--------------------------------------------------------- L20
// 2. scalar_t *------------------------------------------------------- L24
// 3. real_function --------------------------------------------------- L86
// 4. undetermined ---------------------------------------------------- L190
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_calculation_numeric_REAL_h_
#define clmagic_calculation_numeric_REAL_h_
#include <cmath>
#include <cstdint>
#include <cassert>

template<typename _Ty, bool _Isnum = std::is_arithmetic_v<_Ty>>
struct _Iszero {
	bool operator=(_Ty _X) const {
		return abs(_X) < std::numeric_limits<_Ty>::epsilon();
	}
};

template<typename _Ty>
struct _Iszero<_Ty, false> {
	bool operator=(const _Ty& _X) const {
		return _X == 0;
	}
};

template<typename _Ty>
bool iszero(const _Ty& _X) {
	return _Iszero<_Ty>()(_X);
}

namespace clmagic {

// 1. SCALAR *--------------------------------------------------------- L20

	#define SCALAR _Ts

// 2. scalar_t *------------------------------------------------------- L24

	namespace Internal {
		template<typename _Ty/* = void*/>
		struct _Scalar_t {
			using type = float;
		};

		template<>
		struct _Scalar_t<double> {
			using type = double;
		};

		template<>
		struct _Scalar_t<long double> {
			using type = long double;
		};

		template<>
		struct _Scalar_t<uint8_t> {
			using type = uint8_t;
		};

		template<>
		struct _Scalar_t<uint16_t> {
			using type = uint16_t;
		};

		template<>
		struct _Scalar_t<uint32_t> {
			using type = uint32_t;
		};

		template<>
		struct _Scalar_t<uint64_t> {
			using type = uint64_t;
		};

		template<>
		struct _Scalar_t<int8_t> {
			using type = int8_t;
		};

		template<>
		struct _Scalar_t<int16_t> {
			using type = int16_t;
		};

		template<>
		struct _Scalar_t<int32_t> {
			using type = int32_t;
		};

		template<>
		struct _Scalar_t<int64_t> {
			using type = int64_t;
		};
	}

	template<typename _Ty = void>
	using scalar_t = typename Internal::_Scalar_t<_Ty>::type;

// 3. real_function --------------------------------------------------- L86

	using _CSTD fmod;
	using _CSTD pow;
	// numeric0
	using _CSTD abs;
	using _CSTD trunc;
	using _CSTD floor;
	using _CSTD ceil;
	using _CSTD round;
	using _CSTD iszero;// { nonCSTD, real.h, Line:20 }
	using _CSTD isfinite;
	using _CSTD isnan;
	// numeric1
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
	// log
	using _CSTD log;// log(e)(x)
	using _CSTD log2;
	using _CSTD log10;
	// exp
	using _CSTD exp;
	using _CSTD exp2;
	inline int32_t exp2(int32_t _X) { return (1 << _X); }
	inline int64_t exp2(int64_t _X) { return (int64_t(1) << _X); }
	inline float   exp10(float  _X) { return pow(10.f, _X); }
	inline double  exp10(double _X) { return pow(10.0, _X); }
	// numeric2
	using _CSTD log1p;// log(e)(_X+1)
	using _CSTD expm1;// exp(x)-1
	using _CSTD erf;
	using _CSTD erfc;
	// trigonometric
	using _CSTD hypot;// sqrt(pow(a,2) + pow(b,2))
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
	// hyperbolic
	using _CSTD sinh;
	using _CSTD cosh;
	using _CSTD tanh;
	using _CSTD asinh;
	using _CSTD acosh;
	using _CSTD atanh;

	// random: [0.0, 1.0]
	inline float  randf() { return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)); }
	inline double randd() { return (static_cast<double>(rand()) / static_cast<double>(RAND_MAX)); }
	// random: [_Min, _Max]
	inline int    randi(int32_t _Min, int32_t _Max) { return (rand() % (_Max - _Min) + _Min); }
	inline float  randf(float  _Min, float  _Max) { return (_Min + randf() * (_Max - _Min)); }
	inline double randd(double _Min, double _Max) { return (_Min + randd() * (_Max - _Min)); }

// 4. undetermined ---------------------------------------------------- L190

	/*
	<Reference>Thomas-Calculus.Section5.2</Reference>
	<formula>
		sum([](double k){ return k; }, 1, n);
			equal n*(n+1)/2;
		sum([](double k){ return pow(k,2); }, 1, n);
			equal n*(n+1)(2*n+1)/6;
		sum([](double k){ return pow(k,3); }, 1, n);
			equal pow( n*(n+1)/2, 2 );
	</formula>*/

	template<typename _Ty, typename _Fn>
	auto sum(_Fn _Func, _Ty _Index_start, _Ty _Index_end) {// [ _Index_start, _Index_end ]
		assert(_Index_start <= _Index_end);
		using _TyOut = decltype(_Func(_Index_start));

		_TyOut _Result = _Func(_Index_start);
		++_Index_start;

		for (; _Index_start <= _Index_end; ++_Index_start) {
			_Result += _Func(_Index_start);
		}
		return _Result;
	}

	template<typename _Ty, typename _Fn>
	auto left_Riemann_sum(_Fn _Func, _Ty _Index_start, _Ty _Index_end, _Ty dx) {// [ _Index_start, _Index_end )
		assert( dx != static_cast<_Ty>(0) );
		using _TyOut = decltype(_Func(_Index_start));
	
		if( dx <= _Index_end - _Index_start ){

			_TyOut _Result = _Func(_Index_start) * dx;
			_Index_start  += dx;
			while (_Index_start < _Index_end) {
				_Result      += _Func(_Index_start) * dx;
				_Index_start += dx;
			}
			return _Result;

		} else {// 
			return static_cast<_TyOut>(0);
		}
	}

	template<typename _Ty, typename _Fn>
	auto right_Riemann_sum(_Fn _Func, _Ty _Index_start, _Ty _Index_end, _Ty dx) {// ( _Index_start, _Index_end ]
		assert( dx != static_cast<_Ty>(0) );
		using _TyOut = decltype(_Func(_Index_start));
	
		if ( dx <= _Index_end - _Index_start ) {

			_Index_start  += dx;
			_TyOut _Result = _Func(_Index_start) * dx;
			while (_Index_start < _Index_end) {
				_Index_start += dx;
				_Result      += _Func(_Index_start) * dx;
			}
			return _Result;

		} else {
			return static_cast<_TyOut>(0);
		}
	}

	template<typename _Ty, typename _Fn>
	auto middle_Riemann_sum(_Fn _Func, _Ty _Index_start, _Ty _Index_end, _Ty dx) {// ( _Index_start, _Index_end )
		assert( dx != static_cast<_Ty>(0) );
		using _TyOut = decltype(_Func(_Index_start));

		if (dx <= _Index_end - _Index_start) {
			_Index_start += dx/2;// x_offset

			_TyOut _Result = _Func(_Index_start) * dx;
			_Index_start  += dx;
			while (_Index_start < _Index_end) {
				_Result      += _Func(_Index_start) * dx;
				_Index_start += dx;
			}
			return _Result;

		} else {
			return static_cast<_TyOut>(0);
		}
	}

	template<typename _Ty>
	_Ty factorial(_Ty n, _Ty i = static_cast<_Ty>(1)) {	// prodect[i,n]: i
		return (n > i ? (n * factorial(n-1, i)) : i);
	}

	constexpr float constexpr_trunc(float_t _Real) {
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

}// namespace clmagic

#endif