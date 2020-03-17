//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_math_CLMAGIC_h_
#define clmagic_math_CLMAGIC_h_
#include <string>

namespace clmagic
{
	/* @_clmagic: graphics library */
}

/* reference */
//#define CLMAGIC_USING_MKL 1
#define CLMAGIC_USING_DXmatH 1

/* Intel math kernel library */
#if defined(CLMAGIC_USING_MKL)
#include <mkl.h>
#endif

/* MicrosoftDirectX math library */
#if defined(CLMAGIC_USING_DXmatH)
#include <DirectXmath.h>
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

#ifndef _friend
#define _friend(...)
#endif

namespace clmagic {
	template<typename _Tydst, typename _Tysrc> inline
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
	
	constexpr size_t constexpr_align_div(size_t N, size_t _Bound) {
		return (constexpr_align(N, _Bound) / _Bound);
	}

	inline size_t offset_align_of(size_t _Bound, const void* _Ptr) {
		// _Ptr offset on boundary _Bound
		return static_cast<size_t>(reinterpret_cast<uintptr_t>(_Ptr)& (_Bound - 1));
	}


	constexpr bool constexpr_aligned16(size_t N) {
		// 0x00000552d0
		// &
		// 0x0000000001 = (0x0000000010-1)
		// 0x0000000000
		constexpr size_t _Mask = 16 - 1;
		return ((N & _Mask) == 0 ? true : false);
	}

	
	template<typename _OutTy, typename _InTy, typename ..._Tys>
	void _Shuffle_fill(_out(_OutTy) _Dest, size_t i, _in(_InTy) _Source, size_t s, _Tys... _Args) {
		_Dest[i] = _Source[s];
		_Shuffle_fill(_Dest, i + 1, _Source, _Args...);
	}

	template<typename _OutTy, typename _InTy>
	void _Shuffle_fill(_out(_OutTy) _Dest, size_t i, _in(_InTy) _Source, size_t s) {
		_Dest[i] = _Source[s];
	}

	/* 'shuffle' is often used in vector mathematics
	@_Example: See #simd_vec<> or #vec_<>
	*/
	template<typename _OutTy, typename _InTy>
	_OutTy shuffle(_in(_InTy) _Source, size_t i0, size_t i1) {
		return (_OutTy{ _Source[i0], _Source[i1] });
	}

	template<typename _OutTy, typename _InTy>
	_OutTy shuffle(_in(_InTy) _Source, size_t i0, size_t i1, size_t i2) {
		return (_OutTy{ _Source[i0], _Source[i1], _Source[i2] });
	}

	template<typename _OutTy, typename _InTy>
	_OutTy shuffle(_in(_InTy) _Source, size_t i0, size_t i1, size_t i2, size_t i3) {
		return (_OutTy{ _Source[i0], _Source[i1], _Source[i2], _Source[i3] });
	}

	template<typename _OutTy, typename _InTy>
	_OutTy shuffle(_in(_InTy) _Source, size_t i0, size_t i1, size_t i2, size_t i3, size_t i4) {
		return (_OutTy{ _Source[i0], _Source[i1], _Source[i2], _Source[i3], _Source[i4] });
	}

	template<typename _OutTy, typename _InTy>
	_OutTy shuffle(_in(_InTy) _Source, size_t i0, size_t i1, size_t i2, size_t i3, size_t i4, size_t i5) {
		return (_OutTy{ _Source[i0], _Source[i1], _Source[i2], _Source[i3], _Source[i4], _Source[i5] });
	}

	template<typename _OutTy, typename _InTy, typename ..._Tys>
	_OutTy shuffle(_in(_InTy) _Source, size_t i0, size_t i1, size_t i2, size_t i3, size_t i4, size_t i5, _Tys... _Selector) {
		_OutTy _Dest = shuffle<_OutTy>(_Source, i0, i1, i2, i3, i4, i5);
		_Shuffle_fill(_Dest, 6, _Source, _Selector...);
		return (_Dest);
	}

	template<typename _OutTy, typename _InTy, typename ..._Tys>
	void shuffle(_out(_OutTy) _Dest, _in(_InTy) _Source, _Tys... _Selector) {
		_Shuffle_fill(_Dest, 0, _Source, _Selector...);
	}

	template<typename ..._Tys>
	constexpr size_t types_size_v = std::tuple_size_v<std::tuple<_Tys...>>;
	

	/*
	@_template scalar type
	*/
	template<typename _Ty>
		constexpr bool is_support_scalar_operator = false;

	template<typename _Ty>
	struct _Unsupport_scalar {
		using scalar_type = _Ty;
	};

	template<typename _Ty>
	struct _Support_scalar {
		using scalar_type = typename _Ty::scalar_type;
	};

	template<typename _Ty>
	using scalar_type = typename std::conditional_t<is_support_scalar_operator<_Ty>,
			_Support_scalar<_Ty>, _Unsupport_scalar<_Ty> >::scalar_type;


	template<typename _Ty>
	struct _Basic_type {
		constexpr _Basic_type(_Ty _Val) : _Myval(_Val) { }

		constexpr operator _Ty() const { return (_Myval); }

		constexpr _Basic_type operator-() const { return (-_Myval); }
		constexpr _Basic_type operator~() const { return ~_Myval; }
		constexpr _Basic_type operator!() const { return !_Myval; }
		constexpr _Basic_type operator<<(size_t _Right) const { return _Myval << _Right; }
		constexpr _Basic_type operator>>(size_t _Right) const { return _Myval >> _Right; }
		constexpr _Basic_type operator+(_Basic_type _Right) const { return _Myval + _Right._Myval; }
		constexpr _Basic_type operator-(_Basic_type _Right) const { return _Myval - _Right._Myval; }
		constexpr _Basic_type operator*(_Basic_type _Right) const { return _Myval * _Right._Myval; }
		constexpr _Basic_type operator/(_Basic_type _Right) const { return _Myval / _Right._Myval; }
		constexpr _Basic_type operator%(_Basic_type _Right) const { return _Myval % _Right._Myval; }
		constexpr _Basic_type operator&(_Basic_type _Right) const { return _Myval & _Right._Myval; }
		constexpr _Basic_type operator|(_Basic_type _Right) const { return _Myval | _Right._Myval; }
		constexpr _Basic_type operator^(_Basic_type _Right) const { return _Myval ^ _Right._Myval; }
		
		constexpr _Basic_type& operator<<=(size_t _Right) { _Myval <<= _Right; return (*this); }
		constexpr _Basic_type& operator>>=(size_t _Right) { _Myval >>= _Right; return (*this); }
		constexpr _Basic_type& operator+=(_Basic_type _Right) { _Myval += _Right._Myval; return (*this); }
		constexpr _Basic_type& operator-=(_Basic_type _Right) { _Myval -= _Right._Myval; return (*this); }
		constexpr _Basic_type& operator*=(_Basic_type _Right) { _Myval *= _Right._Myval; return (*this); }
		constexpr _Basic_type& operator/=(_Basic_type _Right) { _Myval /= _Right._Myval; return (*this); }
		constexpr _Basic_type& operator%=(_Basic_type _Right) { _Myval %= _Right._Myval; return (*this); }
		constexpr _Basic_type& operator&=(_Basic_type _Right) { _Myval &= _Right._Myval; return (*this); }
		constexpr _Basic_type& operator|=(_Basic_type _Right) { _Myval |= _Right._Myval; return (*this); }
		constexpr _Basic_type& operator^=(_Basic_type _Right) { _Myval ^= _Right._Myval; return (*this); }

		friend std::ostream& operator<<(std::ostream & _Ostr, const _Basic_type<_Ty>& _Right) {
			return (_Ostr << std::to_string(_Right._Myval));
		}

	private: 
		_Ty _Myval;
	};

	template<typename _Ty, bool = std::is_arithmetic_v<_Ty>>
	struct any_class {
		using type = typename _Basic_type<_Ty>;
	};

	template<typename _Ty>
	struct any_class<_Ty, false> {
		using type = _Ty;
	};


}// namespace clmagic

#endif