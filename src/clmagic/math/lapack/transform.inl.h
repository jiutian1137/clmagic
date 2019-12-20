#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH_LAPACK___TRANSFORM___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH_LAPACK___TRANSFORM___H__
#include "../lapack.h"
#include "../complex.h"

namespace clmagic 
{
	template<typename T> inline
		MatrixTransform_<T>::MatrixTransform_( )
		: _Mymat( Mat_<4, 4, T>::identity() ),
		_Myinvmat( Mat_<4, 4, T>::identity() )
		{
		}

	template<typename T> inline
		MatrixTransform_<T>::MatrixTransform_( _in(Mat_<4, 4, T>) _Matrix )
		: _Mymat(_Matrix), _Myinvmat(_Matrix.inverse())
		{
		}

	template<typename T> inline
		Vec3_<T> MatrixTransform_<T>::transform(_in(Vec3_<T>) _Vector) const
		{	// _Vector * matrix
		return ( transform_coord(_Vector, _Mymat) );
		/*
		return ( Vec3_<T>(_Vector.x * _Mymat[0][0] + _Vector.y * _Mymat[1][0] + _Vector.z * _Mymat[2][0] + _Mymat[3][0],
						  _Vector.x * _Mymat[0][1] + _Vector.y * _Mymat[1][1] + _Vector.z * _Mymat[2][1] + _Mymat[3][1],
						  _Vector.x * _Mymat[0][2] + _Vector.y * _Mymat[1][2] + _Vector.z * _Mymat[2][2] + _Mymat[3][2] ) );
		*/
		}

	template<typename T> inline
		Vec3_<T> MatrixTransform_<T>::transform(_in(Vec3_<T>) _Normal, _Tag_normal) const
		{	// _Normal * Mat3(transpose(_Myinvmat))
		return ( Vec3_<T>(dot( _Normal, reference_cast<const Vec3_<T>>(_Myinvmat.row(0)) ),
						  dot( _Normal, reference_cast<const Vec3_<T>>(_Myinvmat.row(1)) ),
						  dot( _Normal, reference_cast<const Vec3_<T>>(_Myinvmat.row(2)) ) ) );
	/*	return ( Vec3_<T>(_Myinvmat[0][0] * _Normal.x + _Myinvmat[0][1] * _Normal.y + _Myinvmat[0][2] * _Normal.z,
						  _Myinvmat[1][0] * _Normal.x + _Myinvmat[1][1] * _Normal.y + _Myinvmat[1][2] * _Normal.z,
						  _Myinvmat[2][0] * _Normal.x + _Myinvmat[2][1] * _Normal.y + _Myinvmat[2][2] * _Normal.z) );*/
		}

	template<typename T> inline
		Vec3_<T> MatrixTransform_<T>::inv_transform(_in(Vec3_<T>) _Vector) const 
		{
		return ( transform_coord(_Vector, _Myinvmat) );
		}

	template<typename T> inline
		Vec3_<T> MatrixTransform_<T>::inv_transform(_in(Vec3_<T>) _Normal, _Tag_normal) const
		{
		return (Vec3_<T>(dot(_Normal, reference_cast<const Vec3_<T>>(_Mymat.row(0))),
						 dot(_Normal, reference_cast<const Vec3_<T>>(_Mymat.row(1))),
						 dot(_Normal, reference_cast<const Vec3_<T>>(_Mymat.row(2)))));
		}



	template<typename T> inline
		SQTTransform_<T>::SQTTransform_() 
		: _Myscl(T(1)), _Pand1(), _Myrot(), _Mymov(), _Pand2() 
		{ /* empty */
		}

	template<typename T> inline
		SQTTransform_<T>::SQTTransform_(_in(Mat4) _Matrix)
		{
		using namespace::DirectX;
		vec128f _S;
		vec128f _Q;
		vec128f _T;
		XMMATRIX _M = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&_Matrix));
		XMMatrixDecompose(&_S, &_Q, &_T, _M);
		_Myscl = reference_cast<Vec3>(_S);
		_Myrot = Quaternion(_Q.m128_f32[3], _Q.m128_f32[0], _Q.m128_f32[1], _Q.m128_f32[2]);
		_Mymov = reference_cast<Vec3>(_T);
		}
	// </construct>

	// <convert>	
	template<typename T> inline
		SQTTransform_<T>::operator Mat_<4, 4, T>() const
		{
		Mat_<4, 4, T> _Result = scaling_matrix(_Myscl) * Mat_<4, 4, T>(_Myrot) * translation_matrix(_Mymov);
		return (_Result);
		}

	template<typename T> inline
		SQTTransform_<T>::operator std::string() const
		{
		std::string _Str(44 + 90, '\0');
		snprintf(&_Str[0], _Str.size(), "SQT[S[%f,%f,%f],Q[%f,%f,%f,%f],T[%f,%f,%f]]",
			_Myscl.x, _Myscl.y, _Myscl.z,
			_Myrot.w, _Myrot.x, _Myrot.y, _Myrot.z,
			_Mymov.x, _Mymov.y, _Mymov.z);		
		return (_Str);
		}
	// </convert>

	template<typename T> inline
		Vec3_<T> SQTTransform_<T>::transform(_in(Vec3_<T>) _Vector) const
		{
		return ( _Mymov + _Myrot.rotate(_Vector * _Myscl) );
		}

	template<typename T> inline
		Vec3_<T> SQTTransform_<T>::transform(_in(Vec3_<T>) _Normal, _Tag_normal) const
		{
		return ( normalize( _Myrot.rotate(_Normal * _Myscl) ) );
		}

	/* < ? sqt_transform sqt_transform > */
	template<typename T> inline 
		SQTTransform_<T> operator+ ( _in(SQTTransform_<T>) _Lhs, _in(SQTTransform_<T>) _Rhs ) 
		{
		SQTTransform_<T> _Result;
		_Result._Myscl = _Lhs._Myscl + _Rhs._Myscl;
		_Result._Myrot = _Lhs._Myrot * _Rhs._Myrot;
		_Result._Mymov = _Lhs._Mymov + _Rhs._Mymov;
		return ( _Result );
		}

	template<typename T> inline
		SQTTransform_<T> operator- ( _in(SQTTransform_<T>) _Lhs, _in(SQTTransform_<T>) _Rhs ) 
		{
		SQTTransform_<T> _Result;
		_Result._Myscl = _Lhs._Myscl - _Rhs._Myscl;
		_Result._Myrot = _Lhs._Myrot / _Rhs._Myrot;
		_Result._Mymov = _Lhs._Mymov - _Rhs._Mymov;
		return ( _Result );
		}

	template<typename T> inline 
		SQTTransform_<T>& operator+= ( _inout(SQTTransform_<T>) _Lhs, _in(SQTTransform_<T>) _Rhs ) 
		{
		_Lhs._Myscl += _Rhs._Myscl;
		_Lhs._Myrot *= _Rhs._Myrot;
		_Lhs._Mymov += _Rhs._Mymov;
		return ( _Lhs );
		}

	template<typename T> inline 
		SQTTransform_<T>& operator-= ( _inout(SQTTransform_<T>) _Lhs, _in(SQTTransform_<T>) _Rhs ) 
		{
		_Lhs._Myscl -= _Rhs._Myscl;
		_Lhs._Myrot /= _Rhs._Myrot;
		_Lhs._Mymov -= _Rhs._Mymov;
		return ( _Lhs );
		}
	/* </ ? sqt_transform sqt_transform > */


	inline tbn::tbn(const Vec3& _P0, const Vec3& _P1, const Vec3& _P2, 
		const Vec2& _Texcoord0, const Vec2& _Texcoord1, const Vec2& _Texcoord2)
		{
		Vec3 _E0 = _P1 - _P0;
		Vec3 _E1 = _P2 - _P0;
		Vec2 _Duv0 = _Texcoord1 - _Texcoord0;
		Vec2 _Duv1 = _Texcoord2 - _Texcoord0;
		Mat_<2, 3, float> _Deltapos = Mat_<2, 3, float>(_E0.x, _E0.y, _E0.z, 
														_E1.x, _E1.y, _E1.z);
		Mat_<2, 2, float> _Deltatexcoord = Mat_<2, 2, float>(_Duv0.x, _Duv0.y,
															 _Duv1.x, _Duv1.y);
		
	/*	auto _Result = determinant(_Deltatexcoord) * (_Deltatexcoord * _Deltapos);
		tangent = vec3(_Result.at(0, 0), _Result.at(0, 1), _Result.at(0, 2));
		btangent = vec3(_Result.at(1, 0), _Result.at(1, 1), _Result.at(1, 2));*/
		auto _F = determinant(_Deltatexcoord);
		tangent.x = _F * (_Duv1.y * _E0.x - _Duv0.y * _E1.x);
		tangent.y = _F * (_Duv1.y * _E0.y - _Duv0.y * _E1.y);
		tangent.z = _F * (_Duv1.y * _E0.z - _Duv0.y * _E1.z);
		btangent.x = _F * (-_Duv1.x * _E0.x + _Duv0.x * _E1.x);
		btangent.y = _F * (-_Duv1.x * _E0.y + _Duv0.x * _E1.y);
		btangent.z = _F * (-_Duv1.x * _E0.z + _Duv0.x * _E1.z);
		tangent = normalize( tangent );
		btangent = normalize( btangent );
		_Mynormal = cross( tangent, btangent );
		}
}// namespace clmagic

#endif