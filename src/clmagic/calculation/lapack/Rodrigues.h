//--------------------------------------------------------------------------------------
// Copyright (c) (Unknown) Benjamin-Olinde-Rodrigues(1795–1851)
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_calculation_lapack_Rodrigues_h_
#define clmagic_calculation_lapack_Rodrigues_h_
#include "matrix.h"
#include "../real/radians.h"

namespace Rodrigues {
	/*<Biographie>http://mathshistory.st-andrews.ac.uk/Biographies/Rodrigues.html</Biographie>*/

	template<typename _Tm>
	struct rotation {
		template<typename _Ts>
		static clmagic::radians<_Ts> _Get_angle(_Ts M00, _Ts M11, _Ts M22) {
			const auto theta = acos( ( M00 + M11 + M22 - ((_Ts)1) ) / ((_Ts)2) );
			return clmagic::radians<_Ts>(theta);
			/*
			θ = acos( (trace(R)-1) / 2 )
				= acos( [c+(1-c)x² + c+(1-c)y² + c+(1-c)z² - 1] / 2 )
				= acos( [3*c + (1-c)(x²+y²+z²) - 1] / 2 )
				= acos( [3*c + (1-c)*1 - 1] / 2 )
				= acos( [2*c + 1 - 1] / 2 )
				= acos( c )
			*/
		}

		template<typename _Tv, typename _Ts, typename _TTm>
		static _Tv _Get_axis(const _TTm& M, clmagic::radians<_Ts> angle) {
			using clmagic::sin;

			_Ts theta_sin_m2 = sin((_Ts)angle) * ((_Ts)2);
			
			if _CONSTEXPR_IF( _TTm::col_major() ) {
				const auto x = M.at(2, 1) - M.at(1, 2);
				const auto y = M.at(0, 2) - M.at(2, 0);
				const auto z = M.at(1, 0) - M.at(0, 1);
				return _Tv{ x, y, z } / theta_sin_m2;
			} else {
				const auto x = M.at(1, 2) - M.at(2, 1);
				const auto y = M.at(2, 0) - M.at(0, 2);
				const auto z = M.at(0, 1) - M.at(1, 0);
				return _Tv{ x, y, z } / theta_sin_m2;
			}
			/*
					   1      [j[2] - k[1]]
			axis = -------- * [k[0] - i[2]]
					2sin(θ)  [i[1] - j[0]]
					  1       [ [(1-c)yz+sx] - [(1-c)yz-sx] ]
				 = -------- * [ [(1-c)xz+sy] - [(1-c)xz-sy] ]
					2sin(θ)  [ [(1-c)xy+sz] - [(1-c)xy-sz] ]
					  1       [ 2*sx ]
				 = -------- * [ 2*sy ]
					2sin(θ)  [ 2*sz ]

					2*sin(θ)[x y z]
				 = --------------
						2sin(θ)
				 = [x y z]
			*/
		}
	};

	template<typename _Ts, typename _Tb, bool _Major>
	struct rotation< clmagic::matrix3x3<_Ts, _Tb, _Major> > {// matrix3x3
		using matrix_type = clmagic::matrix3x3 <_Ts, _Tb, _Major>;

		static matrix_type get_matrix(clmagic::unit_vector3<_Ts, _Tb> axis, clmagic::radians<_Ts> angle) {
			const auto c = clmagic::cos(static_cast<_Ts>(angle));
			const auto s = clmagic::sin(static_cast<_Ts>(angle));
			const auto x = axis[0];
			const auto y = axis[1];
			const auto z = axis[2];
			const auto tmp = (static_cast<_Ts>(1) - c) * axis;// from GLM library

			if _CONSTEXPR_IF(_Major == clmagic::_COL_MAJOR_) {
				return matrix_type{
					c + tmp[0]*x,           tmp[0]*y - s*z,     tmp[0]*z + s*y,
					    tmp[1]*x + s*z, c + tmp[1]*y,           tmp[1]*z - s*x,
					    tmp[2]*x - s*y,     tmp[2]*y + s*x, c + tmp[2]*z
				};
			} else {
				return matrix_type{
					c + tmp[0]*x,           tmp[1]*x + s*z,     tmp[2]*x - s*y,
						tmp[0]*y - s*z, c + tmp[1]*y,           tmp[2]*y + s*x,
						tmp[0]*z + s*y,     tmp[1]*z + s*x, c + tmp[2]*z
				};
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

		static clmagic::radians<_Ts> get_angle(const matrix_type& M) {
			return rotation<void>::_Get_angle(M.at(0, 0), M.at(1, 1), M.at(2, 2));
		}
		static clmagic::unit_vector3<_Ts, _Tb> get_axis(const matrix_type& M, clmagic::radians<_Ts> angle) {
			return clmagic::unit_vector3<_Ts, _Tb>(
				rotation<void>::_Get_axis< clmagic::vector3<_Ts, _Tb> >(M, angle) 
				);
		}
	};

	template<typename _Ts, typename _Tb, bool _Major>
	struct rotation< clmagic::matrix4x4<_Ts, _Tb, _Major> > {// matrix4x4
		using matrix_type = clmagic::matrix4x4<_Ts, _Tb, _Major>;

		static matrix_type get_matrix(clmagic::unit_vector3<_Ts, _Tb> axis, clmagic::radians<_Ts> angle) {
			const auto c   = clmagic::cos(static_cast<_Ts>(angle));
			const auto s   = clmagic::sin(static_cast<_Ts>(angle));
			const auto x   = axis[0];
			const auto y   = axis[1];
			const auto z   = axis[2];
			const auto tmp = (static_cast<_Ts>(1) - c) * axis;// from GLM library

			if _CONSTEXPR_IF(_Major == clmagic::_COL_MAJOR_) {
				return matrix_type{
					c + tmp[0]*x,           tmp[0]*y - s*z,     tmp[0]*z + s*y, (_Ts)0,
						tmp[1]*x + s*z, c + tmp[1]*y,           tmp[1]*z - s*x, (_Ts)0,
						tmp[2]*x - s*y,     tmp[2]*y + s*x, c + tmp[2]*z,       (_Ts)0,
						(_Ts)0,             (_Ts)0,             (_Ts)0,         (_Ts)1
				};
			} else {
				return matrix_type{
					c + tmp[0]*x,           tmp[1]*x + s*z,     tmp[2]*x - s*y, (_Ts)0,
						tmp[0]*y - s*z, c + tmp[1]*y,           tmp[2]*y + s*x, (_Ts)0,
						tmp[0]*z + s*y,     tmp[1]*z - s*x, c + tmp[2]*z,       (_Ts)0,
						(_Ts)0,             (_Ts)0,             (_Ts)0,         (_Ts)1
				};
			}
		}
	
		static clmagic::radians<_Ts> get_angle(const matrix_type& M) {
			return rotation<void>::_Get_angle(M.at(0, 0), M.at(1, 1), M.at(2, 2));
		}
		static clmagic::unit_vector3<_Ts> get_axis(const matrix_type& M, clmagic::radians<_Ts> angle) {
			return clmagic::unit_vector3<_Ts, _Tb>(
				rotation<void>::_Get_axis< clmagic::vector3<_Ts, _Tb> >(M, angle)
				);
		}
	};
}// namespace Rodrigues

#endif