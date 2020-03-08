﻿#pragma once
#ifndef clmagic_math_LAPACK_h_
#define clmagic_math_LAPACK_h_
#include "lapack/matrix.h"
#include "general/unit.h"
//#include "complex.h"

namespace clmagic {
	/*
	c: complex (std::complex float[2] double[2])
	s: real	   (int float double)
	v: vector  (int* float* double*)
	m: matrix  (int** float** double**)
	*/
	template<typename _Ty>
	void matrix_mul(const _Ty* _SrcMxN, const _Ty* _SrcNxP, size_t M, size_t N, size_t P, /*out*/_Ty* _DstMxP );

	template<typename _Ty>
	int Lu(/*inout*/_Ty* pA, size_t M, size_t N, _Ty* pAinv);
	
	

	template<typename T>
	T determinant(T* _Pmat, size_t _M) {
		/* ∑(0,!M): (-1)^t * D[0][s₀] * D[1][s₁] * D[2][s₂]... * D[M-1][s(m-₁)] */
		auto D = [_Pmat, _M](size_t _Row, size_t _Col) {
			return (_Pmat[_Row * _M + _Col]);
		};

		if (_M == 2) {
			return (D(0, 0) * D(1, 1) - D(0, 1) * D(1, 0));
		}

		auto sigma = T(0);
		auto S = determinant_seque(set_number<determinant_seque::number_type>(0, _M));
		assert(S.size() == factorial(_M));
		for (size_t i = 0; i != S.size(); ++i) {
			const auto& s = S[i].first;
			const auto& t = S[i].second;
			auto      rou = T(1);
			for (size_t j = 0; j != s.size(); ++j) {
				rou *= D(j, s[j]);// high light
			}
			sigma += T(std::pow(-1, t)) * rou;
		}
		return (sigma);
	}
	


	

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */



/*
OpenVINO-serial number: CZL3-BH28FBG4
*/



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

//#define ortho_matrixN ortho_matrix<T, N, _Major, V>
//
//	template<typename T, size_t N, bool _Major = true, size_t V = vec_t>
//	struct ortho_matrix : public matrix<T, N, N, _Major, V> {
//		using _Mybase = matrix<T, N, N, _Major, V>;
//
//	public:
//		void assign(std::initializer_list<vector<T, N, V>> _IVlist);
//
//		ortho_matrix() = default;
//
//		ortho_matrix(std::initializer_list<vector<T, N, V>> _IVlist);
//
//		void inv();
//	};
//
//	template<typename T, size_t N, bool _Major, size_t V>
//		ortho_matrixN inverse(const ortho_matrixN& _Matrix);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
	template<typename _Matx>
	struct reference_rotation {
		using scalar_type = typename _Matx::scalar_type;
		using T           = scalar_type;


		reference_rotation(_Matx& _Matrix) : _Myref(_Matrix) { }

		radians<T> angle() const {
			return ( (_Myref[0][0] + _Myref[1][1] + _Myref[2][2] - (T)1)
				    /*--------------------------------------------------*/
					                  / (T)2 );
			/*
			θ = acos( (trace(R)-1) / 2 )
			   = acos( [c+(1-c)x² + c+(1-c)y² + c+(1-c)z² - 1] / 2 )
			   = acos( [3*c + (1-c)(x²+y²+z²) - 1] / 2 )
			   = acos( [3*c + (1-c)*1 - 1] / 2 )
			   = acos( [2*c + 1 - 1] / 2 )
			   = acos( c )
			*/
		}

		unit_vector3<T> axis() const {
			if _CONSTEXPR_IF(_Myref.col_major()) {
				return ( vector3<T>(_Myref[1][2] - _Myref[2][1], 
					                _Myref[2][0] - _Myref[0][2],
					                _Myref[0][1] - _Myref[2][0])
					   /*-------------------------------------------------------------*/
							  / ( (T)2 * sin((T)angle()) ) );
			} else {

			}

			/*
					  1       [j[2] - k[1]]
			axis = -------- * [k[0] - i[2]]
				   2sin(θ)   [i[1] - j[0]]
					  1       [ [(1-c)yz+sx] - [(1-c)yz-sx] ]
				 = -------- * [ [(1-c)xz+sy] - [(1-c)xz-sy] ]
				   2sin(θ)   [ [(1-c)xy+sz] - [(1-c)xy-sz] ]
			 		  1       [ 2*sx ]
				 = -------- * [ 2*sy ]
				   2sin(θ)   [ 2*sz ]

				   2*sin(θ)[x y z]
				 = --------------
					 2sin(θ)
				 = [x y z]
			*/
		}
	private:
		_Matx& _Myref;
	};


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
		matrix3x3<_Ty, true> yaw(radians<_Ty> _Angle) {/* rotate X-Z */
			const auto _Zero = static_cast<_Ty>(0);
			const auto _One  = static_cast<_Ty>(1);
			const auto _Cos  = cos(static_cast<_Ty>(_Angle));
			const auto _Sin  = sin(static_cast<_Ty>(_Angle));
			return matrix3x3<_Ty, true>{
				 _Cos, -_Sin,  _Zero,
				 _Sin,  _Cos,  _Zero,
				_Zero,  _Zero, _One
			};
			/*
			[c -s 0]
			[s  c 0]
			[0  0 1]
			*/
		}

		template<typename _Ty> inline
		matrix3x3<_Ty, true> pitch(radians<_Ty> _Angle) {/* rotate Z-Y */
			const auto _Zero = static_cast<_Ty>(0);
			const auto _One  = static_cast<_Ty>(1);
			const auto _Cos  = cos(static_cast<_Ty>(_Angle));
			const auto _Sin  = sin(static_cast<_Ty>(_Angle));
			return matrix3x3<_Ty, true>{
				_One,  _Zero, _Zero,
				_Sin,  _Cos,  _Zero,
				_Cos, -_Sin,  _One
			};
			/*
			[1  0 0]
			[s  c 0]
			[c -s 1]
			*/
		}
	
		template<typename _Ty> inline
		matrix3x3<_Ty, true> roll(radians<_Ty> _Angle) {/* rotate X-Y */
			const auto _Zero = static_cast<_Ty>(0);
			const auto _One = static_cast<_Ty>(1);
			const auto _Cos = cos(static_cast<_Ty>(_Angle));
			const auto _Sin = sin(static_cast<_Ty>(_Angle));
			return matrix3x3<_Ty, true>{
				_Cos, -_Sin,  _Zero,
				_Sin,  _Cos,  _Zero,
				_Zero, _Zero, _One
			};
			/*
			[c -s 0]
			[s  c 0]
			[0  0 1]
			*/
		}
	}

	namespace Rodrigues {
		template<typename _Ty, typename _Vtag> inline
		matrix3x3<_Ty, true> rotation(unit_vector3<_Ty, _Vtag> axis, radians<_Ty> theta) {
			const auto c   = cos(static_cast<_Ty>(theta));
			const auto s   = sin(static_cast<_Ty>(theta));
			const auto x   = axis[0];
			const auto y   = axis[1];
			const auto z   = axis[2];
			const auto tmp = (static_cast<_Ty>(1) - c) * axis;// from GLM library

			return matrix3x3<_Ty, true>{
				c + tmp[0]*x,           tmp[0]*y - s*z,     tmp[0]*z + s*y,
				    tmp[1]*x + s*z, c + tmp[1]*y,           tmp[1]*z + s*x,
				    tmp[2]*x - s*y,     tmp[2]*y + s*x, c + tmp[2]*z
			};
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

			③:
						[c 0 0]
			cos(θ)*I = [0 c 0]
						[0 0 c]
								   [x]             [(1-c)x², (1-c)yx, (1-c)zx]
			(1-cos(θ))k*kT = (1-c)[y] * [x y z] = [(1-c)xy, (1-c)y², (1-c)zy]
								   [z]             [(1-c)xz, (1-c)yz, (1-c)z²]
					   [ 0  -sz  sy]
			sin(θ)K = [ sz  0  -sx]
					   [-sy  sx   0]

			④:
			θ = acos( (trace(R)-1) / 2 )
			   = acos( [c+(1-c)x² + c+(1-c)y² + c+(1-c)z² - 1] / 2 )
			   = acos( [3*c + (1-c)(x²+y²+z²) - 1] / 2 )
			   = acos( [3*c + (1-c)*1 - 1] / 2 )
			   = acos( [2*c + 1 - 1] / 2 )
			   = acos( c )
					  1       [j[2] - k[1]]
			axis = -------- * [k[0] - i[2]]
				   2sin(θ)   [i[1] - j[0]]
					  1       [ [(1-c)yz+sx] - [(1-c)yz-sx] ]
				 = -------- * [ [(1-c)xz+sy] - [(1-c)xz-sy] ]
				   2sin(θ)   [ [(1-c)xy+sz] - [(1-c)xy-sz] ]
					  1       [ 2*sx ]
				 = -------- * [ 2*sy ]
				   2sin(θ)   [ 2*sz ]

				   2*sin(θ)[x y z]
				 = --------------
					 2sin(θ)
				 = [x y z]

			⑤: to quaternion
				  Real             Imag
			Q = cos(θ/2) + [i,j,k][axis * sin(θ/2)]
			*/
		}
	}


	/*
	④:
	θ = acos( (trace(R)-1) / 2 )
	   = acos( [c+(1-c)x² + c+(1-c)y² + c+(1-c)z² - 1] / 2 )
	   = acos( [3*c + (1-c)(x²+y²+z²) - 1] / 2 )
	   = acos( [3*c + (1-c)*1 - 1] / 2 )
	   = acos( [2*c + 1 - 1] / 2 )
	   = acos( c )
			  1       [j[2] - k[1]]
	axis = -------- * [k[0] - i[2]]
		   2sin(θ)   [i[1] - j[0]]
			  1       [ [(1-c)yz+sx] - [(1-c)yz-sx] ]
		 = -------- * [ [(1-c)xz+sy] - [(1-c)xz-sy] ]
		   2sin(θ)   [ [(1-c)xy+sz] - [(1-c)xy-sz] ]
			  1       [ 2*sx ]
		 = -------- * [ 2*sy ]
		   2sin(θ)   [ 2*sz ]

			2*sin(θ)[x y z]
		 = ------------------
			  2sin(θ)
		 = [x y z]
	*/
	//template<typename _TyVec, size_t _Rows, size_t _Cols, bool _Major, size_t _Vec>
	//struct _Reference_translation {
	//	using scalar_type = typename _TyVec::scalar_type;
	//	using matrix_type = matrix<scalar_type, _Rows, _Cols, _Major, _Vec>;

	//	_Reference_translation(matrix_type& _Ref) : _Myref(_Ref) {}

	//private:
	//	matrix_type& _Myref;
	//
	//public:
	//	auto position() const ->decltype(_Myref.row(0)) {
	//		decltype(_Myref.row(0)) _Vector;
	//		if _CONSTEXPR_IF(_Myref.col_major()) {
	//			_Vector = _Myref.col(3);
	//		} else {
	//			_Vector = _Myref.row(3);
	//		}
	//		return _Vector;
	//	}

	//	_TyVec* _Vptr;
	//};

	/*
	@_Keyword: S=scale
	           R=rotate
			   T=translate
			   Q=quaternion
			   E=Euler(yaw,roll, pitch)

	@_Row-major:[   ivector   0 ]
				[   jvector   0 ]
				[   kvector   0 ]
				[ translation 1 ]
	@_Col-major:[ i   j   k   tr]
	            [ ve  ve  ve  an]
				[ c   c   c   s ]
				[ 0   0   0   1 ]
	*/
	//template<typename _Ty = real_t, bool _Major = true, typename _Vtag = normal_vector_tag>
	//class world_matrix : public matrix4x4<_Ty, _Major, _Vec> {
	//	static_assert(std::is_arithmetic_v<_Ty>, "in world_matrix<>");

	//	using _Mybase = matrix4x4<_Ty, _Major, _Vec>;

	//public:
	//	static world_matrix<_Ty, true, _Vec> scaling(_Ty _Sx, _Ty _Sy, _Ty _Sz);
	//	
	//	static world_matrix<_Ty, true, _Vec> translation(meters<_Ty> Tx, meters<_Ty> Ty, meters<_Ty> Tz);

	//	world_matrix();// default identity matrix
	//	world_matrix(std::initializer_list<_Ty> _Ilist);
	//	world_matrix(const vector3<_Ty, _Vtag>& S, const radians<vector3<_Ty, _Vec>>& E, const meters<vector3<_Ty, _Vec>>& T);
	//	world_matrix(const matrix<_Ty, 4, 4, !_Major, _Vtag>& _Invorder);

	//	reference_rotation<world_matrix> rotation() {
	//		return reference_rotation(*this);
	//	}

	//	void scale(const vector3<_Ty, _Vtag>& S);
	//	void scale(_Ty _Sx, _Ty _Sy, _Ty _Sz);
	//	void rotate(const vector3<_Ty, _Vtag>& E);
	//	void translate(const vector3<_Ty, _Vtag>& _Tv);
	//	void translate(meters<_Ty> _X, meters<_Ty> _Y, meters<_Ty> _Z);
	//};

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
	
	template<typename _Vec>
		auto distance(
			_in(_Vec) _A,
			_in(_Vec) _B);

	template<typename _Vec>
		_Vec proj(
			_in(_Vec) _Vector, 
			_in(_Vec) _Proj);

	template<typename _Vec>
		_Vec reflect(
			_in(_Vec) _Direction, 
			_in(_Vec) _Normal);

	///* dot(_I, _Nref) < 0 ? _Normal : -_Normal */
	//template<typename _Vec>
	//	_Vec faceforward(
	//		_in(_Vec) _Normal, 
	//		_in(_Vec) _I, 
	//		_in(_Vec) _Nref);

	//template<size_t N, typename T>
	//	T radians(
	//		_in(vector_<N, T>) _A, 
	//		_in(vector_<N, T>) _B);

	//template<typename T>
	//	vector3<T> proj(
	//		_in(vector3<T>) _Vector, 
	//		_in(unit_vector<T>) _Proj);

	//template<typename T>
	//	vector3<T> proj(
	//		_in(vector3<T>) _Vector, 
	//		_in(unit_vector<T>)  _Rt, 
	//		_in(unit_vector<T>)  _Up, 
	//		_in(unit_vector<T>)  _Fwd );

	//template<typename T>
	//	vector3<T> reflect(
	//		_in(vector3<T>) _Direction, 
	//		_in(unit_vector<T>)  _Normal);

	//template<typename T> 
	//	vector2<T> orthogonal(
	//		_in(vector2<T>) _Lhs);

	//template<typename T> 
	//	void orthogonal(
	//		_inout(std::vector<vector3<T>>) _Vectors);

	
	//template<size_t M, size_t N, typename T> matrix_<N, M, T> transpose( _in(matrix_<M, N, T>) _Lhs);
	//template<size_t M, size_t N, typename T> matrix_<M, N, T> inverse( _in(matrix_<M, N, T>) _Lhs);
	//template<size_t M, typename T> T determinant(const matrix_<M, M, T> _Src);

	//template<typename T> 
	//	vector3<T> transform_coord(_in(vector3<T>) _Lhs, _in(matrix_<4, 4, T>) _matrix);
	//template<typename T> 
	//	unit_vector<T> transform_normal(_in(unit_vector<T>) _Lhs, _in(matrix_<4, 4, T>) _matrix);
	//
	//vector3<float> screen_to_world(_in(vector3<float>) _Vec, _in(mat4) _Viewmatrix, _in(mat4) _Projmatrix);

	//	
	//template<typename T> 
	//	matrix_<4, 4, T> scaling_matrix(
	//		_in(T) _Width, 
	//		_in(T) _Height, 
	//		_in(T) _Depth );

	//template<typename T>
	//	matrix_<4, 4, T> scaling_matrix( 
	//		_in(vector3<T>) _sXyz );

	//template<typename T> 
	//	matrix_<4, 4, T> rotation_matrix( 
	//		_in(unit_vector<T>) _Axis, 
	//		_in(T)          _Radians );

	//template<typename T>
	//matrix_<4, 4, T> rotation_matrix(const T Ax, const T Ay, const T Az, const T rad);

	//template<typename T>
	//	matrix_<4, 4, T> translation_matrix(
	//		_in(T) _X, 
	//		_in(T) _Y, 
	//		_in(T) _Z );

	//template<typename T>
	//	matrix_<4, 4, T> translation_matrix(
	//		_in(vector3<T>) _dXyz );

	///*
	//@_Eye as eye position
	//@_Fwd as forward vector
	//@_Up  as up vector, require normalize vector
	//*/
	//template<typename T = real_t>
	//	matrix_<4, 4, T> Lookat( 
	//		_in(vector3<T>) _Eye, 
	//		_in(unit_vector<T>)  _Fwd, 
	//		_in(unit_vector<T>)  _Up);

	//// @_Target as target position
	//template<typename T>
	//	matrix_<4, 4, T> Lookat2( 
	//		_in(vector3<T>) _Eye, 
	//		_in(vector3<T>) _Target, 
	//		_in(unit_vector<T>)  _Up);

	//template<typename T> 
	//	matrix_<4, 4, T> Ortho(
	//		_in(T) _Left, 
	//		_in(T) _Right, 
	//		_in(T) _Bottom, 
	//		_in(T) _Top, 
	//		_in(T) _Nearplane = 0.0f, 
	//		_in(T) _Farplane = 10000.0f);

	//template<typename T> 
	//	matrix_<4, 4, T> Perspective(
	//		_in(T) _Fov, 
	//		_in(T) _Aspect, 
	//		_in(T) _Znear, 
	//		_in(T) _Zfar);

	template<typename T>
	matrix<T, 4, 4> PerspectiveLh(_in(T) _Fov, _in(T) _Aspect, _in(T) _Znear, _in(T) _Zfar) {
		T height = tanf(_Fov * T(0.5));
		T width = height * _Aspect;
		T diff = _Zfar - _Znear;
		T div = _Zfar / diff;

		return matrix<T, 4, 4>(
			1.0f / width,          0.0f,          0.0f, 0.0f,
			0.0f,         1.0f / height,          0.0f, 0.0f,
			0.0f,                  0.0f,           div, 1.0f,
			0.0f,                  0.0f, -_Znear * div, 0.0f
		);
	}
		
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 



	template<typename T = real_t>
	struct matrix_transform 
	{
		matrix_transform();
		matrix_transform(_in(matrix<T, 4, 4>) _matrix);

		unit_vector3<T>& transform(_inout(unit_vector3<T>) _Normal) const;
		vector3<T>&      transform(_inout(vector3<T>) _Vector) const;
		unit_vector3<T> transform_copy(_in(unit_vector3<T>) _Normal) const;
		vector3<T>      transform_copy(_in(vector3<T>) _Vector) const;

		unit_vector3<T>& inv_transform(_inout(unit_vector3<T>) _Normal) const;
		vector3<T>&      inv_transform(_inout(vector3<T>) _Vector) const;
		unit_vector3<T> inv_transform_copy(_in(unit_vector3<T>) _Normal) const;
		vector3<T>      inv_transform_copy(_in(vector3<T>) _Vector) const;

		matrix4x4<T> _Mymat;
		matrix4x4<T> _Myinvmat;
	};
	



	/*
	@_Name: Transform
	@_interface: to_matrix(), transform(=), transform_copy(&)
	*/

#if defined(clmagic_using_SIMDmath)
	template<typename T>
		struct _declspec(align(16)) quaternion;/* forward definition */
#else
	template<typename T>
		struct Quaternion_;/* forward definition */
#endif


	/* @_S: scale, scaling */
	template<typename T = real_t>
	struct scale_transform {
		scale_transform( _in(vector3<T>) _Vec = vector3<T>(T(1)) );
		scale_transform(T _sX, T _sY, T _sZ);
		
		matrix4x4<T> to_matrix() const;
		vector3<T>&  transform(_inout(vector3<T>) _Input) const;
		vector3<T>   transform_copy(_in(vector3<T>) _Input) const;

		scale_transform& operator+=(_in(scale_transform) _B) { _Mydata += _B._Mydata; return (*this); }
		scale_transform& operator-=(_in(scale_transform) _B) { _Mydata -= _B._Mydata; return (*this); }
		scale_transform& operator*=(_in(T)				 _B) { _Mydata *= _B; return (*this); }
		scale_transform& operator/=(_in(T)				 _B) { _Mydata /= _B; return (*this); }
		friend scale_transform operator+(_in(scale_transform) _A, _in(scale_transform) _B) { return (_A._Mydata + _B._Mydata); }
		friend scale_transform operator-(_in(scale_transform) _A, _in(scale_transform) _B) { return (_A._Mydata - _B._Mydata); }
		friend scale_transform operator*(_in(scale_transform) _A, _in(T) _B) { return (_A._Mydata * _B); }
		friend scale_transform operator/(_in(scale_transform) _A, _in(T) _B) { return (_A._Mydata / _B); }
		friend scale_transform operator*(_in(T) _A, _in(scale_transform) _B) { return (_A * _B._Mydata); }
		friend scale_transform operator/(_in(T) _A, _in(scale_transform) _B) { return (_A / _B._Mydata); }
			
		vector3<T> _Mydata;
	};


	/* @_Q: rotate */
	template<typename T = real_t>
	struct rotate_transform {
		rotate_transform(_in(Quaternion_<T>) _Quat = Quaternion_<T>());
		rotate_transform(_in(vector3<T>) _Axis, T _Radians);
		rotate_transform(_in(vector3<T>) _From, _in(vector3<T>) _To);

		matrix4x4<T> to_matrix() const;
		vector3<T>&  transform(_inout(vector3<T>) _Input) const;
		vector3<T>   transform_copy(_in(vector3<T>) _Input) const;

		/* unsupport scalar mul div */
		rotate_transform& operator+=(_in(rotate_transform) _B) { _Mydata *= _B._Mydata; return (*this); }
		rotate_transform& operator-=(_in(rotate_transform) _B) { _Mydata /= _B._Mydata; return (*this); }
		friend rotate_transform operator+(_in(rotate_transform) _A, _in(rotate_transform) _B) { return (_A._Mydata * _B._Mydata); }
		friend rotate_transform operator-(_in(rotate_transform) _A, _in(rotate_transform) _B) { return (_A._Mydata / _B._Mydata); }

		Quaternion_<T> _Mydata;
	};


	/* @_T: translate, move */
	template<typename T = real_t>
	struct translate_transform {
		translate_transform(_in(vector3<T>) _Vec = vector3<T>());
		translate_transform(T _X, T _Y, T _Z);
			
		matrix4x4<T> to_matrix() const;
		vector3<T>&  transform(_inout(vector3<T>) _Input) const;
		vector3<T>   transform_copy(_in(vector3<T>) _Input) const;

		translate_transform& operator+=(_in(translate_transform) _B) { _Mydata += _B._Mydata; return (*this); }
		translate_transform& operator-=(_in(translate_transform) _B) { _Mydata -= _B._Mydata; return (*this); }
		translate_transform& operator*=(_in(T)					 _B) { _Mydata *= _B; return (*this); }
		translate_transform& operator/=(_in(T)					 _B) { _Mydata /= _B; return (*this); }
		friend translate_transform operator+(_in(translate_transform) _A, _in(translate_transform) _B) { return (_A._Mydata + _B._Mydata); }
		friend translate_transform operator-(_in(translate_transform) _A, _in(translate_transform) _B) { return (_A._Mydata - _B._Mydata); }
		friend translate_transform operator*(_in(translate_transform) _A, _in(T) _B) { return (_A._Mydata * _B); }
		friend translate_transform operator/(_in(translate_transform) _A, _in(T) _B) { return (_A._Mydata / _B); }
		friend translate_transform operator*(_in(T) _A, _in(translate_transform) _B) { return (_A * _B._Mydata); }
		friend translate_transform operator/(_in(T) _A, _in(translate_transform) _B) { return (_A / _B._Mydata); }

		vector3<T> _Mydata;
	};


	/* @_SQT: scale and rotate and translate */
	template<typename T = real_t>
	struct SQT_transform {
		SQT_transform();
		SQT_transform(_in(scale_transform<T>) _Scale);
		SQT_transform(_in(rotate_transform<T>) _Scale);
		SQT_transform(_in(translate_transform<T>) _Scale);
		SQT_transform(_in(scale_transform<T>) _Scale, _in(rotate_transform<T>) _Rotate, _in(translate_transform<T>) _Translate);
		explicit SQT_transform(_in(matrix4x4<T>) _matrix);

		matrix4x4<T> to_matrix( ) const;
		matrix<T, 3, 4> to_matrix3x4() const;
		vector3<T>&  transform(_inout(vector3<T>) _Input) const;
		vector3<T>   transform_copy(_in(vector3<T>) _Input) const;

		SQT_transform& operator+=(_in(SQT_transform) _B) { _Myscl += _B._Myscl; _Myrot += _B._Myrot; _Mymov += _B._Mymov; return (*this); }
		SQT_transform& operator-=(_in(SQT_transform) _B) { _Myscl -= _B._Myscl; _Myrot -= _B._Myrot; _Mymov -= _B._Mymov; return (*this); }
		SQT_transform& operator*=(_in(T)             _B) { _Myscl *= _B; _Mymov *= _B; return (*this); }
		SQT_transform& operator/=(_in(T)             _B) { _Myscl /= _B; _Mymov /= _B; return (*this); }
		friend SQT_transform operator+(_in(SQT_transform) _A, _in(SQT_transform) _B) { return SQT_transform(_A._Myscl + _B._Myscl, _A._Myrot + _B._Myrot, _A._Mymov + _B._Mymov); }
		friend SQT_transform operator-(_in(SQT_transform) _A, _in(SQT_transform) _B) { return SQT_transform(_A._Myscl - _B._Myscl, _A._Myrot - _B._Myrot, _A._Mymov - _B._Mymov); }
		friend SQT_transform operator*(_in(SQT_transform) _A, _in(T) _B) { return SQT_transform(_A._Myscl * _B, _A._Myrot, _A._Mymov * _B); }
		friend SQT_transform operator/(_in(SQT_transform) _A, _in(T) _B) { return SQT_transform(_A._Myscl / _B, _A._Myrot, _A._Mymov / _B); }
		friend SQT_transform operator*(_in(T) _A, _in(SQT_transform) _B) { return SQT_transform(_A * _B._Myscl, _A._Myrot, _A * _B._Mymov); }
		friend SQT_transform operator/(_in(T) _A, _in(SQT_transform) _B) { return SQT_transform(_A / _B._Myscl, _A._Myrot, _A * _B._Mymov); }

		scale_transform<T>     _Myscl;/*S*/
		rotate_transform<T>    _Myrot;/*Q*/
		translate_transform<T> _Mymov;/*T*/
	};
	

	/*
	@_tangent_btangent_normal
	@_tangent_space
	*/
	//struct tbn
	//{
	//	tbn(const Vec3& _P0, const Vec3& _P1, const Vec3& _P2,
	//		const Vec2& _Texcoord0, const Vec2& _Texcoord1, const Vec2& _Texcoord2);

	//	Vec3 tangent;
	//	Vec3 btangent;
	//	Vec3 _Mynormal;
	//};



	/* < following: is object [in] the region [with] crosspoint > */
	template<typename _Ty>
		bool intersect(
			_in(vector2<_Ty>) _Pos,
			/*in*/_Ty	    _Left,
			/*in*/_Ty		_Right,
			/*in*/_Ty		_Bottom,
			/*in*/_Ty		_Top);

	/*
	@_Return no hit if( 0 )
	*/
	template<typename T> 
		int intersect(
			_in(vector3<T>) _Pos,
			_in(vector3<T>) _V0,
			_in(vector3<T>) _V1,
			_in(vector3<T>) _V2);

	template<typename T> 
		bool intersect(
			_in(vector3<T>) _Origin,
			_in(vector3<T>) _Dir,
			_in(vector3<T>) _V0,
			_in(vector3<T>) _V1,
			_in(vector3<T>) _V2,
			_out(vector3<T>) _Point);

	/* </ following: is object [in] the region [with] crosspoint > */
}// namespace clmagic

#endif