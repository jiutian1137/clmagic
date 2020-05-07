//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
// Apache Licene 2.0
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_calculation_calculus_DIFFERENTIATION_h_
#define clmagic_calculation_calculus_DIFFERENTIATION_h_

namespace clmagic {

	template<typename _Ty, typename _Fn>
	_Ty slope(_Fn f, const _Ty& x0, const _Ty& x1) {
		const auto dy = f(x1) - f(x0);
		const auto dx = x1 - x0;
		return dy / dx;
		/*<another>
			f(x1) - f(x0)
			-------------
			   x1 - x0
		  </another>*/
	}


	/*<Differentiation-Rule>
		_Ty X = any_value;

		<first>
			if f = [](){ return constant_value; }
				derivative(f, X) == 0;
		</first>
		<second>
			if f = [&n](auto&& x){ return pow(x, n); }
				derivative(f, X) == n * pow(X, n-1);
		</second>
		<third>
			if f = any_function, c = constant_value
				derivative(f, X * c) == c * derivative(f, X);
		</third>
		<fourth>
			if f1 = .., v = f2 = .., f1_add_f2 = [](auto&& x){ return f1(x) + f2(x); }
				derivative(f1_add_f2, X) == derivative(f1, X) + derivative(f2, X)
		</fourth>
		<fifth>
			if f1 = .., v = f2 = .., f1_mul_f2 = [](auto&& x){ return f1(x) * f2(x); }
				derivative(f1_mul_f2, X) == f1(X) * derivative(f2, X) + f2(X) * derivative(f1, X)
		</fifth>
		<sixth>
			if f1 = .., v = f2 = .., f1_div_f2 = [](auto&& x){ return f1(x) / f2(x); }
				derivative(f1_div_f2, X) == derivative(f1_mul_f2, X) / (f2(X) * f2(X))
		</sixth>

		<basic>         #0.section3.2 </basic>
		<trigonometric> #0.section3.4 </trigonometric>

		<Reference id=0>Thomas-Calculus</Reference>
	</Differentiation-Rule>*/

	/*
	if dF(..) exist at particular x                    
		we say that F(..) is Differentiable
	if dF(..) exist at every point in the domain F(..)
		we say F(..) is Differentiable
	*/
	template<typename _Ty, typename _Fn>
	_Ty derivative(_Fn f, const _Ty& x, const _Ty& h) {
		const auto dfx = f(x + h) - f(x);
		const auto dx  = h;
		return dfx / dx;
		/*<another>
			 f(x+h) - f(x)
			--------------
				  h
		</another>*/
		/*<another>
			slope(f, x. x+h)
		</another>*/
	}

	template<typename _Ty, typename _Fn>
	_Ty derivative(_Fn f, const _Ty& x) {
		return ::clmagic::derivative(f, x, static_cast<_Ty>(0.0001));
	}

}// namespace clmagic

#endif // !clmagic_math_calculus_DERIVATIVE_h_
