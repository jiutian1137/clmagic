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
		C1:u+i*v, C2:x+i*y
		ux + u*i*y + x*i*v - vy
		(ux - vy) + i(uy + vx) complex-multiple

		C1: r1 * cos(x) + i * r1 * sin(x)
		C2: cos(theta) + i*sin(theta)
		C1 * C2
		= r1 * cos(x) * cos(theta) - r1 * sin(x) * sin(theta) + i(r1 * cos(x) * sin(theta) + r1 * sin(x) * cos(theta))
		  r1 * cos(x+theta) + i * r1 * (sin(x+theta))

		= X2 = r1 * cos(x) * cos(theta) - r1 * sin(x) * sin(theta)
		  Y2 = r1 * cos(x) * sin(theta) + r1 * sin(x) * cos(theta)

		= X2 = X1 * cos(theta) - Y1 * sin(theta)
		  Y2 = X1 * sin(theta) + Y1 * cos(theta)
		*/
	template<typename _Ty> inline
	clmagic::matrix3x3<_Ty> yaw(clmagic::radians _Angle) {/* rotate X-Z */
		const auto _Zero = static_cast<_Ty>(0);
		const auto _One  = static_cast<_Ty>(1);
		const auto _Cos  = clmagic::cos(static_cast<_Ty>(_Angle));
		const auto _Sin  = clmagic::sin(static_cast<_Ty>(_Angle));
		return clmagic::matrix3x3<_Ty>{
			_Cos, -_Sin, _Zero,
			_Sin,  _Cos, _Zero,
			_Zero, _Zero, _One
		};
		/*
		[c -s 0]
		[s  c 0]
		[0  0 1]
		
		[c*X-s*Y]
		[s*X+c*Y]
		[Z]
		*/
	}

	template<typename _Ty> inline
	clmagic::matrix3x3<_Ty> pitch(clmagic::radians _Angle) {/* rotate Z-Y */
		const auto _Zero = static_cast<_Ty>(0);
		const auto _One  = static_cast<_Ty>(1);
		const auto _Cos  = clmagic::cos(static_cast<_Ty>(_Angle));
		const auto _Sin  = clmagic::sin(static_cast<_Ty>(_Angle));
		return clmagic::matrix3x3<_Ty>{
			_One, _Zero, _Zero,
			_Sin,  _Cos, _Zero,
			_Cos, -_Sin, _One
		};
		/*
		[1  0 0]
		[s  c 0]
		[c -s 1]
		*/
	}

	template<typename _Ty> inline
	clmagic::matrix3x3<_Ty> roll(clmagic::radians _Angle) {/* rotate X-Y */
		const auto _Zero = static_cast<_Ty>(0);
		const auto _One  = static_cast<_Ty>(1);
		const auto _Cos  = clmagic::cos(static_cast<_Ty>(_Angle));
		const auto _Sin  = clmagic::sin(static_cast<_Ty>(_Angle));
		return clmagic::matrix3x3<_Ty>{
			_Cos, -_Sin, _Zero,
			_Sin,  _Cos, _Zero,
			_Zero, _Zero, _One
		};
		/*
		[c -s 0]
		[s  c 0]
		[0  0 1]
		*/
	}
}// namespace Euler

#endif