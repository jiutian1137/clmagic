#pragma once
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
		VECTOR3<T>&      transform(_inout(VECTOR3<T>) _Vector) const;
		unit_vector3<T> transform_copy(_in(unit_vector3<T>) _Normal) const;
		VECTOR3<T>      transform_copy(_in(VECTOR3<T>) _Vector) const;

		unit_vector3<T>& inv_transform(_inout(unit_vector3<T>) _Normal) const;
		VECTOR3<T>&      inv_transform(_inout(VECTOR3<T>) _Vector) const;
		unit_vector3<T> inv_transform_copy(_in(unit_vector3<T>) _Normal) const;
		VECTOR3<T>      inv_transform_copy(_in(VECTOR3<T>) _Vector) const;

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
		scale_transform( _in(VECTOR3<T>) _Vec = VECTOR3<T>(T(1)) );
		scale_transform(T _sX, T _sY, T _sZ);
		
		matrix4x4<T> to_matrix() const;
		VECTOR3<T>&  transform(_inout(VECTOR3<T>) _Input) const;
		VECTOR3<T>   transform_copy(_in(VECTOR3<T>) _Input) const;

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
			
		VECTOR3<T> _Mydata;
	};


	/* @_Q: rotate */
	template<typename T = real_t>
	struct rotate_transform {
		rotate_transform(_in(Quaternion_<T>) _Quat = Quaternion_<T>());
		rotate_transform(_in(VECTOR3<T>) _Axis, T _Radians);
		rotate_transform(_in(VECTOR3<T>) _From, _in(VECTOR3<T>) _To);

		matrix4x4<T> to_matrix() const;
		VECTOR3<T>&  transform(_inout(VECTOR3<T>) _Input) const;
		VECTOR3<T>   transform_copy(_in(VECTOR3<T>) _Input) const;

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
		translate_transform(_in(VECTOR3<T>) _Vec = VECTOR3<T>());
		translate_transform(T _X, T _Y, T _Z);
			
		matrix4x4<T> to_matrix() const;
		VECTOR3<T>&  transform(_inout(VECTOR3<T>) _Input) const;
		VECTOR3<T>   transform_copy(_in(VECTOR3<T>) _Input) const;

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

		VECTOR3<T> _Mydata;
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
		VECTOR3<T>&  transform(_inout(VECTOR3<T>) _Input) const;
		VECTOR3<T>   transform_copy(_in(VECTOR3<T>) _Input) const;

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
			_in(VECTOR2<_Ty>) _Pos,
			/*in*/_Ty	    _Left,
			/*in*/_Ty		_Right,
			/*in*/_Ty		_Bottom,
			/*in*/_Ty		_Top);

	/*
	@_Return no hit if( 0 )
	*/
	template<typename T> 
		int intersect(
			_in(VECTOR3<T>) _Pos,
			_in(VECTOR3<T>) _V0,
			_in(VECTOR3<T>) _V1,
			_in(VECTOR3<T>) _V2);

	template<typename T> 
		bool intersect(
			_in(VECTOR3<T>) _Origin,
			_in(VECTOR3<T>) _Dir,
			_in(VECTOR3<T>) _V0,
			_in(VECTOR3<T>) _V1,
			_in(VECTOR3<T>) _V2,
			_out(VECTOR3<T>) _Point);

	/* </ following: is object [in] the region [with] crosspoint > */
}// namespace clmagic

#endif