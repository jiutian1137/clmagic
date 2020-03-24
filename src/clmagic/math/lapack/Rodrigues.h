﻿#pragma once
#ifndef clmagic_math_lapack_Rodrigues_h_
#define clmagic_math_lapack_Rodrigues_h_
#include "matrix.h"

namespace Rodrigues {
	template<typename _Ty, size_t _Rows, bool _Major = clmagic::_COL_MAJOR_, typename _Block = _Ty>
	struct Rotation {// default matrix3x3
		static_assert(_Rows == 3 || _Rows == 4, "Rodrigues::Rotation<_Ty, _Rows, _Cols, _Major, _Block>");
		
		using matrix_type = clmagic::square_matrix<_Ty, _Rows, _Major, _Block, clmagic::normal_matrix_tag>;

		matrix_type get_matrix(clmagic::unit_vector3<_Ty, _Block> axis, clmagic::radians angle) {
			const auto c = clmagic::cos(static_cast<_Ty>(angle));
			const auto s = clmagic::sin(static_cast<_Ty>(angle));
			const auto x = axis[0];
			const auto y = axis[1];
			const auto z = axis[2];
			const auto tmp = (static_cast<_Ty>(1) - c) * axis;// from GLM library

			if _CONSTEXPR_IF(_Major == clmagic::_COL_MAJOR_) {
				return matrix_type{
					c + tmp[0]*x,           tmp[0]*y - s*z,     tmp[0]*z + s*y,
						tmp[1]*x + s*z, c + tmp[1]*y,           tmp[1]*z + s*x,
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

		clmagic::radians get_angle(const matrix_type& M) {
			const auto theta = acos(
				(M.at(0,0) + M.at(1,1) + M.at(2,2) - (_Ty)1)
			   /*------------------------------------------*/
				              / (_Ty)2 );
			return clmagic::radians(clmagic::cvtfloating_rational(theta));
			/*
			θ = acos( (trace(R)-1) / 2 )
				= acos( [c+(1-c)x² + c+(1-c)y² + c+(1-c)z² - 1] / 2 )
				= acos( [3*c + (1-c)(x²+y²+z²) - 1] / 2 )
				= acos( [3*c + (1-c)*1 - 1] / 2 )
				= acos( [2*c + 1 - 1] / 2 )
				= acos( c )
			*/
		}
	
		clmagic::unit_vector3<_Ty, _Block> get_axis(const matrix_type& M, clmagic::radians angle) {
			using vector3 = clmagic::vector3<_Ty, _Block>;

			_Ty theta = static_cast<_Ty>(angle);
			
			if _CONSTEXPR_IF(_Major == clmagic::_COL_MAJOR_) {
				return clmagic::unit_vector3<_Ty, _Block>(
					vector3(M.at(1,2)-M.at(2,1), M.at(2,0)-M.at(0,2), M.at(0,1)-M.at(2,0))
					/*---------------------------------------------------------------------*/
					                 / ((_Ty)2 * sin(theta))
					);
			} else {
				return clmagic::unit_vector3<_Ty, _Block>(
					vector3(M.at(2,1)-M.at(1,2), M.at(0,2)-M.at(2,0), M.at(1,0)-M.at(0,2))
					/*---------------------------------------------------------------------*/
					                 / ((_Ty)2 * sin(theta))
					);
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

	template<typename _Ty, bool _Major, typename _Block>
	struct Rotation<_Ty, 4, _Major, _Block> {// matrix4x4
		using matrix_type = clmagic::square_matrix<_Ty, 4, _Major, _Block, clmagic::normal_matrix_tag>;

		matrix_type get_matrix(clmagic::unit_vector3<_Ty, _Block> axis, clmagic::radians angle) {
			const auto c   = clmagic::cos(static_cast<_Ty>(angle));
			const auto s   = clmagic::sin(static_cast<_Ty>(angle));
			const auto x   = axis[0];
			const auto y   = axis[1];
			const auto z   = axis[2];
			const auto tmp = (static_cast<_Ty>(1) - c) * axis;// from GLM library

			if _CONSTEXPR_IF(_Major == clmagic::_COL_MAJOR_) {
				return matrix_type{
					c + tmp[0]*x,           tmp[0]*y - s*z,     tmp[0]*z + s*y, (_Ty)0,
						tmp[1]*x + s*z, c + tmp[1]*y,           tmp[1]*z + s*x, (_Ty)0,
						tmp[2]*x - s*y,     tmp[2]*y + s*x, c + tmp[2]*z,       (_Ty)0,
						(_Ty)0,             (_Ty)0,             (_Ty)0,         (_Ty)1
				};
			} else {
				return matrix_type{
					c + tmp[0]*x,           tmp[1]*x + s*z,     tmp[2]*x - s*y, (_Ty)0,
						tmp[0]*y - s*z, c + tmp[1]*y,           tmp[2]*y + s*x, (_Ty)0,
						tmp[0]*z + s*y,     tmp[1]*z + s*x, c + tmp[2]*z,       (_Ty)0,
						(_Ty)0,             (_Ty)0,             (_Ty)0,         (_Ty)1
				};
			}
		}
	
		clmagic::radians get_angle(const matrix_type& M) {
			const auto theta = acos(
				(M.at(0,0) + M.at(1,1) + M.at(2,2) - (_Ty)1)
			   /*------------------------------------------*/
				              / (_Ty)2 );
			return clmagic::radians(clmagic::cvtfloating_rational(theta));
		}

		clmagic::unit_vector3<_Ty> get_axis(const matrix_type& M, clmagic::radians angle) {
			using vector3 = clmagic::vector3<_Ty, _Block>;

			_Ty theta = static_cast<_Ty>(angle);
			
			if _CONSTEXPR_IF(_Major == clmagic::_COL_MAJOR_) {
				return clmagic::unit_vector3<_Ty>(
					vector3(M.at(1,2)-M.at(2,1), M.at(2,0)-M.at(0,2), M.at(0,1)-M.at(2,0))
					/*---------------------------------------------------------------------*/
					                 / ((_Ty)2 * sin(theta))
					);
			} else {
				return clmagic::unit_vector3<_Ty>(
					vector3(M.at(2,1)-M.at(1,2), M.at(0,2)-M.at(2,0), M.at(1,0)-M.at(0,2))
					/*---------------------------------------------------------------------*/
					                 / ((_Ty)2 * sin(theta))
					);
			}
		}
	};
}// namespace Rodrigues

#endif