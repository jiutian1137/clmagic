#pragma once
#ifndef clmagic_math_real_REAL_h_
#define clmagic_math_real_REAL_h_
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


	/* - - - - - - - numeric calculate - - - - - - - - - */
	inline int32_t abs(int32_t _X) { return _CSTD abs(_X); }
	inline float   abs(float   _X) { return _CSTD abs(_X); }
	inline double  abs(double  _X) { return _CSTD abs(_X); }

	// to_fraction(_X)
	inline int32_t trunc(int32_t _X) { assert(false); return 0; }
	inline float   trunc(float   _X) { return _CSTD truncf(_X); }
	inline double  trunc(double  _X) { return _CSTD trunc(_X); }

	// to_lowbound(_X)
	inline int32_t floor(int32_t _X) { assert(false); return _X; }
	inline float   floor(float   _X) { return _CSTD floorf(_X); }
	inline double  floor(double  _X) { return _CSTD floor(_X); }

	// to_upbound(_X)
	inline int32_t ceil(int32_t _X) { assert(false); return _X; }
	inline float   ceil(float   _X) { return _CSTD ceilf(_X); }
	inline double  ceil(double  _X) { return _CSTD ceil(_X); }

	inline int32_t round(int32_t _X) { assert(false); return _X; }
	inline float   round(float   _X) { return _CSTD roundf(_X); }
	inline double  round(double  _X) { return _CSTD round(_X); }

	inline int32_t mod(int32_t _X, int32_t _Y) { return _X % _Y; }
	inline float   mod(float   _X, float   _Y) { return (_X - _Y * (floor(_X / _Y))); }
	inline double  mod(double  _X, double  _Y) { return (_X - _Y * (floor(_X / _Y))); }

	/* _X.exponent(_Y) */
	inline int32_t pow(int32_t _X, int32_t _Y) { return static_cast<int32_t>(_CSTD pow(_X, _Y)); }
	inline float   pow(float   _X, int32_t _Y) { return _CSTD pow(_X, _Y); }
	inline double  pow(double  _X, int32_t _Y) { return _CSTD pow(_X, _Y); }
	inline float   pow(float   _X, float   _Y) { return _CSTD powf(_X, _Y); }
	inline double  pow(double  _X, double  _Y) { return _CSTD pow(_X, _Y); }

	// x^(1/2), sqrt(x)=a, x=a*a
	inline int32_t sqrt(int32_t _X) { return static_cast<int32_t>(_CSTD sqrt(_X)); }
	inline float   sqrt(float   _X) { return _CSTD sqrtf(_X); }
	inline double  sqrt(double  _X) { return _CSTD sqrt(_X); }

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

	// _X(¹/³)
	inline float  cbrt(float  _X) { return _CSTD cbrtf(_X); }
	inline double cbrt(double _X) { return _CSTD cbrt(_X); }

	inline float  invcbrt(float  _X) { return (1.f / cbrt(_X)); }
	inline double invcbrt(double _X) { return (1.0 / cbrt(_X)); }

	// log(e)(x)
	inline float  log(float  _X) { return _CSTD logf(_X); }
	inline double log(double _X) { return _CSTD log(_X); }

	inline float  log2(float  _X) { return _CSTD log2f(_X); }
	inline double log2(double _X) { return _CSTD log2(_X); }

	inline float  log10(float  _X) { return _CSTD log10f(_X); }
	inline double log10(double _X) { return _CSTD log10(_X); }

	// pow(e,x)
	inline float  exp(float  _X) { return _CSTD expf(_X); }
	inline double exp(double _X) { return _CSTD exp(_X); }

	// pow(2, _X)
	inline int32_t exp2(int32_t _X) { return (1 << _X); }
	inline int64_t exp2(int64_t _X) { return (int64_t(1) << _X); }
	inline float   exp2(float   _X) { return _CSTD exp2f(_X); }
	inline double  exp2(double  _X) { return _CSTD exp2(_X); }
	inline float   exp10(float  _X) { return pow(10.f, _X); }
	inline double  exp10(double _X) { return pow(10.0, _X); }

	// log(e)(_X+1)
	inline float  log1p(float  _X) { return _CSTD log1pf(_X); }
	inline double log1p(double _X) { return _CSTD log1p(_X); }

	// exp(x)-1
	inline float  expm1(float  x) { return _CSTD expm1f(x); }
	inline double expm1(double x) { return _CSTD expm1(x); }

	inline float  erf(float  x) { return _CSTD erff(x); }
	inline double erf(double x) { return _CSTD erf(x); }

	inline float  erfc(float x) { return _CSTD erfcf(x); }
	inline double erfc(double x) { return _CSTD erfc(x); }

	// region: [0.0, 1.0]
	inline float  randf() { return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)); }
	inline double randd() { return (static_cast<double>(rand()) / static_cast<double>(RAND_MAX)); }

	// region: [_Min, _Max]
	inline float  randf(float  _Min, float  _Max) { return (_Min + randf() * (_Max - _Min)); }
	inline double randd(double _Min, double _Max) { return (_Min + randd() * (_Max - _Min)); }


	/* - - - - - - - geometry - - - - - - - - - */
	// sqrt(a² + b²)
	inline float  hypot(float  _A, float  _B) { return _CSTD hypotf(_A, _B); }
	inline double hypot(double _A, double _B) { return _CSTD hypot(_A, _B); }

	inline float   sin(float  _X) { return _CSTD sinf(_X); }
	inline double  sin(double _X) { return _CSTD sin(_X); }

	inline float   cos(float  _X) { return _CSTD cosf(_X); }
	inline double  cos(double _X) { return _CSTD cos(_X); }

	inline float   tan(float  _X) { return _CSTD tanf(_X); }
	inline double  tan(double _X) { return _CSTD tan(_X); }

	inline float  asin(float  _X) { return _CSTD asinf(_X); }
	inline double asin(double _X) { return _CSTD asin(_X); }

	inline float  acos(float  _X) { return _CSTD acosf(_X); }
	inline double acos(double _X) { return _CSTD acos(_X); }

	inline float  atan(float  _X) { return _CSTD atanf(_X); }
	inline double atan(double _X) { return _CSTD atan(_X); }

	inline float  atan2(float  _Y, float  _X) { return _CSTD atan2f(_Y, _X); }
	inline double atan2(double _Y, double _X) { return _CSTD atan2(_Y, _X); }

	inline float   sinh(float  _X) { return _CSTD sinhf(_X); }
	inline double  sinh(double _X) { return _CSTD sinh(_X); }

	inline float   cosh(float  _X) { return _CSTD coshf(_X); }
	inline double  cosh(double _X) { return _CSTD cosh(_X); }

	inline float   tanh(float  _X) { return _CSTD tanhf(_X); }
	inline double  tanh(double _X) { return _CSTD tanh(_X); }

	inline float  asinh(float  _X) { return _CSTD asinhf(_X); }
	inline double asinh(double _X) { return _CSTD asinh(_X); }

	inline float  acosh(float  _X) { return _CSTD acoshf(_X); }
	inline double acosh(double _X) { return _CSTD acosh(_X); }

	inline float  atanh(float  _X) { return _CSTD atanhf(_X); }
	inline double atanh(double _X) { return _CSTD atanh(_X); }

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


#undef _FUNC_IMPLEMENT

}// namespace clmagic

#endif