//--------------------------------------------------------------------------------------
// Copyright (c) (Unknown) Benjamin-Olinde-Rodrigues(1795–1851)
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_calculation_lapack_Rodrigues_h_
#define clmagic_calculation_lapack_Rodrigues_h_
#include "matrix.h"
#include "../physics/fundamental.h"

namespace Rodrigues {
	/*<Biographie>http://mathshistory.st-andrews.ac.uk/Biographies/Rodrigues.html</Biographie>*/

	template<typename _Ts, typename _Tb, clmagic::matrix_major _Major = clmagic::DEFAULT_MAJOR>
	MATRIX4x4 rotation(UNIT_VECTOR3 axis, RADIANS angle) {
		const auto c = clmagic::cos(static_cast<SCALAR>(angle));
		const auto s = clmagic::sin(static_cast<SCALAR>(angle));
		const auto x = axis[0];
		const auto y = axis[1];
		const auto z = axis[2];
		const auto tmp = (1 - c) * axis;// from GLM library

		if _CONSTEXPR_IF( MATRIX4x4::col_major() ) {
			return MATRIX4x4{
				c + tmp[0]*x,           tmp[0]*y - s*z,     tmp[0]*z + s*y, 0,
					tmp[1]*x + s*z, c + tmp[1]*y,           tmp[1]*z - s*x, 0,
					tmp[2]*x - s*y,     tmp[2]*y + s*x, c + tmp[2]*z,       0,
						0,                  0,                  0,          1 };
		} else {
			return MATRIX4x4{
				c + tmp[0]*x,           tmp[1]*x + s*z,     tmp[2]*x - s*y, 0,
					tmp[0]*y - s*z, c + tmp[1]*y,           tmp[2]*y + s*x, 0,
					tmp[0]*z + s*y,     tmp[1]*z + s*x, c + tmp[2]*z,       0,
						0,                  0,                  0,          1 };
		}
		/*
		@_Note: colume_major
		@_Title: Rodrigues's rotation formula
		①:
			i            j          k
		[c+(1-c)x²  (1-c)xy-sz (1-c)xz+sy]
		[(1-c)xy+sz c+(1-c)y²  (1-c)yz-sx]
		[(1-c)xz-sy (1-c)yz+sx c+(1-c)z² ]

		②:
		Vrot = cos(θ)v + (1-cos(θ))(k(k·v)) + sin(θ)(k X v)
		Rv = Vrot
		R  = cos(θ)I + (1-cos(θ))k*kT + sin(θ)K

		③: c: cos(theta), s: sin(theta)
					[cos(θ)   0      0   ]
		cos(θ)*I = [0      cos(θ)   0   ]
					[0         0   cos(θ)]
								[x]           [(1-c)x², (1-c)yx, (1-c)zx]
		(1-cos(θ))k*kT = (1-c)*[y]*[x y z] = [(1-c)xy, (1-c)y², (1-c)zy]
								[z]           [(1-c)xz, (1-c)yz, (1-c)z²]
					      [ 0 -z  y]
		sin(θ)K = sin(θ)[ z  0 -x]
					      [-y  x  0]

		*/
	}

	template<typename _Ts, typename _Tb, clmagic::matrix_major _Major = clmagic::DEFAULT_MAJOR>
	MATRIX3x3 rotation3x3(UNIT_VECTOR3 axis, RADIANS angle) {
		const auto c = clmagic::cos(static_cast<SCALAR>(angle));
		const auto s = clmagic::sin(static_cast<SCALAR>(angle));
		const auto x = axis[0];
		const auto y = axis[1];
		const auto z = axis[2];
		const auto tmp = (1 - c) * axis;// from GLM library

		if _CONSTEXPR_IF( MATRIX3x3::col_major() ) {
			return MATRIX3x3{
				c + tmp[0]*x,           tmp[0]*y - s*z,     tmp[0]*z + s*y,
					tmp[1]*x + s*z, c + tmp[1]*y,           tmp[1]*z - s*x,
					tmp[2]*x - s*y,     tmp[2]*y + s*x, c + tmp[2]*z };
		} else {
			return MATRIX3x3{
				c + tmp[0]*x,           tmp[1]*x + s*z,     tmp[2]*x - s*y,
					tmp[0]*y - s*z, c + tmp[1]*y,           tmp[2]*y + s*x,
					tmp[0]*z + s*y,     tmp[1]*z + s*x, c + tmp[2]*z };
		}
	}

	template<typename _Ts>
	RADIANS _Get_angle(SCALAR M00, SCALAR M11, SCALAR M22) {
		return RADIANS( clmagic::acos((M00 + M11 + M22 - 1) / 2) );
		/*
		θ = acos( (trace(R)-1) / 2 )
			= acos( [c+(1-c)x² + c+(1-c)y² + c+(1-c)z² - 1] / 2 )
			= acos( [3*c + (1-c)(x²+y²+z²) - 1] / 2 )
			= acos( [3*c + (1-c)*1 - 1] / 2 )
			= acos( [2*c + 1 - 1] / 2 )
			= acos( c )
		*/
	}

	template<typename _Ts, typename _Tb, typename _TTm>
	VECTOR3 _Get_axis(const _TTm& M, RADIANS angle) {
		auto angle_sin    = clmagic::sin(static_cast<SCALAR>(angle));
		auto angle_sin_m2 = angle_sin * 2;
			
		if _CONSTEXPR_IF( _TTm::col_major() ) {
			const auto x = M.at(2, 1) - M.at(1, 2);
			const auto y = M.at(0, 2) - M.at(2, 0);
			const auto z = M.at(1, 0) - M.at(0, 1);
			return VECTOR3{ x, y, z } / angle_sin_m2;
		} else {
			const auto x = M.at(1, 2) - M.at(2, 1);
			const auto y = M.at(2, 0) - M.at(0, 2);
			const auto z = M.at(0, 1) - M.at(1, 0);
			return VECTOR3{ x, y, z } / angle_sin_m2;
		}
		/*
					1     [j[2] - k[1]]
		axis = -------- * [k[0] - i[2]]
				2sin(θ)  [i[1] - j[0]]
				  1       [ [(1-c)y*z+s*x] - [(1-c)y*z-s*x] ]
			 = -------- * [ [(1-c)x*z+s*y] - [(1-c)x*z-s*y] ]
				2sin(θ)  [ [(1-c)x*y+s*z] - [(1-c)x*y-s*z] ]
				  1       [ 2*s*x ]
			 = -------- * [ 2*s*y ]
				2sin(θ)  [ 2*s*z ]

				2*sin(θ)[x y z]
			 = -----------------
				  2sin(θ)
			 = [x y z]
		*/
	}

	template<typename _Ts, typename _Tb, clmagic::matrix_major _Major>
	RADIANS rotation_angle(const MATRIX4x4& M) {
		return _Get_angle<_Ts>(M.at(0,0), M.at(1,1), M.at(2,2));
	}
	
	template<typename _Ts, typename _Tb, clmagic::matrix_major _Major>
	UNIT_VECTOR3 rotation_axis(const MATRIX4x4& M) {
		return UNIT_VECTOR3( _Get_axis<_Ts, _Tb, MATRIX4x4>( M, rotation_angle(M) ) );
	}

	template<typename _Ts, typename _Tb, clmagic::matrix_major _Major>
	RADIANS rotation3x3_angle(const MATRIX3x3& M) {
		return _Get_angle<_Ts>(M.at(0,0), M.at(1,1), M.at(2,2));
	}
	
	template<typename _Ts, typename _Tb, clmagic::matrix_major _Major>
	UNIT_VECTOR3 rotation3x3_axis(const MATRIX3x3& M) {
		return UNIT_VECTOR3( _Get_axis<_Ts, _Tb, MATRIX3x3>( M, rotation_angle(M) ) );
	}

}// namespace Rodrigues

#endif