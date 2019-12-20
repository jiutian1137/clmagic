#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH_LAPACK___MATRIX___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH_LAPACK___MATRIX___H__
#include "../lapack.h"

namespace clmagic 
{
	/* < static > */
	template< size_t M, size_t N, typename T > inline
		Mat_<M, N, T> Mat_< M, N, T >::all(_in(T) _Val)
		{	// dst[i,j] = _Val
		Mat_< M, N, T > _Result;
		for (size_t i = 0; i != M; ++i)
			for (size_t j = 0; j != N; ++j)
				_Result.at(i, j) = _Val;
		return ( _Result );
		}

	template< size_t M, size_t N, typename T > inline
		Mat_<M, N, T> Mat_< M, N, T >::zero()
		{	// dst[i,j] = 0
		return ( all( static_cast<T>(0) ) );
		}

	template< size_t M, size_t N, typename T > inline
		Mat_<M, N, T> Mat_< M, N, T >::identity()
		{
		Mat_< M, N, T > _Result = zero();
		size_t _Minsize = minval(M, N);

		for (size_t i = 0; i < _Minsize; ++i)
			_Result.at(i, i) = static_cast<T>( 1 );

		return (_Result);
		}
	/* </ static > */

	/* < construct > */
	template< size_t M, size_t N, typename T > inline
		Mat_<M, N, T>::Mat_( /*in*/const T* _Array )
		{	// construct matrix by T array
		for ( size_t i = 0; i != M; ++i )
			for ( size_t j = 0; j != N; ++j )
				this->at(i, j) = _Array[i * N + j];
		}

	template< size_t M, size_t N, typename T > inline
		Mat_<M, N, T>::Mat_( std::initializer_list<T> _Ilist )
		{	// construct by initializer_list<basic type>
		assign( _Ilist );
		}

	template< size_t M, size_t N, typename T >
	template<size_t _Rm, size_t _Rn> inline 
		Mat_<M, N, T>::Mat_( _in(Mat_<_Rm, _Rn, T>) _Right )
		{	// (*this) = Mat_<rM, rN, T>
		for ( size_t i = 0; i != minval(M, _Rm); ++i )
			for ( size_t j = 0; j != minval(N, _Rn); ++j )
				this->at( i, j ) = _Right.at( i, j );
		}
	/* </ construct > */

	/* < convert > */
	template< size_t M, size_t N, typename _Ty > inline
		Mat_< M, N, _Ty >::operator std::string() const
		{	// convert to string
		using std::to_string;
		std::string _Str;

		_Str += "mat";
		_Str += std::to_string(M);
		_Str += "x";
		_Str += std::to_string(N);
		_Str += "[\n";
		for (int i = 0; i != M; ++i)
			{
			for (size_t j = 0; j != N; ++j) 
				{
				_Str += to_string(this->at(i, j));
				_Str += ", ";
				}
			_Str.back() =  '\n';
			}
		_Str += "]";
		return (_Str);
		}
	/* </ convert > */

	/* < behavior > */
	template< size_t M, size_t N, typename T > inline
		Vec_<N, T>& Mat_<M, N, T>::row(size_t _Pos)
		{
		return ( *((Vec_<N, T>*)((*this)[_Pos])) );
		}

	template< size_t M, size_t N, typename T > inline
		const Vec_<N, T>& Mat_<M, N, T>::row(size_t _Pos) const
		{
		return ( *((const Vec_<N, T>*)((*this)[_Pos])) );
		}

	template< size_t M, size_t N, typename T > inline
		void Mat_<M, N, T>::assign(std::initializer_list<T> _Ilist)
		{	// construct by initializer_list<basic type>
		assert( this->size() >= _Ilist.size() );
		std::move( _Ilist.begin(), _Ilist.end(), (T*)(this->ptr()) );
		}

	template< size_t M, size_t N, typename T > inline
		Mat_<N, M, T> Mat_<M, N, T>::transpose() const
		{
		Mat_<N, M, T> _Result;
		for (size_t i = 0; i != M; ++i)
			for (size_t j = 0; j != N; ++j)
				_Result.at(j, i) = this->at(i, j);
		return ( _Result );
		}

	template< size_t M, size_t N, typename T > inline
		Mat_<M, N, T> Mat_<M, N, T>::inverse() const
		{
		Mat_< M, N, T > _Tmp = (*this);
		Mat_< M, N, T > _Result = Mat_<M, N, T>::identity();
		return ( _Lu(_Result.ptr(), _Tmp.ptr(), M, N) != 0
				? _Result
				: Mat_<M, N, T>::zero() );
		}

	template< size_t M, size_t N, typename T > inline
		bool Mat_<M, N, T>::is_identity() const
		{
		return ( at(0, 0) == T(1) && at(0, 1) == T(0) && at(0, 2) == T(0) && at(0, 3) == T(0) &&
			     at(1, 0) == T(0) && at(1, 1) == T(1) && at(1, 2) == T(0) && at(1, 3) == T(0) &&
			     at(2, 0) == T(0) && at(2, 1) == T(0) && at(2, 2) == T(1) && at(2, 3) == T(0) &&
			     at(2, 0) == T(0) && at(2, 1) == T(0) && at(2, 2) == T(0) && at(2, 3) == T(1) );
		}
	/* </ behavior > */


	/* < ? matrix > */
	template<size_t M, size_t N, typename T> inline
		Mat_<M, N, T> operator- (_in(Mat_<M, N, T>) _Lhs)
		{	// return ( _Lhs.inverse )
		return ( inverse(_Lhs) );
		}

	template<size_t M, size_t N, typename T> inline
		Mat_<M, N, T> operator+ (_in(Mat_<M, N, T>) _Lhs, _in(Mat_<M, N, T>) _Rhs)
		{	// return (_Lhs + _Rhs)
		Mat_<M,N,T> _Result;
		_Add_vector_vector( _Result.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Result.size() );
		return ( _Result );
		}

	template<size_t M, size_t N, typename T> inline
		Mat_<M, N, T> operator- (_in(Mat_<M, N, T>) _Lhs, _in(Mat_<M, N, T>) _Rhs)
		{	// return (_Lhs - _Rhs)
		Mat_<M,N,T> _Result;
		_Sub_vector_vector( _Result.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Result.size() );
		return ( _Result );
		}

	// < gems_mul(matrix single) >
	template<size_t M, size_t N, typename T> inline
		Mat_<M, N, T> operator* (_in(Mat_<M, N, T>) _Lhs, _in(T) _Rhs)
		{
		Mat_<M, N, T> _Result;
		_Mul_vector_scalar<T>( _Result.ptr(), _Lhs.ptr(), _Rhs, _Result.size() );
		return ( _Result );
		}

	template<size_t M, size_t N, typename T> inline
		Mat_<M, N, T> operator* (_in(T) _Lhs, _in(Mat_<M, N, T>) _Rhs)
		{
		return ( _Rhs * _Lhs );
		}
	// </ gems_mul(matrix single) >

	// < gemm_mul(matrix matrix) >	
	template<size_t M, size_t N, size_t P, typename T> inline
		Mat_<M, P, T> operator* ( _in(Mat_<M, N, T>) _Lhs,  _in(Mat_<N, P, T>) _Rhs )
		{
		Mat_<M, P, T> _Result;
		_Mul_matrix<T>( _Result.ptr(), _Lhs.ptr(), M, N, _Rhs.ptr(), P );
		return ( _Result );
		}
	// </ gemm_mul(matrix matrix) >

	template<size_t M, size_t N, typename T> inline
		Mat_<M, N, T>& operator+= ( _inout(Mat_<M,N, T>) _Lhs, _in(Mat_<M,N, T>) _Rhs )
		{
		_Add_vector_vector( _Lhs.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Lhs.size() );
		return ( _Lhs );
		}

	template<size_t M, size_t N, typename T> inline
		Mat_<M, N, T>& operator-= ( _inout(Mat_<M,N, T>) _Lhs, _in(Mat_<M,N, T>) _Rhs)
		{
		_Sub_vector_vector(_Lhs.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Lhs.size());
		return (_Lhs);
		}

	template<size_t M, size_t N, typename T> inline
		Mat_<M, N, T>& operator*= ( _inout(Mat_<M, N, T>) _Lhs, _in(T) _Rhs )
		{
		_Mul_vector_scalar( _Lhs.ptr(), _Lhs.ptr(), _Rhs, _Lhs.size() );
		return (_Lhs);
		}

	template<size_t M, typename T> inline
		Mat_<M, M, T>& operator*= ( _inout(Mat_<M, M, T>) _Lhs, _in(Mat_<M, M, T>) _Rhs )
		{
		_Lhs = _Lhs * _Rhs;
		return (_Lhs);
		}
	/* </ ? matrix > */

	template<size_t M, size_t N, typename T> inline
		Mat_<N, M, T> transpose( _in(Mat_<M, N, T>) _Lhs )
		{
		return ( _Lhs.transpose() ); 
		}

	template<size_t M, size_t N, typename T> inline
		Mat_<M, N, T> inverse( _in(Mat_<M, N, T>) _Lhs ) 
		{
		return ( _Lhs.inverse() );
		}

	template<typename T> inline
		Vec3_<T> transform_coord(_in(Vec3_<T>) _Lhs, _in(Mat_<4, 4, T>) _Matrix) 
		{	// right hand matrix operation
		Vec_<4, T> _Result = _Matrix.row(3);
		_Result += _Lhs.z * _Matrix.row(2);
		_Result += _Lhs.y * _Matrix.row(1);
		_Result += _Lhs.x * _Matrix.row(0);

		if ( _Result[3] != T(1) ) 
			{
			_Result /= _Result[3];
			}

		return ( static_cast<Vec3_<T>>(_Result) );
		}

	template<typename T> inline 
		Vec3_<T> transform_normal(_in(Vec3_<T>) _Lhs, _in(Mat_<4, 4, T>) _Matrix) 
		{	// right hand matrix operation
		Vec_<4, T> _Result  = _Lhs.z * _Matrix.row(2);
				   _Result += _Lhs.y * _Matrix.row(1);
				   _Result += _Lhs.x * _Matrix.row(0);
		return ( static_cast<Vec3_<T>>(_Result) );
		}

	inline Vec3_<float> screen_to_world(_in(Vec3_<float>) _Vec, _in(Mat4) _Viewmatrix, _in(Mat4) _Projmatrix)
		{
		auto _Vec128 = DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&_Vec));
		auto _Mat128view = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&_Viewmatrix));
		auto _Mat128proj = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&_Projmatrix));
		_Vec128.m128_f32[3] = 1.0f;
		_Mat128view = DirectX::XMMatrixInverse(nullptr, _Mat128view);
		_Mat128proj = DirectX::XMMatrixInverse(nullptr, _Mat128proj);

		_Vec128 = DirectX::XMVector3TransformCoord(_Vec128, _Mat128proj);
		_Vec128 = DirectX::XMVector3TransformCoord(_Vec128, _Mat128view);
		_Vec128 = DirectX::XMVector4Normalize(_Vec128);

		return (*reinterpret_cast<Vec3_<float>*>(&_Vec128));
		}

	template<typename _Ty> inline
		_Ty
		determinant(const Mat_<2, 2, _Ty> _Src) 
		{
		return ( _Ty(1) / ( _Src.at(0, 0) * _Src.at(1, 1) + _Src.at(0, 1) * _Src.at(1, 0) ) );
		}

	/* < gen_mat > */
	template<typename T> inline 
		Mat_<4, 4, T> scaling_matrix(_in(T) _Width, _in(T) _Height, _in(T) _Depth)
		{
		return ( Mat_<4, 4, T>(
			_Width,    T(0),   T(0), T(0),
			  T(0), _Height,   T(0), T(0),
			  T(0),    T(0), _Depth, T(0),
			  T(0),    T(0),   T(0), T(1)) );
		}

	template<typename T> inline 
		Mat_<4, 4, T> scaling_matrix(_in(Vec3_<T>) _sXyz)
		{
		return ( scaling_matrix(_sXyz.x, _sXyz.y, _sXyz.z) );
		}

	template<typename T> inline
		Mat_<4, 4, T> rotation_matrix(_in(Vec3_<T>) _Axis, _in(T) _Radians)
		{
		using namespace::DirectX;
		auto _A = _mm_loadu_ps(static_cast<const float*>(_Axis));
		return (*(Mat4*)(&DirectX::XMMatrixRotationAxis(_A, _Radians)));
		}

	template<typename T> inline
		Mat_<4, 4, T> translation_matrix( _in(T) _X, _in(T) _Y, _in(T) _Z) 
		{
		return ( Mat_<4, 4, T>(
			T(1), T(0), T(0), T(0),
			T(0), T(1), T(0), T(0),
			T(0), T(0), T(1), T(0),
			  _X,   _Y,   _Z, T(1)) );
		}

	template<typename T> inline
		Mat_<4, 4, T> translation_matrix( _in(Vec3_<T>) _dXyz) 
		{
		return (translation_matrix(_dXyz.x, _dXyz.y, _dXyz.z));
		}

	template<typename T> inline 
		Mat_<4, 4, T> Lookat( _in(Vec3_<T>) _Eye, _in(Vec3_<T>) _Forward, _in(Vec3_<T>) _Up) 
		{
		/*
		right hand coordinate
		[  r.x   u.x    -f.x   0 ]
		[  r.y   u.y    -f.y   0 ]
		[  r.z   u.z    -f.z   0 ]
		[-e¡¤r  -e¡¤u   e¡¤f   1 ]
		*/
		Vec3_<T> f = normalize(_Forward);
		Vec3_<T> r = normalize( cross(f, _Up) );
		Vec3_<T> u = normalize( cross(r, f) );

		return ( Mat_<4, 4, T>(
			r.x, u.x, -f.x, T(0),
			r.y, u.y, -f.y, T(0),
			r.z, u.z, -f.z, T(0),
			-dot(_Eye, r), -dot(_Eye, u), dot(_Eye, f), T(1)) );
		/*auto _Eyeposition = simd_128f::load(_Eye.ptr());
		auto _Focus = _Eye + _Forward;
		auto _Focusposition = simd_128f::load(_Focus.ptr());
		auto _Upvector = simd_128f::load(_Up.ptr());
		return (const_reference_cast<const Mat_<4, 4, T>>( DirectX::XMMatrixLookAtRH(_Eyeposition, _Focusposition, _Upvector)));*/
		}

	template<typename T> inline
		Mat_<4, 4, T> Lookat2(_in(Vec3_<T>) _Eye, _in(Vec3_<T>) _Target, _in(Vec3_<T>) _Up)
		{
		return ( Lookat(_Eye, _Target - _Eye, _Up) );
		}

	template<typename T> inline
		Mat_<4, 4, T> Ortho( _in(T) l, _in(T) r, _in(T) b, _in(T) t, _in(T) n, _in(T) f) 
		{
		/*
		[    2/(r-l),            0,            0, 0]
		[		   0,      2/(t-b),            0, 0]
		[          0,            0,      2/(f-n), 0]
		[-(r+l)(r-l), -(t+b)/(t-b), -(f+n)/(f-n), 1]
		*/
		return (Mat_<4, 4, T>(
			T(2)/(r-l),	T(0),	    T(0),       T(0),
			T(1),		T(2)/(t-b), T(0),       T(0),
			T(1),       T(0),		T(2)/(f-n), T(0),
			-(r+l)/(r-l), -(t+b)/(t-b), -(f+n)/(f-n), T(1)) );
		}

	template<typename T> inline
		Mat_<4, 4, T> Perspective( _in(T) _Fov, _in(T) _Aspect, _in(T) _Znear, _in(T) _Zfar) 
		{
		//return (const_reference_cast<const Mat_<4, 4, T>>( DirectX::XMMatrixPerspectiveFovRH(_Fov, _Aspect, _Znear, _Zfar)));
		T _Sin = sin(_Fov * T(0.5));
		T _Cos = cos(_Fov * T(0.5));

		return (Mat_<4, 4, T>(
			_Cos/_Sin/_Aspect,      T(0),                          T(0), T( 0),
			T(0),              _Cos/_Sin,                          T(0), T( 0),
			T(0),                   T(0),        _Zfar/(_Znear - _Zfar), T(-1),
			T(0),                   T(0), _Zfar/(_Znear - _Zfar)*_Znear, T( 0) ) );
		}
	/* </ gen_mat > */

}// namespace clmagic

#endif