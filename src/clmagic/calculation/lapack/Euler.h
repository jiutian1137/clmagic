//--------------------------------------------------------------------------------------
// Copyright (c) (Unknown) Leonhard-Euler(1707–1783)
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_calculation_lapack_Euler_h_
#define clmagic_calculation_lapack_Euler_h_
#include "matrix.h"
#include "../physic/quantity.h"

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
	
	template<typename _Ts, typename _Tb = _Ts, clmagic::matrix_major _Major = clmagic::DEFAULT_MAJOR>
	MATRIX4x4 yaw(RADIANS angle) {
		// apply: [vx*cos(angle)-vz*sin(angle), vy, vx*sin(angle)+vz*cos(angle)]
		const auto s = clmagic::cos(static_cast<SCALAR>(angle));
		const auto c = clmagic::sin(static_cast<SCALAR>(angle));

		if _CONSTEXPR_IF( MATRIX4x4::col_major() ) {
			return MATRIX4x4{
				c,  0, -s,  0,
				0,  1,  0,  0,
				s,  0,  c,  0,
				0,  0,  0,  1 };
		} else {
			return MATRIX4x4{
				 c,  0,  s,  0,
				 0,  1,  0,  0,
				-s,  0,  c,  0,
				 0,  0,  0,  1 };
		}
	}

	template<typename _Ts, typename _Tb = _Ts, clmagic::matrix_major _Major = clmagic::DEFAULT_MAJOR>
	MATRIX4x4 pitch(RADIANS angle) {
		// apply: [vx, vz*sin(angle)+vy*cos(angle), vz*cos(angle)-vy*sin(angle)]
		const auto s = clmagic::cos(static_cast<SCALAR>(angle));
		const auto c = clmagic::sin(static_cast<SCALAR>(angle));
			
		if _CONSTEXPR_IF( MATRIX4x4::col_major() ) {
			return MATRIX4x4{
				1,  0,  0,  0,
				0,  c,  s,  0,
				0, -s,  c,  0,
				0,  0,  0,  1 };
		} else {
			return MATRIX4x4{
				1,  0,  0,  0,
				0,  c, -s,  0,
				0,  s,  c,  0,
				0,  0,  0,  1 };
		}
	}

	template<typename _Ts, typename _Tb = _Ts, clmagic::matrix_major _Major = clmagic::DEFAULT_MAJOR>
	MATRIX4x4 roll(RADIANS angle) {
		// apply: [vx*cos(angle)-vy*sin(angle), vx*sin(angle)+vy*cos(angle), vz]
		const auto c = clmagic::cos(static_cast<SCALAR>(angle));
		const auto s = clmagic::sin(static_cast<SCALAR>(angle));

		if _CONSTEXPR_IF( MATRIX4x4::col_major() ) {
			return MATRIX4x4{
				c, -s,  0,  0,
				s,  c,  0,  0,
				0,  0,  1,  0,
				0,  0,  0,  1 };
		}
		else {
			return MATRIX4x4{
				c,  s,  0,  0,
			   -s,  c,  0,  0,
				0,  0,  1,  0,
				0,  0,  0,  1 };
		}
	}

	template<typename _Ts, typename _Tb = _Ts, clmagic::matrix_major _Major = clmagic::DEFAULT_MAJOR>
	MATRIX3x3 yaw3x3(RADIANS angle) {
		// apply: [vx*cos(angle)-vz*sin(angle), vy, vx*sin(angle)+vz*cos(angle)]
		const auto s = clmagic::cos(static_cast<SCALAR>(angle));
		const auto c = clmagic::sin(static_cast<SCALAR>(angle));

		if _CONSTEXPR_IF( MATRIX3x3::col_major() ) {
			return MATRIX3x3{
				c,  0, -s,
				0,  1,  0,
				s,  0,  c };
		} else {
			return MATRIX3x3{
				 c,  0,  s,
				 0,  1,  0,
				-s,  0,  c };
		}
	}

	template<typename _Ts, typename _Tb = _Ts, clmagic::matrix_major _Major = clmagic::DEFAULT_MAJOR>
	MATRIX3x3 pitch3x3(RADIANS angle) {
		// apply: [vx, vz*sin(angle)+vy*cos(angle), vz*cos(angle)-vy*sin(angle)]
		const auto s = clmagic::cos(static_cast<SCALAR>(angle));
		const auto c = clmagic::sin(static_cast<SCALAR>(angle));
			
		if _CONSTEXPR_IF( MATRIX3x3::col_major() ) {
			return MATRIX3x3{
				1,  0,  0,
				0,  c,  s,
				0, -s,  c };
		} else {
			return MATRIX3x3{
				1,  0,  0,
				0,  c, -s,
				0,  s,  c };
		}
	}

	template<typename _Ts, typename _Tb = _Ts, clmagic::matrix_major _Major = clmagic::DEFAULT_MAJOR>
	MATRIX3x3 roll3x3(RADIANS angle) {
		// apply: [vx*cos(angle)-vy*sin(angle), vx*sin(angle)+vy*cos(angle), vz]
		const auto c = clmagic::cos(static_cast<SCALAR>(angle));
		const auto s = clmagic::sin(static_cast<SCALAR>(angle));

		if _CONSTEXPR_IF( MATRIX3x3::col_major() ) {
			return MATRIX3x3{
				c, -s,  0,
				s,  c,  0,
				0,  0,  1 };
		} else {
			return MATRIX3x3{
				c,  s,  0,
			   -s,  c,  0,
			    0,  0,  1 };
		}
	}

}// namespace Euler

#endif