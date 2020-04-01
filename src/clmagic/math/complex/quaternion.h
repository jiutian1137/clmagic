#pragma once
#ifndef clmagic_math_complex_QUATERNION_h_
#define clmagic_math_complex_QUATERNION_h_
#include <complex>
#include "../lapack/vector.h"
#include "../general//unit.h"

namespace clmagic {
	template<typename _Ty>
	struct complex : std::complex<_Ty>{
		using _Mybase = std::complex<_Ty>;

		constexpr complex(const _Ty& _Realval = _Ty(), const _Ty& _Imagval = _Ty()) : _Mybase(_Realval, _Imagval) {}
	};



	


}// namespace clmagic

#endif