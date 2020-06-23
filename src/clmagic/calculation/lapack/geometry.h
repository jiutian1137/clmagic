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

	enum coordinate_system{
		LEFT_HAND,
		RIGHT_HAND,
		DEFAULT_HAND = LEFT_HAND
	};

	/*- - - - - - - - - - - - - - - - - - perspective - - - - - - - - - - - - - - -*/
	template<typename _Ts, typename _Tb = _SIMD4_t<_Ts>, matrix_major _Major = DEFAULT_MAJOR>
	MATRIX4x4 perspectiveLH(RADIANS fov, SCALAR r, SCALAR n, SCALAR f) {
		const auto d  = 1 / tan(static_cast<SCALAR>(fov) / 2);// cot(fov/2)
		const auto fn = f / (f - n);
		/* A*z+B = (z-n)*f/(f-n)
				 = z*(f/(f-n)]) + -n*(f/(f-n))
		*/
		
		if _CONSTEXPR_IF( MATRIX4x4::col_major() ) {
			return MATRIX4x4{
				d/r, 0,   0,   0,
				0,   d,   0,   0,
				0,   0,   fn, -n*fn,
				0,   0,   1,   0 };
		} else {
			return MATRIX4x4{
				d/r, 0,   0,   0,
				0,   d,   0,   0,
				0,   0,   fn,  1,
				0,   0, -n*fn, 0 };
		}
	}

	template<typename _Ts, typename _Tb = _SIMD4_t<_Ts>, matrix_major _Major = DEFAULT_MAJOR>
	MATRIX4x4 perspectiveLH(SCALAR w, SCALAR h, SCALAR n, SCALAR f) {
		/* h/2 / n = tan(fov/2)
			=> h/(2n)  = tan(fov/2)
			=> 2n/h    = d

			w/h=aspect
			=> w=aspect*h
			d/aspect = (2n/h) / aspect
			=> d/aspect = 2n/(h*aspect)
			=> d/aspect = 2n / w
		*/
		const auto n_m2 = 2 * n;
		const auto fn   = f / (f - n);
		
		if _CONSTEXPR_IF( MATRIX4x4::col_major() ) {
			return MATRIX4x4{
				n_m2/w,    0,    0,   0,
				   0,   n_m2/h,  0,   0,
				   0,      0,   fn, -n/fn,
				   0,      0,    1,   0 };
		} else {
			return MATRIX4x4{
				n_m2/w,    0,     0,   0,
				   0,   n_m2/h,   0,   0,
				   0,      0,    fn,   1,
				   0,      0,   -n/fn, 0 };
		}
	}

	template<typename _Ts, typename _Tb, matrix_major _Major>
	SCALAR perspectiveLH_Znaer(MATRIX4x4 M) {
		/*<solve-col_major>
			M[2][3] = -n * M[2][2]
			n       = -M[2][3] / M[2][2]
		</solve-col_major>*/
		if _CONSTEXPR_IF( MATRIX4x4::col_major() ) {
			return -M.at(2, 3) / M.at(2, 2);
		} else {
			return -M.at(3, 2) / M.at(2, 2);
		}
	}

	template<typename _Ts, typename _Tb, matrix_major _Major>
	SCALAR perspectiveLH_Zfar(MATRIX4x4 M, SCALAR& n) {
		/*<solve>
			 M[2][2] = fn = f/(f-n)
			    M22*(f-n) = f
			  M22*f-M22*n = f
			       -M22*n = f - M22*f
				   -M22*n = f(1 - M22)
			-M22*n/(1-M22)= f
		</solve>*/
		SCALAR M22 = M.at(2, 2);
		n = perspectiveLH_Znaer(M);
		return ( -M22*n/(1-M22) );
	}

	template<typename _Ts, typename _Tb = _SIMD4_t<_Ts>, matrix_major _Major = DEFAULT_MAJOR>
	MATRIX4x4 perspectiveRH(RADIANS fov, SCALAR r, SCALAR n, SCALAR f) {
		auto d  = 1 / tan( static_cast<SCALAR>(fov)/2 );// cot(fov/2)
		auto fn = f / (n - f);
		
		if _CONSTEXPR_IF( MATRIX4x4::col_major() ) {
			return MATRIX4x4{
				d/r, 0,   0,   0,
				0,   d,   0,   0,
				0,   0,  fn,  n*fn,
				0,   0,  -1,   0 };
		} else {
			return MATRIX4x4{
				d/r, 0,   0,   0,
				0,   d,   0,   0,
				0,   0,   fn, -1,
				0,   0, n*fn,  0 };
		}
	}

	template<typename _Ts, typename _Tb = _SIMD4_t<_Ts>, matrix_major _Major = DEFAULT_MAJOR>
	MATRIX4x4 perspectiveRH(SCALAR w, SCALAR h, SCALAR n, SCALAR f) {
		const auto n_m2 = 2 * n;
		const auto fn   = f / (n - f);
	
		if _CONSTEXPR_IF( MATRIX4x4::col_major() ) {
			return MATRIX4x4{
				n_m2/w,    0,    0,   0,
				   0,   n_m2/h,  0,   0,
				   0,      0,   fn, n*fn,
				   0,      0,   -1,   0 };
		} else {
			return MATRIX4x4{
				n_m2/w,    0,     0,   0,
				   0,   n_m2/h,   0,   0,
				   0,      0,     fn, -1,
				   0,      0,   n*fn,  0 };
		}
	}

	template<typename _Ts, typename _Tb, matrix_major _Major>
	SCALAR perspectiveRH_Znaer(MATRIX4x4 M) {
		/*<solve-col_major>
			M[2][3] = n * M[2][2]
			n       = M[2][3] / M[2][2]
		</solve-col_major>*/
		if _CONSTEXPR_IF( MATRIX4x4::col_major() ) {
			return M.at(2, 3) / M.at(2, 2);
		} else {
			return M.at(3, 2) / M.at(2, 2);
		}
	}

	template<typename _Ts, typename _Tb, matrix_major _Major>
	SCALAR perspectiveRH_Zfar(MATRIX4x4 M, SCALAR& n) {
		/*<solve>
			 M[2][2] = fn = f/(n-f)
			    M22*(n-f) = f
			  M22*n-M22*f = f
			        M22*n = f + M22*f
				    M22*n = f(1 + M22)
			 M22*n/(1+M22)= f
		</solve>*/
		SCALAR M22 = M.at(2, 2);
		n = perspectiveRH_Znaer(M);
		return ( M22*n/(1+M22) );
	}

	template<typename _Ts, typename _Tb = _SIMD4_t<_Ts>, matrix_major _Major = DEFAULT_MAJOR, coordinate_system _Cs = DEFAULT_HAND>
	MATRIX4x4 perspective(RADIANS fov, SCALAR r, SCALAR n, SCALAR f) {
		/* 
		@_fov: virtical field of angle
		@_r: aspect, width/height 16/9
		@_n: near-plane Z, must grater 0.000002
		@_f: far-plane Z
		*/
		if _CONSTEXPR_IF(_Cs == LEFT_HAND){
			return perspectiveLH<_Ts, _Tb, _Major>(fov, r, n, f);
		} else {
			return perspectiveRH<_Ts, _Tb, _Major>(fov, r, n, f);
		}
	}

	template<typename _Ts, typename _Tb = _SIMD4_t<_Ts>, matrix_major _Major = DEFAULT_MAJOR, coordinate_system _Cs = DEFAULT_HAND>
	MATRIX4x4 perspective(SCALAR w, SCALAR h, SCALAR n, SCALAR f) {
		if _CONSTEXPR_IF(_Cs == LEFT_HAND) {
			return perspectiveLH<_Ts, _Tb, _Major>(w, h, n, f);
		} else {
			return perspectiveRH<_Ts, _Tb, _Major>(w, h, n, f);
		}
	}

	template<typename _Ts, typename _Tb, matrix_major _Major>
	RADIANS perspective_fov(MATRIX4x4 M) {
		/*<solve>
			M[1][1] = 1/tan(fov/2)
			1/M11   = tan(fov/2)
			fov     = atan(1/M11)*2
		</solve>*/
		const auto M11 = M.at(1, 1);
		return RADIANS( atan(1 / M11) * 2 );
	}

	template<typename _Ts, typename _Tb, matrix_major _Major>
	SCALAR perspective_aspect(MATRIX4x4 M) {
		/*<solve>
			M[1][1]/aspect = M[0][0]
			M11 / M00      = aspect
		</solve>*/
		return M.at(1, 1) / M.at(0, 0);
	}

	/*- - - - - - - - - - - - - - - - - - planar_projection - - - - - - - - - - - - - - -*/
	template<typename _Ts, typename _Tb = _Ts, matrix_major _Major = DEFAULT_MAJOR>
	struct planar_projection {
		// convert V to P in the Plane
		using matrix_type       = matrix4x4<_Ts, _Tb, _Major>;
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

	/*- - - - - - - - - - - - - - - - - - translation - - - - - - - - - - - - - - -*/
	template<typename _Ts, typename _Tb = _SIMD4_t<_Ts>, matrix_major _Major = DEFAULT_MAJOR>
	MATRIX4x4 translation(SCALAR x, SCALAR y, SCALAR z) {
		if _CONSTEXPR_IF( MATRIX4x4::col_major() ) {
			return MATRIX4x4{
				(_Ts)1, (_Ts)0, (_Ts)0,     x,
				(_Ts)0, (_Ts)1, (_Ts)0,     y,
				(_Ts)0, (_Ts)0, (_Ts)1,     z, 
				(_Ts)0, (_Ts)0, (_Ts)0, (_Ts)1 };
		} else {
			return MATRIX4x4{
				(_Ts)1, (_Ts)0, (_Ts)0, (_Ts)0,
				(_Ts)0, (_Ts)1, (_Ts)0, (_Ts)0,
				(_Ts)0, (_Ts)0, (_Ts)1, (_Ts)0,
					 x,      y,      z, (_Ts)1 };
		}
	}
	template<typename _Ts, typename _Tb, matrix_major _Major>
	MATRIX4x4 translation(MATRIX4x4 axis, SCALAR x, SCALAR y, SCALAR z) {
		/*
		<idea>
			axis * translation(x,y,z,1) = translation(x1,y1,z1,1)
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
		if _CONSTEXPR_IF( MATRIX4x4::col_major() ) {
			const auto rx = axis.ref(0), ry = axis.ref(4), rz = axis.ref(8);// col(0)
			const auto ux = axis.ref(1), uy = axis.ref(5), uz = axis.ref(9);// col(1)
			const auto fx = axis.ref(2), fy = axis.ref(6), fz = axis.ref(10);// col(2)
			return MATRIX4x4{
					rx,     ux,     fx,   rx*x+ux*y+fx*z,
					ry,     uy,     fy,   ry*x+uy*y+fy*z,
					rz,     uz,     fz,   rz*x+uz*y+fz*z,
				(_Ts)0, (_Ts)0, (_Ts)0,           (_Ts)1 };
		} else {
			const auto rx = axis.ref(0), ry = axis.ref(1), rz = axis.ref(2);// row(0)
			const auto ux = axis.ref(4), uy = axis.ref(5), uz = axis.ref(6);// row(1)
			const auto fx = axis.ref(8), fy = axis.ref(9), fz = axis.ref(10);// row(2)
			return MATRIX4x4{
					  rx,             ry,             rz,       (_Ts)0,
					  ux,             uy,             uz,       (_Ts)0,
					  fx,             fy,             fz,       (_Ts)0,
				rx*x+ux*y+fx*z, ry*x+uy*y+fy*z, rz*x+uz*y+fz*z, (_Ts)1 };
		}
	}
	template<typename _Ts, typename _Tb, matrix_major _Major>
	VECTOR3 translation_vector(MATRIX4x4 _Tm) {
		if _CONSTEXPR_IF( MATRIX4x4::col_major() ) {
			return VECTOR3{ _Tm.ref(3), _Tm.ref(3+4), _Tm.ref(3+4+4) };
		} else {
			return VECTOR3{ _Tm.ref(8), _Tm.ref(8+1), _Tm.ref(8+2) };
		}
	}
	
	template<typename _Ts, typename _Tb, matrix_major _Major>
	MATRIX4x4 translate(MATRIX4x4 M, SCALAR x, SCALAR y, SCALAR z) {
		if _CONSTEXPR_IF( MATRIX4x4::col_major() ) {
			M.ref(3) += x; 
			M.ref(3+4) += y; 
			M.ref(3+4+4) += z;
		} else {
			M.ref(8) += x;
			M.ref(8 + 1) += y;
			M.ref(3 + 2) += z;
		}
		return M;
	}
	template<typename _Ts, typename _Tb, matrix_major _Major>
	MATRIX4x4 translate_local(MATRIX4x4 M, SCALAR x, SCALAR y, SCALAR z) {
		MATRIX4x4 Tm = translation(x, y, z);
		return Tm(M);
	}

	/*- - - - - - - - - - - - - - - - - - scaling - - - - - - - - - - - - - - -*/
	template<typename _Ts, typename _Tb = _SIMD4_t<_Ts>, matrix_major _Major = DEFAULT_MAJOR>
	DIAG_MATRIX4x4 scaling(SCALAR sx, SCALAR sy, SCALAR sz) {
		return DIAG_MATRIX4x4{ sx, sy, sz, static_cast<SCALAR>(1) };
	}
	template<typename _Ts, typename _Tb = _SIMD4_t<_Ts>, matrix_major _Major = DEFAULT_MAJOR>
	DIAG_MATRIX4x4 scaling(SCALAR s) {
		return DIAG_MATRIX4x4{ static_cast<SCALAR>(1), static_cast<SCALAR>(1), static_cast<SCALAR>(1), s };
	}
	template<typename _Ts, typename _Tb = _SIMD4_t<_Ts>, matrix_major _Major = DEFAULT_MAJOR>
	DIAG_MATRIX3x3 scaling3x3(SCALAR sx, SCALAR sy, SCALAR sz) {
		return DIAG_MATRIX3x3{ sx, sy, sz };
	}

	template<typename _Ts, typename _Tb, matrix_major _Major>
	MATRIX4x4 scale(MATRIX4x4 M, SCALAR sx, SCALAR sy, SCALAR sz) {
		return scaling(sx, sy, sz) * M;// diag_matrix left multiples same of the right multiples  
	}

	/*- - - - - - - - - - - - - - - - - - rotation - - - - - - - - - - - - - - -*/
	using Euler::yaw;
	using Euler::yaw3x3;
	using Euler::pitch;
	using Euler::pitch3x3;
	using Euler::roll;
	using Euler::roll3x3;
	using Rodrigues::rotation;
	using Rodrigues::rotation_angle;
	using Rodrigues::rotation_axis;
	using Rodrigues::rotation3x3;
	using Rodrigues::rotation3x3_angle;
	using Rodrigues::rotation3x3_axis;
	using WilliamRowanHamilton::quaternion;
	using WilliamRowanHamilton::polar;

	/*<Reference>RealTimeRendering-4th-Edition.</Reference>*/
	template<typename _Ts, typename _Tb, matrix_major _Major = DEFAULT_MAJOR>
	MATRIX4x4 rotation(QUATERNION q) {
		const auto qx = q.imag()[0];
		const auto qy = q.imag()[1];
		const auto qz = q.imag()[2];
		const auto qw = q.real();
		const auto s  = 2/(qx*qx + qy*qy + qz*qz + qw*qw);
			
		if _CONSTEXPR_IF( MATRIX4x4::col_major() ) {
			return MATRIX4x4{
				1-s*(qy*qy+qz*qz),   s*(qx*qy-qw*qz),   s*(qx*qz+qw*qy), 0,
				  s*(qx*qy+qw*qz), 1-s*(qx*qx+qz*qz),   s*(qy*qz-qw*qx), 0,
				  s*(qx*qz-qw*qy),	 s*(qy*qz+qw*qx), 1-s*(qx*qx+qy*qy), 0,
				        0,                 0,                 0,         1 };
		} else {
			return MATRIX4x4{
				1-s*(qy*qy+qz*qz),   s*(qx*qy+qw*qz),   s*(qx*qz-qw*qy), 0,
				  s*(qx*qy-qw*qz), 1-s*(qx*qx+qz*qz),   s*(qy*qz+qw*qx), 0,
				  s*(qx*qz+qw*qy),   s*(qy*qz-qw*qx), 1-s*(qx*qx+qy*qy), 0,
				        0,                 0,                 0,         1 };
		}
	}
	template<typename _Ts, typename _Tb, matrix_major _Major = DEFAULT_MAJOR>
	MATRIX3x3 rotation3x3(QUATERNION q) {
		const auto qx = q.imag()[0];
		const auto qy = q.imag()[1];
		const auto qz = q.imag()[2];
		const auto qw = q.real();
		const auto s  = 2/(qx*qx + qy*qy + qz*qz + qw*qw);
			
		if _CONSTEXPR_IF( MATRIX3x3::col_major() ) {
			return MATRIX3x3{
				1-s*(qy*qy+qz*qz),   s*(qx*qy-qw*qz),   s*(qx*qz+qw*qy),
				  s*(qx*qy+qw*qz), 1-s*(qx*qx+qz*qz),   s*(qy*qz-qw*qx),
				  s*(qx*qz-qw*qy),	 s*(qy*qz+qw*qx), 1-s*(qx*qx+qy*qy) };
		} else {
			return MATRIX3x3{
				1-s*(qy*qy+qz*qz),   s*(qx*qy+qw*qz),   s*(qx*qz-qw*qy),
				  s*(qx*qy-qw*qz), 1-s*(qx*qx+qz*qz),   s*(qy*qz+qw*qx),
				  s*(qx*qz+qw*qy),   s*(qy*qz-qw*qx), 1-s*(qx*qx+qy*qy) };
		}
	}

	template<typename _Tq, typename _Tm, typename _Ts>
	_Tq _Matrix_to_quaternion(SCALAR qw, _Tm M) {
		/*
		s = 2/pow(norm(q),2)

		tr(M) = 4-2s(qx*qx+qy*qy+qz*qz), 
			    = 4-4*(qx*qx+qy*qy+qz*qz)/pow(norm(q),2)
				= 4 * (1 - (qx*qx+qy*qy+qz*qz)/pow(norm(q),2))
				= 4 * (1 - (qx*qx+qy*qy+qz*qz)/(qx*qx+qy*qy+qz*qz+qw*qw))
				= 4 * ((qw*qw)/(qx*qx+qy*qy+qz*qz+qw*qw))
				= 4*qw*qw / pow(norm(q),2)
		4*qw*qw = tr(M)
		qw = sqrt( tr(M)/4 ) <------------

		M[2,0]-M[0,2] = s*(qx*qz+qw*qy)-s*(qx*qz-qw*qy)
			            = s*qx*qz + s*qw*qy - s*qx*qz + s*qw*qy
					    = 2*s*qw*qy
		qy = (M[2,0]-M[0,2])/(2*s*qw)
		   = (M[2,0]-M[0,2])/(4/pow(norm(q),2)*qw)
		qy = (M[2,0]-M[0,2])/(4*qw) <------------

		M[0,1]-M[1,0] = s*(qx*qy+qw*qz) - s*(qx*qy-qw*qz)
			            = s*qx*qy + s*qw*qz - s*qx*qy + s*qw*qz
						= 2*s*qw*qz
		qz = (M[0,1]-M[1,0])/(2*s*qw)
		   = (M[0,1]-M[1,0])/(4/pow(norm(q),2)*qw)
		qz = (M[0,1]-M[1,0])/(4*qw) <------------

		M[1,2]-M[2,1] = s*(qy*qz+qw*qx)-s*(qy*qz-qw*qx)
			            = s*qy*qz + s*qw*qx - s*qy*qz + s*qw*qx
						= 2*s*qw*qx
		qx = (M[1,2]-M[2,1]) / (2*s*qw)
		   = (M[1,2]-M[2,1]) / (4*qw) <------------
		*/
		const auto qw_mul4 = qw * 4;

		if _CONSTEXPR_IF(_Tm::col_major() ) {
			const auto qx = (M.at(2,1) - M.at(1,2)) / qw_mul4;
			const auto qy = (M.at(0,2) - M.at(2,0)) / qw_mul4;
			const auto qz = (M.at(1,0) - M.at(0,1)) / qw_mul4;
			return _Tq(qw, qx, qy, qz);
		} else {
			const auto qx = (M.at(1,2) - M.at(2,1)) / qw_mul4;
			const auto qy = (M.at(2,0) - M.at(0,2)) / qw_mul4;
			const auto qz = (M.at(0,1) - M.at(1,0)) / qw_mul4;
			return _Tq(qw, qx, qy, qz);
		}
	}
	template<typename _Tq, typename _Tm, typename _Ts>
	_Tq _Matrix_to_quaternion_0qw(SCALAR qw, _Tm M) {
		const auto M00 = M.at(0, 0);
		const auto M11 = M.at(1, 1);
		const auto M22 = M.at(2, 2);
		const auto M33 = static_cast<SCALAR>(1);
		const auto qx  = clmagic::sqrt((+M00 - M11 - M22 + M33) / 4);
		const auto qy  = clmagic::sqrt((-M00 + M11 - M22 + M33) / 4);
		const auto qz  = clmagic::sqrt((-M00 - M11 + M22 + M33) / 4);
		return _Tq(qw, qx, qy, qz);
	}

	template<typename _Ts, typename _Tb, matrix_major _Major>
	QUATERNION rotation_quaternion(const MATRIX4x4& M) {
		const auto trM = M.at(0,0) + M.at(1,1) + M.at(2,2) + M.at(3,3);
		const auto qw  = clmagic::sqrt(trM / 4);
		if (qw > std::numeric_limits<_Ts>::epsilon()) {
			return _Matrix_to_quaternion<QUATERNION>(qw, M);
		} else {
			return _Matrix_to_quaternion_0qw<QUATERNION>(qw, M);
		}
	}
	template<typename _Ts, typename _Tb, matrix_major _Major>
	QUATERNION rotation3x3_quaternion(const MATRIX4x4& M) {
		const auto trM = M.at(0,0) + M.at(1,1) + M.at(2,2) + 1;
		const auto qw  = clmagic::sqrt(trM / 4);
		if (qw > std::numeric_limits<_Ts>::epsilon()) {
			return _Matrix_to_quaternion<QUATERNION>(qw, M);
		} else {
			return _Matrix_to_quaternion_0qw<QUATERNION>(qw, M);
		}
	}

	template<typename _Ts, typename _Tb, matrix_major _Major>
	MATRIX4x4 rotate(MATRIX4x4 M, UNIT_VECTOR3 axis, RADIANS angle) {
		MATRIX4x4 Rm = rotation(axis, angle);
		return Rm(M);
	}


	/*- - - - - - - - - - - - - - - - - - rigid_body_transform - - - - - - - - - - - - - - -*/
	template<typename _Tm>
	struct rigid_body_transform {};

	template<typename _Ts, typename _Tb, matrix_major _Major>
	struct rigid_body_transform< MATRIX4x4 > {
		using matrix_type = MATRIX4x4;
		
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
	template<typename _Ts, typename _Tb, matrix_major _Major = DEFAULT_MAJOR, coordinate_system _Cs = DEFAULT_HAND>
	MATRIX4x4 look_to(VECTOR3 Peye, UNIT_VECTOR3 f_vector, UNIT_VECTOR3 u_vector) {
		if _CONSTEXPR_IF(_Cs == LEFT_HAND) {
			auto r_vector = UNIT_VECTOR3(cross3(u_vector, f_vector));
			     u_vector = UNIT_VECTOR3(cross3(f_vector, r_vector));
			return rigid_body_transform<MATRIX4x4>::inverse(r_vector, u_vector, f_vector, Peye);
		} else {
			auto r_vector = UNIT_VECTOR3(cross3(f_vector, u_vector));
				 u_vector = UNIT_VECTOR3(cross3(r_vector, f_vector));
			return rigid_body_transform<MATRIX4x4>::inverse(r_vector, u_vector, f_vector, Peye);
		}
	}
	template<typename _Ts, typename _Tb, matrix_major _Major = DEFAULT_MAJOR, coordinate_system _Cs = DEFAULT_HAND>
	MATRIX4x4 look_at(VECTOR3 Peye, VECTOR3 Ptarget, UNIT_VECTOR3 u_vector) {
		return look_to<_Ts, _Tb, _Major, _Cs>(Peye, UNIT_VECTOR3(Ptarget - Peye), u_vector);
	}
	
	template<typename _Ts, typename _Tb, matrix_major _Major = DEFAULT_MAJOR, coordinate_system _Cs = DEFAULT_HAND>
	MATRIX4x4 look_to(VECTOR3 Peye, UNIT_VECTOR3 _Target_vector, UNIT_VECTOR3& r_vector, UNIT_VECTOR3& u_vector, UNIT_VECTOR3& f_vector) {
		if _CONSTEXPR_IF(_Cs == LEFT_HAND) {
			f_vector = _Target_vector;
			r_vector = UNIT_VECTOR3(cross3(u_vector, f_vector));
			u_vector = UNIT_VECTOR3(cross3(f_vector, r_vector));
			return rigid_body_transform<MATRIX4x4>::inverse(r_vector, u_vector, f_vector, Peye);
		} else {
			f_vector = _Target_vector;
			r_vector = UNIT_VECTOR3(cross3(f_vector, u_vector));
			u_vector = UNIT_VECTOR3(cross3(r_vector, f_vector));
			return rigid_body_transform<MATRIX4x4>::inverse(r_vector, u_vector, f_vector, Peye);
		}
	}
	template<typename _Ts, typename _Tb, matrix_major _Major = DEFAULT_MAJOR, coordinate_system _Cs = DEFAULT_HAND>
	MATRIX4x4 look_at(VECTOR3 Peye, VECTOR3 Ptarget, UNIT_VECTOR3& r_vector, UNIT_VECTOR3& u_vector, UNIT_VECTOR3& f_vector) {
		return look_to<_Ts, _Tb, _Major, _Cs>(Peye, UNIT_VECTOR3(Ptarget - Peye), std::ref(r_vector), std::ref(u_vector), std::ref(f_vector));
	}

	/*- - - - - - - - - - - - - - - - - - transform - - - - - - - - - - - - - - -*/
	/*template<typename _Ts>
	struct linear_momentum {
		using scalar_type = _Ts;

		template<typename _Ts2, typename _Tb2, typename _Major2>
		matrix4x4<_Ts2, _Tb2, _Major2> to_translation() const {
			return translation<_Ts2, _Tb2, _Major2>(this->x, this->y, this->z);
		}

		template<typename _Ts2, size_t _Size2, typename _Tb2>
		vector<_Ts2, _Size2, _Tb2> to_vector() const {
			return vector<_Ts2, _Size2, _Tb2>{ this->x, this->y, this->z };
		}

		scalar_type& operator[](size_t _Pos) { 
			return *((&x) + _Pos);
		}
		scalar_type operator[](size_t _Pos) const { 
			return *((&x) + _Pos);
		}

		scalar_type x;
		scalar_type y;
		scalar_type z;
	};
	template<typename _Ts>
	struct angular_momentum {
		translator() = default;
		translator(_Ts _Tx, _Ts _Ty, _Ts _Tz) : _My_data{ _Tx, _Ty, _Tz } {}

		template<typename _Ts2, typename _Tb2, matrix_major _Major2>
		matrix4x4<_Ts2, _Tb2, _Major2> translate(const matrix4x4<_Ts2, _Tb2, _Major2>& _Right) {
			auto _Transform = translation<_Ts2, _Tb2, _Major2>(_My_data[0], _My_data[1], _My_data[2]);
			return _Transform(_Right);
		}

		template<typename _Ts2, size_t _Size2, typename _Tb2>
		vector<_Ts2, _Size2, _Tb2> translate(vector<_Ts2, _Size2, _Tb2> _Right) {
			auto _Displacement = vector<_Ts2, _Size2, _Tb2>(0);
			_Displacement[0] = _My_data[0];
			_Displacement[1] = _My_data[1];
			_Displacement[2] = _My_data[2];
			return _Right + _Displacement;
		}

		_Ts _My_data[3];
	};*/

}// namespace clmagic

#endif