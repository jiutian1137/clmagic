#pragma once
#ifndef clmagic_math_lapack_GEOMETRY_h_
#define clmagic_math_lapack_GEOMETRY_h_
#include "vector.h"
#include "matrix.h"
#include "Euler.h"
#include "Rodrigues.h"
#include "../complex/WilliamRowanHamilton.h"

namespace clmagic {
	template<typename _SclTy>
	struct spherical_coordinate {
		spherical_coordinate()
			: radius(static_cast<_SclTy>(0)), azimuth(static_cast<_SclTy>(0)), zenith(static_cast<_SclTy>(0)) {}
		
		spherical_coordinate(_SclTy _Radius, radians _Azimuth, radians _Zenith)
			: radius(_Radius), azimuth(static_cast<_SclTy>(_Azimuth)), zenith(static_cast<_SclTy>(_Zenith)) {}
		
		template<typename _VecTy>
		spherical_coordinate(const _VecTy& XYZ) {		
			const auto x = XYZ[0];
			const auto y = XYZ[1];
			const auto z = XYZ[2];

			radius = sqrt(x*x + y*y + z*z);
			// y = radius * cos(zenith) => zenith = acos(y/radius)
			zenith = acos(y / radius);
			//    x = radius * sin(zenith) * cos(azimuth)
			//    z = radius * sin(zenith) * sin(azimuth)
			// => x/z = tan(azimuth) => atan(x/z) = azimuth
			azimuth = atan(x / z);
		}

		template<typename _VecTy>
		_VecTy to_() const {
			/*
				  Y|  /Z
				   | /
				   |/
			-------+--------X
				  /|
				 / |
				   |
			XZ_radius = cos(Pi/2 - zenith) = sin(zenith)
			*/
			const auto XZ_radius = radius * sin(zenith);
			return _VecTy{ XZ_radius * cos(azimuth),
						   radius    * cos(zenith), 
						   XZ_radius * sin(azimuth) };
		}

		_SclTy radius;
		_SclTy azimuth;
		_SclTy zenith;
	};

	template<typename _Fn, typename _SclTy, size_t _Size, typename _BlkTy>
	vector<_SclTy, _Size, _BlkTy> coordinate_cast(const vector<_SclTy, _Size, _BlkTy>& _Source) {
	}

	template<typename _SclTy, typename _BlkTy = _SclTy, bool _Major = _COL_MAJOR_>
	struct PerspectiveLH {
		using matrix_type = matrix4x4<_SclTy, _BlkTy, _Major, normal_matrix_tag>;

		/* @_fov: virtical field of angle
		   @_r: aspect, window-width/window-height
		   @_n: near-plane Z, must grater 0.000002
		   @_f: far-plane Z
		*/
		static matrix_type get_matrix(radians fov, _SclTy r, _SclTy n, _SclTy f) {
			/*
				w/h = _Aspect
				w/1 = _Aspect
				w = _Aspect = r, if w=1 h=? result same of above...
			*/
			const auto d  = (_SclTy)1 / tan(static_cast<_SclTy>(fov / 2));// cot(fov/2)
			const auto fn = f / (f - n);
			/* A*z+B = (z-n)*f/(f-n)
					 = z*(f/(f-n)]) + -n*(f/(f-n))
			*/
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					 d / r, (_SclTy)0, (_SclTy)0, (_SclTy)0,
					(_SclTy)0,      d, (_SclTy)0, (_SclTy)0,
					(_SclTy)0, (_SclTy)0,     fn,  -n*fn,
					(_SclTy)0, (_SclTy)0, (_SclTy)1, (_SclTy)0 };
			} else {
				return matrix_type{
					 d / r, (_SclTy)0, (_SclTy)0, (_SclTy)0,
					(_SclTy)0,      d, (_SclTy)0, (_SclTy)0,
					(_SclTy)0, (_SclTy)0,     fn, (_SclTy)1,
					(_SclTy)0, (_SclTy)0,  -n*fn, (_SclTy)0 };
			}
		}
	
		static matrix_type get_matrix(_SclTy w, _SclTy h, _SclTy n, _SclTy f) {
			/*
				h/2 / n = tan(fov/2)
				h/(2n)  = tan(fov/2)
				2n/h    = d

				w/h=r
				w=r*h
				d/r = 2n/h/r
				    = 2n/(h*r)
					= 2n/w
			*/
			const auto n_m2 = static_cast<_SclTy>(2) * n;
			const auto fn   = f / (f - n);
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					n_m2/w, (_SclTy)0, (_SclTy)0, (_SclTy)0,
					(_SclTy)0, n_m2/h, (_SclTy)0, (_SclTy)0,
					(_SclTy)0, (_SclTy)0,     fn,  -n/fn,
					(_SclTy)0, (_SclTy)0, (_SclTy)1, (_SclTy)0 };
			} else {
				return matrix_type{
					n_m2/w, (_SclTy)0, (_SclTy)0, (_SclTy)0,
					(_SclTy)0, n_m2/h, (_SclTy)0, (_SclTy)0,
					(_SclTy)0, (_SclTy)0,     fn, (_SclTy)1,
					(_SclTy)0, (_SclTy)0,  -n/fn, (_SclTy)0 };
			}
		}

		static _SclTy get_Znear(const matrix_type& M) {
			if _CONSTEXPR_IF(_Major == _COL_MAJOR_) {
				// M[2][3] = -Znear*M[2][2]
				// Znear   = -M[2][3] / M[2][2]
				return -(M.at(2, 3) / M.at(2, 2));
			} else {
				return -(M.at(3, 2) / M.at(2, 2));
			}
		}

		static _SclTy get_Zfar(const matrix_type& _Matrix, const _SclTy _Znear) {
			/* M[2][2] = fn = _Zfar / (_Zfar - _Znear)
			M[2][2] * (_Zfar - _Znear)     = _Zfar
			M[2][2]*_Zfar - M[2][2]*_Znear = _Zfar
				            - M[2][2]*_Znear = _Zfar - M[2][2]*_Zfar
				            - M[2][2]*_Znear = _Zfar(1 - M[2][2])
			( - M[2][2]*_Znear)/(1-M[2][2])= _Zfar
			*/
			const auto fn = _Matrix.at(2, 2);
			return ((-fn*_Znear) / ((_SclTy)1 - fn));
		}

		static _SclTy get_Zfar(const matrix_type& _Matrix) {
			return get_Zfar(_Matrix, get_Znear(_Matrix));
		}
	
		static std::pair<_SclTy, _SclTy> get_Zrange(const matrix_type& _Matrix) {
			const auto _Znear = get_Znear(_Matrix);
			const auto _Zfar  = get_Zfar(_Matrix, _Znear);
			return std::pair<_SclTy, _SclTy>{ _Znear, _Zfar };
		}
	
		// Lh == RH
		static _SclTy get_aspect(const matrix_type& M) {
			// d / r = M[0][0]
			// M[1][1] / r = M[0][0], r = M[1][1]/M[0][0]
			return M.at(1, 1) / M.at(0, 0);
		}
	
		static radians get_fov(const matrix_type& M) {// get virtical_field_of_angle
			/* d = 1 / tan(_Fov / 2)
			  1/d = tan(_Fov / 2)
			  atan(1/d) = _Fov / 2
			  atan(1/d)*2 = _Fov
			  atan(1/M[1][1])*2 = _Fov
			*/
			const auto fov1_2 = radians(cvtfloating_rational<intmax_t>( atan((_SclTy)1 / M.at(1,1)) ));
			return (fov1_2 * 2);
		}
		
		static radians get_foh(radians fov, _SclTy aspect) {
			const auto foh_1_2 = radians(cvtfloating_rational<intmax_t>(
				atan(aspect * tan(static_cast<_SclTy>(fov / 2)))
			) );
			return (foh_1_2 * 2);
		}

		static radians get_foh(const matrix_type& M) {
			return get_foh(get_fov(M), get_aspect(M));
		}
	};

	template<typename _SclTy, typename _BlkTy = _SclTy, bool _Major = _COL_MAJOR_>
	struct PerspectiveRH : public PerspectiveLH<_SclTy, _BlkTy, _Major> {
		using matrix_type = matrix4x4<_SclTy, _BlkTy, _Major, normal_matrix_tag>;

		static matrix_type get_matrix(radians fov,  _SclTy aspect, _SclTy Znear, _SclTy Zfar) {
			auto r  = aspect;
			auto d  = (_SclTy)1 / tan(static_cast<_SclTy>(fov / 2));
			auto fn = Zfar / (Znear - Zfar);
			if _CONSTEXPR_IF(_Major == _COL_MAJOR_) {
				return matrix_type{
					 d / r, (_SclTy)0,  (_SclTy)0, (_SclTy)0,
					(_SclTy)0,      d,  (_SclTy)0, (_SclTy)0,
					(_SclTy)0, (_SclTy)0,      fn, Znear*fn,
					(_SclTy)0, (_SclTy)0, (_SclTy)-1, (_SclTy)0 };
			} else {
				return matrix_type{
					 d / r, (_SclTy)0,   (_SclTy)0, (_SclTy)0,
					(_SclTy)0,      d,   (_SclTy)0, (_SclTy)0,
					(_SclTy)0, (_SclTy)0,       fn, (_SclTy)-1,
					(_SclTy)0, (_SclTy)0, Znear*fn, (_SclTy)0 };
			}
		}
	
		static _SclTy get_Znear(const matrix_type& M) {
			if _CONSTEXPR_IF(_Major == _COL_MAJOR_) {
				// M[2][3] = _Znear*M[2][2]
				// _Znear  = M[2][3] / M[2][2]
				return (M.at(2, 3) / M.at(2, 2));
			} else {
				return (M.at(3, 2) / M.at(2, 2));
			}
		}

		static _SclTy get_Zfar(const matrix_type& M, const _SclTy Znear) {
			/* M[2][2] = fn = _Zfar / (_Znear - _Zfar)
			M[2][2] * (_Znear - _Zfar)     = _Zfar
			M[2][2]*_Znear - M[2][2]*_Zfar = _Zfar
				            M[2][2]*_Znear = _Zfar + M[2][2]*_Zfar
				            M[2][2]*_Znear = _Zfar(1 + M[2][2])
			M[2][2]*_Znear / (1 + M[2][2]) = _Zfar
			*/
			const auto fn = M.at(2, 2);
			return (fn * Znear / ((_SclTy)1 + fn));
		}

		static _SclTy get_Zfar(const matrix_type& M) {
			return get_Zfar(M, get_Znear(M));
		}
	
		static std::pair<_SclTy, _SclTy> get_Zrange(const matrix_type& M) {
			const auto _Znear = get_Znear(M);
			const auto _Zfar  = get_Zfar(M, _Znear);
			return std::pair<_SclTy, _SclTy>{ _Znear, _Zfar };
		}
	};


	// inverse( rotation(theta) ) = rotation(-theta)
	// inverse( rotation(theta) ) = transpose( rotation(theta) )
	// reflection() = rotation( Pi/2 )
	template<typename _SclTy, size_t _Rows, typename _BlkTy = _SclTy, bool _Major = _COL_MAJOR_>
	struct rotation : public ::Rodrigues::rotation<_SclTy, _Rows, _BlkTy, _Major>,
		::Euler::rotation<_SclTy, _Rows, _BlkTy, _Major>, 
		::WilliamRowanHamilton::rotation<_SclTy, _Rows, _BlkTy, _Major> {
		using matrix_type  = ::clmagic::square_matrix<_SclTy, _Rows, _BlkTy, _Major, clmagic::normal_matrix_tag>;
		using unit_vector3 = ::clmagic::unit_vector3<_SclTy, _BlkTy>;
		using quaternion   = ::WilliamRowanHamilton::quaternion<_SclTy, _BlkTy>;

		static matrix_type get_matrix(unit_vector3 axis, radians angle) {
			return ::Rodrigues::rotation<_SclTy, _Rows, _BlkTy, _Major>::get_matrix(axis, angle);
		}

		static matrix_type get_matrix(quaternion q) {
			return ::WilliamRowanHamilton::rotation<_SclTy, _Rows, _BlkTy, _Major>::get_matrix(q);
		}
	};

	// inverse( translation(x,y,z) ) = translation(-x,-y,-z)
	template<typename _SclTy, typename _BlkTy = _SclTy, bool _Major = _COL_MAJOR_>
	struct translation {
		using matrix_type = matrix4x4<_SclTy, _BlkTy, _Major, normal_matrix_tag>;

		static matrix_type get_matrix(_SclTy x, _SclTy y, _SclTy z) {
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					(_SclTy)1, (_SclTy)0, (_SclTy)0,     x,
					(_SclTy)0, (_SclTy)1, (_SclTy)0,     y,
					(_SclTy)0, (_SclTy)0, (_SclTy)1,     z, 
					(_SclTy)0, (_SclTy)0, (_SclTy)0, (_SclTy)1 };
			} else {
				return matrix_type{
					(_SclTy)1, (_SclTy)0, (_SclTy)0, (_SclTy)0,
					(_SclTy)0, (_SclTy)1, (_SclTy)0, (_SclTy)0,
					(_SclTy)0, (_SclTy)0, (_SclTy)1, (_SclTy)0,
					     x,      y,      z, (_SclTy)1 };
			}
		}

		static matrix_type get_matrix(const matrix_type& M, _SclTy x, _SclTy y, _SclTy z) {
			/* @_node: col-major-order
			M*TM(x,y,z,1) = E*TM(x1,y1,z1,1)
			M*TM(x,y,z,1) = TM(x1,y1,z1,1)

			[rx ux fx 0]   [1 0 0 x]   [rx ux fx rx*x+ux*y+fx*z]
			[ry uy fy 0]   [0 1 0 y]   [ry uy fy ry*x+uy*y+fy*z]
			[rz uz fz 0] * [0 0 1 z] = [rz uz fz rz*x+uz*y+fz*z]
			[ 0  0  0 1]   [0 0 0 1]   [ 0  0  0       1       ]

			row-major-order:
			[      rx             ry            rz             0 ]
			[      ux             uy            uz             0 ]
			[      fx             fy            fz             0 ]
			[rx*x+ux*y+fx*z ry*x+uy*y+fy*z rz*x+uz*y+fz*z      1 ]
			*/
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				/*const auto rx = M.ref(0), ry = M.ref(4), rz = M.ref(8);
				const auto ux = M.ref(1), uy = M.ref(5), uz = M.ref(9);
				const auto fx = M.ref(2), fy = M.ref(6), fz = M.ref(10);
				return matrix_type{
					  rx,     ux,     fx,   rx*x+ux*y+fx*z,
					  ry,     uy,     fy,   ry*x+uy*y+fy*z,
					  rz,     uz,     fz,   rz*x+uz*y+fz*z,
					(_SclTy)0, (_SclTy)0, (_SclTy)0,      (_SclTy)1 };*/
				return (M * get_matrix(x, y, z));
			} else {
				/*const auto rx = _Axis.ref(0), ry = _Axis.ref(1), rz = _Axis.ref(2);
				const auto ux = _Axis.ref(4), uy = _Axis.ref(5), uz = _Axis.ref(6);
				const auto fx = _Axis.ref(8), fy = _Axis.ref(9), fz = _Axis.ref(10);
				return matrix_type{
					      rx,             ry,             rz,       (_SclTy)0,
					      ux,             uy,             uz,       (_SclTy)0,
					      fx,             fy,             fz,       (_SclTy)0,
					rx*x+ux*y+fx*z, ry*x+uy*y+fy*z, rz*x+uz*y+fz*z, (_SclTy)1 };*/
				return (get_matrix(x,y,z) * M);
			}
		}

		static matrix_type get_matrix(const matrix_type& _Axis, vector3<_SclTy, _BlkTy> pos) {
			return get_matrix(_Axis, pos[0], pos[1], pos[2]);
		}
	};


	template<typename _SclTy, typename _BlkTy = _SclTy>
	struct scaling {
		using matrix_type = ::clmagic::diagonal_matrix<_SclTy, 4, 4, _BlkTy, _COL_MAJOR_>;

		static matrix_type get_matrix(_SclTy x, _SclTy y, _SclTy z) {
			return matrix_type{ x, y, z, (_SclTy)1 };
		}

		static matrix_type get_matrix(_SclTy s) {
			return matrix_type{ (_SclTy)1, (_SclTy)1, (_SclTy)1, ((_SclTy)1) / s };
		}
	};

	template<typename _SclTy, typename _BlkTy = _SclTy, bool _Major = _COL_MAJOR_>
	struct rigid_body_transform {
		using translation = ::clmagic::translation<_SclTy, _BlkTy, _Major>;
		using rotation    = ::clmagic::rotation<_SclTy, 4, _BlkTy, _Major>;
		using matrix_type = ::clmagic::matrix4x4<_SclTy, _BlkTy, _Major>;

		/* inverse( rigid_body )
		 = inverse( translation(t)*rotation )
		 = inverse(rotation) * inverse(translation(t))
		 = transpose(rotation) * translation(-t)
		               
		transpose(rotation) * translation(-t)
		 [rx ry rz 0]           [1 0 0 -x]   [rx ry rz dot(r,-t)]
		 [ux uy uz 0]     *     [0 1 0 -y] = [ux uy uz dot(u,-t)]
		 [fx fy fz 0]           [0 0 1 -z]   [fx fy fz dot(f,-t)]
		 [0  0  0  1]           [0 0 0  1]   [0  0  0       1   ]
		*/
		static matrix_type inverse(const unit_vector3<_SclTy, _BlkTy>& r, const unit_vector3<_SclTy, _BlkTy>& u, const unit_vector3<_SclTy, _BlkTy>& f, const vector3<_SclTy, _BlkTy>& t) {
			const vector3<_SclTy, _BlkTy> neg_t = -t;
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					  r[0],   r[1],   r[2], dot(neg_t, r),
					  u[0],   u[1],   u[2], dot(neg_t, u),
					  f[0],   f[1],   f[2], dot(neg_t, f),
					(_SclTy)0, (_SclTy)0, (_SclTy)0,    (_SclTy)1 };
			} else {
				return matrix_type{
					    r[0],         u[0],        f[0],      (_SclTy)0,
					    r[1],         u[1],        f[1],      (_SclTy)0,
					    r[2],         u[2],        f[2],      (_SclTy)0,
					dot(neg_t,r), dot(neg_t,u), dot(neg_t,f), (_SclTy)1 };
			}
		}

		static matrix_type inverse(const matrix_type& M) {
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				const auto r = unit_vector3<_SclTy, _BlkTy>({ M.at(0,0), M.at(1,0), M.at(2,0) }, true);
				const auto u = unit_vector3<_SclTy, _BlkTy>({ M.at(0,1), M.at(1,1), M.at(2,1) }, true);
				const auto f = unit_vector3<_SclTy, _BlkTy>({ M.at(0,2), M.at(1,2), M.at(2,2) }, true);
				const auto t =      vector3<_SclTy, _BlkTy>({ M.at(0,3), M.at(1,3), M.at(2,3) });
				return inverse(r, u, f, t);
			} else {
				const auto r = unit_vector3<_SclTy, _BlkTy>({ M.at(0,0), M.at(0,1), M.at(0,2) }, true);
				const auto u = unit_vector3<_SclTy, _BlkTy>({ M.at(1,0), M.at(1,1), M.at(1,2) }, true);
				const auto f = unit_vector3<_SclTy, _BlkTy>({ M.at(2,0), M.at(2,1), M.at(2,2) }, true);
				const auto t =      vector3<_SclTy, _BlkTy>({ M.at(3,0), M.at(3,1), M.at(3,2) });
				return inverse(r, u, f, t);
			}
		}
	};


	template<typename _SclTy, typename _BlkTy = _SclTy, bool _Major = _COL_MAJOR_>
	struct LookatLH {
		using matrix_type = matrix4x4<_SclTy, _BlkTy, _Major, normal_matrix_tag>;
		using vector3      = clmagic::vector3<_SclTy, _BlkTy>;
		using unit_vector3 = clmagic::unit_vector3<_SclTy, _BlkTy>;

		static matrix_type get_matrix(vector3 Peye, unit_vector3 f, unit_vector3 u) {
			/* decompose M = QR, col-major-order
			invser(M) = inverse(Q*R)
			          = inverse(R) * inverse(Q)
					              [r u f 0]    [1 0 0 -x]
						          [r u f 0]    [0 1 0 -y]
					  = transpose([r u f 0]) * [0 0 1 -z]
					              [0 0 0 1]    [0 0 0  1]
			From Book<<3D-Game-Programming-With-DirectX12>>
			*/
			auto r = unit_vector3(cross3(u, f));
			     u = unit_vector3(cross3(f, r), true);
			return rigid_body_transform<_SclTy, _BlkTy, _Major>::inverse(r, u, f, Peye);
			/*const auto Pneg = -Peye;

			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					r[0],     r[1],   r[2], dot(Pneg, r),
					u[0],     u[1],   u[2], dot(Pneg, u),
					f[0],     f[1],   f[2], dot(Pneg, f),
					(_SclTy)0, (_SclTy)0, (_SclTy)0,       (_SclTy)1 };
			} else {
				return matrix_type{
					r[0],         u[0],         f[0],      (_SclTy)0,
					r[1],         u[1],         f[1],      (_SclTy)0,
					r[2],         u[2],         f[2],      (_SclTy)0,
					dot(Pneg,r), dot(Pneg,u), dot(Pneg,f), (_SclTy)1 };
			}*/
		}
	};

	template<typename _SclTy, typename _BlkTy = _SclTy, bool _Major = _COL_MAJOR_>
	struct LookatRH {
		using matrix_type = matrix4x4<_SclTy, _BlkTy, _Major, normal_matrix_tag>;
		using vector3      = clmagic::vector3<_SclTy, _BlkTy>;
		using unit_vector3 = clmagic::unit_vector3<_SclTy, _BlkTy>;

		static matrix_type get_matrix(vector3 Peye, unit_vector3 f, unit_vector3 u) {
			return LookatLH<_SclTy, _Major, _BlkTy>::get_matrix(Peye, -f, u);
		}
	};

}// namespace clmagic

#endif