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

	template<typename _Ty>
	constexpr size_t alignment_mask_of_v = std::alignment_of_v<_Ty> - 1;

	template<typename _Ty>
	constexpr bool is_aligned(const void* _Ptr) {
		return (static_cast<size_t>(_Ptr) & alignment_mask_of_v<_Ty> == 0);
	}

	constexpr bool constexpr_aligned16(size_t N) {
		// 0x00000552d0
		// &
		// 0x0000000001 = (0x0000000010-1)
		// 0x0000000000
		constexpr size_t _Mask = 16 - 1;
		return ((N & _Mask) == 0 ? true : false);
	}



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