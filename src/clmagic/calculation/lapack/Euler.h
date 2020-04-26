//--------------------------------------------------------------------------------------
// Copyright (c) (Unknown) Leonhard-Euler(1707–1783)
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_calculation_lapack_Euler_h_
#define clmagic_calculation_lapack_Euler_h_
#include "matrix.h"
#include "../real/unit.h"

namespace Euler {

	/*<Biographie>http://mathshistory.st-andrews.ac.uk/Biographies/Euler.html</Biographie>*/

	/*
			   Y1
		 Y2     |             / X2
		\       |          /
		  \     |       /
			\   |    /
			  \ | / ）θ
				+--------------- X1
		①:
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

		②:
		            [r * cos(x + theta)]
		(vx2,vy2) = [r * sin(x + theta)]

		   [ r * ( cos(x)cos(theta) - sin(x)sin(theta) ) ]
		=> [ r * ( sin(x)cos(theta) + cos(x)sin(theta) ) ]

		=> [ (r * cos(x)) * cos(theta) - (r * sin(x)) * sin(theta) ]
		   [ (r * cos(x)) * sin(theta) + (r * sin(x)) * cos(theta) ] add_swap, unpack

		=> [ vx * cos(theta) - vy * sin(theta) ]
		   [ vx * sin(theta) + vy * cos(theta) ]

		=> [cos(theta) - sin(theta)] * [vx]
		   [sin(theta) + cos(theta)]   [vy]
	*/
	template<typename _SclTy, size_t _Rows, typename _BlkTy = _SclTy, bool _Major = clmagic::_COL_MAJOR_>
	struct rotation {/*default matrix3x3*/
		static_assert(_Rows == 3 || _Rows == 4, "->[::Euler::rotation<_SclTy, _Rows, ...>]");

		using matrix_type = clmagic::square_matrix<_SclTy, _Rows, _BlkTy, _Major>;

		matrix_type yaw(clmagic::radians theta) {// apply: [vx*cos(theta)-vz*sin(theta), vy, vx*sin(theta)+vz*cos(theta)]
			const auto theta_cos = clmagic::cos(static_cast<_SclTy>(theta));
			const auto theta_sin = clmagic::sin(static_cast<_SclTy>(theta));
			
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					theta_cos, (_SclTy)0, -theta_sin,
					(_SclTy)0, (_SclTy)1,  (_SclTy)0,
					theta_sin, (_SclTy)0,  theta_cos };
			} else {
				return matrix_type{
					 theta_cos, (_SclTy)0, theta_sin,
					 (_SclTy)0, (_SclTy)1, (_SclTy)0,
					-theta_sin, (_SclTy)0, theta_cos };
			}
		}
	
		matrix_type pitch(clmagic::radians theta) {// apply: [vx, vz*sin(theta)+vy*cos(theta), vz*cos(theta)-vy*sin(theta)]
			const auto theta_cos = clmagic::cos(static_cast<_SclTy>(theta));
			const auto theta_sin = clmagic::sin(static_cast<_SclTy>(theta));
			
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					(_SclTy)1,  (_SclTy)0, (_SclTy)0,
					(_SclTy)0,  theta_cos, theta_sin,
					(_SclTy)0, -theta_sin, theta_cos };
			} else {
				return matrix_type{
					(_SclTy)1, (_SclTy)0,  (_SclTy)0,
					(_SclTy)0, theta_cos, -theta_sin,
					(_SclTy)0, theta_sin,  theta_cos };
			}
		}
	
		matrix_type roll(clmagic::radians theta) {// apply: [vx*cos(theta)-vy*sin(theta), vx*sin(theta)+vy*cos(theta), vz]
			const auto theta_cos = clmagic::cos(static_cast<_SclTy>(theta));
			const auto theta_sin = clmagic::sin(static_cast<_SclTy>(theta));
			
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					theta_cos, -theta_sin, (_SclTy)0,
					theta_sin,  theta_cos, (_SclTy)0,
					(_SclTy)0,  (_SclTy)0, (_SclTy)1 };
			} else {
				return matrix_type{
					 theta_cos, theta_sin, (_SclTy)0,
					-theta_sin, theta_cos, (_SclTy)0,
					 (_SclTy)0, (_SclTy)0, (_SclTy)1 };
			}
		}
	};

	template<typename _SclTy, typename _BlkTy, bool _Major>
	struct rotation<_SclTy, 4, _BlkTy, _Major> {/*matrix4x4*/
		using matrix_type = clmagic::square_matrix<_SclTy, 4, _BlkTy, _Major>;

		matrix_type yaw(clmagic::radians theta) {// apply: [vx*cos(theta)-vz*sin(theta), vy, vx*sin(theta)+vz*cos(theta)]
			const auto theta_sin = clmagic::cos(static_cast<_SclTy>(theta));
			const auto theta_cos = clmagic::sin(static_cast<_SclTy>(theta));

			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					theta_cos, (_SclTy)0, -theta_sin, (_SclTy)0,
					(_SclTy)0, (_SclTy)1,  (_SclTy)0, (_SclTy)0,
					theta_sin, (_SclTy)0,  theta_cos, (_SclTy)0,
					(_SclTy)0, (_SclTy)0,  (_SclTy)0, (_SclTy)1 };
			} else {
				return matrix_type{
					 theta_cos, (_SclTy)0, theta_sin, (_SclTy)0,
					 (_SclTy)0, (_SclTy)1, (_SclTy)0, (_SclTy)0,
					-theta_sin, (_SclTy)0, theta_cos, (_SclTy)0,
					 (_SclTy)0, (_SclTy)0, (_SclTy)0, (_SclTy)1 };
			}
		}
	
		matrix_type pitch(clmagic::radians theta) {// apply: [vx, vz*sin(theta)+vy*cos(theta), vz*cos(theta)-vy*sin(theta)]
			const auto theta_sin = clmagic::cos(static_cast<_SclTy>(theta));
			const auto theta_cos = clmagic::sin(static_cast<_SclTy>(theta));
			
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					(_SclTy)1,  (_SclTy)0, (_SclTy)0, (_SclTy)0,
					(_SclTy)0,  theta_cos, theta_sin, (_SclTy)0,
					(_SclTy)0, -theta_sin, theta_cos, (_SclTy)0,
					(_SclTy)0,  (_SclTy)0, (_SclTy)0, (_SclTy)1 };
			} else {
				return matrix_type{
					(_SclTy)1, (_SclTy)0,  (_SclTy)0, (_SclTy)0,
					(_SclTy)0, theta_cos, -theta_sin, (_SclTy)0,
					(_SclTy)0, theta_sin,  theta_cos, (_SclTy)0,
					(_SclTy)0, (_SclTy)0,  (_SclTy)0, (_SclTy)1 };
			}
		}
	
		matrix_type roll(clmagic::radians theta) {// apply: [vx*cos(theta)-vy*sin(theta), vx*sin(theta)+vy*cos(theta), vz]
			const auto theta_cos = clmagic::cos(static_cast<_SclTy>(theta));
			const auto theta_sin = clmagic::sin(static_cast<_SclTy>(theta));
			
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					theta_cos, -theta_sin, (_SclTy)0, (_SclTy)0,
					theta_sin,  theta_cos, (_SclTy)0, (_SclTy)0,
					(_SclTy)0,  (_SclTy)0, (_SclTy)1, (_SclTy)0,
					(_SclTy)0,  (_SclTy)0, (_SclTy)0, (_SclTy)1 };
			} else {
				return matrix_type{
					 theta_cos, theta_sin, (_SclTy)0, (_SclTy)0,
					-theta_sin, theta_cos, (_SclTy)0, (_SclTy)0,
					 (_SclTy)0, (_SclTy)0, (_SclTy)1, (_SclTy)0,
					 (_SclTy)0, (_SclTy)0, (_SclTy)0, (_SclTy)1 };
			}
		}
	};

}// namespace Euler

#endif