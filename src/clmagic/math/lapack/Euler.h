#pragma once
#ifndef clmagic_math_lapack_Euler_h_
#define clmagic_math_lapack_Euler_h_
#include "matrix.h"

namespace Euler {
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
		          [r * cos(x + theta)]   [r * ( cos(x)cos(theta) - sin(x)sin(theta) ) ]
		(X2,Y2) = [r * sin(x + theta)] = [r * ( sin(x)cos(theta) + cos(x)sin(theta) ) ] <>
		                               = [r * ( cos(x)cos(theta) - sin(x)sin(theta) ) ]
		                                 [r * ( cos(x)sin(theta) + sin(x)cos(theta) ) ] <>
				= [cos(theta) - sin(theta)] [r * cos(x)]
				  [sin(theta) + cos(theta)] [r * sin(x)]
	*/
	template<typename _SclTy, size_t _Rows, typename _BlkTy = _SclTy, bool _Major = clmagic::_COL_MAJOR_>
	struct rotation {/*default matrix3x3*/
		static_assert(_Rows == 3 || _Rows == 4, "->[::Euler::rotation<_SclTy, _Rows, ...>]");

		using matrix_type = clmagic::square_matrix<_SclTy, _Rows, _BlkTy, _Major>;

		matrix_type yaw(clmagic::radians _Angle) {// X-Z
			const auto _Cos = clmagic::cos(static_cast<_SclTy>(_Angle));
			const auto _Sin = clmagic::sin(static_cast<_SclTy>(_Angle));
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					_Cos,   (_SclTy)0, -_Sin,
					(_SclTy)0, (_SclTy)1, (_SclTy)0,
					_Sin,   (_SclTy)0, _Cos };
				/*
				[c*X-s*Z]
				[Y]
				[s*X+c*Z]
				*/
			} else {
				return matrix_type{
					_Cos,   (_SclTy)0, _Sin,
					(_SclTy)0, (_SclTy)1, (_SclTy)0,
					-_Sin,  (_SclTy)0, _Cos };
			}
		}
	
		matrix_type pitch(clmagic::radians _Angle) {// Z-Y
			const auto _Cos = clmagic::cos(static_cast<_SclTy>(_Angle));
			const auto _Sin = clmagic::sin(static_cast<_SclTy>(_Angle));
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					(_SclTy)1, (_SclTy)0, (_SclTy)0,
					(_SclTy)0,   _Cos, _Sin,
					(_SclTy)0,  -_Sin, _Cos };
				/*
				[X]
				[s*Z+c*Y]
				[c*Z-s*Y]
				*/
			} else {
				return matrix_type{
					(_SclTy)1, (_SclTy)0, (_SclTy)0,
					(_SclTy)0,   _Cos, -_Sin,
					(_SclTy)0,   _Sin, _Cos };
			}
		}
	
		matrix_type roll(clmagic::radians _Angle) {// X-Y
			const auto _Cos = clmagic::cos(static_cast<_SclTy>(_Angle));
			const auto _Sin = clmagic::sin(static_cast<_SclTy>(_Angle));
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					_Cos,   -_Sin,  (_SclTy)0,
					_Sin,   _Cos,   (_SclTy)0,
					(_SclTy)0, (_SclTy)0, (_SclTy)1 };
				/*
				[c*X-s*Y]
				[s*X+c*Y]
				[Z]
				*/
			} else {
				return matrix_type{
					_Cos,   _Sin,   (_SclTy)0,
					-_Sin,  _Cos,   (_SclTy)0,
					(_SclTy)0, (_SclTy)0, (_SclTy)1 };
			}
		}
	};

	template<typename _SclTy, typename _BlkTy, bool _Major>
	struct rotation<_SclTy, 4, _BlkTy, _Major> {/*matrix4x4*/
		using matrix_type = clmagic::square_matrix<_SclTy, 4, _BlkTy, _Major>;

		matrix_type yaw(clmagic::radians _Angle) {// X-Z
			const auto _Cos = clmagic::cos(static_cast<_SclTy>(_Angle));
			const auto _Sin = clmagic::sin(static_cast<_SclTy>(_Angle));
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					_Cos,   (_SclTy)0,  -_Sin, (_SclTy)0,
					(_SclTy)0, (_SclTy)1, (_SclTy)0, (_SclTy)0,
					_Sin,   (_SclTy)0,   _Cos, (_SclTy)0,
					(_SclTy)0, (_SclTy)0, (_SclTy)0, (_SclTy)1 };
				/*
				[c*X-s*Z]
				[Y]
				[s*X+c*Z]
				*/
			} else {
				return matrix_type{
					_Cos,   (_SclTy)0,   _Sin, (_SclTy)0,
					(_SclTy)0, (_SclTy)1, (_SclTy)0, (_SclTy)0,
					-_Sin,  (_SclTy)0,   _Cos, (_SclTy)0,
					(_SclTy)0, (_SclTy)0, (_SclTy)0, (_SclTy)1 };
			}
		}
	
		matrix_type pitch(clmagic::radians _Angle) {// Z-Y
			const auto _Cos = clmagic::cos(static_cast<_SclTy>(_Angle));
			const auto _Sin = clmagic::sin(static_cast<_SclTy>(_Angle));
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					(_SclTy)1, (_SclTy)0, (_SclTy)0, (_SclTy)0,
					(_SclTy)0,   _Cos,   _Sin, (_SclTy)0,
					(_SclTy)0,  -_Sin,   _Cos, (_SclTy)0,
					(_SclTy)0, (_SclTy)0, (_SclTy)0, (_SclTy)1 };
				/*
				[X]
				[s*Z+c*Y]
				[c*Z-s*Y]
				*/
			} else {
				return matrix_type{
					(_SclTy)1, (_SclTy)0, (_SclTy)0, (_SclTy)0,
					(_SclTy)0,   _Cos,  -_Sin, (_SclTy)0,
					(_SclTy)0,   _Sin,   _Cos, (_SclTy)0,
					(_SclTy)0, (_SclTy)0, (_SclTy)0, (_SclTy)1 };
			}
		}
	
		matrix_type roll(clmagic::radians _Angle) {// X-Y
			const auto _Cos = clmagic::cos(static_cast<_SclTy>(_Angle));
			const auto _Sin = clmagic::sin(static_cast<_SclTy>(_Angle));
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					_Cos,   -_Sin,  (_SclTy)0, (_SclTy)0,
					_Sin,    _Cos,  (_SclTy)0, (_SclTy)0,
					(_SclTy)0, (_SclTy)0, (_SclTy)1, (_SclTy)0,
					(_SclTy)0, (_SclTy)0, (_SclTy)0, (_SclTy)1 };
				/*
				[c*X-s*Y]
				[s*X+c*Y]
				[Z]
				*/
			} else {
				return matrix_type{
					_Cos,    _Sin,  (_SclTy)0, (_SclTy)0,
					-_Sin,   _Cos,  (_SclTy)0, (_SclTy)0,
					(_SclTy)0, (_SclTy)0, (_SclTy)1, (_SclTy)0,
					(_SclTy)0, (_SclTy)0, (_SclTy)0, (_SclTy)1 };
			}
		}
	};

}// namespace Euler

#endif