//--------------------------------------------------------------------------------------
// Copyright (c) (Unknown) Leonhard-Euler(1707–1783)
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_calculation_lapack_Euler_h_
#define clmagic_calculation_lapack_Euler_h_
#include "matrix.h"
#include "../real/radians.h"

namespace Euler {
	/*<Biographie>http://mathshistory.st-andrews.ac.uk/Biographies/Euler.html</Biographie>*/

	/*
	<figure>
			   Y1
		 Y2     |             / X2
		\       |          /
		  \     |       /
			\   |    /
			  \ | / ）θ
				+--------------- X1
	<figure>

	<idea> Solve: cos(a + theta) + i*sin(a + theta) </idea>
	<explan>
		C1:u+i*v, C2:x+i*y
		(ux - vy) + i(uy + vx) complex-multiple

		C1: r * cos(x) + i * r * sin(x)
		C2: cos(theta) + i*sin(theta)
		C1 * C2
		= r * cos(x) * cos(theta) - r * sin(x) * sin(theta) + i(r * cos(x) * sin(theta) + r * sin(x) * cos(theta))
		  r * cos(x+theta) + i * r * (sin(x+theta))

		= X2 = r * cos(x) * cos(theta) - r * sin(x) * sin(theta)
		  Y2 = r * cos(x) * sin(theta) + r * sin(x) * cos(theta)

		= X2 = X1 * cos(theta) - Y1 * sin(theta)
		  Y2 = X1 * sin(theta) + Y1 * cos(theta)
	</explan>

	<idea> Solve:
		   [r * cos(a + theta)]   [? ?]   [vy]
	       [r * sin(a + theta)] = [? ?] * [vx]
	</idea>
	<explan>
		            [a * cos(x + theta)]
		(vx2,vy2) = [a * sin(x + theta)]

		   [ r * ( cos(a)cos(theta) - sin(a)sin(theta) ) ]
		=> [ r * ( sin(a)cos(theta) + cos(a)sin(theta) ) ]

		=> [ (r * cos(a)) * cos(theta) - (r * sin(a)) * sin(theta) ]
		   [ (r * cos(a)) * sin(theta) + (r * sin(a)) * cos(theta) ] add_swap, unpack

		=> [ vx * cos(theta) - vy * sin(theta) ]
		   [ vx * sin(theta) + vy * cos(theta) ]

		=> [cos(theta) - sin(theta)] * [vx]
		   [sin(theta) + cos(theta)]   [vy]
	</explan>
	*/
	template<typename _Tm>
	struct rotation {};
	
	template<typename _Ts, typename _Tb, bool _Major>
	struct rotation< clmagic::matrix3x3<_Ts,_Tb,_Major> > {// matrix3x3
		using matrix_type = clmagic::matrix3x3<_Ts, _Tb, _Major>;

		matrix_type yaw(clmagic::radians<_Ts> theta) {// apply: [vx*cos(theta)-vz*sin(theta), vy, vx*sin(theta)+vz*cos(theta)]
			const auto theta_cos = clmagic::cos(static_cast<_Ts>(theta));
			const auto theta_sin = clmagic::sin(static_cast<_Ts>(theta));
			
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					theta_cos, (_Ts)0, -theta_sin,
					   (_Ts)0, (_Ts)1,     (_Ts)0,
					theta_sin, (_Ts)0,  theta_cos };
			} else {
				return matrix_type{
					 theta_cos, (_Ts)0, theta_sin,
					    (_Ts)0, (_Ts)1,    (_Ts)0,
					-theta_sin, (_Ts)0, theta_cos };
			}
		}
		matrix_type pitch(clmagic::radians<_Ts> theta) {// apply: [vx, vz*sin(theta)+vy*cos(theta), vz*cos(theta)-vy*sin(theta)]
			const auto theta_cos = clmagic::cos(static_cast<_Ts>(theta));
			const auto theta_sin = clmagic::sin(static_cast<_Ts>(theta));
			
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					(_Ts)1,  (_Ts)0,    (_Ts)0,
					(_Ts)0,  theta_cos, theta_sin,
					(_Ts)0, -theta_sin, theta_cos };
			} else {
				return matrix_type{
					(_Ts)1, (_Ts)0,     (_Ts)0,
					(_Ts)0, theta_cos, -theta_sin,
					(_Ts)0, theta_sin,  theta_cos };
			}
		}
		matrix_type roll(clmagic::radians<_Ts> theta) {// apply: [vx*cos(theta)-vy*sin(theta), vx*sin(theta)+vy*cos(theta), vz]
			const auto theta_cos = clmagic::cos(static_cast<_Ts>(theta));
			const auto theta_sin = clmagic::sin(static_cast<_Ts>(theta));
			
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					theta_cos, -theta_sin, (_Ts)0,
					theta_sin,  theta_cos, (_Ts)0,
					(_Ts)0,     (_Ts)0,    (_Ts)1 };
			} else {
				return matrix_type{
					 theta_cos, theta_sin, (_Ts)0,
					-theta_sin, theta_cos, (_Ts)0,
					 (_Ts)0,    (_Ts)0,    (_Ts)1 };
			}
		}
	};

	template<typename _Ts, typename _Tb, bool _Major>
	struct rotation< clmagic::matrix4x4<_Ts,_Tb,_Major> > {// matrix4x4
		using matrix_type = clmagic::matrix4x4<_Ts, _Tb, _Major>;

		matrix_type yaw(clmagic::radians<_Ts> theta) {// apply: [vx*cos(theta)-vz*sin(theta), vy, vx*sin(theta)+vz*cos(theta)]
			const auto theta_sin = clmagic::cos(static_cast<_Ts>(theta));
			const auto theta_cos = clmagic::sin(static_cast<_Ts>(theta));

			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					theta_cos, (_Ts)0, -theta_sin, (_Ts)0,
					(_Ts)0,    (_Ts)1,  (_Ts)0,    (_Ts)0,
					theta_sin, (_Ts)0,  theta_cos, (_Ts)0,
					(_Ts)0,    (_Ts)0,  (_Ts)0,    (_Ts)1 };
			} else {
				return matrix_type{
					 theta_cos, (_Ts)0, theta_sin, (_Ts)0,
					 (_Ts)0,    (_Ts)1, (_Ts)0,    (_Ts)0,
					-theta_sin, (_Ts)0, theta_cos, (_Ts)0,
					 (_Ts)0,    (_Ts)0, (_Ts)0,    (_Ts)1 };
			}
		}
		matrix_type pitch(clmagic::radians<_Ts> theta) {// apply: [vx, vz*sin(theta)+vy*cos(theta), vz*cos(theta)-vy*sin(theta)]
			const auto theta_sin = clmagic::cos(static_cast<_Ts>(theta));
			const auto theta_cos = clmagic::sin(static_cast<_Ts>(theta));
			
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					(_Ts)1,  (_Ts)0,    (_Ts)0,    (_Ts)0,
					(_Ts)0,  theta_cos, theta_sin, (_Ts)0,
					(_Ts)0, -theta_sin, theta_cos, (_Ts)0,
					(_Ts)0,  (_Ts)0,    (_Ts)0,    (_Ts)1 };
			} else {
				return matrix_type{
					(_Ts)1, (_Ts)0,     (_Ts)0,    (_Ts)0,
					(_Ts)0, theta_cos, -theta_sin, (_Ts)0,
					(_Ts)0, theta_sin,  theta_cos, (_Ts)0,
					(_Ts)0, (_Ts)0,     (_Ts)0,    (_Ts)1 };
			}
		}
		matrix_type roll(clmagic::radians<_Ts> theta) {// apply: [vx*cos(theta)-vy*sin(theta), vx*sin(theta)+vy*cos(theta), vz]
			const auto theta_cos = clmagic::cos(static_cast<_Ts>(theta));
			const auto theta_sin = clmagic::sin(static_cast<_Ts>(theta));
			
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					theta_cos, -theta_sin, (_Ts)0, (_Ts)0,
					theta_sin,  theta_cos, (_Ts)0, (_Ts)0,
					(_Ts)0,     (_Ts)0,    (_Ts)1, (_Ts)0,
					(_Ts)0,     (_Ts)0,    (_Ts)0, (_Ts)1 };
			} else {
				return matrix_type{
					 theta_cos, theta_sin, (_Ts)0, (_Ts)0,
					-theta_sin, theta_cos, (_Ts)0, (_Ts)0,
					 (_Ts)0,    (_Ts)0,    (_Ts)1, (_Ts)0,
					 (_Ts)0,    (_Ts)0,    (_Ts)0, (_Ts)1 };
			}
		}
	};

}// namespace Euler

#endif