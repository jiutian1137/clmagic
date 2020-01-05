#pragma once
#ifndef clmagic_math_CLMAGIC_h_
#define clmagic_math_CLMAGIC_h_

namespace clmagic
{
	/* @_clmagic: graphics library */
}

/* reference */
//#define CLMAGIC_USING_MKL 1
#define CLMAGIC_USING_DXMATH 1

/* Intel math kernel library */
#if defined(CLMAGIC_USING_MKL)
#include <mkl.h>
#endif

/* MicrosoftDirectX math library */
#if defined(CLMAGIC_USING_DXMATH)
#include <DirectXMath.h>
#include <DirectXCollision.h>
#endif

/* GLM math library */
#if defined(CLMAGIC_USING_GLM)
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif


/* please enter type */
#ifndef _in
#define _in(...) const __VA_ARGS__&
#endif
#ifndef _inout
#define _inout(...) __VA_ARGS__&
#endif
#ifndef _out
#define _out(...) __VA_ARGS__&
#endif

namespace clmagic {
	template<typename _Tydst, typename _Tysrc>
		_Tydst& reference_cast(_Tysrc& _Src) {
			return (*reinterpret_cast<_Tydst*>(&_Src));
		}

	template<typename _Tydst, typename _Tysrc> inline
		_Tydst& const_reference_cast(const _Tysrc& _Src) {
			return (*reinterpret_cast<_Tydst*>(&_Src));
		}

	constexpr size_t constexpr_align(size_t N, size_t _Bound) {
		size_t _Off = N & (_Bound - 1);
		return (_Off == 0 ? N : (N + _Bound - _Off));
	}

	/* 'shuffle' is often used in vector mathematics  */
	template<typename _OutTy, typename _InTy, typename ..._Tys>
		void _Shuffle_fill(_out(_OutTy) _Dest, _in(_InTy) _Source, size_t i, size_t s, _Tys... _Args) {
			_Dest[i] = _Source[s];
			_Shuffle_fill(_Dest, _Source, i + 1, _Args...);
		}

	template<typename _OutTy, typename _InTy>
		void _Shuffle_fill(_out(_OutTy) _Dest, _in(_InTy) _Source, size_t i, size_t s) {
			_Dest[i] = _Source[s];
		}

	template<typename _OutTy, typename _InTy, typename ..._Tys>
		_OutTy shuffle(_in(_InTy) _Source, _Tys... _Selector) {
			_OutTy _Dest;
			_Shuffle_fill(_Dest, _Source, 0, _Selector...);
			return (_Dest);
		}

	template<typename _OutTy, typename _InTy, typename ..._Tys>
		void shuffle(_out(_OutTy) _Dest, _in(_InTy) _Source, _Tys... _Selector) {
			_Shuffle_fill(_Dest, _Source, 0, _Selector...);
		}

}// namespace clmagic

#endif