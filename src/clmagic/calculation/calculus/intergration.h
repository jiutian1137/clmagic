//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
// Apache Licene 2.0
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_calculation_calculus_INTERGRATION_h_
#define clmagic_calculation_calculus_INTERGRATION_h_
#include "../general/general.h"

namespace clmagic {
	template<typename _Ty, typename _Fn>
	auto integrate(_Fn f, _Ty t0, _Ty t1, _Ty dx) {
		return left_Riemann_sum(f, t0, t1, dx);
	}

	template<typename _Fn, typename _Ty>
	auto integrate_n(_Fn f, _Ty t0, _Ty t1, size_t n) {
		const auto dx = (t1 - t0) / static_cast<_Ty>(n);
		return left_Riemann_sum(f, t0, t1, dx);
	}

	/*<another>
		integrate(f, t0, t1) = antiderivate(f, t1) - antiderivative(f, t0) 
	</another>*/

}// namespace clmagic

#endif // !clmagic_calculation_calculus_INTERGRATION_h_
