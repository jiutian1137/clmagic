//--------------------------------------------------------------------------------------
// Copyright (c) 2020 LongJiangnan
// All Rights Reserved
// Apache Licene 2.0
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_calculation_lapack_GEOMETRY_h_
#define clmagic_calculation_lapack_GEOMETRY_h_
#include "vector.h"
#include "matrix.h"
#include "Euler.h"
#include "Rodrigues.h"
#include "../complex/WilliamRowanHamilton.h"

namespace clmagic {
	template<typename _Ts>
	struct spherical_coordinate {
		spherical_coordinate()
			: radius(static_cast<_Ts>(0)), azimuth(static_cast<_Ts>(0)), zenith(static_cast<_Ts>(0)) {}
		
		spherical_coordinate(_Ts _Radius, radians<_Ts> _Azimuth, radians<_Ts> _Zenith)
			: radius(_Radius), azimuth(static_cast<_Ts>(_Azimuth)), zenith(static_cast<_Ts>(_Zenith)) {}
		
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
			-------+--------X start
				  /|
				 / |
				   |
			XZ_radius = sin(Pi/2 - zenith) = cos(zenith)
			*/
			const auto XZ_radius = radius * cos(zenith);
			return _VecTy{ XZ_radius * cos(azimuth),
						   radius * sin(zenith),
						   XZ_radius * sin(azimuth) };
		}

		_Ts radius;
		_Ts azimuth;
		_Ts zenith;
	};

	template<typename _Ts>
	struct cylindrical_coordinate {
		cylindrical_coordinate()
			: radius(static_cast<_Ts>(0)), azimuth(static_cast<_Ts>(0)), height(static_cast<_Ts>(0)) {}

		cylindrical_coordinate(_Ts _Radius, radians<_Ts> _Azimuth, _Ts _Height)
			: radius(_Radius), azimuth(_Azimuth), height(_Height) {}

		template<typename _VecTy>
		cylindrical_coordinate(const _VecTy& XYZ) {
			const auto x = XYZ[0];
			const auto y = XYZ[1];
			const auto z = XYZ[2];
			radius  = sqrt(x*x + z*z);
			azimuth = acos(x / radius);
			height  = y;
		}

		template<typename _VecTy>
		_VecTy to_() const {
			return _VecTy{ radius*cos(azimuth), height, radius*sin(azimuth) };
		}

		_Ts radius;
		_Ts azimuth;
		_Ts height;
	};

	enum Coordinates{
		_LH_,
		_RH_
	};


	/*- - - - - - - - - - - - - - - - - - perspective - - - - - - - - - - - - - - -*/
	template<typename _Tm, int _Cd = Coordinates::_LH_>
	struct perspective {};

	template<typename _Ts, typename _Tb, bool _Major>
	struct perspective< MATRIX4x4, Coordinates::_LH_ > {// perspective_left_hand
		using matrix_type  = MATRIX4x4;
		using scalar_type  = _Ts;
		using radians_type = radians<_Ts>;

		/* @_fov: virtical field of angle
		   @_r: aspect, window-width/window-height
		   @_n: near-plane Z, must grater 0.000002
		   @_f: far-plane Z
		*/
		static matrix_type get_matrix(radians_type fov, scalar_type aspect, scalar_type n, scalar_type f) {
			/* aspect = w/h
			   => w = aspect*h
			   => h = w/aspect

			   w = 1
			   h = 1/aspect
			*/
			const auto d  = 1 / tan( static_cast<_Ts>(fov)/2 );// cot(fov/2)
			const auto fn = f / (f - n);
			/* A*z+B = (z-n)*f/(f-n)
					 = z*(f/(f-n)]) + -n*(f/(f-n))
			*/
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					d/aspect, (_Ts)0, (_Ts)0, (_Ts)0,
					(_Ts)0,        d, (_Ts)0, (_Ts)0,
					(_Ts)0,   (_Ts)0,     fn,  -n*fn,
					(_Ts)0,   (_Ts)0, (_Ts)1, (_Ts)0 };
			} else {
				return matrix_type{
					d/aspect, (_Ts)0, (_Ts)0, (_Ts)0,
					(_Ts)0,        d, (_Ts)0, (_Ts)0,
					(_Ts)0,   (_Ts)0,     fn, (_Ts)1,
					(_Ts)0,   (_Ts)0,  -n*fn, (_Ts)0 };
			}
		}
		static matrix_type get_matrix(scalar_type w, scalar_type h, scalar_type n, scalar_type f) {
			/* h/2 / n = tan(fov/2)
			   => h/(2n)  = tan(fov/2)
			   => 2n/h    = d

			   w/h=aspect
			   => w=aspect*h
			   d/aspect = (2n/h) / aspect
			   => d/aspect = 2n/(h*aspect)
			   => d/aspect = 2n / w
			*/
			const auto n_m2 = ((_Ts)2) * n;
			const auto fn   = f / (f - n);
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					n_m2/w, (_Ts)0, (_Ts)0, (_Ts)0,
					(_Ts)0, n_m2/h, (_Ts)0, (_Ts)0,
					(_Ts)0, (_Ts)0,     fn,  -n/fn,
					(_Ts)0, (_Ts)0, (_Ts)1, (_Ts)0 };
			} else {
				return matrix_type{
					n_m2/w, (_Ts)0, (_Ts)0, (_Ts)0,
					(_Ts)0, n_m2/h, (_Ts)0, (_Ts)0,
					(_Ts)0, (_Ts)0,     fn, (_Ts)1,
					(_Ts)0, (_Ts)0,  -n/fn, (_Ts)0 };
			}
		}

		static scalar_type get_Znear(const matrix_type& M) {
			if _CONSTEXPR_IF(_Major == _COL_MAJOR_) {
				// M[2][3] = -Znear*M[2][2]
				// Znear   = -M[2][3] / M[2][2]
				return -(M.at(2, 3) / M.at(2, 2));
			} else {
				return -(M.at(3, 2) / M.at(2, 2));
			}
		}
		static scalar_type get_Zfar(const matrix_type& _Matrix, scalar_type _Znear) {
			/* M[2][2] = fn = _Zfar / (_Zfar - _Znear)
			M[2][2] * (_Zfar - _Znear)     = _Zfar
			M[2][2]*_Zfar - M[2][2]*_Znear = _Zfar
				            - M[2][2]*_Znear = _Zfar - M[2][2]*_Zfar
				            - M[2][2]*_Znear = _Zfar(1 - M[2][2])
			( - M[2][2]*_Znear)/(1-M[2][2])= _Zfar
			*/
			const scalar_type fn = _Matrix.at(2, 2);
			return ((-fn*_Znear) / (1 - fn));
		}
		static scalar_type get_Zfar(const matrix_type& _Matrix) {
			return get_Zfar(_Matrix, get_Znear(_Matrix));
		}
		static std::pair<scalar_type, scalar_type> get_Zrange(const matrix_type& _Matrix) {
			const scalar_type _Znear = get_Znear(_Matrix);
			const scalar_type _Zfar  = get_Zfar(_Matrix, _Znear);
			return std::pair<scalar_type, scalar_type>{ _Znear, _Zfar };
		}
	
		// Lh == RH
		static scalar_type get_aspect(const matrix_type& M) {
			/* M11 = d, M00 = d/r
			   => r = d * (r/d)
			   => r = M11/M00
			*/
			return M.at(1,1) / M.at(0,0);
		}
		static radians_type get_fov(const matrix_type& M) {// get virtical_field_of_angle
			/* d = 1 / tan(_Fov / 2)
			  => 1/d = tan(_Fov / 2)
			  => atan(1/d)*2 = _Fov
			  M11 = d
			  => atan(1/M11)*2 = _Fov
			*/
			const auto M11 = M.at(1, 1);
			return atan( 1 / M11 ) * 2;
		}
		static radians_type get_foh(radians_type fov, scalar_type aspect) {
			return atan( aspect * tan(static_cast<_Ts>(fov)/2) ) * 2;
		}
		static radians_type get_foh(const matrix_type& M) {
			return get_foh(get_fov(M), get_aspect(M));
		}
	};

	template<typename _Ts, typename _Tb, bool _Major>
	struct perspective< MATRIX4x4, Coordinates::_RH_ > : public perspective< MATRIX4x4, Coordinates::_LH_ > {// perspective_right_hand
		using matrix_type  = MATRIX4x4;
		using scalar_type  = _Ts;
		using radians_type = radians<_Ts>;

		static matrix_type get_matrix(radians_type fov, scalar_type aspect, scalar_type n, scalar_type f) {
			auto d  = 1 / tan( static_cast<_Ts>(fov)/2 );// cot(fov/2)
			auto fn = f / (n - f);
			if _CONSTEXPR_IF( matrix_type::col_major() ) {
				return matrix_type{
					d/aspect, (_Ts)0,  (_Ts)0, (_Ts)0,
					(_Ts)0,        d,  (_Ts)0, (_Ts)0,
					(_Ts)0,   (_Ts)0,      fn,   n*fn,
					(_Ts)0,   (_Ts)0, (_Ts)-1, (_Ts)0 };
			} else {
				return matrix_type{
					d/aspect, (_Ts)0, (_Ts)0, (_Ts)0,
					(_Ts)0,        d, (_Ts)0, (_Ts)0,
					(_Ts)0,   (_Ts)0,     fn, (_Ts)-1,
					(_Ts)0,   (_Ts)0,   n*fn, (_Ts)0 };
			}
		}
		static matrix_type get_matrix(scalar_type w, scalar_type h, scalar_type n, scalar_type f) {
			const auto n_m2 = 2 * n;
			const auto fn   = f / (n - f);
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					n_m2/w, (_Ts)0,  (_Ts)0, (_Ts)0,
					(_Ts)0, n_m2/h,  (_Ts)0, (_Ts)0,
					(_Ts)0, (_Ts)0,      fn,   n*fn,
					(_Ts)0, (_Ts)0, (_Ts)-1, (_Ts)0 };
			} else {
				return matrix_type{
					n_m2/w, (_Ts)0, (_Ts)0, (_Ts)0,
					(_Ts)0, n_m2/h, (_Ts)0, (_Ts)0,
					(_Ts)0, (_Ts)0,     fn, (_Ts)-1,
					(_Ts)0, (_Ts)0,   n*fn, (_Ts)0 };
			}
		}
	
		static scalar_type get_Znear(const matrix_type& M) {
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				/* M23 = n*fn, M22 = fn
				   => n = M23/M22
				*/
				return (M.at(2,3) / M.at(2,2));
			} else {
				return (M.at(3,2) / M.at(2,2));
			}
		}
		static scalar_type get_Zfar(const matrix_type& M, scalar_type Znear) {
			/* <idea> Sovle: M22 = fn = f/(n-f) </idea>
			=> f = M22 * (n - f)
			=> f = M22*n - M22*f
			=> f(1+M22) = M22*n
			=> f = M22*n/(1+M22) or f = fn*n/(1+fn)
			*/
			const scalar_type fn = M.at(2, 2);
			return ( fn * Znear / (1 + fn) );
		}
		static scalar_type get_Zfar(const matrix_type& M) {
			return get_Zfar(M, get_Znear(M));
		}
		static std::pair<scalar_type, scalar_type> get_Zrange(const matrix_type& M) {
			const auto _Znear = get_Znear(M);
			const auto _Zfar  = get_Zfar(M, _Znear);
			return std::pair<scalar_type, scalar_type>{ _Znear, _Zfar };
		}
	};

	/*- - - - - - - - - - - - - - - - - - planar_projection - - - - - - - - - - - - - - -*/
	template<typename _Ts, typename _Tb = _Ts, bool _Major = _COL_MAJOR_>
	struct planar_projection {
		// convert V to P in the Plane
		using matrix_type       = matrix4x4<_Ts, _Tb, _Major, normal_matrix_tag>;
		using scalar_type       = _Ts;
		using vector3_type      = vector3<_Ts, _Tb>;
		using unit_vector3_type = unit_vector3<_Ts, _Tb>;
		
		/*<figure>
				. <-- Peye
			   /\ \ 
			 //  \\ \
			/vvvv\  \  \
			ssssss\   \   \
			sssssss\    \    \
			ssssssss\     \     \
			=========================y=0
		  </figure>*/
		static matrix_type get_matrix(vector3_type Peye) {// project plane(y=0)
			/*<describ>
				<idea> similar-triangles </idea>
				<important> 
					solve-equation: V.x-Peye.x : P.x-Peye.x = V.y-Peye.y : P.y-Peye.y 
					solve-equation: Peye.x-V.x : Peye.x-P.x = Peye.y-V.y : Peye.y 
				</important>
				<process>
					 X:   (Peye.x-V.x)     (Peye.y-V.y)
						 --------------- = ------------
						  (Peye.x-P.x)      (Peye.y)

					 =>   (Peye.x-V.x) * Peye.y
						 ----------------------- = (Peye.x-P.x)
							  (Peye.y-V.y)

					 =>     (Peye.x-V.x) * Peye.y
						 - ----------------------- + Peye.x = P.x
							    (Peye.y-V.y)

					 =>     (Peye.x-V.x) * Peye.y    Peye.x*(Peye.y-V.y)
						 - ----------------------- + -------------------- = P.x    
							    (Peye.y-V.y)           (Peye.y-V.y)

					 =>   Peye.x*(Peye.y-V.y)-(Peye.x-V.x)*Peye.y
						 ------------------------------------------ = P.x    
							           (Peye.y-V.y)

					 =>    -Peye.x*V.y + V.x*Peye.y
						 ----------------------------- = P.x
								(Peye.y-V.y)
			
					 Z:   -Peye.z*V.y + V.z*Peye.y 
						 ------------------------- = P.z    
							     Peye.y-V.y
				</process>
				<result>
					[-Peye.x*V.y + V.x*Peye.y]
					[            0           ]
					[-Peye.z*V.y + V.z*Peye.y]
					[     Peye.y - V.y       ]
				</result>
				<reference>
					is equivalent to <<RealTimeReandering-4th>> page:225
				</reference>
			  </describ>*/
			if (matrix_type::col_major()) {
				return matrix_type{
					Peye[1], -Peye[0], (_Ts)0,  (_Ts)0,
					(_Ts)0,   (_Ts)0,  (_Ts)0,  (_Ts)0,
					(_Ts)0,  -Peye[2], Peye[1], (_Ts)0,
					(_Ts)0,   (_Ts)-1, (_Ts)0,  Peye[1] };
			} else {
				return matrix_type{
					 Peye[1], (_Ts)0,  (_Ts)0,  (_Ts)0,
					-Peye[0], (_Ts)0, -Peye[2], (_Ts)0,
					 (_Ts)0,  (_Ts)0,  Peye[1], (_Ts)0,
					 (_Ts)0,  (_Ts)-1, (_Ts)0,  Peye[1] };
			}
		}

		static matrix_type get_matrix(vector3_type Peye, unit_vector3_type N, scalar_type d) {// project any-plane
			/*<describ>
				<process>
				                d + dot(N,Peye)
					P = Peye - ----------------(V-Peye)
					            dot(N,(V-Peye))
							(d + dot(N,Peye))*(V-Peye)    Peye*dot(N,(V-Peye))
					P =  - --------------------------- + ---------------------
					             dot(N,(V-Peye))           dot(N,(V-Peye))
				</process>
				<result>
					[(dot(N,Peye) + d)*Vx - Peye.x*N.x*V.x - Peye.x*N.y*V.y - Peye.x*N.z*V.z - Peye.x*d]
					[(dot(N,Peye) + d)*Vy - Peye.y*N.x*V.x - Peye.y*N.y*V.y - Peye.y*N.z*V.z - Peye.y*d]
					[(dot(N,Peye) + d)*Vz - Peye.z*N.x*V.x - Peye.z*N.y*V.y - Peye.z*N.z*V.z - Peye.z*d]
					[-dot(N,V)+dot(N,Peye)]
				</result>
			  </describ>*/
			const auto N_dot_Peye  = dot(N, Peye);
			const auto Nx_mul_Peye = Peye * N[0];
			const auto Ny_mul_Peye = Peye * N[1];
			const auto Nz_mul_Peye = Peye * N[2];
			const auto d_mul_Pey   = Peye * d;
			if ( matrix_type::col_major() ) {
				return matrix_type{
					N_dot_Peye+d - Nx_mul_Peye[0],              - Ny_mul_Peye[0],              - Nz_mul_Peye[0], -d_mul_Pey[0],
					             - Nx_mul_Peye[1], N_dot_Peye+d - Ny_mul_Peye[1],              - Nz_mul_Peye[1], -d_mul_Pey[1],
					             - Nx_mul_Peye[2],              - Ny_mul_Peye[2], N_dot_Peye+d - Nz_mul_Peye[2], -d_mul_Pey[2],
					                        -N[0],                         -N[1],                         -N[3],  N_dot_Peye };
			} else {
				return matrix_type{
					N_dot_Peye+d - Nx_mul_Peye[0],              - Nx_mul_Peye[1],              - Nx_mul_Peye[2],       -N[0],
							     - Ny_mul_Peye[0], N_dot_Peye+d - Ny_mul_Peye[1],              - Ny_mul_Peye[2],       -N[1],
								 - Nz_mul_Peye[0],              - Nz_mul_Peye[1], N_dot_Peye+d - Nz_mul_Peye[2],       -N[3],
								   - d_mul_Pey[0],                - d_mul_Pey[1],                - d_mul_Pey[2],  N_dot_Peye };
			}
		}
	};

	/*- - - - - - - - - - - - - - - - - - rotation - - - - - - - - - - - - - - -*/
	template<typename _Tm>
	struct rotation { };

	template<typename _Ts, size_t _Rows, typename _Tb, bool _Major>
	struct rotation< SQUARE_MATRIXN > :
		public Rodrigues::rotation< SQUARE_MATRIXN >, Euler::rotation< SQUARE_MATRIXN >, WilliamRowanHamilton::rotation< SQUARE_MATRIXN > {
		// inverse( rotation(theta) ) = rotation(-theta)
		// inverse( rotation(theta) ) = transpose( rotation(theta) )
		// reflection() = rotation( Pi/2 )
		using matrix_type       = square_matrix<_Ts, _Rows, _Tb, _Major>;
		using quaternion_type   = WilliamRowanHamilton::quaternion<_Ts, _Tb>;
		using radians_type      = radians<_Ts>;
		using unit_vector3_type = unit_vector3<_Ts, _Tb>;

		static matrix_type get_matrix(unit_vector3_type axis, radians_type angle) {
			return ::Rodrigues::rotation<matrix_type>::get_matrix(axis, angle);
		}

		static matrix_type get_matrix(quaternion_type q) {
			return ::WilliamRowanHamilton::rotation<matrix_type>::get_matrix(q);
		}
	};

	/*- - - - - - - - - - - - - - - - - - translation - - - - - - - - - - - - - - -*/
	template<typename _Tm>
	struct translation {};

	template<typename _Ts, typename _Tb, bool _Major>
	struct translation< MATRIX4x4 > {
		// inverse( translation(x,y,z) ) = translation(-x,-y,-z)
		using matrix_type  = MATRIX4x4;
		using scalar_type  = _Ts;
		using vector3_type = vector3<_Ts, _Tb>;

		static matrix_type get_matrix(scalar_type x, scalar_type y, scalar_type z) {
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					(_Ts)1, (_Ts)0, (_Ts)0,     x,
					(_Ts)0, (_Ts)1, (_Ts)0,     y,
					(_Ts)0, (_Ts)0, (_Ts)1,     z, 
					(_Ts)0, (_Ts)0, (_Ts)0, (_Ts)1 };
			} else {
				return matrix_type{
					(_Ts)1, (_Ts)0, (_Ts)0, (_Ts)0,
					(_Ts)0, (_Ts)1, (_Ts)0, (_Ts)0,
					(_Ts)0, (_Ts)0, (_Ts)1, (_Ts)0,
					     x,      y,      z, (_Ts)1 };
			}
		}
		static matrix_type get_matrix(const matrix_type& M, scalar_type x, scalar_type y, scalar_type z) {
			/*
			<idea>
				M*TM(x,y,z,1) = E*TM(x1,y1,z1,1)
				M*TM(x,y,z,1) = TM(x1,y1,z1,1)
			</idea>
			<col-major-order>
				[rx ux fx 0]   [1 0 0 x]   [rx ux fx rx*x+ux*y+fx*z]
				[ry uy fy 0]   [0 1 0 y]   [ry uy fy ry*x+uy*y+fy*z]
				[rz uz fz 0] * [0 0 1 z] = [rz uz fz rz*x+uz*y+fz*z]
				[ 0  0  0 1]   [0 0 0 1]   [ 0  0  0       1       ]
			</col-major-order>
			<row-major-order>
				[1 0 0 0]   [rx ry rz 0]   [rx ry rz 0]
				[0 1 0 0]   [ux uy uz 0]   [ux uy uz 0]
				[0 0 1 0] * [fx fy fz 0] = [fx fy fz 0]
				[x y z 1]   [ 0  0  0 1]   [rx*x+ux*y+fx*z ry*x+uy*y+fy*z rz*x+uz*y+fz*z 1]
			</row-major-order>
			*/
			if _CONSTEXPR_IF( matrix_type::col_major() ) {
				const auto rx = M.ref(0), ry = M.ref(4), rz = M.ref(8);// col(0)
				const auto ux = M.ref(1), uy = M.ref(5), uz = M.ref(9);// col(1)
				const auto fx = M.ref(2), fy = M.ref(6), fz = M.ref(10);// col(2)
				return matrix_type{
					    rx,     ux,     fx,   rx*x+ux*y+fx*z,
					    ry,     uy,     fy,   ry*x+uy*y+fy*z,
					    rz,     uz,     fz,   rz*x+uz*y+fz*z,
					(_Ts)0, (_Ts)0, (_Ts)0,           (_Ts)1 };
				/*<another> (M * get_matrix(x, y, z)) </another>*/
			} else {
				const auto rx = M.ref(0), ry = M.ref(1), rz = M.ref(2);// row(0)
				const auto ux = M.ref(4), uy = M.ref(5), uz = M.ref(6);// row(1)
				const auto fx = M.ref(8), fy = M.ref(9), fz = M.ref(10);// row(2)
				return matrix_type{
					      rx,             ry,             rz,       (_Ts)0,
					      ux,             uy,             uz,       (_Ts)0,
					      fx,             fy,             fz,       (_Ts)0,
					rx*x+ux*y+fx*z, ry*x+uy*y+fy*z, rz*x+uz*y+fz*z, (_Ts)1 };
				/*<another> (get_matrix(x, y, z) * M) </another>*/
			}
		}
		static matrix_type get_matrix(const matrix_type& _Axis, vector3_type pos) {
			return get_matrix(_Axis, pos[0], pos[1], pos[2]);
		}
	};

	/*- - - - - - - - - - - - - - - - - - scaling - - - - - - - - - - - - - - -*/
	template<typename _Tm>
	struct scaling {};

	template<typename _Ts, typename _Tb, bool _Major>
	struct scaling< MATRIX3x3 > {
		using matrix_type = MATRIX3x3;

		static matrix_type get_matrix(_Ts x, _Ts y, _Ts z) {
			return matrix_type{ 
				  x,    (_Ts)0, (_Ts)0, 
				(_Ts)0,   y,    (_Ts)0,
				(_Ts)0, (_Ts)0,   z 
			};
		}
	};

	template<typename _Ts, typename _Tb, bool _Major>
	struct scaling< MATRIX4x4 > {
		using matrix_type = MATRIX4x4;

		static matrix_type get_matrix(_Ts x, _Ts y, _Ts z) {
			return matrix_type{
				  x,    (_Ts)0, (_Ts)0, (_Ts)0,
				(_Ts)0,   y,    (_Ts)0, (_Ts)0,
				(_Ts)0, (_Ts)0,   z,    (_Ts)0,
				(_Ts)0, (_Ts)0, (_Ts)0, (_Ts)1
			};
		}
		static matrix_type get_matrix(_Ts s) {
			return matrix_type{
				(_Ts)1, (_Ts)0, (_Ts)0, (_Ts)0,
				(_Ts)0, (_Ts)1, (_Ts)0, (_Ts)0,
				(_Ts)0, (_Ts)0, (_Ts)1, (_Ts)0,
				(_Ts)0, (_Ts)0, (_Ts)0, ((_Ts)1) / s
			};
		}
	};

	template<typename _Ts, typename _Tb, bool _Major>
	struct scaling< DIAG_MATRIX3x3 > {
		using matrix_type = DIAG_MATRIX3x3;

		static matrix_type get_matrix(_Ts x, _Ts y, _Ts z) {
			return matrix_type{ x, y, z };
		}
	};

	template<typename _Ts, typename _Tb, bool _Major>
	struct scaling< DIAG_MATRIX4x4 > {
		using matrix_type = DIAG_MATRIX4x4;

		static matrix_type get_matrix(_Ts x, _Ts y, _Ts z) {
			return matrix_type{ x, y, z, (_Ts)1 };
		}
		static matrix_type get_matrix(_Ts s) {
			return matrix_type{ (_Ts)1, (_Ts)1, (_Ts)1, ((_Ts)1) / s };
		}
	};

	/*- - - - - - - - - - - - - - - - - - rigid_body_transform - - - - - - - - - - - - - - -*/
	template<typename _Tm>
	struct rigid_body_transform {};

	template<typename _Ts, typename _Tb, bool _Major>
	struct rigid_body_transform< MATRIX4x4 > {
		using matrix_type = MATRIX4x4;
		using translation = clmagic::translation< matrix_type >;
		using rotation    = clmagic::rotation< matrix_type >;
		
		using vector3_type      = vector3<_Ts, _Tb>;
		using unit_vector3_type = unit_vector3<_Ts, _Tb>;

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
		static matrix_type inverse(unit_vector3_type r, unit_vector3_type u, unit_vector3_type f, vector3_type t) {
			const vector3_type neg_t = -t;

			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					  r[0],   r[1],   r[2], dot(neg_t, r),
					  u[0],   u[1],   u[2], dot(neg_t, u),
					  f[0],   f[1],   f[2], dot(neg_t, f),
					(_Ts)0, (_Ts)0, (_Ts)0,    (_Ts)1 };
			} else {
				return matrix_type{
					    r[0],         u[0],        f[0],      (_Ts)0,
					    r[1],         u[1],        f[1],      (_Ts)0,
					    r[2],         u[2],        f[2],      (_Ts)0,
					dot(neg_t,r), dot(neg_t,u), dot(neg_t,f), (_Ts)1 };
			}
		}
		static matrix_type inverse(const matrix_type& M) {
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				const auto r = unit_vector3_type({ M.at(0,0), M.at(1,0), M.at(2,0) }, true);
				const auto u = unit_vector3_type({ M.at(0,1), M.at(1,1), M.at(2,1) }, true);
				const auto f = unit_vector3_type({ M.at(0,2), M.at(1,2), M.at(2,2) }, true);
				const auto t =       vector3_type{ M.at(0,3), M.at(1,3), M.at(2,3) };
				return inverse(r, u, f, t);
			} else {
				const auto r = unit_vector3_type({ M.at(0,0), M.at(0,1), M.at(0,2) }, true);
				const auto u = unit_vector3_type({ M.at(1,0), M.at(1,1), M.at(1,2) }, true);
				const auto f = unit_vector3_type({ M.at(2,0), M.at(2,1), M.at(2,2) }, true);
				const auto t =       vector3_type{ M.at(3,0), M.at(3,1), M.at(3,2) };
				return inverse(r, u, f, t);
			}
		}
	};

	/*- - - - - - - - - - - - - - - - - - look_at - - - - - - - - - - - - - - -*/
	template<typename _Tm, int _Cd = Coordinates::_LH_>
	struct look_at {};

	template<typename _Ts, typename _Tb, bool _Major>
	struct look_at< MATRIX4x4, Coordinates::_LH_ > {
		using matrix_type       = MATRIX4x4;
		using vector3_type      = vector3<_Ts, _Tb>;
		using unit_vector3_type = unit_vector3<_Ts, _Tb>;

		static matrix_type get_matrix(vector3_type Peye, unit_vector3_type f, unit_vector3_type u) {
			/* decompose M = QR, col-major-order
			invser(M) = inverse(Q*R)
			          = inverse(R) * inverse(Q)
					              [r u f 0]    [1 0 0 -x]
						          [r u f 0]    [0 1 0 -y]
					  = transpose([r u f 0]) * [0 0 1 -z]
					              [0 0 0 1]    [0 0 0  1]
			From Book<<3D-Game-Programming-With-DirectX12>>
			*/
			auto r = unit_vector3_type(cross3(u, f));
			     u = unit_vector3_type(cross3(f, r), true);
			return rigid_body_transform<matrix_type>::inverse(r, u, f, Peye);
		}
	};

	template<typename _Ts, typename _Tb, bool _Major>
	struct look_at< MATRIX4x4, Coordinates::_RH_ > {
		using matrix_type       = MATRIX4x4;
		using vector3_type      = vector3<_Ts, _Tb>;
		using unit_vector3_type = unit_vector3<_Ts, _Tb>;

		static matrix_type get_matrix(vector3_type Peye, unit_vector3_type f, unit_vector3_type u) {
			return look_at<matrix_type, Coordinates::_LH_>::get_matrix(Peye, -f, u);
		}
	};

}// namespace clmagic

#endif