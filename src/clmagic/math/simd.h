#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH___SIMD___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH___SIMD___H__
#include "clmagic.h"
#include <string>
#include <iostream>

#include <ivec.h>
#include <fvec.h>
#include <dvec.h>

#define _ALIGN16 _declspec(align(16))


//class F32mat4 
//{
//protected:
//	F32vec4 _Mydata[4];
//public:
//	F32mat4() : _Mydata() { }
//
//	F32mat4(_in(F32vec4) _Row0, _in(F32vec4) _Row1, _in(F32vec4) _Row2, _in(F32vec4) _Row3)
//		: _Mydata{ _Row0, _Row1, _Row2, _Row3 } 
//		{
//		}
//
//	F32mat4(float _00, float _01, float _02, float _03, 
//		float _10, float _11, float _12, float _13,
//		float _20, float _21, float _22, float _23, 
//		float _30, float _31, float _32, float _33)
//		: _Mydata{ F32vec4(_00, _01, _02, _03),
//					F32vec4(_10, _11, _12, _13),
//					F32vec4(_20, _21, _22, _23),
//					F32vec4(_30, _31, _32, _33) }
//		{	// ...
//		}
//
//	friend F32mat4 operator+(_in(F32mat4) _A, _in(F32mat4) _B) {
//		return ( F32mat4(_A[0] + _B[0], _A[1] + _B[1], _A[2] + _B[2], _A[3] + _B[3]) );
//	}
//
//	friend F32mat4 operator-(_in(F32mat4) _A, _in(F32mat4) _B) {
//		return (F32mat4(_A[0] - _B[0], _A[1] - _B[1], _A[2] - _B[2], _A[3] - _B[3]));
//	}
//
//	friend F32vec4 operator*(_in(F32mat4) _A, _in(F32vec4) _B) {
//		return (F32vec4(simd_dot(_A[0], F32vec4(_B[0])),
//						simd_dot(_A[1], F32vec4(_B[1])),
//						simd_dot(_A[2], F32vec4(_B[2])),
//						simd_dot(_A[3], F32vec4(_B[3]))));
//	}
//
//	friend F32mat4 operator*(_in(F32mat4) _A, float _B) {
//		auto _Tmp = F32vec4(_B);
//		return ( F32mat4(_A[0] * _Tmp, _A[1] * _Tmp, _A[2] * _Tmp, _A[3] * _Tmp) );
//	}
//
//	friend F32mat4 operator*(_in(F32mat4) _A, _in(F32mat4) _B) {
//		/*
//			 [ 0 1 2 3 ]  [ 0       ]     [ 0        ]
//		dot( [ ...     ], [ 1       ] ) = [ ...      ]
//			 [ ...     ]  [ 2       ]     [ ...      ]
//			 [ ...     ]  [ 3       ]     [ ...      ]
//		*/
//		auto _Col0 = F32vec4(_B[0][0], _B[1][0], _B[2][0], _B[3][0]);
//		auto _Col1 = F32vec4(_B[0][1], _B[1][1], _B[2][1], _B[3][1]);
//		auto _Col2 = F32vec4(_B[0][2], _B[1][2], _B[2][2], _B[3][2]);
//		auto _Col3 = F32vec4(_B[0][3], _B[1][3], _B[2][3], _B[3][3]);
//		return ( F32mat4( simd_dot(_A[0], _Col0), simd_dot(_A[0], _Col1), simd_dot(_A[0], _Col2), simd_dot(_A[0], _Col3),
//					      simd_dot(_A[1], _Col0), simd_dot(_A[1], _Col1), simd_dot(_A[1], _Col2), simd_dot(_A[1], _Col3), 
//						  simd_dot(_A[2], _Col0), simd_dot(_A[2], _Col1), simd_dot(_A[2], _Col2), simd_dot(_A[2], _Col3), 
//						  simd_dot(_A[3], _Col0), simd_dot(_A[3], _Col1), simd_dot(_A[3], _Col2), simd_dot(_A[3], _Col3)) );
//	}
//
//	F32vec4& operator[](size_t _Pos) {
//		return (_Mydata[_Pos]);
//	}
//
//	const F32vec4& operator[]( size_t _Pos ) const {
//		return ( _Mydata[_Pos] );
//	}
//
//};// class F32mat4


inline __m128 _mm_cross3_ps(__m128 _A, __m128 _B)
	{
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

inline __m256d _mm256_cross3_pd(__m256d _A, __m256d _B)
	{
	auto _Lhs = _mm256_set_pd(_A.m256d_f64[0], _A.m256d_f64[0], _A.m256d_f64[2], _A.m256d_f64[1]);
	auto _Rhs = _mm256_set_pd(_B.m256d_f64[0], _B.m256d_f64[1], _B.m256d_f64[0], _B.m256d_f64[2]);
	auto _Result = _mm256_mul_pd(_Lhs, _Rhs);
	_Lhs = _mm256_set_pd(_A.m256d_f64[0], _A.m256d_f64[1], _A.m256d_f64[0], _A.m256d_f64[2]);
	_Rhs = _mm256_set_pd(_B.m256d_f64[0], _B.m256d_f64[0], _B.m256d_f64[2], _B.m256d_f64[1]);
	_Result = _mm256_sub_pd(_Result, _mm256_mul_pd(_Lhs, _Rhs));

	return (_Result);
	}


namespace clmagic {

	template<typename _Ty>
		constexpr bool is_simd_v = std::_Is_any_of_v< 
			std::remove_cv_t<_Ty>, 
			float, 
			double, 
			int>;


	/*
	@_Note: only contains stl-simd operation
	*/
	template<typename _Ty>
	struct SIMDVec4_ {
		static_assert( is_simd_v<_Ty>, "[Unsupported SIMD type, Please implement by yourself, @SIMDVec4_<>]" );

		using value_type  = _Ty;
		using vector_type = __m128;

		SIMDVec4_()  { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }

		explicit SIMDVec4_(value_type _All) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		
		SIMDVec4_(value_type _Val0, value_type _Val1, value_type _Val2, value_type _Val3) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		
		SIMDVec4_(_in(vector_type) _Vec) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		
		operator vector_type() const { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }

		    size_t  size() const { return (4); }
		      _Ty*  ptr()           { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		const _Ty*  ptr() const        { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
			  _Ty*  ptr(size_t _Pos)      { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		const _Ty*  ptr(size_t _Pos) const   { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		vector_type simd_vector() const         { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		      _Ty&  operator[](size_t _Pos)        { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		const _Ty&  operator[](size_t _Pos) const     { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }

		friend SIMDVec4_ operator&(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ operator|(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ operator^(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ operator+(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ operator-(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ operator*(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ operator*(_in(SIMDVec4_) _A,    value_type  _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ operator/(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ operator/(_in(SIMDVec4_) _A,    value_type  _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }

		SIMDVec4_& operator&=(_in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		SIMDVec4_& operator|=(_in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		SIMDVec4_& operator^=(_in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		SIMDVec4_& operator+=(_in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		SIMDVec4_& operator-=(_in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		SIMDVec4_& operator*=(_in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		SIMDVec4_& operator*=(   value_type  _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		SIMDVec4_& operator/=(_in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		SIMDVec4_& operator/=(   value_type  _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }

		friend SIMDVec4_ mod  (_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ mod  (_in(SIMDVec4_) _A, value_type _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ floor(_in(SIMDVec4_) _A) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ ceil (_in(SIMDVec4_) _A) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ trunc(_in(SIMDVec4_) _A) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		
		friend SIMDVec4_ sqrt (_in(SIMDVec4_) _A) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ exp  (_in(SIMDVec4_) _A) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ exp2 (_in(SIMDVec4_) _A) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }

		friend SIMDVec4_ pow  (_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ pow  (_in(SIMDVec4_) _A, value_type _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }

		friend SIMDVec4_ log10(_in(SIMDVec4_) _A) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ log2 (_in(SIMDVec4_) _A) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ loge (_in(SIMDVec4_) _A) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }

		friend SIMDVec4_ sin  (_in(SIMDVec4_) _A) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ cos  (_in(SIMDVec4_) _A) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ tan  (_in(SIMDVec4_) _A) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }

		friend SIMDVec4_ asin (_in(SIMDVec4_) _A) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ acos (_in(SIMDVec4_) _A) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ atan (_in(SIMDVec4_) _A) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ atan (_in(SIMDVec4_) _Y, _in(SIMDVec4_) _X) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }

		friend value_type dot2(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend value_type dot3(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend value_type dot4(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend SIMDVec4_ cross3(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }

		friend std::string to_string(_in(SIMDVec4_) _Vec) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
		friend std::ostream& operator<<(_inout(std::ostream) _Ostr, _in(SIMDVec4_) _Vec) { throw std::exception("[Please implement by yourself, @SIMDVec4_<>]"); }
	};// struct SIMDVec4_<>

	using SIMDVec4i = SIMDVec4_<int>;
	using SIMDVec4f = SIMDVec4_<float>;
	using SIMDVec4d = SIMDVec4_<double>;
	using SIMDVec4  = SIMDVec4f;


	/* @_Describe: __m128 wrapper */
	template<>
	struct SIMDVec4_<float> {
		using value_type = float;
		using vector_type = __m128;

		SIMDVec4_( ) { _Myvec = _mm_set_ps1(0.f); }

		explicit SIMDVec4_( float _All ) { _Myvec = _mm_set_ps1(_All); }
		
		SIMDVec4_( float _F0, float _F1, float _F2, float _F3 ) { _Myvec = _mm_set_ps(_F3, _F2, _F1, _F0); }
		
		SIMDVec4_( _in(vector_type) _Vec ) { _Myvec = _Vec; }

		operator vector_type( ) const { return (_Myvec); }

		      size_t size() const { return ( 4 ); }
		      float* ptr()           { return (_Myvec.m128_f32); }
		const float* ptr() const        { return (_Myvec.m128_f32); }
		      float* ptr(size_t _Pos)      { return (_Myvec.m128_f32 + _Pos); }
		const float* ptr(size_t _Pos) const   { return (_Myvec.m128_f32 + _Pos); }
		vector_type  simd_vector()    const      { return (_Myvec); }
		      float& operator[](size_t _Pos)        { return (_Myvec.m128_f32[_Pos]); }
		const float& operator[](size_t _Pos) const     { return (_Myvec.m128_f32[_Pos]); }

		friend SIMDVec4_ operator&(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return (_mm_and_ps(_A, _B)); }
		friend SIMDVec4_ operator|(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return ( _mm_or_ps(_A, _B)); }
		friend SIMDVec4_ operator^(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return (_mm_xor_ps(_A, _B)); }
		friend SIMDVec4_ operator+(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return (_mm_add_ps(_A, _B)); }
		friend SIMDVec4_ operator-(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return (_mm_sub_ps(_A, _B)); }
		friend SIMDVec4_ operator*(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return (_mm_mul_ps(_A, _B)); }
		friend SIMDVec4_ operator*(_in(SIMDVec4_) _A,     float      _B) { return (_mm_mul_ps(_A, _mm_set_ps1(_B))); }
		friend SIMDVec4_ operator/(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return (_mm_div_ps(_A, _B)); }
		friend SIMDVec4_ operator/(_in(SIMDVec4_) _A,     float      _B) { return (_mm_div_ps(_A, _mm_set_ps1(_B))); }

		SIMDVec4_& operator&=(_in(SIMDVec4_) _B) { _Myvec = _mm_and_ps(_Myvec, _B); return (*this); }
		SIMDVec4_& operator|=(_in(SIMDVec4_) _B) { _Myvec =  _mm_or_ps(_Myvec, _B); return (*this); }
		SIMDVec4_& operator^=(_in(SIMDVec4_) _B) { _Myvec = _mm_xor_ps(_Myvec, _B); return (*this); }
		SIMDVec4_& operator+=(_in(SIMDVec4_) _B) { _Myvec = _mm_add_ps(_Myvec, _B); return (*this); }
		SIMDVec4_& operator-=(_in(SIMDVec4_) _B) { _Myvec = _mm_sub_ps(_Myvec, _B); return (*this); }
		SIMDVec4_& operator*=(_in(SIMDVec4_) _B) { _Myvec = _mm_mul_ps(_Myvec, _B); return (*this); }
		SIMDVec4_& operator*=(    float      _B) { _Myvec = _mm_mul_ps(_Myvec, _mm_set_ps1(_B)); return (*this); }
		SIMDVec4_& operator/=(_in(SIMDVec4_) _B) { _Myvec = _mm_div_ps(_Myvec, _B); return (*this); }
		SIMDVec4_& operator/=(    float      _B) { _Myvec = _mm_div_ps(_Myvec, _mm_set_ps1(_B)); return (*this); }

		friend SIMDVec4_ mod  (_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return (_mm_fmod_ps(_A, _B)); }
		friend SIMDVec4_ mod  (_in(SIMDVec4_) _A,     float      _B) { return ( _mm_fmod_ps(_A, _mm_set_ps1(_B)) ); }
		friend SIMDVec4_ floor(_in(SIMDVec4_) _A) { return (_mm_floor_ps(_A)); }
		friend SIMDVec4_ ceil (_in(SIMDVec4_) _A) { return (_mm_ceil_ps(_A)); }
		friend SIMDVec4_ trunc(_in(SIMDVec4_) _A) { return (_mm_trunc_ps(_A)); }

		friend SIMDVec4_ sqrt (_in(SIMDVec4_) _A) { return (_mm_sqrt_ps(_A)); }
		friend SIMDVec4_ exp  (_in(SIMDVec4_) _A) { return (_mm_exp_ps(_A)); }
		friend SIMDVec4_ exp2 (_in(SIMDVec4_) _A) { return (_mm_exp2_ps(_A)); }
		/*  expn: pow(_B, _A) */
		
		friend SIMDVec4_ pow  (_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return (_mm_pow_ps(_A, _B)); }
		friend SIMDVec4_ pow  (_in(SIMDVec4_) _A,     float      _B) { return (_mm_pow_ps(_A, _mm_set_ps1(_B))); }
		
		friend SIMDVec4_ log10(_in(SIMDVec4_) _A) { return (_mm_log10_ps(_A)); }
		friend SIMDVec4_ log2 (_in(SIMDVec4_) _A) { return (_mm_log2_ps(_A)); }
		friend SIMDVec4_ loge (_in(SIMDVec4_) _A) { return (_mm_log_ps(_A)); }

		friend SIMDVec4_ sin  (_in(SIMDVec4_) _A) { return (_mm_sin_ps(_A)); }
		friend SIMDVec4_ cos  (_in(SIMDVec4_) _A) { return (_mm_cos_ps(_A)); }
		friend SIMDVec4_ tan  (_in(SIMDVec4_) _A) { return (_mm_tan_ps(_A)); }
		/*  cot: 1.0/tan
			sec: 1.0/sin
			csc: 1.0/cos
		*/

		friend SIMDVec4_ asin (_in(SIMDVec4_) _A) { return (_mm_asin_ps(_A)); }
		friend SIMDVec4_ acos (_in(SIMDVec4_) _A) { return (_mm_acos_ps(_A)); }
		friend SIMDVec4_ atan (_in(SIMDVec4_) _A) { return (_mm_atan_ps(_A)); }
		friend SIMDVec4_ atan (_in(SIMDVec4_) _Y, _in(SIMDVec4_) _X) { return (_mm_atan2_ps(_Y, _X)); }

		friend float dot2(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { auto _Tmp = _A * _B; return (_Tmp[0] + _Tmp[1]); }
		friend float dot3(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { auto _Tmp = _A * _B; return (_Tmp[0] + _Tmp[1] + _Tmp[2]); }
		friend float dot4(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { auto _Tmp = _A * _B; return (_Tmp[0] + _Tmp[1] + _Tmp[2] + _Tmp[3]); }
		friend SIMDVec4_ cross3(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return (_mm_cross3_ps(_A, _B)); }

		friend std::string to_string(_in(SIMDVec4_) _Vec) {
			return ("SIMDVec4f[" + std::to_string(_Vec[0]) + "," + std::to_string(_Vec[1]) + "," + std::to_string(_Vec[2]) + "," + std::to_string(_Vec[3]) + "]");
		}

		friend std::ostream& operator<<(_inout(std::ostream) _Ostr, _in(SIMDVec4_) _Vec) { 
			return (_Ostr << to_string(_Vec));
		}

		__m128 _Myvec;
	};


	/* @_Describe: __m128 wrapper */
	template<>
	struct SIMDVec4_<double> {
		using value_type = double;
		using vector_type = __m256d;

		SIMDVec4_() { _Myvec = _mm256_set1_pd(0.0); }

		explicit SIMDVec4_( float _All ) { _Myvec = _mm256_set1_pd(_All); }
		
		SIMDVec4_( double _F0, double _F1, double _F2, double _F3 ) { _Myvec = _mm256_set_pd(_F3, _F2, _F1, _F0); }
		
		SIMDVec4_( _in(vector_type) _Vec ) { _Myvec = _Vec; }

		operator vector_type( ) const { return (_Myvec); }

		      size_t  size() const { return ( 4 ); }
		      double* ptr()           { return (_Myvec.m256d_f64); }
		const double* ptr() const        { return (_Myvec.m256d_f64); }
			  double* ptr(size_t _Pos)      { return (_Myvec.m256d_f64 + _Pos); }
		const double* ptr(size_t _Pos) const   { return (_Myvec.m256d_f64 + _Pos); }
		vector_type   simd_vector()    const      { return (_Myvec); }
			  double& operator[](size_t _Pos)        { return (_Myvec.m256d_f64[_Pos]); }
		const double& operator[](size_t _Pos) const     { return (_Myvec.m256d_f64[_Pos]); }

		friend SIMDVec4_ operator&(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return (_mm256_add_pd(_A, _B)); }
		friend SIMDVec4_ operator|(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return ( _mm256_or_pd(_A, _B)); }
		friend SIMDVec4_ operator^(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return (_mm256_xor_pd(_A, _B)); }
		friend SIMDVec4_ operator+(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return (_mm256_add_pd(_A, _B)); }
		friend SIMDVec4_ operator-(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return (_mm256_sub_pd(_A, _B)); }
		friend SIMDVec4_ operator*(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return (_mm256_mul_pd(_A, _B)); }
		friend SIMDVec4_ operator*(_in(SIMDVec4_) _A,     double     _B) { return (_mm256_mul_pd(_A, _mm256_set1_pd(_B))); }
		friend SIMDVec4_ operator/(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return (_mm256_div_pd(_A, _B)); }
		friend SIMDVec4_ operator/(_in(SIMDVec4_) _A,     double     _B) { return (_mm256_div_pd(_A, _mm256_set1_pd(_B))); }

		SIMDVec4_& operator&=(_in(SIMDVec4_) _B) { _Myvec = _mm256_add_pd(_Myvec, _B); return (*this); }
		SIMDVec4_& operator|=(_in(SIMDVec4_) _B) { _Myvec =  _mm256_or_pd(_Myvec, _B); return (*this); }
		SIMDVec4_& operator^=(_in(SIMDVec4_) _B) { _Myvec = _mm256_xor_pd(_Myvec, _B); return (*this); }
		SIMDVec4_& operator+=(_in(SIMDVec4_) _B) { _Myvec = _mm256_add_pd(_Myvec, _B); return (*this); }
		SIMDVec4_& operator-=(_in(SIMDVec4_) _B) { _Myvec = _mm256_sub_pd(_Myvec, _B); return (*this); }
		SIMDVec4_& operator*=(_in(SIMDVec4_) _B) { _Myvec = _mm256_mul_pd(_Myvec, _B); return (*this); }
		SIMDVec4_& operator*=(    double     _B) { _Myvec = _mm256_mul_pd(_Myvec, _mm256_set1_pd(_B)); return (*this); }
		SIMDVec4_& operator/=(_in(SIMDVec4_) _B) { _Myvec = _mm256_div_pd(_Myvec, _B); return (*this); }
		SIMDVec4_& operator/=(    double     _B) { _Myvec = _mm256_div_pd(_Myvec, _mm256_set1_pd(_B)); return (*this); }

		friend SIMDVec4_ mod  (_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return (_mm256_fmod_pd(_A, _B)); }
		friend SIMDVec4_ mod  (_in(SIMDVec4_) _A,     double     _B) { return ( _mm256_fmod_pd(_A, _mm256_set1_pd(_B)) ); }
		friend SIMDVec4_ floor(_in(SIMDVec4_) _A) { return (_mm256_floor_pd(_A)); }
		friend SIMDVec4_ ceil (_in(SIMDVec4_) _A) { return ( _mm256_ceil_pd(_A)); }
		friend SIMDVec4_ trunc(_in(SIMDVec4_) _A) { return (_mm256_trunc_pd(_A)); }

		friend SIMDVec4_ sqrt (_in(SIMDVec4_) _A) { return (_mm256_sqrt_pd(_A)); }
		friend SIMDVec4_ exp  (_in(SIMDVec4_) _A) { return ( _mm256_exp_pd(_A)); }
		friend SIMDVec4_ exp2 (_in(SIMDVec4_) _A) { return (_mm256_exp2_pd(_A)); }
		/*  expn: pow(_B, _A) */
		
		friend SIMDVec4_ pow  (_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return (_mm256_pow_pd(_A, _B)); }
		friend SIMDVec4_ pow  (_in(SIMDVec4_) _A,     double     _B) { return (_mm256_pow_pd(_A, _mm256_set1_pd(_B))); }
		
		friend SIMDVec4_ log10(_in(SIMDVec4_) _A) { return (_mm256_log10_pd(_A)); }
		friend SIMDVec4_ log2 (_in(SIMDVec4_) _A) { return ( _mm256_log2_pd(_A)); }
		friend SIMDVec4_ loge (_in(SIMDVec4_) _A) { return (  _mm256_log_pd(_A)); }

		friend SIMDVec4_ sin  (_in(SIMDVec4_) _A) { return (_mm256_sin_pd(_A)); }
		friend SIMDVec4_ cos  (_in(SIMDVec4_) _A) { return (_mm256_cos_pd(_A)); }
		friend SIMDVec4_ tan  (_in(SIMDVec4_) _A) { return (_mm256_tan_pd(_A)); }
		/*  cot: 1.0/tan
			sec: 1.0/sin
			csc: 1.0/cos
		*/

		friend SIMDVec4_ asin (_in(SIMDVec4_) _A) { return (_mm256_asin_pd(_A)); }
		friend SIMDVec4_ acos (_in(SIMDVec4_) _A) { return (_mm256_acos_pd(_A)); }
		friend SIMDVec4_ atan (_in(SIMDVec4_) _A) { return (_mm256_atan_pd(_A)); }
		friend SIMDVec4_ atan (_in(SIMDVec4_) _Y, _in(SIMDVec4_) _X) { return (_mm256_atan2_pd(_Y, _X)); }

		friend double dot2(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { auto _Tmp = _A * _B; return (_Tmp[0] + _Tmp[1]); }
		friend double dot3(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { auto _Tmp = _A * _B; return (_Tmp[0] + _Tmp[1] + _Tmp[2]); }
		friend double dot4(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { auto _Tmp = _A * _B; return (_Tmp[0] + _Tmp[1] + _Tmp[2] + _Tmp[3]); }
		friend SIMDVec4_ cross3(_in(SIMDVec4_) _A, _in(SIMDVec4_) _B) { return (_mm256_cross3_pd(_A, _B)); }

		friend std::string to_string(_in(SIMDVec4_) _Vec) {
			return ("SIMDVec4d[" + std::to_string(_Vec[0]) + "," + std::to_string(_Vec[1]) + "," + std::to_string(_Vec[2]) + "," + std::to_string(_Vec[3]) + "]");
		}

		friend std::ostream& operator<<(_inout(std::ostream) _Ostr, _in(SIMDVec4_) _Vec) { 
			return (_Ostr << to_string(_Vec));
		}

		__m256d _Myvec;
	};

}// namespace clmagic

#endif