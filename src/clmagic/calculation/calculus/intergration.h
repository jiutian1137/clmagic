#pragma once
#ifndef clmagic_calculation_calculus_INTERGRATION_h_
#define clmagic_calculation_calculus_INTERGRATION_h_

namespace clmagic {
	template<typename _Ty, typename _Fn, typename ..._Tys>
	auto intergral(_Ty t0, _Ty t1, _Ty dx, _Fn f, _Tys... args) {
		_Ty result = static_cast<_Ty>(0);
		for (; t0 <= t1; t0 += dx) {
			result += f(t0, args) * dx;
		}
		return result;
	}

}// namespace clmagic

#endif // !clmagic_calculation_calculus_INTERGRATION_h_
