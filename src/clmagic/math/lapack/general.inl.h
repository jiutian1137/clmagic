#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH_LAPACK___GENERAL___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH_LAPACK___GENERAL___H__
#include "../lapack.h"

namespace clmagic 
{
	// < compare >
	template<typename _Ty, typename _Fn> inline
		void _Compare_vector_vector(/*out*/bool* _Pdst, /*in*/const _Ty* _Plhs, /*in*/const _Ty* _Prhs, 
			size_t _Size, _Fn* _Comparetor) 
		{
		while (_Size--) { _Pdst[_Size] = _Comparetor(_Plhs[_Size], _Prhs[_Size]); }
		}

	template<typename _Ty, typename _Fn> inline
		bool _Compare_vector_vector(/*in*/const _Ty* _Plhs, /*in*/const _Ty* _Prhs,
			size_t _Size, _Fn* _Comparetor) 
		{
		while ( _Size-- ) 
			{ 
			if ( !_Comparetor( _Plhs[_Size], _Prhs[_Size] ) ) 
				{
				return ( false );
				}	
			}
		return ( true );
		}
	// </ compare >

	// < add >
	template<typename _Ty> inline
		void _Add_vector_scalar(_Ty* _Pdst, const _Ty* _Psrc, _in(_Ty) _Scalar, size_t _Size) 
		{	// _Pdst[i] = _Psrc[i] + _Scalar
		while (_Size--) { _Pdst[_Size] = _Psrc[_Size] + _Scalar; }
		}

	template<> inline
		void _Add_vector_scalar<float>(float* _Pdst, const float* _Psrc, _in(float) _Scalar, size_t _Size)
		{	// _Pdst[i] = _Psrc[i] + _Scalar
		vec128f _Lhs;
		vec128f _Rhs = simd_128f::all(_Scalar);

		while (_Size >= 4)
			{
			_Size -= 4;
			_Lhs = simd_128f::load(_Psrc + _Size);
			_Lhs = simd_128f::add(_Lhs, _Rhs);
			simd_128f::store(_Pdst + _Size, _Lhs);
			}

		if (_Size == 0) return;
		_Lhs = simd_128f::load(_Psrc);
		_Lhs = simd_128f::add(_Lhs, _Rhs);
		while (_Size--) { _Pdst[_Size] = simd_128f::get(_Lhs, _Size); }
		}


	template<typename _Ty> inline
		void _Add_vector_vector(_Ty* _Pdst, const _Ty* _Plhs, const _Ty* _Prhs, size_t _Size) 
		{	// _Pdst[i] = _Plhs[i] + _Prhs[i]
		while (_Size--) { _Pdst[_Size] = _Plhs[_Size] + _Prhs[_Size]; }
		}

	template<> inline
		void _Add_vector_vector<float>(float* _Pdst, const float* _Plhs, const float* _Prhs, size_t _Size)
		{
#if defined(CLMAGIC_USING_MKL)
		vsAdd(int(_Size), _Plhs, _Prhs, _Pdst);
#else
		vec128f _Lhs;
		vec128f _Rhs;

		while (_Size >= 4)
			{
			_Size -= 4;
			_Lhs = simd_128f::load(_Plhs + _Size);
			_Rhs = simd_128f::load(_Prhs + _Size);
			_Lhs = simd_128f::add(_Lhs, _Rhs);
			simd_128f::store(_Pdst + _Size, _Lhs);
			}

		if (_Size != 0)
			{
			_Lhs = simd_128f::load(_Plhs);
			_Rhs = simd_128f::load(_Prhs);
			_Lhs = simd_128f::add(_Lhs, _Rhs);
			while (_Size--) { _Pdst[_Size] = simd_128f::get(_Lhs, _Size); }
			}
#endif
		}

	template<> inline
		void _Add_vector_vector<int>(int* _Pdst, const int* _Plhs, const int* _Prhs, size_t _Size)
		{
		vec128i _Lhs;
		vec128i _Rhs;

		while (_Size >= 4)
			{
			_Size -= 4;
			_Lhs = simd_128i32::load(_Plhs + _Size);
			_Rhs = simd_128i32::load(_Prhs + _Size);
			_Lhs = simd_128i32::add(_Lhs, _Rhs);
			simd_128i32::store(_Pdst + _Size, _Lhs);
			}

		if (_Size != 0)
			{
			_Lhs = simd_128i32::load(_Plhs);
			_Rhs = simd_128i32::load(_Prhs);
			_Lhs = simd_128i32::add(_Lhs, _Rhs);
			while (_Size--) { _Pdst[_Size] = simd_128i32::get(_Lhs, _Size); }
			}
		}
	// </ add >

	// < sub >
	template<typename _Ty> inline
		void _Sub_vector_scalar(_Ty* _Pdst, const _Ty* _Psrc, _in(_Ty) _Scalar, size_t _Size) 
		{	// _Pdst[i] = _Psrc[i] - _Scalar
		while (_Size--) { _Pdst[_Size] = _Psrc[_Size] - _Scalar; }
		}

	template<> inline
		void _Sub_vector_scalar<float>(float* _Pdst, const float* _Psrc, _in(float) _Scalar, size_t _Size)
		{	// _Pdst[i] = _Psrc[i] - _Scalar
		vec128f _Lhs;
		vec128f _Rhs = simd_128f::all(_Scalar);

		while (_Size >= 4)
			{
			_Size -= 4;
			_Lhs = simd_128f::load(_Psrc + _Size);
			_Lhs = simd_128f::sub(_Lhs, _Rhs);
			simd_128f::store(_Pdst + _Size, _Lhs);
			}

		if (_Size == 0) return;
		_Lhs = simd_128f::load(_Psrc);
		_Lhs = simd_128f::sub(_Lhs, _Rhs);
		while (_Size--) { _Pdst[_Size] = simd_128f::get(_Lhs, _Size); }
		}


	template<typename _Ty> inline
		void _Sub_vector_vector(_Ty* _Pdst, const _Ty* _Plhs, const _Ty* _Prhs, size_t _Size) 
		{	// _Pdst[i] = _Plhs[i] - _Prhs[i]
		while (_Size--) { _Pdst[_Size] = _Plhs[_Size] - _Prhs[_Size]; }
		}

	template<> inline
		void _Sub_vector_vector<float>(float* _Pdst, const float* _Plhs, const float* _Prhs, size_t _Size)
		{
#if defined(CLMAGIC_USING_MKL)
		vsSub(int(_Size), _Plhs, _Prhs, _Pdst);
#else
		vec128f _Lhs;
		vec128f _Rhs;

		while (_Size >= 4)
			{
			_Size -= 4;
			_Lhs = simd_128f::load(_Plhs + _Size);
			_Rhs = simd_128f::load(_Prhs + _Size);
			_Lhs = simd_128f::sub(_Lhs, _Rhs);
			simd_128f::store(_Pdst + _Size, _Lhs);
			}

		if (_Size == 0) return;
		_Lhs = simd_128f::load(_Plhs);
		_Rhs = simd_128f::load(_Prhs);
		_Lhs = simd_128f::sub(_Lhs, _Rhs);
		while (_Size--) { _Pdst[_Size] = simd_128f::get(_Lhs, _Size); }
#endif
		}

	template<> inline
		void _Sub_vector_vector<double>(double* _Pdst, const double* _Plhs, const double* _Prhs, size_t _Size)
		{
#if defined(CLMAGIC_USING_MKL)
		vdSub(int(_Size), _Plhs, _Prhs, _Pdst);
#else
		while (_Size--) { _Pdst[_Size] = _Plhs[_Size] - _Prhs[_Size]; }
#endif
		}

	template<> inline
		void _Sub_vector_vector<int>(int* _Pdst, const int* _Plhs, const int* _Prhs, size_t _Size)
		{
		vec128i _Lhs;
		vec128i _Rhs;

		while (_Size >= 4)
			{
			_Size -= 4;
			_Lhs = simd_128i32::load(_Plhs + _Size);
			_Rhs = simd_128i32::load(_Prhs + _Size);
			_Lhs = simd_128i32::sub(_Lhs, _Rhs);
			simd_128i32::store(_Pdst + _Size, _Lhs);
			}

		if (_Size == 0) return;
		_Lhs = simd_128i32::load(_Plhs);
		_Rhs = simd_128i32::load(_Prhs);
		_Lhs = simd_128i32::sub(_Lhs, _Rhs);
		while (_Size--) { _Pdst[_Size] = simd_128i32::get(_Lhs, _Size); }
		}
	// </ sub >

	// < mul >
	template<typename _Ty> inline
		void _Mul_vector_scalar(_Ty* _Pdst, const _Ty* _Psrc, _in(_Ty) _Scalar, size_t _Size) 
		{	// _Pdst[i] = _Psrc[i] * _Scalar
		for (size_t i = 0; i != _Size; ++i)
			{
			_Pdst[i] = _Psrc[i] * _Scalar;
			}
		}

	template<> inline
		void _Mul_vector_scalar<float>(float* _Pdst, const float* _Psrc, _in(float) _Scalar, size_t _Size)
		{
//#if defined(CLMAGIC_USING_MKL)
//		cblas_saxpy(int(_Size), _Scalar, _Psrc, 1, _Pdst, 1);
//#else
		vec128f _Lhs;
		vec128f _Rhs = simd_128f::all(_Scalar);

		while (_Size >= 4)
			{
			_Size -= 4;
			_Lhs = simd_128f::load(_Psrc + _Size);
			_Lhs = simd_128f::mul(_Lhs, _Rhs);
			simd_128f::store(_Pdst + _Size, _Lhs);
			}

		if(_Size == 0) return;
		_Lhs = simd_128f::load(_Psrc);
		_Lhs = simd_128f::mul(_Lhs, _Rhs);
		while(_Size--) { _Pdst[_Size] = simd_128f::get(_Lhs, _Size); }
//#endif
		}

	template<> inline
		void _Mul_vector_scalar<int>(int* _Pdst, const int* _Psrc, _in(int) _Scalar, size_t _Size)
		{
		vec128i _Lhs;
		vec128i _Rhs = simd_128i32::all(_Scalar);

		while (_Size >= 4)
			{
			_Size -= 4;
			_Lhs = simd_128i32::load(_Psrc + _Size);
			_Lhs = simd_128i32::mul(_Lhs, _Rhs);
			simd_128i32::store(_Pdst + _Size, _Lhs);
			}

		if(_Size == 0) return;
		_Lhs = simd_128i32::load(_Psrc);
		_Lhs = simd_128i32::mul(_Lhs, _Rhs);
		while(_Size--) { _Pdst[_Size] = simd_128i32::get(_Lhs, _Size); }
		}


	template<typename _Ty> inline
		void _Mul_vector_vector(_Ty* _Pdst, const _Ty* _Plhs, const _Ty* _Prhs, size_t _Size)
		{	// _Pdst[i] = _Plhs[i] * _Rhs[i]
		while (_Size--) { _Pdst[_Size] = _Plhs[_Size] * _Prhs[_Size]; }
		}

	template<> inline
		void _Mul_vector_vector<float>(float* _Pdst, const float* _Plhs, const float* _Prhs, size_t _Size)
		{	// _Pdst[i] = _Plhs[i] * _Rhs[i]
#if defined(CLMAGIC_USING_MKL)
		vsMul(int(_Size), _Plhs, _Prhs, _Pdst);
#else
		vec128f _Lhs;
		vec128f _Rhs;

		while (_Size >= 4)
			{
			_Size -= 4;
			_Lhs = simd_128f::load(_Plhs + _Size);
			_Rhs = simd_128f::load(_Prhs + _Size);
			_Lhs = simd_128f::mul(_Lhs, _Rhs);
			simd_128f::store(_Pdst + _Size, _Lhs);
			}

		if (_Size == 0) return;
		_Lhs = simd_128f::load(_Plhs);
		_Rhs = simd_128f::load(_Prhs);
		_Lhs = simd_128f::mul(_Lhs, _Rhs);
		while (_Size--) { _Pdst[_Size] = simd_128f::get(_Lhs, _Size); }
#endif
		}
	
	template<> inline
		void _Mul_vector_vector<double>(double* _Pdst, const double* _Plhs, const double* _Prhs, size_t _Size)
		{	// _Pdst[i] = _Plhs[i] * _Rhs[i]
#if defined(CLMAGIC_USING_MKL)
		vdMul(int(_Size), _Plhs, _Prhs, _Pdst);
#else
		while (_Size--) { _Pdst[_Size] = _Plhs[_Size] * _Prhs[_Size]; }
#endif
	}
	// </ mul >

	// < div >
	template<typename _Ty> inline
		void _Div_vector_scalar(_Ty* _Pdst, const _Ty* _Psrc, _in(_Ty) _Scalar, size_t _Size) 
		{	// _Pdst[i] = _Psrc[i] / _Scalar
		while (_Size--) { _Pdst[_Size] = _Psrc[_Size] / _Scalar; }
		}

	template<> inline
		void _Div_vector_scalar<float>(float* _Pdst, const float* _Psrc, _in(float) _Scalar, size_t _Size)
		{
#if defined(CLMAGIC_USING_MKL)
		cblas_saxpy(int(_Size), 1.f / _Scalar, _Psrc, 1, _Pdst, 1);
#else
		vec128f _Lhs;
		vec128f _Rhs = simd_128f::all(_Scalar);

		while (_Size >= 4)
			{
			_Size -= 4;
			_Lhs = simd_128f::load(_Psrc + _Size);
			_Lhs = simd_128f::div(_Lhs, _Rhs);
			simd_128f::store(_Pdst + _Size, _Lhs);
			}

		if (_Size == 0) return;
		_Lhs = simd_128f::load(_Psrc);
		_Lhs = simd_128f::div(_Lhs, _Rhs);
		while (_Size--) { _Pdst[_Size] = simd_128f::get(_Lhs, _Size); }
#endif
		}

	template<> inline
		void _Div_vector_scalar<double>(double* _Pdst, const double* _Psrc, _in(double) _Scalar, size_t _Size)
		{
#if defined(CLMAGIC_USING_MKL)
		cblas_daxpy(int(_Size), 1.f / _Scalar, _Psrc, 1, _Pdst, 1);
#else
	for (size_t i = 0; i != _Size; ++i)
		{
		_Pdst[i] = _Psrc[i] / _Scalar;
		}
#endif
		}


	template<typename _Ty> inline
		void _Div_scalar_vector(_Ty* _Pdst, _in(_Ty) _Scalar, const _Ty* _Prhs, size_t _Size)
		{	// _Pdst[_Size] = _Scalar / _Prhs[_Size]
		while (_Size--) { _Pdst[_Size] = _Scalar / _Prhs[_Size]; }
		}

	template<> inline
		void _Div_scalar_vector<float>(float* _Pdst, _in(float) _Scalar, const float* _Prhs, size_t _Size)
		{
		vec128f _Lhs = simd_128f::all(_Scalar);
		vec128f _Rhs;

		while (_Size >= 4)
			{
			_Size -= 4;
			_Rhs = simd_128f::load(_Prhs + _Size);
			_Rhs = simd_128f::div(_Lhs, _Rhs);
			simd_128f::store(_Pdst + _Size, _Rhs);
			}

		if (_Size == 0) return;
		_Rhs = simd_128f::load(_Prhs);
		_Rhs = simd_128f::div(_Lhs, _Rhs);
		while (_Size--) { _Pdst[_Size] = simd_128f::get(_Rhs, _Size); }
		}


	template<typename _Ty> inline
		void _Div_vector_vector(_Ty* _Pdst, const _Ty* _Plhs, const _Ty* _Prhs, size_t _Size) 
		{	// _Pdst[i] = _Plhs[i] / _Prhs[i]
		for (size_t i = 0; i != _Size; ++i)
			{
			_Pdst[i] = _Plhs[i] / _Prhs[i];
			}
		}

	template<> inline
		void _Div_vector_vector<float>(float* _Pdst, const float* _Plhs, const float* _Prhs, size_t _Size)
		{
#if defined(CLMAGIC_USING_MKL)
		vsDiv(int(_Size), _Plhs, _Prhs, _Pdst);
#else
		vec128f _Lhs;
		vec128f _Rhs;

		while (_Size >= 4)
			{
			_Size -= 4;
			_Lhs = simd_128f::load(_Plhs + _Size);
			_Rhs = simd_128f::load(_Prhs + _Size);
			_Lhs = simd_128f::div(_Lhs, _Rhs);
			simd_128f::store(_Pdst + _Size, _Lhs);
			}

		if (_Size != 0)
			{
			_Lhs = simd_128f::load(_Plhs);
			_Rhs = simd_128f::load(_Prhs);
			_Lhs = simd_128f::div(_Lhs, _Rhs);
			while ((--_Size) != -1)
				{
				_Pdst[_Size] = simd_128f::get(_Lhs, _Size);
				}
			}
#endif
		}

	template<> inline
		void _Div_vector_vector<double>(double* _Pdst, const double* _Plhs, const double* _Prhs, size_t _Size)
		{
#if defined(CLMAGIC_USING_MKL)
		vdDiv(int(_Size), _Plhs, _Prhs, _Pdst);
#else
	for (size_t i = 0; i != _Size; ++i)
		{
		_Pdst[i] = _Plhs[i] / _Prhs[i];
		}
#endif
		}
	// </ div >

	// < mod >
	template<typename _Ty> inline
		void _Mod_vector_scalar(_Ty* _Pdst, const _Ty* _Plhs, _in(_Ty) _Scalar, size_t _Size) 
		{	// _Pdst[i] = _Plhs[i] % _Scalar
		while (_Size--) { _Pdst[_Size] = mod(_Plhs[_Size], _Scalar); }
		}

	template<> inline
		void _Mod_vector_scalar<float>(float* _Pdst, const float* _Plhs, _in(float) _Scalar, size_t _Size)
		{	// _Pdst[i] = _Plhs[i] % _Scalar
		while (_Size--) { _Pdst[_Size] = mod(_Plhs[_Size], _Scalar); }
		}

	template<> inline
		void _Mod_vector_scalar<double>(double* _Pdst, const double* _Plhs, _in(double) _Scalar, size_t _Size)
		{	// _Pdst[i] = _Plhs[i] % _Scalar
		while (_Size--) { _Pdst[_Size] = mod(_Plhs[_Size], _Scalar); }
		}


	template<typename _Ty> inline
		void _Mod_vector_vector(_Ty* _Pdst, const _Ty* _Plhs, const _Ty* _Prhs, size_t _Size) 
		{	// _Pdst[i] = _Plhs[i] % _Prhs[i]
		while (_Size--) { _Pdst[_Size] = mod(_Plhs[_Size], _Prhs[_Size]); }
		}

	template<> inline
		void _Mod_vector_vector<float>(float* _Pdst, const float* _Plhs, const float* _Prhs, size_t _Size)
		{
#if defined(CLMAGIC_USING_MKL)
		vsFmod(static_cast<int>(_Size), _Plhs, _Prhs, _Pdst);
#else
		while (_Size--) { _Pdst[_Size] = mod(_Plhs[_Size], _Prhs[_Size]); }
#endif
		}

	template<> inline
		void _Mod_vector_vector<double>(double* _Pdst, const double* _Plhs, const double* _Prhs, size_t _Size)
		{	// each (dst = lhs % rhs)
#if defined(CLMAGIC_USING_MKL)
		vdFmod(static_cast<int>(_Size), _Plhs, _Prhs, _Pdst);
#else
		while (_Size--) { _Pdst[_Size] = mod(_Plhs[_Size], _Prhs[_Size]); }
#endif
		}
	// </ mod >

	// < pow >
	template<typename _Ty> inline
		void _Pow_vector_scalar(_Ty* _Pdst, const _Ty* _Psrc, int _Power, size_t _Size) 
		{	using std::pow;	// pow int
		while (_Size--) { _Pdst[_Size] = pow(_Psrc[_Size], _Power); }
		}

	template<typename _Ty> inline
		void _Pow_vector_scalar(_Ty* _Pdst, const _Ty* _Psrc, float _Power, size_t _Size)
		{	using std::pow;	// pow float
		while (_Size--) { _Pdst[_Size] = pow(_Psrc[_Size], _Power); }
		}

	template<typename _Ty> inline
		void _Pow_vector_scalar(_Ty* _Pdst, const _Ty* _Psrc, double _Power, size_t _Size)
		{	using std::pow;	// pow double
		while (_Size--) { _Pdst[_Size] = pow(_Psrc[_Size], _Power); }
		}

	template<typename _Ty> inline
		void _Pow_vector_vector(_Ty* _Pdst, const _Ty* _Plhs, const _Ty* _Prhs, size_t _Size) 
		{	using std::pow;
		while (_Size--) { _Pdst[_Size] = pow(_Plhs[_Size], _Prhs[_Size] ); }
		}

	template<> inline
		void _Pow_vector_vector<float>(float* _Pdst, const float* _Plhs, const float* _Prhs, size_t _Size)
		{
#if defined(CLMAGIC_USING_MKL)
		vsPow( int(_Size), _Plhs, _Prhs, _Pdst );
#else
		using std::pow;
		while (_Size--) { _Pdst[_Size] = pow(_Plhs[_Size], _Prhs[_Size]); }
#endif
		}
	// </ pow >

	// < sqrt >
	template<typename _Ty> inline
		void _Sqrt_vector(_Ty* _Pdst, const _Ty* _Psrc, size_t _Size) 
		{	// _Pdst[i] = sqrt(_Psrc[i])
		while (_Size-- != 0) { _Pdst[_Size] = sqrt(_Psrc[_Size]); }
		}

	template<> inline
		void _Sqrt_vector<float>(float* _Pdst, const float* _Psrc, size_t _Size)
		{	// _Pdst[i] = sqrt(_Psrc[i])
#if defined(CLMAGIC_USING_MKL)
		vsSqrt(int(_Size), _Psrc, _Pdst);
#else
		while (_Size-- != 0) { _Pdst[_Size] = sqrt(_Psrc[_Size]); }
#endif
		}

	template<> inline
		void _Sqrt_vector<double>(double* _Pdst, const double* _Psrc, size_t _Size)
		{	// _Pdst[i] = sqrt(_Psrc[i])
#if defined(CLMAGIC_USING_MKL)
		vdSqrt(int(_Size), _Psrc, _Pdst);
#else
		while (_Size-- != 0) { _Pdst[_Size] = sqrt(_Psrc[_Size]); }
#endif
		}
	// </ sqrt >

	// < floor >
	template<typename _Ty> inline
		void _Floor_vector( /*out*/_Ty* _Pdst, /*in*/const _Ty* _Plhs, size_t _Size) 
		{	// dst[i] = floor(lhs[i])
		while (_Size--) { _Pdst[_Size] = floor( _Plhs[_Size] ); }
		}
	// </ floor >

	// < ceil >
	template<typename _Ty> inline
		void _Ceil_vector( /*out*/_Ty* _Pdst, /*in*/const _Ty* _Plhs, size_t _Size)
		{	// dst[i] = ceil(lhs[i])
		while (_Size--) { _Pdst[_Size] = ceil(_Plhs[_Size]); }
		}
	// </ ceil >
	
	/* < triangle function > */
	template<typename _Ty> inline
		void _Sin_vector( /*out*/_Ty* _Pdst, /*in*/const _Ty* _Plhs, size_t _Size)
		{	// dst[i] = sin(lhs[i])
		while (_Size--) { _Pdst[_Size] = sin(_Plhs[_Size]); }
		}

	template<typename _Ty> inline 
		void _Cos_vector( /*out*/ _Ty* _Pdst, /*in*/const _Ty* _Plhs, size_t _Size) 
		{	// dst[i] = cos(lhs[i])
		while (_Size--) { _Pdst[_Size] = cos(_Plhs[_Size]); }
		}

	template<typename _Ty> inline
		void _Tan_vector( /*out*/ _Ty* _Pdst, /*in*/const _Ty* _Plhs, size_t _Size)
		{	// dst[i] = tan(lhs[i])
		while (_Size--) { _Pdst[_Size] = tan(_Plhs[_Size]); }
		}

	template<typename _Ty> inline
		void _Asin_vector( /*out*/ _Ty* _Pdst, /*in*/const _Ty* _Plhs, size_t _Size)
		{	// dst[i] = asin(lhs[i])
		while (_Size--) { _Pdst[_Size] = asin(_Plhs[_Size]); }
		}

	template<typename _Ty> inline
		void _Acos_vector( /*out*/ _Ty* _Pdst, /*in*/const _Ty* _Plhs, size_t _Size)
		{	// dst[i] = acos(lhs[i])
		while (_Size--) { _Pdst[_Size] = acos(_Plhs[_Size]); }
		}

	template<typename _Ty> inline
		void _Atan_vector( /*out*/ _Ty* _Pdst, /*in*/const _Ty* _Plhs, size_t _Size)
		{	// dst[i] = atan(lhs[i])
		while (_Size--) { _Pdst[_Size] = atan(_Plhs[_Size]); }
		}
	/* </ triangle function > */ 

	// < sum >
	template<typename _Ty>
		_Ty _Sum_vector(const _Ty* _Plhs, size_t _Size)
		{	// dst = ¦²lhs[i]
		_Ty _Result = _Ty(0);
		while (_Size--) { _Result += _Plhs[_Size]; }
		return ( _Result );
		}

	template<> inline
		float _Sum_vector<float>(const float* _Psrc, size_t _Size)
		{	// dst = ¦²lhs[i]
#if defined(CLMAGIC_USING_MKL)
		return ( cblas_sasum(static_cast<int>(_Size), _Psrc, 1) );
#else
		float _Result = 0.f;
		vec128f _Lhs;

		while (_Size >= 4)
			{
			_Size -= 4;
			_Lhs = simd_128f::load(_Psrc + _Size);
			_Lhs = simd_128f::sum(_Lhs);
			_Result += simd_128f::get(_Lhs, 0);
			}

		while ((--_Size) != -1)
			{
			_Result += _Psrc[_Size];
			}
		return (_Result);
#endif
		}
	// </ sum >

	// < dot >
	template<typename _Ty> inline
		_Ty _Dot_vector(const _Ty* _Plhs, const _Ty* _Prhs, size_t _Size)
		{	// dst = ¦²(lhs[i] * rhs[i])
		_Ty _Result = _Ty(0); 
		for (size_t i = 0; i != _Size; ++i)
			{
			_Result += (_Plhs[i] * _Prhs[i]);
			}
		return (_Result);
		}

	template<> inline
		float _Dot_vector<float>(const float* _Plhs, const float* _Prhs, size_t _Size)
		{	// dst = ¦²lhs[i]
#if defined(CLMAGIC_USING_MKL)
		return ( cblas_sdot(int(_Size), _Plhs, 1, _Prhs, 1) );
#else
		float _Result = 0.f;
		vec128f _Lhs;
		vec128f _Rhs;

		while ( _Size >= 4 )
			{
			_Size -= 4;
			_Lhs = simd_128f::load(_Plhs + _Size);
			_Rhs = simd_128f::load(_Prhs + _Size);
			_Lhs = simd_128f::mul(_Lhs, _Rhs);
			_Lhs = simd_128f::sum(_Lhs);
			_Result += simd_128f::get(_Lhs, 0);
			}

		if( _Size != 0 )
			{
			_Lhs = simd_128f::load( _Plhs );
			_Rhs = simd_128f::load( _Prhs );
			_Lhs = simd_128f::mul( _Lhs, _Rhs );
			while ((--_Size) != -1) 
				{
				_Result += simd_128f::get(_Lhs, _Size);
				}
			}
		return (_Result);
#endif
		}
	// </ dot >

	// < mul matrix >
	template<typename _Ty> inline
		void _Mul_matrix( /*out*/_Ty* _Pdst, /*in*/const _Ty* _Plhs, size_t _M, size_t _N, /*in*/const _Ty* _Prhs, size_t _P) 
		{	// [m, p]
		auto _Dst = [_Pdst, _P](size_t _Row, size_t _Col) ->_Ty&	   { return (_Pdst[_Row * _P + _Col]); };// mat mxp
		auto _Lhs = [_Plhs, _N](size_t _Row, size_t _Col) ->const _Ty& { return (_Plhs[_Row * _N + _Col]); };// mat_mxn
		auto _Rhs = [_Prhs, _P](size_t _Row, size_t _Col) ->const _Ty& { return (_Prhs[_Row * _P + _Col]); };// mat_nxp
		
		for (size_t i = 0; i < _M; ++i)
			{
			for (size_t j = 0; j < _P; ++j)
				{
				_Ty _Val = static_cast<_Ty>(0);
				for (size_t k = 0; k < _N; ++k)
					{
					_Val += _Lhs(i, k) * _Rhs(k, j);
					}
				_Dst(i, j) = _Val;
				}
			}
		}

	template<> inline
		void _Mul_matrix<float>( /*out*/float* _Pdst, /*in*/const float* _Plhs, size_t _M, size_t _N, /*in*/const float* _Prhs, size_t _P)
		{	// [m, p]
#if defined(CLMAGIC_USING_MKL)
		cblas_sgemm(
			CBLAS_LAYOUT::CblasRowMajor, 
			CBLAS_TRANSPOSE::CblasNoTrans, 
			CBLAS_TRANSPOSE::CblasNoTrans, 
			int(_M), int(_N), int(_P), 
			1.f, _Plhs, int(_N), 
			_Prhs, int(_N), 1.f, 
			_Pdst, int(_P)
			);
#else
		auto _Dst = [_Pdst, _P](size_t _Row, size_t _Col) ->float&		 { return (_Pdst[_Row * _P + _Col]); };// mat mxp
		auto _Lhs = [_Plhs, _N](size_t _Row, size_t _Col) ->const float& { return (_Plhs[_Row * _N + _Col]); };// mat_mxn
		auto _Rhs = [_Prhs, _P](size_t _Row, size_t _Col) ->const float& { return (_Prhs[_Row * _P + _Col]); };// mat_nxp

		for (size_t i = 0; i < _M; ++i)
			{
			for (size_t j = 0; j < _P; ++j)
				{
				float _Val = 0.f;
				for (size_t k = 0; k < _N; ++k)
					{
					_Val += _Lhs(i, k) * _Rhs(k, j);
					}
				_Dst(i, j) = _Val;
				}
			}
#endif
		}
	// </ mul matrix>

	template<typename _Ty> inline
		int _Lu( /*out*/ _Ty* _Pdst, /*inout*/_Ty* _Plhs, size_t _M, size_t _N) 
		{
		size_t i, j, k;
		int _Result = 1;
		
		auto _Dst = [_Pdst, _N](size_t _Row, size_t _Col) ->_Ty& { return (_Pdst[_Row * _N + _Col]); };
		auto _Lhs = [ _Plhs, _N ]( size_t _Row, size_t _Col ) ->_Ty& { return ( _Plhs[_Row * _N + _Col] ); };

		for ( i = 0; i < _M; ++i ) 
			{	
			// 1. major pos
			k = i;
			for ( j = i + 1; j < _M; ++j )
				if ( abs(_Lhs(j, i)) > abs(_Lhs(k, i)) )
					k = j;

			// 2. swap major
			if ( k != i ) 
				{
				for ( j = i; j < _M; ++j ) 
					std::swap( _Lhs(i, j), _Lhs(k, j) );
				for ( j = 0; j < _N; ++j ) 
					std::swap( _Dst(i, j), _Dst(k, j) );
				_Result = -_Result;
				}

			// 3. elimination
			_Ty _Tmp = _Ty(-1) / _Lhs(i, i);
			for ( j = i + 1; j < _M; ++j ) 
				{
				_Ty _Alpha = _Lhs(j, i) * _Tmp;

				for ( k = i + 1; k < _M; ++k ) 
					_Lhs(j, k) += _Alpha * _Lhs(i, k);

				for ( k = 0; k < _N; ++k ) 
					_Dst(j, k) += _Alpha * _Dst(i, k);
				}
			}


		// final
		for (i = _M - 1; i != -1; --i) 
			{	
			for (j = 0; j < _N; ++j) 
				{
				_Ty _Tmp = _Dst(i, j);
				for ( k = i + 1; k < _M; ++k ) 
					_Tmp -= _Lhs(i, k) * _Dst(k, j);

				_Dst(i, j) = _Tmp / _Lhs(i, i);
				}
			}

		return ( _Result );
		}

}// namespace clmagic


#endif