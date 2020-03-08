//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_math_SIMD_h_
#define clmagic_math_SIMD_h_

#include "clmagic.h"

#include <array>
#include <string>
#include <assert.h>
#include <iostream>

#include <ivec.h>
#include <fvec.h>
#include <dvec.h>


// __m128
inline float _mm_get_ps(__m128 _A, size_t _Pos) {
	assert(_Pos >= 0 && _Pos < 4);
	return *(((float*)&_A) + _Pos);
}

inline float _mm_sum_ps(__m128 _X) {
	return _X.m128_f32[0] + _X.m128_f32[1] + _X.m128_f32[2] + _X.m128_f32[3];
}

inline float _mm_product_ps(__m128 _X) {
	return _X.m128_f32[0] * _X.m128_f32[1] * _X.m128_f32[2] * _X.m128_f32[3];
}

inline __m128 _mm_abs_ps(__m128 _X) {
	return _mm_and_ps(_X, _f32vec4_abs_mask);
}

inline __m128 _mm_round_ps(__m128 _X) {
	return _mm_round_ps(_X, _MM_FROUND_NINT);
}

inline __m128 _mm_fmod_ps_fast(__m128 _X, __m128 _Y) {
	return _mm_sub_ps(_X, _mm_mul_ps(_Y, _mm_floor_ps(_mm_div_ps(_X, _Y))));
}

inline __m128 _mm_cross3_ps(__m128 _A, __m128 _B) {
	/*
	A: [y]   [z],   [z]   [x],   [x]   [y]
	   [*] - [*]    [*] - [*]    [*] - [*]
	B: [z]   [y],   [x]   [z],   [y]   [x]

	1. A:[1][2][0] * B:[2][0][1]
	3.          substract
	2. A:[2][0][1] * B:[1][2][0]
	*/
	auto _Lhs = _mm_shuffle_ps(_A, _A, _MM_SHUFFLE(3, 0, 2, 1));
	auto _Rhs = _mm_shuffle_ps(_B, _B, _MM_SHUFFLE(3, 1, 0, 2));
	auto _Result = _mm_mul_ps(_Lhs, _Rhs);

	_Lhs = _mm_shuffle_ps(_Lhs, _Lhs, _MM_SHUFFLE(3, 0, 2, 1));
	_Rhs = _mm_shuffle_ps(_Rhs, _Rhs, _MM_SHUFFLE(3, 1, 0, 2));

	_Result = _mm_sub_ps(_Result, _mm_mul_ps(_Lhs, _Rhs));

	return (_Result);
}


// __m128i
inline int _mm_get_epi32(__m128i _A, size_t _Pos) {
	assert(_Pos >= 0 && _Pos < 4);
	return ( *(reinterpret_cast<int*>(&_A) + _Pos) );
}

inline __m128i _mm_load_epi32(const int* _Ptr) {
	return ( *reinterpret_cast<const __m128i*>(_Ptr) );
}

inline __m128i _mm_loadu_epi32(const int* _Ptr) {
	__m128i _Result;
	_Result.m128i_i32[0] = *_Ptr++;
	_Result.m128i_i32[1] = *_Ptr++;
	_Result.m128i_i32[2] = *_Ptr++;
	_Result.m128i_i32[3] = *_Ptr;
	return _Result;
}

inline int32_t _mm_sum_epi32(__m128i _X) {
	return _X.m128i_i32[0] + _X.m128i_i32[1] + _X.m128i_i32[2] + _X.m128i_i32[3];
}

inline int32_t _mm_product_epi32(__m128i _X) {
	return _X.m128i_i32[0] * _X.m128i_i32[1] * _X.m128i_i32[2] * _X.m128i_i32[3];
}

inline __m128i _mm_floor_epi32(__m128i _X) {
	return _X;
}

inline __m128i _mm_ceil_epi32(__m128i _X) {
	return _X;
}

inline __m128i _mm_trunc_epi32(__m128i _X) {
	return _mm_set1_epi32(0);
}

inline __m128i _mm_round_epi32(__m128i _X) {
	return _X;
}

inline __m128i _mm_mod_epi32(__m128i _X, __m128i _Y) {
	return _mm_set_epi32(_X.m128i_i32[3] % _Y.m128i_i32[3],
		_X.m128i_i32[2] % _Y.m128i_i32[2],
		_X.m128i_i32[1] % _Y.m128i_i32[1],
		_X.m128i_i32[0] % _Y.m128i_i32[0]);
}

inline __m128i _mm_pow_epi32(__m128i _X, __m128i _Y) {
	return _mm_set_epi32(static_cast<int32_t>(_CSTD pow(_X.m128i_i32[3], _Y.m128i_i32[3])),
		static_cast<int32_t>(_CSTD pow(_X.m128i_i32[2], _Y.m128i_i32[2])),
		static_cast<int32_t>(_CSTD pow(_X.m128i_i32[1], _Y.m128i_i32[1])),
		static_cast<int32_t>(_CSTD pow(_X.m128i_i32[0], _Y.m128i_i32[0])));
}

#define _mm_psfunc_epi32_1st(NAME, X) \
_mm_cvtps_epi32(_mm_round_ps(     \
	_mm_##NAME##_ps(_mm_cvtepi32_ps(X)) ));

#define _mm_psfunc_epi32_2nd(NAME, X, Y) \
_mm_cvtps_epi32(_mm_round_ps(     \
	_mm_##NAME##_ps(_mm_cvtepi32_ps(X), _mm_cvtepi32_ps(Y)) ));

#define _mm_sqrt_epi32(X) _mm_psfunc_epi32_1st(sqrt, X)
#define _mm_cbrt_epi32(X) _mm_psfunc_epi32_1st(cbrt, X)
#define _mm_exp_epi32(X) _mm_psfunc_epi32_1st(exp, X)
#define _mm_exp2_epi32(X) _mm_psfunc_epi32_1st(exp2, X)
#define _mm_exp10_epi32(X) _mm_psfunc_epi32_1st(exp10, X)
#define _mm_log_epi32(X) _mm_psfunc_epi32_1st(log, X)
#define _mm_log2_epi32(X) _mm_psfunc_epi32_1st(log2, X)
#define _mm_log10_epi32(X) _mm_psfunc_epi32_1st(log10, X)

#define _mm_log1p_epi32(X) _mm_psfunc_epi32_1st(log1p, X)
#define _mm_expm1_epi32(X) _mm_psfunc_epi32_1st(expm1, X)
#define _mm_erf_epi32(X) _mm_psfunc_epi32_1st(erf, X)
#define _mm_erfc_epi32(X) _mm_psfunc_epi32_1st(erfc, X)
#define _mm_erfinv_epi32(X) _mm_psfunc_epi32_1st(erfinv, X)
#define _mm_erfcinv_epi32(X) _mm_psfunc_epi32_1st(erfcinv, X)

#define _mm_hypot_epi32(X, Y) _mm_psfunc_epi32_2nd(hypot, X, Y)
#define _mm_sin_epi32(X) _mm_psfunc_epi32_1st(sin, X)
#define _mm_cos_epi32(X) _mm_psfunc_epi32_1st(cos, X)
#define _mm_tan_epi32(X) _mm_psfunc_epi32_1st(tan, X)
#define _mm_asin_epi32(X) _mm_psfunc_epi32_1st(asin, X)
#define _mm_acos_epi32(X) _mm_psfunc_epi32_1st(acos, X)
#define _mm_atan_epi32(X) _mm_psfunc_epi32_1st(atan, X)
#define _mm_atan2_epi32(X, Y) _mm_psfunc_epi32_2nd(atan2, X, Y)
#define _mm_sinh_epi32(X) _mm_psfunc_epi32_1st(sinh, X)
#define _mm_cosh_epi32(X) _mm_psfunc_epi32_1st(cosh, X)
#define _mm_tanh_epi32(X) _mm_psfunc_epi32_1st(tanh, X)
#define _mm_asinh_epi32(X) _mm_psfunc_epi32_1st(asinh, X)
#define _mm_acosh_epi32(X) _mm_psfunc_epi32_1st(acosh, X)
#define _mm_atanh_epi32(X) _mm_psfunc_epi32_1st(atanh, X)

inline __m128i _mm_except_epi32(__m128i _X) { 
	throw std::exception("unsupport this function");
}

inline __m128i _mm_except2_epi32(__m128i _X, __m128i _Y) { 
	throw std::exception("unsupport this function"); 
}


// __m256
union {
	int i[8];
	__m256 m;
} __f32vec8_abs_mask_cheat = { 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff };

inline float _mm256_get_ps(__m256 _A, size_t _Pos) {
	assert(_Pos >= 0 && _Pos < 8);
	return *(((float*)&_A) + _Pos);
}

inline float _mm256_sum_ps(__m256 _X) {
	return _X.m256_f32[0] + _X.m256_f32[1] + _X.m256_f32[2] + _X.m256_f32[3]
		+ _X.m256_f32[4] + _X.m256_f32[5] + _X.m256_f32[6] + _X.m256_f32[7];
}

inline float _mm256_product_ps(__m256 _X) {
	return _X.m256_f32[0] * _X.m256_f32[1] * _X.m256_f32[2] * _X.m256_f32[3]
		* _X.m256_f32[4] * _X.m256_f32[5] * _X.m256_f32[6] * _X.m256_f32[7];
}

inline __m256 _mm256_abs_ps(__m256 _X) {
	return _mm256_and_ps(_X, __f32vec8_abs_mask_cheat.m);
}

inline __m256 _mm256_cross3_ps(__m256 _X, __m256 _Y) {
	auto _Lhs    = _mm256_set_ps(0.f, 0.f, 0.f, 0.f, 0.f, _X.m256_f32[0], _X.m256_f32[2], _X.m256_f32[1]);
	auto _Rhs    = _mm256_set_ps(0.f, 0.f, 0.f, 0.f, 0.f, _Y.m256_f32[1], _Y.m256_f32[0], _Y.m256_f32[2]);
	auto _Result = _mm256_mul_ps(_Lhs, _Rhs);
	_Lhs    = _mm256_set_ps(0.f, 0.f, 0.f, 0.f, 0.f, _X.m256_f32[1], _X.m256_f32[0], _X.m256_f32[2]);
	_Rhs    = _mm256_set_ps(0.f, 0.f, 0.f, 0.f, 0.f, _Y.m256_f32[0], _Y.m256_f32[2], _Y.m256_f32[1]);
	_Result = _mm256_sub_ps(_Result, _mm256_mul_ps(_Lhs, _Rhs));

	return (_Result);
}


// __m256d
union {
	int i[8];
	__m256d m;
} __f64vec4_abs_mask_cheat = { -1, 0x7fffffff, -1, 0x7fffffff, -1, 0x7fffffff, -1, 0x7fffffff };

inline double _mm256_get_pd(__m256d _A, size_t _Pos) {
	assert(_Pos >= 0 && _Pos < 4);
	return *(((double*)&_A) + _Pos);
}

inline double _mm256_sum_pd(__m256d _X) {
	return _X.m256d_f64[0] + _X.m256d_f64[1] + _X.m256d_f64[2] + _X.m256d_f64[3];
}

inline double _mm256_product_pd(__m256d _X) {
	return _X.m256d_f64[0] * _X.m256d_f64[1] * _X.m256d_f64[2] * _X.m256d_f64[3];
}

inline __m256d _mm256_abs_pd(__m256d _X) {
	return _mm256_and_pd(_X, __f64vec4_abs_mask_cheat.m);
}

inline __m256d _mm256_fmod_pd_fast(__m256d _X, __m256d _Y) {
	return _mm256_sub_pd(_X, _mm256_mul_pd(_Y, _mm256_floor_pd(_mm256_div_pd(_X, _Y))));
}

inline __m256d _mm256_cross3_pd(__m256d _A, __m256d _B) {
	auto _Lhs    = _mm256_set_pd(0.0, _A.m256d_f64[0], _A.m256d_f64[2], _A.m256d_f64[1]);
	auto _Rhs    = _mm256_set_pd(0.0, _B.m256d_f64[1], _B.m256d_f64[0], _B.m256d_f64[2]);
	auto _Result = _mm256_mul_pd(_Lhs, _Rhs);
	_Lhs    = _mm256_set_pd(0.0, _A.m256d_f64[1], _A.m256d_f64[0], _A.m256d_f64[2]);
	_Rhs    = _mm256_set_pd(0.0, _B.m256d_f64[0], _B.m256d_f64[2], _B.m256d_f64[1]);
	_Result = _mm256_sub_pd(_Result, _mm256_mul_pd(_Lhs, _Rhs));

	return (_Result);
}


// to_string
inline std::string _mm_to_string_ps(__m128 _A) {
	const auto* _Ptr  = reinterpret_cast<float*>(&_A);
	      auto  _Buf  = std::array<char, 128>();
	      auto  _Size = size_t(0);
	_Size = snprintf(_Buf.data(), 127, "%f, %f, %f, %f", *_Ptr, *(_Ptr+1), *(_Ptr+2), *(_Ptr+3));

	return std::string(&_Buf[0], &_Buf[_Size]);
}

inline std::string _mm_to_string_epi32(__m128i _A) {
	const auto* _Ptr  = reinterpret_cast<int*>(&_A);
	      auto  _Buf  = std::array<char, 128>();
	      auto  _Size = size_t(0);
	_Size = snprintf(_Buf.data(), 127, "%i, %i, %i, %i", *_Ptr, *(_Ptr+1), *(_Ptr+2), *(_Ptr+3));

	return std::string(&_Buf[0], &_Buf[_Size]);
}

inline std::string _mm256_to_string_ps(__m256 _A) {
	const auto* _Ptr  = reinterpret_cast<float*>(&_A);
	      auto  _Buf  = std::array<char, 256>();
		  auto  _Size = size_t(0);
	_Size = snprintf(_Buf.data(), 255, "%f, %f, %f, %f, %f, %f, %f, %f",
		*_Ptr, *(_Ptr + 1), *(_Ptr + 2), *(_Ptr + 3), *(_Ptr + 4), *(_Ptr + 5), *(_Ptr + 6), *(_Ptr + 7));

	return std::string(&_Buf[0], &_Buf[_Size]);
}

inline std::string _mm256_to_string_pd(__m256d _A) {
	const auto* _Ptr  = reinterpret_cast<double*>(&_A);
	      auto  _Buf  = std::array<char, 256>();
	      auto  _Size = size_t(0);
	_Size = snprintf(_Buf.data(), 255, "%16.16f, %16.16f, %16.16f, %16.16f", *_Ptr, *(_Ptr+1), *(_Ptr+2), *(_Ptr+3));

	return std::string(&_Buf[0], &_Buf[_Size]);
}



namespace clmagic {

	template<typename _Ty>
		constexpr bool is_simd_v = std::_Is_any_of_v< 
			std::remove_cv_t<_Ty>, __m128, __m128d, __m128i, __m256, __m256d, __m512, __m512d, __m512i>;

#define _SIMD_TRAITS_LOAD(NAME, FUNC) \
static vector_type NAME(const scalar_type* _Ptr) { return FUNC(_Ptr); } 
#define _SIMD_TRAITS_1ST(NAME, FUNC) \
static vector_type NAME(vector_type _X) { return FUNC(_X); } 
#define _SIMD_TRAITS_2ND(NAME, FUNC) \
static vector_type NAME(vector_type _X, vector_type _Y) { return FUNC(_X, _Y); } 


	template<typename _Ty = __m128>
	struct simd_traits {
		using scalar_type = float;
		using vector_type = __m128;

		constexpr static auto scalar_align = static_cast<size_t>(4);

		static vector_type set1(scalar_type _All) { return _mm_set1_ps(_All); }
		static vector_type set (scalar_type x0, scalar_type x1, scalar_type x2, scalar_type x3) { return _mm_set_ps(x3, x2, x1, x0); }
		static scalar_type get (vector_type A, size_t i){ return _mm_get_ps(A, i); }
		static scalar_type sum (vector_type _X) { return _mm_sum_ps(_X); }
		static scalar_type product(vector_type _X) { return _mm_product_ps(_X); }

		_SIMD_TRAITS_LOAD(load, _mm_load_ps)
		_SIMD_TRAITS_LOAD(loadu, _mm_loadu_ps)

		_SIMD_TRAITS_2ND(bit_and, _mm_and_ps)
		_SIMD_TRAITS_2ND(bit_or,  _mm_or_ps)
		_SIMD_TRAITS_2ND(bit_xor, _mm_xor_ps)
		_SIMD_TRAITS_2ND(add, _mm_add_ps)
		_SIMD_TRAITS_2ND(sub, _mm_sub_ps)
		_SIMD_TRAITS_2ND(mul, _mm_mul_ps)
		_SIMD_TRAITS_2ND(div, _mm_div_ps)
		//_SIMD_TRAITS_TWO(mod, _mm_fmod_ps)
		_SIMD_TRAITS_2ND(mod, _mm_fmod_ps_fast)
		_SIMD_TRAITS_2ND(pow, _mm_pow_ps)
		_SIMD_TRAITS_2ND(vmin, _mm_min_ps)
		_SIMD_TRAITS_2ND(vmax, _mm_max_ps)
		_SIMD_TRAITS_2ND(cross3, _mm_cross3_ps)
		
		_SIMD_TRAITS_1ST(abs,   _mm_abs_ps)
		_SIMD_TRAITS_1ST(floor, _mm_floor_ps)
		_SIMD_TRAITS_1ST(ceil,  _mm_ceil_ps)
		_SIMD_TRAITS_1ST(trunc, _mm_trunc_ps)
		_SIMD_TRAITS_1ST(round, _mm_round_ps)

		_SIMD_TRAITS_1ST(sqrt, _mm_sqrt_ps)
		_SIMD_TRAITS_1ST(cbrt, _mm_cbrt_ps)
		_SIMD_TRAITS_1ST(invsqrt, _mm_invsqrt_ps)// 1/sqrt(x)
		_SIMD_TRAITS_1ST(invcbrt, _mm_invcbrt_ps)
		_SIMD_TRAITS_1ST(exp, _mm_exp_ps)
		_SIMD_TRAITS_1ST(exp2, _mm_exp2_ps)
		_SIMD_TRAITS_1ST(exp10, _mm_exp10_ps)
		_SIMD_TRAITS_1ST(log, _mm_log_ps)
		_SIMD_TRAITS_1ST(log2, _mm_log2_ps)
		_SIMD_TRAITS_1ST(log10, _mm_log10_ps)

		_SIMD_TRAITS_1ST(log1p, _mm_log1p_ps)// logE(x+1)
		_SIMD_TRAITS_1ST(expm1, _mm_expm1_ps)// exp(x)+1
		_SIMD_TRAITS_1ST(erf, _mm_erf_ps)
		_SIMD_TRAITS_1ST(erfc, _mm_erfc_ps)
		_SIMD_TRAITS_1ST(erfinv, _mm_erfinv_ps)
		_SIMD_TRAITS_1ST(erfcinv, _mm_erfcinv_ps)

		_SIMD_TRAITS_2ND(hypot, _mm_hypot_ps)
		_SIMD_TRAITS_1ST(sin, _mm_sin_ps)
		_SIMD_TRAITS_1ST(cos, _mm_cos_ps)
		_SIMD_TRAITS_1ST(tan, _mm_tan_ps)
		_SIMD_TRAITS_1ST(asin, _mm_asin_ps)
		_SIMD_TRAITS_1ST(acos, _mm_acos_ps)
		_SIMD_TRAITS_1ST(atan, _mm_atan_ps)
		_SIMD_TRAITS_2ND(atan2, _mm_atan2_ps)
		_SIMD_TRAITS_1ST(sinh, _mm_sin_ps)
		_SIMD_TRAITS_1ST(cosh, _mm_cos_ps)
		_SIMD_TRAITS_1ST(tanh, _mm_tan_ps)
		_SIMD_TRAITS_1ST(asinh, _mm_sin_ps)
		_SIMD_TRAITS_1ST(acosh, _mm_cos_ps)
		_SIMD_TRAITS_1ST(atanh, _mm_tan_ps)
		static vector_type sincos(vector_type* C, vector_type V) { return _mm_sincos_ps(C, V); }

		static std::string to_string(vector_type _A) { return _mm_to_string_ps(_A); }
	};

	template<>
	struct simd_traits<__m128i> {
		using scalar_type = int32_t;
		using vector_type = __m128i;

		constexpr static auto scalar_align = static_cast<size_t>(4);

		static vector_type set1(int32_t _All) { return _mm_set1_epi32(_All); }
		static vector_type set (int32_t x0, int32_t x1, int32_t x2, int32_t x3) { return _mm_set_epi32(x3, x2, x1, x0); }
		static scalar_type get (vector_type A, size_t i) { return _mm_get_epi32(A, i); }
		static scalar_type sum(vector_type _X) { return _mm_sum_epi32(_X); }
		static scalar_type product(vector_type _X) { return _mm_product_epi32(_X); }

		_SIMD_TRAITS_LOAD(load,  _mm_load_epi32)
		_SIMD_TRAITS_LOAD(loadu, _mm_loadu_epi32)

		_SIMD_TRAITS_2ND(bit_and, _mm_and_si128)
		_SIMD_TRAITS_2ND(bit_or,  _mm_or_si128)
		_SIMD_TRAITS_2ND(bit_xor, _mm_xor_si128)
		_SIMD_TRAITS_2ND(add, _mm_add_epi32)
		_SIMD_TRAITS_2ND(sub, _mm_sub_epi32)
		_SIMD_TRAITS_2ND(mul, _mm_mul_epi32)
		_SIMD_TRAITS_2ND(div, _mm_div_epi32)
		_SIMD_TRAITS_2ND(mod, _mm_mod_epi32)
		_SIMD_TRAITS_2ND(pow, _mm_pow_epi32)
		_SIMD_TRAITS_2ND(vmin, _mm_min_epi32)
		_SIMD_TRAITS_2ND(vmax, _mm_max_epi32)
		_SIMD_TRAITS_2ND(cross3, _mm_except2_epi32)

		_SIMD_TRAITS_1ST(abs,   _mm_abs_epi32)
		_SIMD_TRAITS_1ST(floor, _mm_floor_epi32)
		_SIMD_TRAITS_1ST(ceil,  _mm_ceil_epi32)
		_SIMD_TRAITS_1ST(trunc, _mm_trunc_epi32)
		_SIMD_TRAITS_1ST(round, _mm_round_epi32)

		_SIMD_TRAITS_1ST(sqrt, _mm_sqrt_epi32)
		_SIMD_TRAITS_1ST(cbrt, _mm_cbrt_epi32)
		_SIMD_TRAITS_1ST(invsqrt, _mm_except_epi32)
		_SIMD_TRAITS_1ST(invcbrt, _mm_except_epi32)
		_SIMD_TRAITS_1ST(exp, _mm_exp_epi32)
		_SIMD_TRAITS_1ST(exp2, _mm_exp2_epi32)
		_SIMD_TRAITS_1ST(exp10, _mm_exp10_epi32)
		_SIMD_TRAITS_1ST(log, _mm_log_epi32)
		_SIMD_TRAITS_1ST(log2, _mm_log2_epi32)
		_SIMD_TRAITS_1ST(log10, _mm_log10_epi32)

		_SIMD_TRAITS_1ST(log1p, _mm_log1p_epi32)// logE(x+1)
		_SIMD_TRAITS_1ST(expm1, _mm_expm1_epi32)// exp(x)+1
		_SIMD_TRAITS_1ST(erf, _mm_erf_epi32)
		_SIMD_TRAITS_1ST(erfc, _mm_erfc_epi32)
		_SIMD_TRAITS_1ST(erfinv, _mm_erfinv_epi32)
		_SIMD_TRAITS_1ST(erfcinv, _mm_erfcinv_epi32)

		_SIMD_TRAITS_2ND(hypot, _mm_hypot_epi32)
		_SIMD_TRAITS_1ST(sin, _mm_sin_epi32)
		_SIMD_TRAITS_1ST(cos, _mm_cos_epi32)
		_SIMD_TRAITS_1ST(tan, _mm_tan_epi32)
		_SIMD_TRAITS_1ST(asin, _mm_asin_epi32)
		_SIMD_TRAITS_1ST(acos, _mm_acos_epi32)
		_SIMD_TRAITS_1ST(atan, _mm_atan_epi32)
		_SIMD_TRAITS_2ND(atan2, _mm_atan2_epi32)
		_SIMD_TRAITS_1ST(sinh, _mm_sin_epi32)
		_SIMD_TRAITS_1ST(cosh, _mm_cos_epi32)
		_SIMD_TRAITS_1ST(tanh, _mm_tan_epi32)
		_SIMD_TRAITS_1ST(asinh, _mm_sin_epi32)
		_SIMD_TRAITS_1ST(acosh, _mm_cos_epi32)
		_SIMD_TRAITS_1ST(atanh, _mm_tan_epi32)
		static vector_type sincos(vector_type* C, vector_type V) { return V; }

		static std::string to_string(vector_type _A) { return _mm_to_string_epi32(_A); }
	};
	
	template<>
	struct simd_traits<__m256> {
		using scalar_type = float;
		using vector_type = __m256;

		constexpr static auto scalar_align = static_cast<size_t>(8);

		static vector_type set1(float _All) { return _mm256_set1_ps(_All); }
		static vector_type set(float x0, float x1, float x2, float x3, float x4, float x5, float x6, float x7) { return _mm256_set_ps(x7, x6, x5, x4, x3, x2, x1, x0); }
		static scalar_type get(vector_type A, size_t i) { return _mm256_get_ps(A, i); }
		static scalar_type sum(vector_type _X) { return _mm256_sum_ps(_X); }
		static scalar_type product(vector_type _X) { return _mm256_product_ps(_X); }

		_SIMD_TRAITS_LOAD(load, _mm256_load_ps)
		_SIMD_TRAITS_LOAD(loadu, _mm256_loadu_ps)

		_SIMD_TRAITS_2ND(bit_and, _mm256_and_ps)
		_SIMD_TRAITS_2ND(bit_or, _mm256_or_ps)
		_SIMD_TRAITS_2ND(bit_xor, _mm256_xor_ps)
		_SIMD_TRAITS_2ND(add, _mm256_add_ps)
		_SIMD_TRAITS_2ND(sub, _mm256_sub_ps)
		_SIMD_TRAITS_2ND(mul, _mm256_mul_ps)
		_SIMD_TRAITS_2ND(div, _mm256_div_ps)
		_SIMD_TRAITS_2ND(mod, _mm256_fmod_ps)
		_SIMD_TRAITS_2ND(pow, _mm256_pow_ps)
		_SIMD_TRAITS_2ND(vmin, _mm256_min_ps)
		_SIMD_TRAITS_2ND(vmax, _mm256_max_ps)
		_SIMD_TRAITS_2ND(cross3, _mm256_cross3_ps)

		_SIMD_TRAITS_1ST(abs, _mm256_abs_ps)
		_SIMD_TRAITS_1ST(floor, _mm256_floor_ps)
		_SIMD_TRAITS_1ST(ceil,  _mm256_ceil_ps)
		_SIMD_TRAITS_1ST(trunc, _mm256_trunc_ps)
		//_SIMD_TRAITS_1ST(round, _mm256_round_ps) ERROR
		vector_type round(vector_type A) { return _mm256_round_ps(A, _MM_FROUND_NINT); };

		_SIMD_TRAITS_1ST(sqrt, _mm256_sqrt_ps)
		_SIMD_TRAITS_1ST(cbrt, _mm256_cbrt_ps)
		_SIMD_TRAITS_1ST(invsqrt, _mm256_invsqrt_ps)// 1/sqrt(x)
		_SIMD_TRAITS_1ST(invcbrt, _mm256_invcbrt_ps)
		_SIMD_TRAITS_1ST(exp, _mm256_exp_ps)
		_SIMD_TRAITS_1ST(exp2, _mm256_exp2_ps)
		_SIMD_TRAITS_1ST(exp10, _mm256_exp10_ps)
		_SIMD_TRAITS_1ST(log, _mm256_log_ps)
		_SIMD_TRAITS_1ST(log2, _mm256_log2_ps)
		_SIMD_TRAITS_1ST(log10, _mm256_log10_ps)

		_SIMD_TRAITS_1ST(log1p, _mm256_log1p_ps)// logE(x+1)
		_SIMD_TRAITS_1ST(expm1, _mm256_expm1_ps)// exp(x)+1
		_SIMD_TRAITS_1ST(erf, _mm256_erf_ps)
		_SIMD_TRAITS_1ST(erfc, _mm256_erfc_ps)
		_SIMD_TRAITS_1ST(erfinv, _mm256_erfinv_ps)
		_SIMD_TRAITS_1ST(erfcinv, _mm256_erfcinv_ps)

		_SIMD_TRAITS_2ND(hypot, _mm256_hypot_ps)
		_SIMD_TRAITS_1ST(sin, _mm256_sin_ps)
		_SIMD_TRAITS_1ST(cos, _mm256_cos_ps)
		_SIMD_TRAITS_1ST(tan, _mm256_tan_ps)
		_SIMD_TRAITS_1ST(asin, _mm256_asin_ps)
		_SIMD_TRAITS_1ST(acos, _mm256_acos_ps)
		_SIMD_TRAITS_1ST(atan, _mm256_atan_ps)
		_SIMD_TRAITS_2ND(atan2, _mm256_atan2_ps)
		_SIMD_TRAITS_1ST(sinh, _mm256_sin_ps)
		_SIMD_TRAITS_1ST(cosh, _mm256_cos_ps)
		_SIMD_TRAITS_1ST(tanh, _mm256_tan_ps)
		_SIMD_TRAITS_1ST(asinh, _mm256_sin_ps)
		_SIMD_TRAITS_1ST(acosh, _mm256_cos_ps)
		_SIMD_TRAITS_1ST(atanh, _mm256_tan_ps)
		static vector_type sincos(vector_type* C, vector_type V) { return _mm256_sincos_ps(C, V); }

		static std::string to_string(vector_type _A) { return _mm256_to_string_ps(_A); }
	};

	template<>
	struct simd_traits<__m256d> {
		using scalar_type = double;
		using vector_type = __m256d;

		constexpr static auto scalar_align = static_cast<size_t>(4);

		static vector_type set1(double _All) { return _mm256_set1_pd(_All); }
		static vector_type set (double x0, double x1, double x2, double x3) { return _mm256_set_pd(x3, x2, x1, x0); }
		static scalar_type get (vector_type A, size_t i) { return _mm256_get_pd(A, i); }
		static scalar_type sum(vector_type _X) { return _mm256_sum_pd(_X); }
		static scalar_type product(vector_type _X) { return _mm256_product_pd(_X); }

		_SIMD_TRAITS_LOAD(load,  _mm256_load_pd)
		_SIMD_TRAITS_LOAD(loadu, _mm256_loadu_pd)

		_SIMD_TRAITS_2ND(bit_and, _mm256_and_pd)
		_SIMD_TRAITS_2ND(bit_or,  _mm256_or_pd)
		_SIMD_TRAITS_2ND(bit_xor, _mm256_xor_pd)
		_SIMD_TRAITS_2ND(add, _mm256_add_pd)
		_SIMD_TRAITS_2ND(sub, _mm256_sub_pd)
		_SIMD_TRAITS_2ND(mul, _mm256_mul_pd)
		_SIMD_TRAITS_2ND(div, _mm256_div_pd)
		//_SIMD_TRAITS_TWO(mod, _mm256_fmod_pd)
		_SIMD_TRAITS_2ND(mod, _mm256_fmod_pd_fast)
		_SIMD_TRAITS_2ND(pow, _mm256_pow_pd)
		_SIMD_TRAITS_2ND(vmin, _mm256_min_pd)
		_SIMD_TRAITS_2ND(vmax, _mm256_max_pd)
		_SIMD_TRAITS_2ND(cross3, _mm256_cross3_pd)

		_SIMD_TRAITS_1ST(abs,   _mm256_abs_pd)
		_SIMD_TRAITS_1ST(floor, _mm256_floor_pd)
		_SIMD_TRAITS_1ST(ceil,  _mm256_ceil_pd)
		_SIMD_TRAITS_1ST(trunc, _mm256_trunc_pd)
		//_SIMD_TRAITS_1ST(round, _mm256_round_pd) ERROR
		vector_type round(vector_type A) { return _mm256_round_pd(A, _MM_FROUND_NINT); };

		_SIMD_TRAITS_1ST(sqrt, _mm256_sqrt_pd)
		_SIMD_TRAITS_1ST(cbrt, _mm256_cbrt_pd)
		_SIMD_TRAITS_1ST(invsqrt, _mm256_invsqrt_pd)// 1/sqrt(x)
		_SIMD_TRAITS_1ST(invcbrt, _mm256_invcbrt_pd)
		_SIMD_TRAITS_1ST(exp, _mm256_exp_pd)
		_SIMD_TRAITS_1ST(exp2, _mm256_exp2_pd)
		_SIMD_TRAITS_1ST(exp10, _mm256_exp10_pd)
		_SIMD_TRAITS_1ST(log, _mm256_log_pd)
		_SIMD_TRAITS_1ST(log2, _mm256_log2_pd)
		_SIMD_TRAITS_1ST(log10, _mm256_log10_pd)

		_SIMD_TRAITS_1ST(log1p, _mm256_log1p_pd)// logE(x+1)
		_SIMD_TRAITS_1ST(expm1, _mm256_expm1_pd)// exp(x)+1
		_SIMD_TRAITS_1ST(erf, _mm256_erf_pd)
		_SIMD_TRAITS_1ST(erfc, _mm256_erfc_pd)
		_SIMD_TRAITS_1ST(erfinv, _mm256_erfinv_pd)
		_SIMD_TRAITS_1ST(erfcinv, _mm256_erfcinv_pd)

		_SIMD_TRAITS_2ND(hypot, _mm256_hypot_pd)
		_SIMD_TRAITS_1ST(sin, _mm256_sin_pd)
		_SIMD_TRAITS_1ST(cos, _mm256_cos_pd)
		_SIMD_TRAITS_1ST(tan, _mm256_tan_pd)
		_SIMD_TRAITS_1ST(asin, _mm256_asin_pd)
		_SIMD_TRAITS_1ST(acos, _mm256_acos_pd)
		_SIMD_TRAITS_1ST(atan, _mm256_atan_pd)
		_SIMD_TRAITS_2ND(atan2, _mm256_atan2_pd)
		_SIMD_TRAITS_1ST(sinh, _mm256_sin_pd)
		_SIMD_TRAITS_1ST(cosh, _mm256_cos_pd)
		_SIMD_TRAITS_1ST(tanh, _mm256_tan_pd)
		_SIMD_TRAITS_1ST(asinh, _mm256_sin_pd)
		_SIMD_TRAITS_1ST(acosh, _mm256_cos_pd)
		_SIMD_TRAITS_1ST(atanh, _mm256_tan_pd)
		static vector_type sincos(vector_type* C, vector_type V) { return _mm256_sincos_pd(C, V); }

		static std::string to_string(vector_type _A) { return _mm256_to_string_pd(_A); }
	};


#ifdef _DEBUG
#define clmagic_debug_assert_aligned16(N) assert((N & (16 -1)) == 0)
#else
#define clmagic_debug_assert_aligned16(N)
#endif

#define simd_vecN simd_vec_<_Ty, _Size>

	/*
	@_Note: only contains stl-simd operation
	*/
	template<typename _Ty, size_t _Size>// template sort: _Ty important than _Size
	class simd_vec_ {
	private:
		static_assert( is_simd_v<_Ty>, "[Unsupported SIMD type, Please implement by yourself, @SIMDVec_<>]" );

		using _Mytraits = simd_traits<_Ty>;

		constexpr static auto _Scalar_size = _Mytraits::scalar_align * _Size;

	public:
		using scalar_type          = typename _Mytraits::scalar_type;/* !!!important */
		using vector_type          = typename _Mytraits::vector_type;
		using scalar_pointer       = scalar_type*;
		using scalar_const_pointer = scalar_type const*;

		using siterator       = typename std::_Array_iterator<scalar_type, _Scalar_size>;
		using const_siterator = typename std::_Array_const_iterator<scalar_type, _Scalar_size>;
		using viterator       = typename std::_Array_iterator<vector_type, _Size>;
		using const_viterator = typename std::_Array_const_iterator<vector_type, _Size>;

		static constexpr size_t size() {
			return _Scalar_size; }
		static constexpr size_t vector_size() {
			return _Size; }
		static constexpr size_t align() { 
			return _Mytraits::scalar_align; }

		void assign(scalar_type _Val);
		void assign(scalar_const_pointer _Ptr);
		void assign(_STD initializer_list<scalar_type> _Ilist);
		void assign(_STD initializer_list<vector_type> _IVlist);
		template<typename _Iter> void assign(_Iter _First, _Iter _Last);// !!!scalar_iterator
		template<typename _Fn> void assign(const simd_vecN& _Left, _Fn _Func);
		template<typename _Fn> void assign(const simd_vecN& _Left, const simd_vecN& _Right, _Fn _Func);
		template<typename _Fn> void assign(const simd_vecN& _Left, scalar_type _Right, _Fn _Func);
		template<typename _Fn> void assign(scalar_type _Left, const simd_vecN& _Right, _Fn _Func);

		/* <construct> */
		simd_vec_();
		explicit simd_vec_(scalar_const_pointer _Ptr);
		explicit simd_vec_(scalar_type _All);
		simd_vec_(_STD initializer_list<scalar_type> _Ilist);
		simd_vec_(_STD initializer_list<vector_type> _IVlist);
		template<typename _Iter> simd_vec_(_Iter _First, _Iter _Last);
		template<typename _Fn> simd_vec_(const simd_vecN& _Left, _Fn _Func);
		template<typename _Fn> simd_vec_(const simd_vecN& _Left, const simd_vecN& _Right, _Fn _Func);
		template<typename _Fn> simd_vec_(const simd_vecN& _Left, scalar_type _Right, _Fn _Func);
		template<typename _Fn> simd_vec_(scalar_type _Left, const simd_vecN& _Right, _Fn _Func);
		/* </construct> */

		operator vector_type() const { 
			return _Myvec[0]; }

		scalar_pointer ptr(size_t _Pos = 0) { 
			return (reinterpret_cast<scalar_pointer>(&_Myvec) + _Pos); }
		scalar_const_pointer ptr(size_t _Pos = 0) const { 
			return (reinterpret_cast<scalar_const_pointer>(&_Myvec) + _Pos); }
		scalar_type& operator[](size_t _Pos) { 
			return (*this->ptr(_Pos)); }
		const scalar_type& operator[](size_t _Pos) const {
			return (*this->ptr(_Pos)); }
		vector_type& _Get_simd_vector(size_t _Pos) {
			return _Myvec[_Pos]; }
		const vector_type& _Get_simd_vector(size_t _Pos) const {
			return _Myvec[_Pos]; }

		siterator begin() { 
			return siterator(this->ptr(0), 0/*_Off*/); }
		siterator end()   { 
			return siterator(this->ptr(0), this->size()/*_Off*/); }
		const_siterator begin() const { 
			return const_siterator(this->ptr(0), 0/*_Off*/); }
		const_siterator end() const   { 
			return const_siterator(this->ptr(0), this->size()/*_Off*/); }
		viterator vbegin() { 
			return viterator(_Myvec, 0); }
		viterator vend()   { 
			return viterator(_Myvec, this->vector_size()); }
		const_viterator vbegin() const { 
			return const_viterator(_Myvec, 0); }
		const_viterator vend() const   { 
			return const_viterator(_Myvec, this->vector_size()); }

#ifdef clmagic_math_CLMAGIC_h_
		template<typename ..._Tys>
		auto shuffle(_Tys... _Selector) const {
			using _OutTy = simd_vec_<_Ty, constexpr_align_div(types_size_v<_Tys...>, _Mytraits::scalar_align)>;
			return clmagic::shuffle<_OutTy>(*this, _Selector...);
		}
#endif
		
		simd_vecN operator-() const;
		simd_vecN operator&(const simd_vecN& _Right) const;
		simd_vecN operator|(const simd_vecN& _Right) const;
		simd_vecN operator^(const simd_vecN& _Right) const;
		simd_vecN operator+(const simd_vecN& _Right) const;
		simd_vecN operator-(const simd_vecN& _Right) const;
		simd_vecN operator*(const simd_vecN& _Right) const;
		simd_vecN operator/(const simd_vecN& _Right) const;
		simd_vecN operator+(scalar_type _Right) const;
		simd_vecN operator-(scalar_type _Right) const;
		simd_vecN operator*(scalar_type _Right) const;
		simd_vecN operator/(scalar_type _Right) const;
		simd_vecN& operator&=(const simd_vecN& _B);
		simd_vecN& operator|=(const simd_vecN& _B);
		simd_vecN& operator^=(const simd_vecN& _B);
		simd_vecN& operator+=(const simd_vecN& _B);
		simd_vecN& operator-=(const simd_vecN& _B);
		simd_vecN& operator*=(const simd_vecN& _B);
		simd_vecN& operator/=(const simd_vecN& _B);
		simd_vecN& operator+=(scalar_type b);
		simd_vecN& operator-=(scalar_type b);
		simd_vecN& operator*=(scalar_type b);
		simd_vecN& operator/=(scalar_type b);
		scalar_type sum() const;// ∑A[i]
		scalar_type product() const;// ∏A[i]
		scalar_type normL1() const;// ||A||₁ = ∑|A[i]|
		scalar_type normL2_sq() const;// ∑A[i]*A[i]
		scalar_type normL2() const;// ||A||₂ = sqrt(∑A[i]²)
		scalar_type norm(size_t p) const;// ||A||p = pow( ∑pow(|A[i]|,p), 1/p)
		simd_vecN&  normalize ();// A/=normL2(A)

	private:
		vector_type _Myvec[_Size];
	};// struct simd_vec_<>

	//template<size_t N> using simd_ivec = simd_vec_<N, int>;
	template<size_t _Size> using simd128_fvec = simd_vec_<__m128, _Size>;
	template<size_t _Size> using simd128_ivec = simd_vec_<__m128i, _Size>;
	template<size_t _Size> using simd256_fvec = simd_vec_<__m256, _Size>;
	template<size_t _Size> using simd256_dvec = simd_vec_<__m256d, _Size>;

#define SCALAR_TYPE typename simd_vec_<_Ty, _Size>::scalar_type
	
	template<typename _Ty, size_t _Size>
		_STD string to_string(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		_STD ostream& operator<<(_STD ostream& _Ostr, const simd_vecN& _X);

	template<typename _Ty, size_t _Size>
		simd_vecN mod(const simd_vecN& _Left, const simd_vecN& _Right);
	template<typename _Ty, size_t _Size>
		simd_vecN pow(const simd_vecN& _Left, const simd_vecN& _Right);
	template<typename _Ty, size_t _Size>
		simd_vecN vmin(const simd_vecN& _Left, const simd_vecN& _Right);
	template<typename _Ty, size_t _Size>
		simd_vecN vmax(const simd_vecN& _Left, const simd_vecN& _Right);
	template<typename _Ty, size_t _Size>
		simd_vecN cross3(const simd_vecN& _Left, const simd_vecN& _Right);
	template<typename _Ty, size_t _Size>
		SCALAR_TYPE dot(const simd_vecN& _Left, const simd_vecN& _Right);

	template<typename _Ty, size_t _Size>
		simd_vecN mod(const simd_vecN& _Left, SCALAR_TYPE _Right);
	template<typename _Ty, size_t _Size>
		simd_vecN pow(const simd_vecN& _Left, SCALAR_TYPE _Right);// why not have pow(scalar, vector)? the function should is EXP_()

	template<typename _Ty, size_t _Size>
		simd_vecN operator+(SCALAR_TYPE _Left, const simd_vecN& _Right);
	template<typename _Ty, size_t _Size>
		simd_vecN operator-(SCALAR_TYPE _Left, const simd_vecN& _Right);
	template<typename _Ty, size_t _Size>
		simd_vecN operator*(SCALAR_TYPE _Left, const simd_vecN& _Right);
	template<typename _Ty, size_t _Size>
		simd_vecN operator/(SCALAR_TYPE _Left, const simd_vecN& _Right);
	template<typename _Ty, size_t _Size>
		simd_vecN mod(SCALAR_TYPE _Left, const simd_vecN& _Right);

	template<typename _Ty, size_t _Size>
		simd_vecN abs(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN floor(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN ceil(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN trunc(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN round(const simd_vecN& _X);

	template<typename _Ty, size_t _Size>
		simd_vecN sqrt(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN cbrt(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN invsqrt(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN invcbrt(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN log(const simd_vecN& _X);// log(base, X), log(e, _X)
	template<typename _Ty, size_t _Size>
		simd_vecN log2(const simd_vecN& _X);// log(2, _X)
	template<typename _Ty, size_t _Size>
		simd_vecN log10(const simd_vecN& _X);// log(10, _X)
	template<typename _Ty, size_t _Size>
		simd_vecN exp(const simd_vecN& _X);// pow(e, _X)
	template<typename _Ty, size_t _Size>
		simd_vecN exp2(const simd_vecN& _X);// pow(2, _X)
	template<typename _Ty, size_t _Size>
		simd_vecN exp10(const simd_vecN& _X);// pow(10, _X)
	template<typename _Ty, size_t _Size>
		simd_vecN log1p(const simd_vecN& _X);// log(_X+1)
	template<typename _Ty, size_t _Size>
		simd_vecN expm1(const simd_vecN& _X);// exp(_X)-1
	template<typename _Ty, size_t _Size>
		simd_vecN erf(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN erfc(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN erfinv(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN erfcinv(const simd_vecN& _X);

	template<typename _Ty, size_t _Size>
		simd_vecN hypot(const simd_vecN& _A, const simd_vecN& _B);// sqrt(_A², _B²)
	template<typename _Ty, size_t _Size>
		simd_vecN sin(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN cos(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN tan(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN sincos(simd_vecN& _Cos, const simd_vecN& _X);// return _Sin
	template<typename _Ty, size_t _Size>
		simd_vecN asin(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN acos(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN atan(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN atan2(const simd_vecN& _Y, const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN sinh(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN cosh(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN tanh(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN asinh(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN acosh(const simd_vecN& _X);
	template<typename _Ty, size_t _Size>
		simd_vecN atanh(const simd_vecN& _X);

	template<typename _Ty, size_t _Size>
		SCALAR_TYPE length(const simd_vecN _X);
	template<typename _Ty, size_t _Size>
		simd_vecN normalize(const simd_vecN _X);

#undef SCALAR_TYPE

}// namespace clmagic;


namespace clmagic {
	/* @_Note: Sperate definition and implement because all function can be large than now */
	
	/* @_Macro: Not use Macro, because VisualStudio compiler error tips--[Function definition for XXX not found].
	   It not have expands Macro when scancode */

	template<typename _Ty, size_t _Size> inline
	void simd_vec_<_Ty, _Size>::assign(scalar_type _Val) {
		std::fill(this->begin(), this->end(), _Val);
	}

	template<typename _Ty, size_t _Size> inline
	void simd_vec_<_Ty, _Size>::assign(scalar_const_pointer _Ptr) {
		if ( (size_t(_Ptr) & (16 - 1)) == 0 ) {// is aligned 16
			for (auto _First = this->vbegin(); _First != this->vend(); ++_First, _Ptr += sizeof(vector_type)) {
				*_First = _Mytraits::load(_Ptr);
			}
		} else {
			for (auto _First = this->vbegin(); _First != this->vend(); ++_First, _Ptr += sizeof(vector_type)) {
				*_First = _Mytraits::loadu(_Ptr);
			}
		}
	}

	template<typename _Ty, size_t _Size> inline
	void simd_vec_<_Ty, _Size>::assign(_STD initializer_list<scalar_type> _Ilist) {
		assert(_Ilist.size() <= this->size());
		std::copy(_Ilist.begin(), _Ilist.end(), this->begin());
	}

	template<typename _Ty, size_t _Size> inline
	void simd_vec_<_Ty, _Size>::assign(_STD initializer_list<vector_type> _IVlist) {
		assert( _IVlist.size() <= this->vector_size() );
		std::copy(_IVlist.begin(), _IVlist.end(), this->vbegin());
	}

	template<typename _Ty, size_t _Size> template<typename _Iter> inline
	void simd_vec_<_Ty, _Size>::assign(_Iter _First, _Iter _Last) {
		assert(std::distance(_First, _Last) <= static_cast<std::_Iter_diff_t<_Iter>>(this->size()));
		std::copy(_First, _Last, this->begin());
	}

	template<typename _Ty, size_t _Size> template<typename _Fn> inline
	void simd_vec_<_Ty, _Size>::assign(const simd_vecN& _Left, _Fn _Func) {// this = _Func(A[i])
		std::transform(_Left.vbegin(), _Left.vend(), this->vbegin(), _Func);
	}

	template<typename _Ty, size_t _Size> template<typename _Fn> inline
	void simd_vec_<_Ty, _Size>::assign(const simd_vecN& _Left, const simd_vecN& _Right, _Fn _Func) {// this = _Func(A[i], B[i])
		std::transform(_Left.vbegin(), _Left.vend(), _Right.vbegin(), this->vbegin(), _Func);
	}

	template<typename _Ty, size_t _Size> template<typename _Fn> inline
	void simd_vec_<_Ty, _Size>::assign(const simd_vecN& _Left, scalar_type _Right, _Fn _Func) {	// this = _Func(A[i], B)
		const auto _X     = _Mytraits::set1(_Right);
		auto       _First = _Left.vbegin();
		auto       _Last  = _Left.vend();
		auto       _Dest  = this->vbegin();
		for (; _First != _Last; ++_First, ++_Dest) {
			*_Dest = _Func(*_First, _X);
		}
	}

	template<typename _Ty, size_t _Size> template<typename _Fn> inline
	void simd_vec_<_Ty, _Size>::assign(scalar_type _Left, const simd_vecN& _Right, _Fn _Func) {	// this = _Func(_Left, _Right)
		const auto _X     = _Mytraits::set1(_Left);
		auto       _First = _Right.vbegin();
		auto       _Last  = _Right.vend();
		auto       _Dest  = this->vbegin();
		for ( ; _First != _Last; ++_First, ++_Dest) {
			*_Dest = _Func(_X, *_First);
		}
	}


	template<typename _Ty, size_t _Size> inline
	simd_vec_<_Ty, _Size>::simd_vec_() : _Myvec{ 0 } {
		clmagic_debug_assert_aligned16(size_t(this));
	}

	template<typename _Ty, size_t _Size> inline
	simd_vec_<_Ty, _Size>::simd_vec_(scalar_const_pointer _Ptr) : simd_vec_() { this->assign(_Ptr); }

	template<typename _Ty, size_t _Size> inline
	simd_vec_<_Ty, _Size>::simd_vec_(scalar_type _Val) : simd_vec_() { this->assign(_Val); }

	template<typename _Ty, size_t _Size> inline
	simd_vec_<_Ty, _Size>::simd_vec_(_STD initializer_list<scalar_type> _Ilist) : simd_vec_() { this->assign(_Ilist); }
	
	template<typename _Ty, size_t _Size> inline
	simd_vec_<_Ty, _Size>::simd_vec_(_STD initializer_list<vector_type> _IVlist) : simd_vec_() { this->assign(_IVlist); }

	template<typename _Ty, size_t _Size> template<typename _Iter> inline
	simd_vec_<_Ty, _Size>::simd_vec_(_Iter _First, _Iter _Last) : simd_vec_() { this->assign(_First, _Last); }

	template<typename _Ty, size_t _Size> template<typename _Fn> inline
	simd_vec_<_Ty, _Size>::simd_vec_(const simd_vecN& _Left, _Fn _Func) : simd_vec_() { this->assign(_Left, _Func); }

	template<typename _Ty, size_t _Size> template<typename _Fn> inline
	simd_vec_<_Ty, _Size>::simd_vec_(const simd_vecN& _Left, const simd_vecN& _Right, _Fn _Func) : simd_vec_() { this->assign(_Left, _Right, _Func); }

	template<typename _Ty, size_t _Size> template<typename _Fn> inline
	simd_vec_<_Ty, _Size>::simd_vec_(const simd_vecN& _Left, scalar_type _Right, _Fn _Func) : simd_vec_() { this->assign(_Left, _Right, _Func); }

	template<typename _Ty, size_t _Size> template<typename _Fn> inline
	simd_vec_<_Ty, _Size>::simd_vec_(scalar_type _Left, const simd_vecN& _Right, _Fn _Func) : simd_vec_() { this->assign(_Left, _Right, _Func); }


/* - - - - - - - - - - - - - - - - - calculate - - -- - - - - - - - - - - - - - -*/
	template<typename _Ty, size_t _Size> inline
	simd_vecN simd_vec_<_Ty, _Size>::operator-() const {// -1 * this
		return simd_vecN(*this, static_cast<_Ty>(-1), _Mytraits::mul);
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN simd_vec_<_Ty, _Size>::operator&(const simd_vecN& _Right) const {// this & _Right
		return simd_vecN(*this, _Right, _Mytraits::bit_and);
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN simd_vec_<_Ty, _Size>::operator|(const simd_vecN& _Right) const {// this | _Right
		return simd_vecN(*this, _Right, _Mytraits::bit_or);
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN simd_vec_<_Ty, _Size>::operator^(const simd_vecN& _Right) const {// this ^ _Right
		return simd_vecN(*this, _Right, _Mytraits::bit_xor);
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN simd_vec_<_Ty, _Size>::operator+(const simd_vecN& _Right) const {// this + _Right
		return simd_vecN(*this, _Right, _Mytraits::add);
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN simd_vec_<_Ty, _Size>::operator-(const simd_vecN& _Right) const {// this - _Right
		return simd_vecN(*this, _Right, _Mytraits::sub);
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN simd_vec_<_Ty, _Size>::operator*(const simd_vecN& _Right) const {// this * _Right
		return simd_vecN(*this, _Right, _Mytraits::mul);
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN simd_vec_<_Ty, _Size>::operator/(const simd_vecN& _Right) const {// this / _Right
		return simd_vecN(*this, _Right, _Mytraits::div);
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN simd_vec_<_Ty, _Size>::operator+(scalar_type _Scalar) const {// this + vec(_Right)
		return simd_vecN(*this, _Scalar, _Mytraits::add);
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN simd_vec_<_Ty, _Size>::operator-(scalar_type _Scalar) const{// this - vec(_Right)
		return simd_vecN(*this, _Scalar, _Mytraits::sub);
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN simd_vec_<_Ty, _Size>::operator*(scalar_type _Scalar) const{// this * vec(_Right)
		return simd_vecN(*this, _Scalar, _Mytraits::mul);
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN simd_vec_<_Ty, _Size>::operator/(scalar_type _Scalar) const{// this / vec(_Right)
		return simd_vecN(*this, _Scalar, _Mytraits::div);
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN& simd_vec_<_Ty, _Size>::operator&=(const simd_vecN& _Right) {
		this->assign(*this, _Right, _Mytraits::bit_and);
		return (*this);
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN& simd_vec_<_Ty, _Size>::operator|=(const simd_vecN& _Right) {
		this->assign(*this, _Right, _Mytraits::bit_or);
		return (*this); 
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN& simd_vec_<_Ty, _Size>::operator^=(const simd_vecN& _Right) {
		this->assign(*this, _Right, _Mytraits::bit_xor);
		return (*this); 
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN& simd_vec_<_Ty, _Size>::operator+=(const simd_vecN& _Right) {
		this->assign(*this, _Right, _Mytraits::add);
		return (*this);
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN& simd_vec_<_Ty, _Size>::operator-=(const simd_vecN& _Right) {
		this->assign(*this, _Right, _Mytraits::sub);
		return (*this); 
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN& simd_vec_<_Ty, _Size>::operator*=(const simd_vecN& _Right) {
		this->assign(*this, _Right, _Mytraits::mul);
		return (*this); 
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN& simd_vec_<_Ty, _Size>::operator/=(const simd_vecN& _Right) {
		this->assign(*this, _Right, _Mytraits::div);
		return (*this);
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN& simd_vec_<_Ty, _Size>::operator+=(scalar_type _Scalar) {
		this->assign(*this, _Scalar, _Mytraits::add);
		return (*this); 
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN& simd_vec_<_Ty, _Size>::operator-=(scalar_type _Scalar) {
		this->assign(*this, _Scalar, _Mytraits::sub);
		return (*this); 
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN& simd_vec_<_Ty, _Size>::operator*=(scalar_type _Scalar) {
		this->assign(*this, _Scalar, _Mytraits::mul);
		return (*this);
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN& simd_vec_<_Ty, _Size>::operator/=(scalar_type _Scalar) {
		this->assign(*this, _Scalar, _Mytraits::div);
		return (*this); 
	}

#define SCALAR_TYPE typename simd_vec_<_Ty, _Size>::scalar_type

	template<typename _Ty, size_t _Size> inline
	SCALAR_TYPE simd_vec_<_Ty, _Size>::sum() const {// sum of all elements
		auto  _Sum_vector = this->_Get_simd_vector(0);
		for (size_t i = 1; i < this->vector_size(); ++i) {
			_Sum_vector = _Mytraits::add( _Sum_vector, this->_Get_simd_vector(i) );
		}
		return _Mytraits::sum(_Sum_vector);
	}

	template<typename _Ty, size_t _Size> inline
	SCALAR_TYPE simd_vec_<_Ty, _Size>::product() const {
		auto _Product_vector = this->_Get_simd_vector(0);
		for (size_t i = 1; i < this->vector_size(); ++i) {
			_Product_vector = _Mytraits::mul(_Product_vector, this->_Get_simd_vector(i));
		}
		return _Mytraits::product(_Product_vector);
	}

	template<typename _Ty, size_t _Size> inline
	SCALAR_TYPE simd_vec_<_Ty, _Size>::normL1() const {// ||A||₁ = ∑|A[i]|
		auto _Tmp           = this->_Get_simd_vector(0);
		auto _NormL1_vector = _Mytraits::abs(_Tmp);
		for (size_t i = 1; i < this->vector_size(); ++i) {
			_Tmp           = this->_Get_simd_vector(i);
			_NormL1_vector = _Mytraits::add( _NormL1_vector, _Mytraits::abs(_Tmp) );
		}
		return _Mytraits::sum(_NormL1_vector);
	}

	template<typename _Ty, size_t _Size> inline
	SCALAR_TYPE simd_vec_<_Ty, _Size>::normL2_sq() const {// ||A||₂² = ∑A[i]*A[i]
		auto _Tmp           = this->_Get_simd_vector(0);
		auto _NormL1_vector = _Mytraits::mul(_Tmp, _Tmp);
		for (size_t i = 1; i < this->vector_size(); ++i) {
			_Tmp           = this->_Get_simd_vector(i);
			_NormL1_vector = _Mytraits::add( _NormL1_vector, _Mytraits::mul(_Tmp, _Tmp) );
		}
		return _Mytraits::sum(_NormL1_vector);
	}

	template<typename _Ty, size_t _Size> inline
	SCALAR_TYPE simd_vec_<_Ty, _Size>::normL2() const {// ||A||₂ = sqrt(∑A[i]*A[i])
		return static_cast<scalar_type>(_CSTD sqrt(this->normL2_sq()));
	}

	template<typename _Ty, size_t _Size> inline
	SCALAR_TYPE simd_vec_<_Ty, _Size>::norm(size_t p) const {// ΣVi
		if (p == 1) {
			return this->normL1();
		} else if (p == 2) {
			return this->normL2();
		} else {
			const auto _Exp         = static_cast<scalar_type>(p);
			const auto _Exp_vector  = _Mytraits::set1(_Exp);
			auto       _Tmp         = this->_Get_simd_vector(0);
			auto       _Norm_vector = _Mytraits::pow(_Mytraits::abs(_Tmp), _Exp);
			for (size_t i = 1; i != this->vector_size(); ++i) {
				_Tmp         = this->_Get_simd_vector(i);
				_Norm_vector = _Mytraits::add(_Norm_vector, _Mytraits::pow(_Mytraits::abs(_Tmp), _Exp));
			}
			return _CSTD pow(_Mytraits::sum(_Norm_vector), static_cast<scalar_type>(1.0) / _Exp);
		}
	}

	template<typename _Ty, size_t _Size> inline
	simd_vecN& simd_vec_<_Ty, _Size>::normalize() {
		return ( (*this) /= this->normL2() );
	}

	
/* - - - - - - - - - - - - - - - - - friend - - -- - - - - - - - - - - - - - -*/
	template<typename _Ty, size_t _Size>
	_STD string to_string(const simd_vecN& _X) {
		auto _Tmp = _X._Get_simd_vector(0);
		auto _Str = simd_traits<_Ty>::to_string(_Tmp) + " ";
		for (size_t i = 1; i != _X.vector_size(); ++i) { 
			_Tmp  = _X._Get_simd_vector(i);
			_Str += simd_traits<_Ty>::to_string(_Tmp) + " ";
		}
		_Str.pop_back();// pop ' '

		return _Str;
	}

	template<typename _Ty, size_t _Size>
	_STD ostream& operator<<(_STD ostream& _Ostr, const simd_vecN& _X) {
		return (_Ostr << to_string(_X));
	}

	template<typename _Ty, size_t _Size>
	simd_vecN mod(const simd_vecN& _Left, const simd_vecN& _Right) {
		return simd_vecN(_Left, _Right, simd_traits<_Ty>::mod);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN pow(const simd_vecN& _Left, const simd_vecN& _Right) {
		return simd_vecN(_Left, _Right, simd_traits<_Ty>::pow);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN vmin(const simd_vecN& _Left, const simd_vecN& _Right) {
		return simd_vecN(_Left, _Right, simd_traits<_Ty>::vmin);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN vmax(const simd_vecN& _Left, const simd_vecN& _Right) {
		return simd_vecN(_Left, _Right, simd_traits<_Ty>::vmax);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN cross3(const simd_vecN& _Left, const simd_vecN& _Right) {
		return simd_vecN(_Left, _Right, simd_traits<_Ty>::cross3);
	}

	template<typename _Ty, size_t _Size>
	SCALAR_TYPE dot(const simd_vecN& _Left, const simd_vecN& _Right) {
		auto _First1     = _Left.vbegin();
		auto _Last1      = _Left.vend();
		auto _First2     = _Right.vbegin();
		auto _Dot_vector = simd_traits<_Ty>::mul(*_First1++, *_First2++);
		while (_First1 != _Last1) {
			_Dot_vector = simd_traits<_Ty>::mul(*_First1++, *_First2++);
		}
		return simd_traits<_Ty>::sum(_Dot_vector);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN mod(const simd_vecN& _Left, SCALAR_TYPE _Right) {
		return simd_vecN(_Left, _Right, simd_traits<_Ty>::mod);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN pow(const simd_vecN& _Left, SCALAR_TYPE _Right) {// pow(_Left, vec(_Right))
		return simd_vecN(_Left, _Right, simd_traits<_Ty>::pow);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN operator+(SCALAR_TYPE _Left, const simd_vecN& _Right) {// vec(_Left) + _Right
		return simd_vecN(_Left, _Right, simd_traits<_Ty>::add);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN operator-(SCALAR_TYPE _Left, const simd_vecN& _Right) {// vec(_Left) - _Right
		return simd_vecN(_Left, _Right, simd_traits<_Ty>::sub);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN operator*(SCALAR_TYPE _Left, const simd_vecN& _Right) {// vec(_Left) * _Right
		return simd_vecN(_Left, _Right, simd_traits<_Ty>::mul);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN operator/(SCALAR_TYPE _Left, const simd_vecN& _Right) {// vec(_Left) / _Right
		return simd_vecN(_Left, _Right, simd_traits<_Ty>::div);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN mod(SCALAR_TYPE _Left, const simd_vecN& _Right) {// vec(_Left) % _Right
		return simd_vecN(_Left, _Right, simd_traits<_Ty>::mod);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN abs(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::abs);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN floor(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::floor);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN ceil(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::ceil);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN trunc(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::trunc);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN round(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::round);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN sqrt(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::sqrt);
	}
	
	template<typename _Ty, size_t _Size>
	simd_vecN cbrt(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::cbrt);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN invsqrt(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::invsqrt);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN invcbrt(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::invcbrt);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN log(const simd_vecN& _X) {// log(e, _X)
		return simd_vecN(_X, simd_traits<_Ty>::log);
	}
	
	template<typename _Ty, size_t _Size>
	simd_vecN log2(const simd_vecN& _X) {// log(2, _X)
		return simd_vecN(_X, simd_traits<_Ty>::log2);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN log10(const simd_vecN& _X) {// log(10, _X)
		return simd_vecN(_X, simd_traits<_Ty>::log10);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN exp(const simd_vecN& _X) {// pow(e, _X)
		return simd_vecN(_X, simd_traits<_Ty>::exp);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN exp2(const simd_vecN& _X) {// pow(2, _X)
		return simd_vecN(_X, simd_traits<_Ty>::exp2);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN exp10(const simd_vecN& _X) {// pow(10, _X)
		return simd_vecN(_X, simd_traits<_Ty>::exp10);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN log1p(const simd_vecN& _X) {// log(_X+1)
		return simd_vecN(_X, simd_traits<_Ty>::log1p);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN expm1(const simd_vecN& _X) {// exp(_X)-1
		return simd_vecN(_X, simd_traits<_Ty>::expm1);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN erf(const simd_vecN& _X) {// error-function
		return simd_vecN(_X, simd_traits<_Ty>::erf);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN erfc(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::erfc);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN erfinv(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::erfinv);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN erfcinv(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::erfcinv);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN hypot(const simd_vecN& _A, const simd_vecN& _B) {// sqrt(_A², _B²)
		return simd_vecN(_A, _B, simd_traits<_Ty>::hypot);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN sin(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::sin);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN cos(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::cos);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN tan(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::tan);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN sincos(simd_vecN& _Cos, const simd_vecN& _X) {// return _Sin
		auto  _Vsine   = simd_vecN();
		auto* _Pcosine = &_Cos._Get_simd_vector(0);
		for (size_t i = 0; i != _X.vector_size(); ++i) {
			_Vsine._Get_simd_vector(i) = simd_traits<_Ty>::sincos(*_Pcosine++, _X._Get_simd_vector(i));
		}
		return _Vsine;
	}

	template<typename _Ty, size_t _Size>
	simd_vecN asin(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::asin);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN acos(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::acos);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN atan(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::atan);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN atan2(const simd_vecN& _Y, const simd_vecN& _X) {
		return simd_vecN(_Y, _X, simd_traits<_Ty>::atan2);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN sinh(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::sinh);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN cosh(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::cosh);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN tanh(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::tanh);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN asinh(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::asinh);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN acosh(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::acosh);
	}

	template<typename _Ty, size_t _Size>
	simd_vecN atanh(const simd_vecN& _X) {
		return simd_vecN(_X, simd_traits<_Ty>::atanh);
	}

	template<typename _Ty, size_t _Size>
	SCALAR_TYPE length(const simd_vecN _X) {
		return _X.normL2();
	}

	template<typename _Ty, size_t _Size>
	simd_vecN normalize(const simd_vecN _X) {
		auto _Length    = length(_X);
		auto _One       = static_cast<decltype(_Length)>(1);
		auto _Threshold = static_cast<decltype(_Length)>(0.0000'0000'002);
		if (_CSTD abs(_Length - _One) < _Threshold) {
			return (_X / _Length);
		}
		return _X;
	}

#undef SCALAR_TYPE


	/* - - - - - - optimuzation - - - - - - */
#define clmagic_simd_vec_FAST_CONSTRUCT(TY, N) \
	template<> template<typename _Fn> inline \
	simd_vec_<##TY##, ##N##>::simd_vec_(const simd_vec_<##TY##, ##N##>& _Left, _Fn _Func) \
		: _Myvec{ _Func(_Left._Get_simd_vector(0)) } { } \
	\
	template<> template<typename _Fn> inline \
	simd_vec_<##TY##, ##N##>::simd_vec_(const simd_vec_<##TY##, ##N##>& _Left, const simd_vec_<##TY##, ##N##>& _Right, _Fn _Func) \
		: _Myvec{ _Func(_Left._Get_simd_vector(0), _Right._Get_simd_vector(0)) } { } \
	\
	template<> template<typename _Fn> inline \
	simd_vec_<##TY##, ##N##>::simd_vec_(scalar_type _Left, const simd_vec_<##TY##, ##N##>& _Right, _Fn _Func) \
		: _Myvec{ _Func(_Mytraits::set1(_Left), _Right._Get_simd_vector(0)) } { } \
	\
	template<> template<typename _Fn> inline \
	simd_vec_<##TY##, ##N##>::simd_vec_(const simd_vec_<##TY##, ##N##>& _Left, scalar_type _Right, _Fn _Func) \
		: _Myvec{ _Func(_Left._Get_simd_vector(0), _Mytraits::set1(_Right)) } { } 

	clmagic_simd_vec_FAST_CONSTRUCT(__m128,  1)
	clmagic_simd_vec_FAST_CONSTRUCT(__m256d, 1)
	clmagic_simd_vec_FAST_CONSTRUCT(__m128i, 1)

#undef clmagic_simd_vec_FAST_CONSTRUCT

}// namespace clmagic

#endif