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