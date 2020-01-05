#pragma once
#ifndef clmagic_math_SIMD_h_
#define clmagic_math_SIMD_h_
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


inline float _mm_sum_ps(__m128 _A) {
	return (_A.m128_f32[0] + _A.m128_f32[1] + _A.m128_f32[2] + _A.m128_f32[3]);
}

inline double _mm256_sum_pd(__m256d _A) {
	return (_A.m256d_f64[0] + _A.m256d_f64[1] + _A.m256d_f64[2] + _A.m256d_f64[3]);
}

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

inline std::string _mm_to_string_ps(__m128 _A) {
	return ("__m128[" + std::to_string(_A.m128_f32[0]) + "," 
				      + std::to_string(_A.m128_f32[1]) + ","
				  	  + std::to_string(_A.m128_f32[2]) + "," 
				  	  + std::to_string(_A.m128_f32[3]) + "]");
}

inline std::string _mm256_to_string_pd(__m256d _A) {
	return ("__m256d[" + std::to_string(_A.m256d_f64[0]) + ","
			           + std::to_string(_A.m256d_f64[1]) + ","
			           + std::to_string(_A.m256d_f64[2]) + ","
			           + std::to_string(_A.m256d_f64[3]) + "]");
}


namespace clmagic {

	template<typename _Ty>
		constexpr bool is_simd_v = std::_Is_any_of_v< 
			std::remove_cv_t<_Ty>, 
			float, 
			double, 
			int>;

	template<typename _Ty = float>
		struct _SIMD_traits {
			using value_type    = _Ty;
			using vector_type   = __m128;
			using pointer       = _Ty*;
			using const_pointer = _Ty*;

			static vector_type _Load(const_pointer _Ptr) { _mm_load_ps(_Ptr); }
			static vector_type _Loadu(const_pointer _Ptr) { _mm_loadu_ps(_Ptr); }
			static vector_type _All(value_type _Val) { return _mm_set1_ps(_Val); }
			static vector_type _Set(value_type _Val0, value_type _Val1, value_type _Val2, value_type _Val3) { return _mm_set_ps(_Val3, _Val2, _Val1, _Val0); }
			static value_type  _Sum(_in(vector_type) _A) { return _mm_sum_ps(_A); }

			static vector_type _And(_in(vector_type) _A, _in(vector_type) _B) { return _mm_and_ps(_A, _B); }
			static vector_type _Or (_in(vector_type) _A, _in(vector_type) _B) { return _mm_or_ps(_A, _B); }
			static vector_type _Xor(_in(vector_type) _A, _in(vector_type) _B) { return _mm_xor_ps(_A, _B); }
			static vector_type _Add(_in(vector_type) _A, _in(vector_type) _B) { return _mm_add_ps(_A, _B); }
			static vector_type _Sub(_in(vector_type) _A, _in(vector_type) _B) { return _mm_sub_ps(_A, _B); }
			static vector_type _Mul(_in(vector_type) _A, _in(vector_type) _B) { return _mm_mul_ps(_A, _B); }
			static vector_type _Div(_in(vector_type) _A, _in(vector_type) _B) { return _mm_div_ps(_A, _B); }
			static vector_type _Mod(_in(vector_type) _A, _in(vector_type) _B) { return _mm_fmod_ps(_A, _B); }
			static vector_type _Pow(_in(vector_type) _A, _in(vector_type) _B) { return _mm_pow_ps(_A, _B); }
			static vector_type _Dot(_in(vector_type) _A, _in(vector_type) _B) { return _All(_Sum(_Mul(_A, _B))); }
			static value_type  _Dot2(_in(vector_type) _A, _in(vector_type) _B) { auto _Tmp = _Mul(_A, _B); return (_Tmp.m128_f32[0] + _Tmp.m128_f32[1]); }
			static value_type  _Dot3(_in(vector_type) _A, _in(vector_type) _B) { auto _Tmp = _Mul(_A, _B); return (_Tmp.m128_f32[0] + _Tmp.m128_f32[1] + _Tmp.m128_f32[2]); }
			static value_type  _Dot4(_in(vector_type) _A, _in(vector_type) _B) { return _Sum(_Mul(_A, _B)); }
			static vector_type _Cross3(_in(vector_type) _A, _in(vector_type) _B) { return _mm_cross3_ps(_A, _B); }

			static vector_type _Floor(_in(vector_type) _A) { return _mm_floor_ps(_A); }
			static vector_type _Ceil(_in(vector_type) _A) { return _mm_ceil_ps(_A); }
			static vector_type _Trunc(_in(vector_type) _A) { return _mm_trunc_ps(_A); }

			static vector_type _Invsqrt(_in(vector_type) _A) { return (_mm_invsqrt_ps(_A));/* 1.f/sqrt(_A) */ }
			static vector_type _Sqrt(_in(vector_type) _A) { return _mm_sqrt_ps(_A); }
			static vector_type _Exp(_in(vector_type) _A) { return _mm_exp_ps(_A); }
			static vector_type _Exp2(_in(vector_type) _A) { return _mm_exp2_ps(_A); }

			static vector_type _Log10(_in(vector_type) _A) { return _mm_log10_ps(_A); }
			static vector_type _Log2(_in(vector_type) _A) { return _mm_log2_ps(_A); }
			static vector_type _LogE(_in(vector_type) _A) { return _mm_log_ps(_A); }

			static vector_type _Sin(_in(vector_type) _A) { return _mm_sin_ps(_A); }
			static vector_type _Cos(_in(vector_type) _A) { return _mm_cos_ps(_A); }
			static vector_type _Tan(_in(vector_type) _A) { return _mm_tan_ps(_A); }

			static vector_type _Asin(_in(vector_type) _A) { return _mm_asin_ps(_A); }
			static vector_type _Acos(_in(vector_type) _A) { return _mm_acos_ps(_A); }
			static vector_type _Atan(_in(vector_type) _A) { return _mm_atan_ps(_A); }
			static vector_type _Atan2(_in(vector_type) _Y, _in(vector_type) _X) { return _mm_atan2_ps(_Y, _X); }

			static vector_type _Length(_in(vector_type) _A) { return _Sqrt(_Dot(_A, _A)); }
			static vector_type _Normalize(_in(vector_type) _A) { return _Mul(_A, _Invsqrt(_Dot(_A, _A))); }

			static std::string _Tostring(_in(vector_type) _A) { return _mm_to_string_ps(_A); }
	};

	template<>
		struct _SIMD_traits<double> {
			using value_type    = double;
			using vector_type   = __m256d;
			using pointer       = double*;
			using const_pointer = const double*;

			static vector_type _Load(const_pointer _Ptr) { _mm256_load_pd(_Ptr); }
			static vector_type _Loadu(const_pointer _Ptr) { _mm256_loadu_pd(_Ptr); }
			static vector_type _All(value_type _Val) { return _mm256_set1_pd(_Val); }
			static vector_type _Set(value_type _Val0, value_type _Val1, value_type _Val2, value_type _Val3) { return _mm256_set_pd(_Val3, _Val2, _Val1, _Val0); }
			static value_type  _Sum(_in(vector_type) _A) { return _mm256_sum_pd(_A); }

			static vector_type _And(_in(vector_type) _A, _in(vector_type) _B) { return _mm256_and_pd(_A, _B); }
			static vector_type _Or (_in(vector_type) _A, _in(vector_type) _B) { return _mm256_or_pd(_A, _B); }
			static vector_type _Xor(_in(vector_type) _A, _in(vector_type) _B) { return _mm256_xor_pd(_A, _B); }
			static vector_type _Add(_in(vector_type) _A, _in(vector_type) _B) { return _mm256_add_pd(_A, _B); }
			static vector_type _Sub(_in(vector_type) _A, _in(vector_type) _B) { return _mm256_sub_pd(_A, _B); }
			static vector_type _Mul(_in(vector_type) _A, _in(vector_type) _B) { return _mm256_mul_pd(_A, _B); }
			static vector_type _Div(_in(vector_type) _A, _in(vector_type) _B) { return _mm256_div_pd(_A, _B); }
			static vector_type _Mod(_in(vector_type) _A, _in(vector_type) _B) { return _mm256_fmod_pd(_A, _B); }
			static vector_type _Pow(_in(vector_type) _A, _in(vector_type) _B) { return _mm256_pow_pd(_A, _B); }
			static vector_type _Dot(_in(vector_type) _A, _in(vector_type) _B) { return _All( _Sum( _Mul(_A, _B) ) ); }
			static value_type  _Dot2(_in(vector_type) _A, _in(vector_type) _B) { auto _Tmp = _Mul(_A, _B); return (_Tmp.m256d_f64[0] + _Tmp.m256d_f64[1]); }
			static value_type  _Dot3(_in(vector_type) _A, _in(vector_type) _B) { auto _Tmp = _Mul(_A, _B); return (_Tmp.m256d_f64[0] + _Tmp.m256d_f64[1] + _Tmp.m256d_f64[2]); }
			static value_type  _Dot4(_in(vector_type) _A, _in(vector_type) _B) { return _Sum( _Mul(_A, _B) ); }
			static vector_type _Cross3(_in(vector_type) _A, _in(vector_type) _B) { return _mm256_cross3_pd(_A, _B); }

			static vector_type _Floor(_in(vector_type) _A) { return _mm256_floor_pd(_A); }
			static vector_type _Ceil(_in(vector_type) _A) { return _mm256_ceil_pd(_A); }
			static vector_type _Trunc(_in(vector_type) _A) { return _mm256_trunc_pd(_A); }

			static vector_type _Invsqrt(_in(vector_type) _A) { return _mm256_invsqrt_pd(_A); }
			static vector_type _Sqrt(_in(vector_type) _A) { return _mm256_sqrt_pd(_A); }
			static vector_type _Exp(_in(vector_type) _A) { return _mm256_exp_pd(_A); }
			static vector_type _Exp2(_in(vector_type) _A) { return _mm256_exp2_pd(_A); }

			static vector_type _Log10(_in(vector_type) _A) { return _mm256_log10_pd(_A); }
			static vector_type _Log2(_in(vector_type) _A) { return _mm256_log2_pd(_A); }
			static vector_type _LogE(_in(vector_type) _A) { return _mm256_log_pd(_A); }

			static vector_type _Sin(_in(vector_type) _A) { return _mm256_sin_pd(_A); }
			static vector_type _Cos(_in(vector_type) _A) { return _mm256_cos_pd(_A); }
			static vector_type _Tan(_in(vector_type) _A) { return _mm256_tan_pd(_A); }

			static vector_type _Asin(_in(vector_type) _A) { return _mm256_asin_pd(_A); }
			static vector_type _Acos(_in(vector_type) _A) { return _mm256_acos_pd(_A); }
			static vector_type _Atan(_in(vector_type) _A) { return _mm256_atan_pd(_A); }
			static vector_type _Atan2(_in(vector_type) _Y, _in(vector_type) _X) { return _mm256_atan2_pd(_Y, _X); }

			static vector_type _Length(_in(vector_type) _A) { return _Sqrt(_Dot(_A, _A)); }
			static vector_type _Normalize(_in(vector_type) _A) { return _Mul(_A, _Invsqrt(_Dot(_A, _A))); }

			static std::string _Tostring(_in(vector_type) _A) { return _mm256_to_string_pd(_A); }
	};

	


	/*
	@_Note: only contains stl-simd operation
	*/
	template<size_t N, typename T>
	struct SIMDVec_ {
		static_assert( is_simd_v<T>, "[Unsupported SIMD type, Please implement by yourself, @SIMDVec_<>]" );
		
		constexpr static size_t vector_size = constexpr_align(N, 4) / 4;
		using _Mytraits     = _SIMD_traits<T>;
		using value_type    = typename _Mytraits::value_type;
		using vector_type   = typename _Mytraits::vector_type;
		using pointer       = typename _Mytraits::pointer;
		using const_pointer = typename _Mytraits::const_pointer;

		/* <construct> */
		SIMDVec_(/* void */) : SIMDVec_(T(0)) { }

		explicit SIMDVec_(const_pointer _Ptr) {
			if (_Ptr & (16 - 1) == 0) {// aligned 16
				for (size_t i = 0; i != vector_size; ++i, _Ptr += sizeof(vector_type)) {
					_Myvec[i] = _Mytraits::_Load(_Ptr);
				}
			} else {
				for (size_t i = 0; i != vector_size; ++i, _Ptr += sizeof(vector_type)) {
					_Myvec[i] = _Mytraits::_Loadu(_Ptr);
				}
			}
		}

		explicit SIMDVec_(value_type _Val) { 
			for (size_t i = 0; i != vector_size; ++i) { 
				_Myvec[i] = _Mytraits::_All(_Val); 
			} 
		}
		
		SIMDVec_(value_type _Val0, value_type _Val1, value_type _Val2 = T(0), value_type _Val3 = T(0))
			: _Myvec{ _Mytraits::_Set(_Val0, _Val1, _Val2, _Val3) } { }
		
		SIMDVec_(value_type _Val0, value_type _Val1, value_type _Val2, value_type _Val3,
				 value_type _Val4, value_type _Val5, value_type _Val6, value_type _Val7) 
			: _Myvec{ _Mytraits::_Set(_Val0, _Val1, _Val2, _Val3), 
					  _Mytraits::_Set(_Val4, _Val5, _Val6, _Val7) } { }

		SIMDVec_(value_type _Val0, value_type _Val1, value_type _Val2, value_type _Val3,
				 value_type _Val4, value_type _Val5, value_type _Val6, value_type _Val7,
				 value_type _Val8, value_type _Val9, value_type _Val10, value_type _Val11)
			: _Myvec{ _Mytraits::_Set(_Val0, _Val1, _Val2, _Val3), 
					  _Mytraits::_Set(_Val4, _Val5, _Val6, _Val7),
					  _Mytraits::_Set(_Val8, _Val9, _Val10, _Val11) } { }
		
		SIMDVec_(_in(vector_type) _V0) : _Myvec{ _V0 } { }
		SIMDVec_(_in(vector_type) _V0, _in(vector_type) _V1) : _Myvec{ _V0, _V1 } { }
		SIMDVec_(_in(vector_type) _V0, _in(vector_type) _V1, _in(vector_type) _V2) : _Myvec{ _V0, _V1, _V2 } { }

		template<typename _Fn>
		SIMDVec_(_in(SIMDVec_) _A, _Fn _Func) : _Myvec{ 0 } {
			for (size_t i = 0; i != vector_size; ++i) {
				_Myvec[i] = _Func(_A.simd_vector(i));
			}
		}

		template<typename _Fn> 
		SIMDVec_(_in(SIMDVec_) _A, _in(SIMDVec_) _B, _Fn _Func) : _Myvec{ 0 } {
			for (size_t i = 0; i != vector_size; ++i) {
				_Myvec[i] = _Func(_A.simd_vector(i), _B.simd_vector(i));
			}
		}

		template<typename _Fn>
		SIMDVec_(_in(SIMDVec_) _A, _in(vector_type) _B, _Fn _Func) : _Myvec{ 0 } {
			for (size_t i = 0; i != vector_size; ++i) {
				_Myvec[i] = _Func(_A.simd_vector(i), _B);
			}
		}

		template<typename _Fn>
		SIMDVec_(_in(vector_type) _A, _in(SIMDVec_) _B, _Fn _Func) : _Myvec{ 0 } {
			for (size_t i = 0; i != vector_size; ++i) {
				_Myvec[i] = _Func(_A, _B.simd_vector(i));
			}
		}
		/* </construct> */

		operator vector_type() const { return (_Myvec[0]); }

		constexpr size_t size() const { return ( N ); }
		constexpr size_t vsize() const  { return ( vector_size ); }
		T*       ptr()            { return ( (T*)&_Myvec ); }
		const T* ptr() const         { return ( (const T*)&_Myvec ); }
		T*       ptr(size_t _Pos)       { return ( ptr() + _Pos ); }
		const T* ptr(size_t _Pos) const    { return ( ptr() + _Pos ); }
		T&       operator[](size_t _Pos)      { return ( ptr()[_Pos] ); }
		const T& operator[](size_t _Pos) const   { return ( ptr()[_Pos] ); }
		vector_type&       simd_vector(size_t _Pos = 0) { return ( _Myvec[_Pos] ); }
		const vector_type& simd_vector(size_t _Pos = 0) const { return ( _Myvec[_Pos] ); }
		
		SIMDVec_& operator=(_in(SIMDVec_) _B) {
			std::copy(_B.ptr(), _B.ptr(N), this->ptr());
			return (*this);
		}

		template<typename _Fn>
		void assign(_in(SIMDVec_) _A, _Fn _Func) 
			{	// this = _Func(A[i])
			for (size_t i = 0; i != vector_size; ++i) 
				{
				_Myvec[i] = _Func(_A.simd_vector(i));
				}
			}

		template<typename _Fn>
		void assign(_in(SIMDVec_) _A, _in(SIMDVec_) _B, _Fn _Func) 
			{	// this = _Func(A[i], B[i])
			for (size_t i = 0; i != vector_size; ++i) 
				{
				_Myvec[i] = _Func(_A.simd_vector(i), _B.simd_vector(i));
				}
			}

		template<typename _Fn>
		void assign(_in(SIMDVec_) _A, _in(vector_type) _B, _Fn _Func) 
			{	// this = _Func(A[i], B)
			for (size_t i = 0; i != vector_size; ++i) 
				{
				_Myvec[i] = _Func(_A.simd_vector(i), _B);
				}
			}

		template<typename _Fn>
		void assign(_in(vector_type) _A, _in(SIMDVec_) _B, _Fn _Func) 
			{	// this = _Func(A, B[i])
			for (size_t i = 0; i != vector_size; ++i) 
				{
				_Myvec[i] = _Func(_A, _B.simd_vector(i));
				}
			}

		template<size_t _Size>
		SIMDVec_<_Size, T>& prefix() 
			{	// [first, first + _Size] ¡Ê [first, last)
			static_assert(_Size <= N, "[prefix grater than SIMDVec_<>]");
#ifdef _DEBUG
			if ((_Size & (4 - 1)) != 0) {
				std::cout << "[SIMD element must be align 4]" << std::endl;
			}
#endif
			return (reference_cast<SIMDVec_<_Size, T>>(_Myvec));
			}

		template<size_t _Size>
		SIMDVec_<_Size, T>& posfix() 
			{	// [first + _Off, last) ¡Ê [first, last)
			static_assert(_Size <= N, "[posfix grater than SIMDVec_<>]");
#ifdef _DEBUG
			if ((_Size & (4 - 1)) != 0) {
				std::cout << "[SIMD element must be align 4]" << std::endl;
			}
#endif
			size_t _Off = N - _Size;
			return ( reference_cast<SIMDVec_<_Size, T>>(*this->ptr(_Off)) );
			}

		value_type sum() const 
			{	// sum of all elements  
			value_type _Sum = _Mytraits::_Sum(_Myvec[0]);
			for (size_t i = 1; i != vector_size; ++i) { _Sum += _Mytraits::_Sum(_Myvec[i]); }
			return (_Sum);
			}

		value_type length() const 
			{	// sqrt( ¡Æelement[i]^2 ) 
			value_type _Length = _Mytraits::_Dot4(_Myvec[0], _Myvec[0]);
			for (size_t i = 1; i != vector_size; ++i) { _Length += _Mytraits::_Dot4(_Myvec[i], _Myvec[i]); }
			return ( static_cast<value_type>(::sqrt(_Length)) );
			}

		SIMDVec_& operator&=(_in(SIMDVec_) _B) { this->assign(*this, _B, _Mytraits::_And); return (*this); }
		SIMDVec_& operator|=(_in(SIMDVec_) _B) { this->assign(*this, _B, _Mytraits::_Or ); return (*this); }
		SIMDVec_& operator^=(_in(SIMDVec_) _B) { this->assign(*this, _B, _Mytraits::_Xor); return (*this); }
		SIMDVec_& operator+=(_in(SIMDVec_) _B) { this->assign(*this, _B, _Mytraits::_Add); return (*this); }
		SIMDVec_& operator-=(_in(SIMDVec_) _B) { this->assign(*this, _B, _Mytraits::_Sub); return (*this); }
		SIMDVec_& operator*=(_in(SIMDVec_) _B) { this->assign(*this, _B, _Mytraits::_Mul); return (*this); }
		SIMDVec_& operator/=(_in(SIMDVec_) _B) { this->assign(*this, _B, _Mytraits::_Div); return (*this); }
		SIMDVec_& incr      (_in(SIMDVec_) _B) { return (*this += _B); }
		SIMDVec_& decr      (_in(SIMDVec_) _B) { return (*this -= _B); }
		SIMDVec_& operator*=(    value_type _B) { this->assign(*this, _Mytraits::_All(_B), _Mytraits::_Mul); return (*this); }
		SIMDVec_& operator/=(    value_type _B) { this->assign(*this, _Mytraits::_All(_B), _Mytraits::_Div); return (*this); }
		SIMDVec_& incr      (    value_type _B) { this->assign(*this, _Mytraits::_All(_B), _Mytraits::_Add); return (*this); }
		SIMDVec_& decr      (    value_type _B) { this->assign(*this, _Mytraits::_All(_B), _Mytraits::_Sub); return (*this); }
		SIMDVec_& normalize (   /*** void ***/);

		friend SIMDVec_ operator-(_in(SIMDVec_) _A     /*** void ***/) { return SIMDVec_(_A, _Mytraits::_All(T(-1)), _Mytraits::_Mul); }
		friend SIMDVec_ operator&(_in(SIMDVec_) _A, _in(SIMDVec_) _B) { return SIMDVec_(_A, _B, _Mytraits::_And); }
		friend SIMDVec_ operator|(_in(SIMDVec_) _A, _in(SIMDVec_) _B) { return SIMDVec_(_A, _B, _Mytraits::_Or ); }
		friend SIMDVec_ operator^(_in(SIMDVec_) _A, _in(SIMDVec_) _B) { return SIMDVec_(_A, _B, _Mytraits::_Xor); }
		friend SIMDVec_ operator+(_in(SIMDVec_) _A, _in(SIMDVec_) _B) { return SIMDVec_(_A, _B, _Mytraits::_Add); }
		friend SIMDVec_ operator-(_in(SIMDVec_) _A, _in(SIMDVec_) _B) { return SIMDVec_(_A, _B, _Mytraits::_Sub); }
		friend SIMDVec_ operator*(_in(SIMDVec_) _A, _in(SIMDVec_) _B) { return SIMDVec_(_A, _B, _Mytraits::_Mul); }
		friend SIMDVec_ operator/(_in(SIMDVec_) _A, _in(SIMDVec_) _B) { return SIMDVec_(_A, _B, _Mytraits::_Div); }
		friend SIMDVec_ incr     (_in(SIMDVec_) _A, _in(SIMDVec_) _B) { return (_A + _B); }
		friend SIMDVec_ decr     (_in(SIMDVec_) _A, _in(SIMDVec_) _B) { return (_A - _B); }
		friend SIMDVec_ operator*(_in(SIMDVec_) _A,     value_type _B) { return SIMDVec_(_A, _Mytraits::_All(_B), _Mytraits::_Mul); }
		friend SIMDVec_ operator/(_in(SIMDVec_) _A,     value_type _B) { return SIMDVec_(_A, _Mytraits::_All(_B), _Mytraits::_Div); }
		friend SIMDVec_ incr     (_in(SIMDVec_) _A,     value_type _B) { return SIMDVec_(_A, _Mytraits::_All(_B), _Mytraits::_Add); }
		friend SIMDVec_ decr     (_in(SIMDVec_) _A,     value_type _B) { return SIMDVec_(_A, _Mytraits::_All(_B), _Mytraits::_Sub); }
		friend SIMDVec_ operator*(    value_type _A, _in(SIMDVec_) _B) { return SIMDVec_(_Mytraits::_All(_A), _B, _Mytraits::_Mul); }
		friend SIMDVec_ operator/(    value_type _A, _in(SIMDVec_) _B) { return SIMDVec_(_Mytraits::_All(_A), _B, _Mytraits::_Div); }

		friend SIMDVec_ mod      (_in(SIMDVec_) _A, _in(SIMDVec_) _B) { return SIMDVec_(_A, _B, _Mytraits::_Mod);  }
		friend SIMDVec_ mod      (_in(SIMDVec_) _A,    value_type _B) { return SIMDVec_(_A, _Mytraits::_All(_B), _Mytraits::_Mod); }
		friend SIMDVec_ floor    (_in(SIMDVec_) _A    /*** void ***/) { return SIMDVec_(_A, _Mytraits::_Floor); }
		friend SIMDVec_ ceil     (_in(SIMDVec_) _A    /*** void ***/) { return SIMDVec_(_A, _Mytraits::_Ceil); }
		friend SIMDVec_ trunc    (_in(SIMDVec_) _A    /*** void ***/) { return SIMDVec_(_A, _Mytraits::_Trunc); }
		
		friend SIMDVec_ invsqrt  (_in(SIMDVec_) _A    /*** void ***/) { return SIMDVec_(_A, _Mytraits::_Invsqrt); }
		friend SIMDVec_ sqrt     (_in(SIMDVec_) _A    /*** void ***/) { return SIMDVec_(_A, _Mytraits::_Sqrt); }
		friend SIMDVec_ exp      (_in(SIMDVec_) _A    /*** void ***/) { return SIMDVec_(_A, _Mytraits::_Exp); }
		friend SIMDVec_ exp2     (_in(SIMDVec_) _A    /*** void ***/) { return SIMDVec_(_A, _Mytraits::_Exp2); }

		friend SIMDVec_ pow      (_in(SIMDVec_) _A, _in(SIMDVec_) _B) { return SIMDVec_(_A, _B, _Mytraits::_Pow); }
		friend SIMDVec_ pow      (_in(SIMDVec_) _A,    value_type _B) { return SIMDVec_(_A, _Mytraits::_All(_B), _Mytraits::_Pow); }

		friend SIMDVec_ log10    (_in(SIMDVec_) _A        /* void */) { return SIMDVec_(_A, _Mytraits::_Log10); }
		friend SIMDVec_ log2     (_in(SIMDVec_) _A        /* void */) { return SIMDVec_(_A, _Mytraits::_Log2); }
		friend SIMDVec_ logE     (_in(SIMDVec_) _A        /* void */) { return SIMDVec_(_A, _Mytraits::_LogE); }

		friend SIMDVec_ sin      (_in(SIMDVec_) _A        /* void */) { return SIMDVec_(_A, _Mytraits::_Sin); }
		friend SIMDVec_ cos      (_in(SIMDVec_) _A        /* void */) { return SIMDVec_(_A, _Mytraits::_Cos); }
		friend SIMDVec_ tan      (_in(SIMDVec_) _A        /* void */) { return SIMDVec_(_A, _Mytraits::_Tan); }

		friend SIMDVec_ asin     (_in(SIMDVec_) _A        /* void */) { return SIMDVec_(_A, _Mytraits::_Asin); }
		friend SIMDVec_ acos     (_in(SIMDVec_) _A        /* void */) { return SIMDVec_(_A, _Mytraits::_Acos); }
		friend SIMDVec_ atan     (_in(SIMDVec_) _A        /* void */) { return SIMDVec_(_A, _Mytraits::_Atan); }
		friend SIMDVec_ atan2    (_in(SIMDVec_) _Y, _in(SIMDVec_) _X) { return SIMDVec_(_Y, _X, _Mytraits::_Atan2); }

		friend value_type dot    (_in(SIMDVec_) _A, _in(SIMDVec_) _B) 
			{	// A dot B
			value_type _Dot = _Mytraits::_Dot4(_A.simd_vector(0), _B.simd_vector(0));
			for (size_t i = 1; i != vector_size; ++i) { _Dot += _Mytraits::_Dot4(_A.simd_vector(i), _B.simd_vector(i)); }
			return ( _Dot );
			}
		friend value_type dot2   (_in(SIMDVec_) _A, _in(SIMDVec_) _B) { return _Mytraits::_Dot2(_A.simd_vector(0), _B.simd_vector(0)); }
		friend value_type dot3   (_in(SIMDVec_) _A, _in(SIMDVec_) _B) { return _Mytraits::_Dot3(_A.simd_vector(0), _B.simd_vector(0)); }
		friend value_type dot4   (_in(SIMDVec_) _A, _in(SIMDVec_) _B) { return _Mytraits::_Dot4(_A.simd_vector(0), _B.simd_vector(0)); }
		friend SIMDVec_  cross3  (_in(SIMDVec_) _A, _in(SIMDVec_) _B) { return SIMDVec_(_A, _B, _Mytraits::_Cross3); }
		friend SIMDVec_ normalize(_in(SIMDVec_) _A    /*** void ***/) { auto _Tmp = dot(_A, _A); return ( _Tmp == T(1) ? _A : _A * invsqrt(SIMDVec_(_Tmp)) ); }

		friend std::string to_string(_in(SIMDVec_) _Vec) { 
			std::string _Str = _Mytraits::_Tostring(_Vec.simd_vector(0));
			for (size_t i = 1; i != vector_size; ++i) { _Str += _Mytraits::_Tostring(_Vec.simd_vector(i)); }
			return ( _Str );
		}

		friend std::ostream& operator<<(_inout(std::ostream) _Ostr, _in(SIMDVec_) _Vec) { 
			return (_Ostr << to_string(_Vec));
		}
	
		vector_type _Myvec[vector_size];
	};// struct SIMDVec_<>

	//using SIMDVec4i = SIMDVec_<4, int>;
	using SIMDVec4f = SIMDVec_<4, float>;
	using SIMDVec4d = SIMDVec_<4, double>;


#define clmagic_SIMD_FAST_CONSTRUCT_ONE_VECTOR(N, TY) \
	template<> template<typename _Fn> inline \
		SIMDVec_<##N##, ##TY##>::SIMDVec_(_in(vector_type) _A, _in(SIMDVec_) _B, _Fn _Func) \
			: _Myvec{ _Func(_A, _B.simd_vector(0)) } { } 

	clmagic_SIMD_FAST_CONSTRUCT_ONE_VECTOR(2, float)
	clmagic_SIMD_FAST_CONSTRUCT_ONE_VECTOR(3, float)
	clmagic_SIMD_FAST_CONSTRUCT_ONE_VECTOR(4, float)
	clmagic_SIMD_FAST_CONSTRUCT_ONE_VECTOR(2, double)
	clmagic_SIMD_FAST_CONSTRUCT_ONE_VECTOR(3, double)
	clmagic_SIMD_FAST_CONSTRUCT_ONE_VECTOR(4, double)


	template<size_t N, typename T>
	SIMDVec_<N, T>& SIMDVec_<N, T>::normalize( ) {
		T _Tmp = dot(*this, *this);
		if (_Tmp != T(1)) {
			*this *= _Mytraits::_Invsqrt(_Mytraits::_All(_Tmp));
		}
		return ( *this );
	}

}// namespace clmagic

#endif