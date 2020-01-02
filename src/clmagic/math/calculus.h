#pragma once
#ifndef clmagic_math_CALCULUS_h_
#define clmagic_math_CALCULUS_h_
#include "general.h"

namespace clmagic {
	template<typename _Fnd, typename _Fnd2, typename _Ty>
		_Ty Leibniz(_Fnd _Deriv, _Ty _Input, _Fnd2 _Deriv2, _Ty _Input2, size_t n, size_t i) {
			if (i == 0) {
				return ((_Ty)_Deriv(_Input, n));
			} else if (i != n) {
				return (C<_Ty> * (_Ty)_Deriv(_Input, n - i) * (_Ty)_Deriv2(_Input2, i));
			} else {
				return ((_Ty)_Deriv2(_Input2, n));
			}
		}

	template<typename _Fnd, typename _Fnd2, typename _Ty>
		_Ty Leibniz(_Fnd _Deriv, _Ty _Input, _Fnd2 _Deriv2, _Ty _Input2, size_t n) {
			_Ty _Result = Leibniz(_Deriv, _Input, _Deriv2, _Input2, n, 0);
			for (size_t i = 1; i <= n; ++i) {
				_Result += Leibniz(_Deriv, _Input, _Deriv2, _Input2, n, i);
			}
			return (_Result);
		}

	template<typename _Ty>
		_Ty sind(_Ty x) {
			return ((_Ty)cos(x));
		}

	template<typename _Ty>
		_Ty sind(_Ty x, size_t n) {
			/*
			sind(x, 1): cos(x)               = sin(x + ¦Ð/2)
			sind(x, 2): cos(x + ¦Ð/2)        = sin(x + ¦Ð/2 + ¦Ð/2)
			sind(x, 3): cos(x + ¦Ð/2 + ¦Ð/2) = sin(x + 3 * (¦Ð/2))
			*/
			return ((_Ty)sin(x + n * 1.57));
		}

	template<typename _Ty>
		_Ty cosd(_Ty x) {
			return ((_Ty)-sin(x));
		}

	template<typename _Ty>
		_Ty cosd(_Ty x, size_t n) {
			/*
			cosd(x, 1): -sin(x)               = cos(x + ¦Ð/2)
			cosd(x, 2): -sin(x + ¦Ð/2)        = cos(x + ¦Ð/2 + ¦Ð/2)
			cosd(x, 3): -sin(x + ¦Ð/2 + ¦Ð/2) = cos(x + 3 * (¦Ð/2))
			*/
			return ((_Ty)cos(x + n * 1.57));
		}

	template<typename _Ty>
		struct powd {
			_Ty operator()(_Ty x, size_t n) {
				/*
				powd(v, x, 0): v^x
				powd(v, x, 1): x * v^(x-1)
				powd(v, x, 2): 0 + x * (x-1) * v^(x-2)           = x(x-1) * v^(x-2)
				powd(v, x, 3): 0 + x(x-1) * (x-2) * v^(x-3)      = x(x-1)(x-2) * v^(x-3)
				powd(v, x, 4): 0 + x(x-1)(x-2) * (x-3) * v^(x-4) = x(x-1)(x-2)(x-3) * v^(x-4)
				*/
				if (x - _Ty(n) <= _Ty(-1)) {
					if (x - trunc(x) < _Ty(0.0001)) {
						return (_Ty(0));// (v^0)(dx/dy) = 0
					}
				}
				return ((_Ty)factorial(x, x - n) * (_Ty)pow(val, x - n));
			}

			_Ty val;
		};
}// namespace clmagic

#endif