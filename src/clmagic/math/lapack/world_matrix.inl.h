#pragma once
#ifndef clmagic_math_lapack_WORLD_MATRIX_h_
#define clmagic_math_lapack_WORLD_MATRIX_h_
#include "../lapack.h"

namespace clmagic {


#define world_matrix_T_J_V template<typename T, bool J, size_t V>
	
	world_matrix_T_J_V inline
	world_matrix<T, true, V> world_matrix<T, _Major, V>::scaling(T _Sx, T _Sy, T _Sz) {
		const auto c0 = static_cast<T>(0);
		const auto c1 = static_cast<T>(1);
		return world_matrix<T, true, V>{
			_Sx,  c0,  c0, c0,
			 c0, _Sy,  c0, c0,
			 c0,  c0, _Sz, c0,
			 c0,  c0,  c0, c1 };
	}
	
	world_matrix_T_J_V inline
	world_matrix<T, true, V> world_matrix<T, _Major, V>::translation(meters<T> _Tx, meters<T> _Ty, meters<T> _Tz) {
		const auto c0 = static_cast<T>(0);
		const auto c1 = static_cast<T>(1);
		return world_matrix<T, true, V>{
			c1, c0, c0, _Tx,
			c0, c1, c0, _Ty,
			c0, c0, c1, _Tz,
			c0, c0, c0,  c1 };
	}
	
	world_matrix_T_J_V inline
	world_matrix<T, true, V> world_matrix<T, _Major, V>::_Euler_angle_yaw(radians<T> theta) {
		const auto c0      = static_cast<T>(0);
		const auto c1      = static_cast<T>(1);
		const auto yaw_cos = cos(static_cast<T>(theta));
		const auto yaw_sin = sin(static_cast<T>(theta));

		/*
		X = X * yaw_cos - Z * yaw_sin
		Y = Y
		Z = Y * yaw_sin + Z * yaw_cos
		*/
		return world_matrix<T, true, V>{
			 yaw_cos,   c0, -yaw_sin, c0,
			      c0,   c1,      c0,  c0,
			 yaw_sin,   c0, yaw_cos,  c0,
			      c0,   c0,      c0,  c1
		};
		
	}

	world_matrix_T_J_V inline
	world_matrix<T, true, V> world_matrix<T, _Major, V>::_Euler_angle_roll(radians<T> theta) {
		const auto c0      = static_cast<T>(0);
		const auto c1      = static_cast<T>(1);
		const auto roll_cos = cos(static_cast<T>(theta));
		const auto roll_sin = sin(static_cast<T>(theta));

		return world_matrix<T, true, V>{
			roll_cos, -roll_sin,  c0,  c0,
			roll_sin,  roll_cos,  c0,  c0,
				   c0,       c0,  c1,  c0,
				   c0,       c0,  c0,  c1
		};
		/* 
		X = X * roll_cos - Y * roll_sin
		Y = X * roll_sin + Y * roll_cos
		Z = Z
		*/
	}

	world_matrix_T_J_V inline 
	world_matrix<T, true, V> world_matrix<T, _Major, V>::_Euler_angle_pitch(radians<T> theta) {// X-axis invariant
		const auto c0        = static_cast<T>(0);
		const auto c1        = static_cast<T>(1);
		const auto pitch_cos = cos(static_cast<T>(theta));
		const auto pitch_sin = sin(static_cast<T>(theta));

		return world_matrix<T, true, V>{
			c1,        c0,         c0,  c0,
			c0, pitch_cos, -pitch_sin,  c0,
			c0, pitch_sin,  pitch_cos,  c0,
			c0,        c0,         c0,  c1
		};
		/*
		X = X
		Y = Y * pitch_cos - Z * pitch_sin
		Z = Y * pitch_sin + Z * pitch_cos
		*/
	}

	world_matrix_T_J_V inline
	world_matrix<T, true, V> world_matrix<T, _Major, V>::Euler_angle(radians<T> yaw, radians<T> roll, radians<T> pitch) {
		/*const auto _Zero = static_cast<T>(0);
		const auto _One  = static_cast<T>(1);
		const auto _CosV = cos( _Ev );
		const auto _SinV = sin( _Ev );*/

		return (_Euler_angle_pitch(pitch) * _Euler_angle_roll(roll) * _Euler_angle_yaw(yaw));
		/*return world_matrix<T, true, V>{
			                                 _CosV[1] * _CosV[0],           -_SinV[1],                                  -_CosV[1] * _SinV[0], _Zero,
			_CosV[2] * _SinV[1] * _CosV[0] - _SinV[2] * _SinV[0], _CosV[2] * _CosV[1], -_CosV[2] * _SinV[1] * _SinV[0] - _SinV[2] * _CosV[0], _Zero,
			_SinV[2] * _SinV[1] * _CosV[0] + _CosV[2] * _SinV[0], _SinV[2] * _CosV[1], -_SinV[2] * _SinV[1] * _SinV[0] + _CosV[2] * _CosV[0], _Zero,
			_Zero, _Zero, _Zero, _One
		};*/
		/*
			  |y
			  |   _
			/ |  |\
		   /  |    \
		 |/_  |roll \
		pitch +---------x
			 / yaw _
			/      /|
		   /z     /

		rotP = [ 1       0          0     ]
			   [ 0  cos(pitch) sin(pitch) ]
			   [ 0 -sin(pitch) cos(pitch) ]
		rotY = [  cos(yaw) 0 sin(yaw) ]
			   [    0      1     0    ]
			   [ -sin(yaw) 0 cos(yaw) ]
		rotR = [  cos(roll) sin(roll) 0 ]
			   [ -sin(roll) cos(roll) 0 ]
			   [     0         0      1 ]

			   [ ix iy iz ][  cos(yaw) 0 sin(yaw) ]
			   [ jx jy jz ][    0      1     0    ]
			   [ kx ky kz ][ -sin(yaw) 0 cos(yaw) ]
			  =[ ix*c + iz*-s, iy, ix*s + iz*c ]
			   [ jx*c + jz*-s, jy, jx*s + jz*c ]
			   ...
		*/
	}

	template<typename T, bool _Major, size_t V> inline
	world_matrix<T, true, V> world_matrix<T, _Major, V>::Rodrigues_rotation(unit_vector3<T> axis, radians<T> theta) {
		
	}



	template<typename T, bool _Major, size_t V> inline
	world_matrix<T, _Major, V>::world_matrix() : _Mybase(static_cast<T>(1)) { }

	template<typename T, bool _Major, size_t V> inline
	world_matrix<T, _Major, V>::world_matrix(std::initializer_list<T> _Ilist) : _Mybase(_Ilist) { }

	template<typename T, bool _Major, size_t V> inline
	world_matrix<T, _Major, V>::world_matrix(const vector3<T, V>& _Sv, const radians<vector3<T, V>>& _Ev, const meters<vector3<T, V>>& _Tv)
		: world_matrix(rotation(_Ev[0], _Ev[1], _Ev[2])) {
		(*this) *= diag_matrix<4, 4, T, _Major, V>{ _Sv[0], _Sv[1], _Sv[2] };
		for (size_t i = 0; i != 3; ++i) {
			if constexpr (this->col_major()) { this->at(i, 3) = _Tv[i]; } else { this->at(3, i) = _Tv[i]; }
		}
	}

	template<typename T, bool _Major, size_t V> inline
	world_matrix<T, _Major, V>::world_matrix(const matrix<4, 4, T, !_Major, V>& _Invorder) : _Mybase(_Invorder) { }

	world_matrix_T_J_V inline
	void world_matrix<T, _Major, V>::scale(const vector3<T, V>& _Sv) {
		if _CONSTEXPR_IF(_Mybase::col_major()) {
			this->col(0) *= _Sv[0];
			this->col(1) *= _Sv[1];
			this->col(2) *= _Sv[2];
		} else {
			this->row(0) *= _Sv[0];
			this->row(1) *= _Sv[1];
			this->row(2) *= _Sv[2];
		}
		/*
		[a b c Xt]   [X]
		[d e f Yt]   [  Y]
		[g h i Zt] * [    Z]
		[0 0 0  1]   [      1]

		[a*X b*Y c*Z Xt]
		[d*X e*Y f*Z Yt]
		[g*X h*Y i*Z Zt]
		[ 0   0   0  1 ]

		A.0 *= X
		A.1 *= Y
		A.2 *= Z
		*/
	}

	world_matrix_T_J_V inline
	void world_matrix<T, _Major, V>::scale(T _Sx, T _Sy, T _Sz) {
		if _CONSTEXPR_IF(_Mybase::col_major()) {
			this->col(0) *= _Sx;
			this->col(1) *= _Sy;
			this->col(2) *= _Sz;
		}
		else {
			this->row(0) *= _Sx;
			this->row(1) *= _Sy;
			this->row(2) *= _Sz;
		}
	}

	world_matrix_T_J_V inline
	void world_matrix<T, _Major, V>::rotate(const vector3<T, V>& _Ev) {
		rotation(_Ev[0], _Ev[1], _Ev[2]);
	}

	world_matrix_T_J_V inline
	void world_matrix<T, _Major, V>::translate(const vector3<T, V>& _Tv) {
		if _CONSTEXPR_IF(_Mybase::col_major()) {
			this->col(3) += this->col(0) * _Tv[0] + this->col(1) * _Tv[1] + this->col(2) * _Tv[2];
		} else {
			this->row(3) += this->row(0) * _Tv[0] + this->row(1) * _Tv[1] + this->row(2) * _Tv[2];
		}
	}

	world_matrix_T_J_V inline
	void world_matrix<T, _Major, V>::translate(meters<T> _X, meters<T> _Y, meters<T> _Z) {// ix + jy + kz
		if _CONSTEXPR_IF(_Mybase::col_major()) {
			this->col(3) += this->col(0) * _X + this->col(1) * _Y + this->col(2) * _Z;
		} else {
			this->row(3) += this->row(0) * _X + this->row(1) * _Y + this->row(2) * _Z;
		}
		/*
		①:
		[a b c Xs]   [1 0 0 X]
		[d e f Ys] * [0 1 0 Y]
		[g h i Zs]   [0 0 1 Z]
		[0 0 0 1 ]   [0 0 0 1]

		②:
		[a b c] [Xs]   [1 0 0 X]
		[d e f] [Ys] * [0 1 0 Y]             [B1]
		[g h i] [Zs]   [0 0 1 Z] = [A1 A2] * [B2] = [A1*B1+A2*B2]
		[0 0 0] [1 ]   
		               [0 0 0 1]

		③:
		[ a b c [abc] dot [XYZ] ]   [0 0 0 Xs]
		[ d e f [def] dot [XYZ] ] + [0 0 0 Ys]
		[ g h i [ghi] dot [XYZ] ]   [0 0 0 Zs]
		[ 0 0 0       0         ]   [0 0 0 1 ]

		④:
		modify: a*X + b*Y + c*Z + Xs
		        d*X + e*Y + f*Z + Ys
		        g*X + h*Y + i*Z + Zs
				0*X + 0*Y + 0*Z + 1
		result:	A.3 += A.0*X + A.1*Y + A.2*Z
		*/
	}

#undef world_matrix_T_J_V
}// namespace clmagic

#endif