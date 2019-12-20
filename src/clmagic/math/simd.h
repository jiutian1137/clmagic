#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH___SIMD___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH___SIMD___H__
#include "clmagic.h"

namespace clmagic 
{
	using vec128f = __m128;
	using vec128i = __m128i;
	using vec256 = __m256;
	struct __declspec(align(16)) vec128x4 { vec128f r[4]; };

	namespace simd_128f
	{
		inline vec128f empty()
			{
			return ( vec128f() );
			}

		inline vec128f all(float _Val)
			{
			return ( _mm_set_ps1(_Val) );
			}

		inline vec128f load(const float* _Address)
			{
			return (_mm_loadu_ps(_Address));
			}

		inline vec128f load(float _A, float _B, float _C, float _D)
			{
			return (_mm_set_ps(_A, _B, _C, _D));
			}

		inline vec128f load_align16(const float* _Align16_addr)
			{
			return (_mm_load_ps(_Align16_addr));
			}

		inline void store(/*out*/float* _Dst, _in(vec128f) _Src)
			{
			_mm_storeu_ps(_Dst, _Src);
			}

		inline float get(_in(vec128f) _Src, size_t _Pos)
			{
			return ( _Src.m128_f32[_Pos] );
			}

		inline void  set(_inout(vec128f) _Src, size_t _Pos, float _Val) 
			{
			_Src.m128_f32[_Pos] = _Val;
			}

		inline vec128f add(_in(vec128f) _Lhs, _in(vec128f) _Rhs)
			{
			return (_mm_add_ps(_Lhs, _Rhs));
			}

		inline vec128f sub(_in(vec128f) _Lhs, _in(vec128f) _Rhs)
			{
			return (_mm_sub_ps(_Lhs, _Rhs));
			}

		inline vec128f mul(_in(vec128f) _Lhs, _in(vec128f) _Rhs)
			{
			return (_mm_mul_ps(_Lhs, _Rhs));
			}

		inline vec128f div(_in(vec128f) _Lhs, _in(vec128f) _Rhs)
			{
			return (_mm_div_ps(_Lhs, _Rhs));
			}

		inline vec128f sum(_in(vec128f) _Lhs)
			{
			return ( DirectX::XMVectorSum(_Lhs) );
			}

		inline vec128f sin(_in(vec128f) _Lhs)
			{
			return ( DirectX::XMVectorSin(_Lhs) );
			}

		inline vec128f cos(_in(vec128f) _Lhs)
			{
			return ( DirectX::XMVectorCos(_Lhs) );
			}

		inline vec128f tan(_in(vec128f) _Lhs)
			{
			return ( DirectX::XMVectorTan(_Lhs) );
			}

		inline vec128f asin(_in(vec128f) _Lhs)
			{
			return ( DirectX::XMVectorASin(_Lhs) );
			}

		inline vec128f acos(_in(vec128f) _Lhs)
			{
			return ( DirectX::XMVectorACos(_Lhs) );
			}

		inline vec128f atan(_in(vec128f) _Lhs)
			{
			return ( DirectX::XMVectorATan(_Lhs) );
			}
	}// namespace simd_128f
	

	namespace simd_128x4f 
	{
		inline vec128x4 empty()
			{
			return ( vec128x4() );
			}

		inline vec128x4 all(float _Val)
			{
			vec128x4 _Result;
			_Result.r[0] = simd_128f::all(_Val);
			_Result.r[1] = simd_128f::all(_Val);
			_Result.r[2] = simd_128f::all(_Val);
			_Result.r[3] = simd_128f::all(_Val);
			return (_Result);
			}

		inline vec128x4 load(const float* _Address)
		{
			vec128x4 _Result;
			_Result.r[0] = simd_128f::load(_Address);
			_Result.r[1] = simd_128f::load(_Address + 4);
			_Result.r[2] = simd_128f::load(_Address + 8);
			_Result.r[3] = simd_128f::load(_Address + 12);
			return (_Result);
		}

		inline vec128x4 load(float _11, float _12, float _13, float _14,
							 float _21, float _22, float _23, float _24,
							 float _31, float _32, float _33, float _34,
							 float _41, float _42, float _43, float _44)
			{
			vec128x4 _Result;
			_Result.r[0] = simd_128f::load(_11, _12, _13, _14);
			_Result.r[1] = simd_128f::load(_21, _22, _23, _24);
			_Result.r[2] = simd_128f::load(_31, _32, _33, _34);
			_Result.r[3] = simd_128f::load(_41, _42, _43, _44);
			return (_Result);
			}
	}// namespace simd_128x4f

	
	namespace simd_128i32 
	{
		inline vec128i empty()
			{
			return ( vec128i() );
			}

		inline vec128i all(int _Val)
			{
			return (_mm_set1_epi32(_Val));
			}

		inline vec128i load(const int* _Address)
			{
			return (_mm_set_epi32(_Address[3], _Address[2], _Address[1], _Address[0]));
			}

		inline vec128i load(int _A, int _B, int _C, int _D)
			{
			return ( _mm_set_epi32(_D, _C, _B, _A) );
			}

		inline void store(int* _Dst, _in(vec128i) _Src)
			{
			_Dst[0] = _Src.m128i_i32[0];
			_Dst[1] = _Src.m128i_i32[1];
			_Dst[2] = _Src.m128i_i32[2];
			_Dst[3] = _Src.m128i_i32[3];
			}

		inline int get(_in(vec128i) _Src, size_t _Pos)
			{
			return (_Src.m128i_i32[_Pos]);
			}

		inline void  set(_inout(vec128i) _Src, size_t _Pos, int _Val)
			{
			_Src.m128i_i32[_Pos] = _Val;
			}

		inline vec128i add(_in(vec128i) _Lhs, _in(vec128i) _Rhs)
			{
			return ( _mm_add_epi32(_Lhs, _Rhs) );
			}

		inline vec128i sub(_in(vec128i) _Lhs, _in(vec128i) _Rhs)
			{
			return ( _mm_sub_epi32(_Lhs, _Rhs) );
			}

		inline vec128i mul(_in(vec128i) _Lhs, _in(vec128i) _Rhs)
			{
			return (_mm_mullo_epi32(_Lhs, _Rhs));
			}

		// no div
	}// namespace simd_128i32 
	
}// namespace clmagic

#endif