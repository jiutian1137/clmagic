#pragma once
#ifndef clmagic_math_lapack_GEOMETRY_h_
#define clmagic_math_lapack_GEOMETRY_h_
#include "vector.h"
#include "matrix.h"
#include "Rodrigues.h"

namespace clmagic { 
	template<typename _Ty, typename _Block = _Ty, bool _Major = _COL_MAJOR_>
	struct PerspectiveLH {
		using matrix_type = matrix4x4<_Ty, _Block, _Major, normal_matrix_tag>;

		/* @_fov: virtical field of angle
		   @_r: aspect, window-width/window-height
		   @_n: near-plane Z, must grater 0.000002
		   @_f: far-plane Z
		*/
		static matrix_type get_matrix(radians fov, _Ty r, _Ty n, _Ty f) {
			/*
				w/h = _Aspect
				w/1 = _Aspect
				w = _Aspect = r, if w=1 h=? result same of above...
			*/
			const auto d  = (_Ty)1 / tan(static_cast<_Ty>(fov / 2));// cot(fov/2)
			const auto fn = f / (f - n);
			/* A*z+B = (z-n)*f/(f-n)
					 = z*(f/(f-n)]) + -n*(f/(f-n))
			*/
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					 d / r, (_Ty)0, (_Ty)0, (_Ty)0,
					(_Ty)0,      d, (_Ty)0, (_Ty)0,
					(_Ty)0, (_Ty)0,     fn,  -n*fn,
					(_Ty)0, (_Ty)0, (_Ty)1, (_Ty)0 };
			} else {
				return matrix_type{
					 d / r, (_Ty)0, (_Ty)0, (_Ty)0,
					(_Ty)0,      d, (_Ty)0, (_Ty)0,
					(_Ty)0, (_Ty)0,     fn, (_Ty)1,
					(_Ty)0, (_Ty)0,  -n*fn, (_Ty)0 };
			}
		}
	
		static matrix_type get_matrix(_Ty w, _Ty h, _Ty n, _Ty f) {
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
			const auto n_m2 = static_cast<_Ty>(2) * n;
			const auto fn   = f / (f - n);
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					n_m2/w, (_Ty)0, (_Ty)0, (_Ty)0,
					(_Ty)0, n_m2/h, (_Ty)0, (_Ty)0,
					(_Ty)0, (_Ty)0,     fn,  -n/fn,
					(_Ty)0, (_Ty)0, (_Ty)1, (_Ty)0 };
			} else {
				return matrix_type{
					n_m2/w, (_Ty)0, (_Ty)0, (_Ty)0,
					(_Ty)0, n_m2/h, (_Ty)0, (_Ty)0,
					(_Ty)0, (_Ty)0,     fn, (_Ty)1,
					(_Ty)0, (_Ty)0,  -n/fn, (_Ty)0 };
			}
		}

		static _Ty get_Znear(const matrix_type& M) {
			if _CONSTEXPR_IF(_Major == _COL_MAJOR_) {
				// M[2][3] = -Znear*M[2][2]
				// Znear   = -M[2][3] / M[2][2]
				return -(M.at(2, 3) / M.at(2, 2));
			} else {
				return -(M.at(3, 2) / M.at(2, 2));
			}
		}

		static _Ty get_Zfar(const matrix_type& _Matrix, const _Ty _Znear) {
			/* M[2][2] = fn = _Zfar / (_Zfar - _Znear)
			M[2][2] * (_Zfar - _Znear)     = _Zfar
			M[2][2]*_Zfar - M[2][2]*_Znear = _Zfar
				            - M[2][2]*_Znear = _Zfar - M[2][2]*_Zfar
				            - M[2][2]*_Znear = _Zfar(1 - M[2][2])
			( - M[2][2]*_Znear)/(1-M[2][2])= _Zfar
			*/
			const auto fn = _Matrix.at(2, 2);
			return ((-fn*_Znear) / ((_Ty)1 - fn));
		}

		static _Ty get_Zfar(const matrix_type& _Matrix) {
			return get_Zfar(_Matrix, get_Znear(_Matrix));
		}
	
		static std::pair<_Ty, _Ty> get_Zrange(const matrix_type& _Matrix) {
			const auto _Znear = get_Znear(_Matrix);
			const auto _Zfar  = get_Zfar(_Matrix, _Znear);
			return std::pair<_Ty, _Ty>{ _Znear, _Zfar };
		}
	
		// Lh == RH
		static _Ty get_aspect(const matrix_type& M) {
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
			const auto fov1_2 = radians(cvtfloating_rational<intmax_t>( atan((_Ty)1 / M.at(1,1)) ));
			return (fov1_2 * 2);
		}
		
		static radians get_foh(radians fov, _Ty aspect) {
			const auto foh_1_2 = radians(cvtfloating_rational<intmax_t>(
				atan(aspect * tan(static_cast<_Ty>(fov / 2)))
			) );
			return (foh_1_2 * 2);
		}

		static radians get_foh(const matrix_type& M) {
			return get_foh(get_fov(M), get_aspect(M));
		}
	};

	template<typename _Ty, typename _Block = _Ty, bool _Major = _COL_MAJOR_>
	struct PerspectiveRH : public PerspectiveLH<_Ty, _Block, _Major> {
		using matrix_type = matrix4x4<_Ty, _Block, _Major, normal_matrix_tag>;

		static matrix_type get_matrix(radians fov,  _Ty aspect, _Ty Znear, _Ty Zfar) {
			auto r  = aspect;
			auto d  = (_Ty)1 / tan(static_cast<_Ty>(fov / 2));
			auto fn = Zfar / (Znear - Zfar);
			if _CONSTEXPR_IF(_Major == _COL_MAJOR_) {
				return matrix_type{
					 d / r, (_Ty)0,  (_Ty)0, (_Ty)0,
					(_Ty)0,      d,  (_Ty)0, (_Ty)0,
					(_Ty)0, (_Ty)0,      fn, Znear*fn,
					(_Ty)0, (_Ty)0, (_Ty)-1, (_Ty)0 };
			} else {
				return matrix_type{
					 d / r, (_Ty)0,   (_Ty)0, (_Ty)0,
					(_Ty)0,      d,   (_Ty)0, (_Ty)0,
					(_Ty)0, (_Ty)0,       fn, (_Ty)-1,
					(_Ty)0, (_Ty)0, Znear*fn, (_Ty)0 };
			}
		}
	
		static _Ty get_Znear(const matrix_type& M) {
			if _CONSTEXPR_IF(_Major == _COL_MAJOR_) {
				// M[2][3] = _Znear*M[2][2]
				// _Znear  = M[2][3] / M[2][2]
				return (M.at(2, 3) / M.at(2, 2));
			} else {
				return (M.at(3, 2) / M.at(2, 2));
			}
		}

		static _Ty get_Zfar(const matrix_type& M, const _Ty Znear) {
			/* M[2][2] = fn = _Zfar / (_Znear - _Zfar)
			M[2][2] * (_Znear - _Zfar)     = _Zfar
			M[2][2]*_Znear - M[2][2]*_Zfar = _Zfar
				            M[2][2]*_Znear = _Zfar + M[2][2]*_Zfar
				            M[2][2]*_Znear = _Zfar(1 + M[2][2])
			M[2][2]*_Znear / (1 + M[2][2]) = _Zfar
			*/
			const auto fn = M.at(2, 2);
			return (fn * Znear / ((_Ty)1 + fn));
		}

		static _Ty get_Zfar(const matrix_type& M) {
			return get_Zfar(M, get_Znear(M));
		}
	
		static std::pair<_Ty, _Ty> get_Zrange(const matrix_type& M) {
			const auto _Znear = get_Znear(M);
			const auto _Zfar  = get_Zfar(M, _Znear);
			return std::pair<_Ty, _Ty>{ _Znear, _Zfar };
		}
	};


	template<typename _Ty, size_t _Rows, typename _Block = _Ty, bool _Major = _COL_MAJOR_>
	struct Rotation : public ::Rodrigues::Rotation<_Ty, _Rows, _Block, _Major> {
		//
	};

	template<typename _Ty, typename _Block = _Ty, bool _Major = _COL_MAJOR_>
	struct Translation {
		using matrix_type = matrix4x4<_Ty, _Block, _Major, normal_matrix_tag>;

		static matrix_type get_matrix(_Ty x, _Ty y, _Ty z) {
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					(_Ty)1, (_Ty)0, (_Ty)0,     x,
					(_Ty)0, (_Ty)1, (_Ty)0,     y,
					(_Ty)0, (_Ty)0, (_Ty)1,     z, 
					(_Ty)0, (_Ty)0, (_Ty)0, (_Ty)1 };
			} else {
				return matrix_type{
					(_Ty)1, (_Ty)0, (_Ty)0, (_Ty)0,
					(_Ty)0, (_Ty)1, (_Ty)0, (_Ty)0,
					(_Ty)0, (_Ty)0, (_Ty)1, (_Ty)0,
					     x,      y,      z, (_Ty)1 };
			}
		}

		static matrix_type get_matrix(const matrix_type& _Axis, _Ty x, _Ty y, _Ty z) {
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
				const auto rx = _Axis.ref(0), ry = _Axis.ref(4), rz = _Axis.ref(8);
				const auto ux = _Axis.ref(1), uy = _Axis.ref(5), uz = _Axis.ref(9);
				const auto fx = _Axis.ref(2), fy = _Axis.ref(6), fz = _Axis.ref(10);
				return matrix_type{
					  rx,     ry,     rz,   rx*x+ux*y+fx*z,
					  ux,     uy,     uz,   ry*x+uy*y+fy*z,
					  fx,     fy,     fz,   rz*x+uz*y+fz*z,
					(_Ty)0, (_Ty)0, (_Ty)0,      (_Ty)1 };
			} else {
				const auto rx = _Axis.ref(0), ry = _Axis.ref(1), rz = _Axis.ref(2);
				const auto ux = _Axis.ref(4), uy = _Axis.ref(5), uz = _Axis.ref(6);
				const auto fx = _Axis.ref(8), fy = _Axis.ref(9), fz = _Axis.ref(10);
				return matrix_type{
					      rx,             ry,             rz,       (_Ty)0,
					      ux,             uy,             uz,       (_Ty)0,
					      fx,             fy,             fz,       (_Ty)0,
					rx*x+ux*y+fx*z, ry*x+uy*y+fy*z, rz*x+uz*y+fz*z, (_Ty)1 };
			}
		}

		static matrix_type get_matrix(const matrix_type& _Axis, vector3<_Ty, _Block> pos) {
			return get_matrix(_Axis, pos[0], pos[1], pos[2]);
		}
	};

	template<typename _Ty, typename _Block = _Ty, bool _Major = _COL_MAJOR_>
	struct LookatLH {
		using matrix_type = matrix4x4<_Ty, _Block, _Major, normal_matrix_tag>;
		using vector3      = clmagic::vector3<_Ty, _Block>;
		using unit_vector3 = clmagic::unit_vector3<_Ty, _Block>;

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

			const auto Pneg = -Peye;

			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					r[0],     r[1],   r[2], dot(Pneg, r),
					u[0],     u[1],   u[2], dot(Pneg, u),
					f[0],     f[1],   f[2], dot(Pneg, f),
					(_Ty)0, (_Ty)0, (_Ty)0,       (_Ty)1 };
			} else {
				return matrix_type{
					r[0],         u[0],         f[0],      (_Ty)0,
					r[1],         u[1],         f[1],      (_Ty)0,
					r[2],         u[2],         f[2],      (_Ty)0,
					dot(Pneg,r), dot(Pneg,u), dot(Pneg,f), (_Ty)1 };
			}
		}
	};

	template<typename _Ty, typename _Block = _Ty, bool _Major = _COL_MAJOR_>
	struct LookatRH {
		using matrix_type = matrix4x4<_Ty, _Block, _Major, normal_matrix_tag>;
		using vector3      = clmagic::vector3<_Ty, _Block>;
		using unit_vector3 = clmagic::unit_vector3<_Ty, _Block>;

		static matrix_type get_matrix(vector3 Peye, unit_vector3 f, unit_vector3 u) {
			return LookatLH<_Ty, _Major, _Block>::get_matrix(Peye, -f, u);
		}
	};

}// namespace clmagic

#endif