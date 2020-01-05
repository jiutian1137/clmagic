#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH_LAPACK___TRANSFORM___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH_LAPACK___TRANSFORM___H__
#include "../lapack.h"
#include "../complex.h"

namespace clmagic 
{
	/* <scale transform> */
	template<typename T> inline
		ScaleTransform_<T>::ScaleTransform_(_in(Vector3_<T>) _Vec)
		: _Mydata(_Vec[0], _Vec[1], _Vec[2]) { }

	template<typename T> inline
		ScaleTransform_<T>::ScaleTransform_(T _sX, T _sY, T _sZ)
		: _Mydata(_sX, _sY, _sZ) { }

	template<typename T> inline
		Mat_<4, 4, T> ScaleTransform_<T>::to_matrix() const 
		{	// convert to Mat4x4
		return (scaling_matrix(_Mydata[0], _Mydata[1], _Mydata[2])); 
		}

	template<typename T> inline
		Vector3_<T>& ScaleTransform_<T>::transform(_inout(Vector3_<T>) _Input) const 
		{	// transform _Input itself
		return (_Input *= _Mydata);
		}

	template<typename T> inline
		Vector3_<T> ScaleTransform_<T>::transform_copy(_in(Vector3_<T>) _Input) const 
		{	// transform _Input's copy
		return (_Input * _Mydata);
		}
	/* </scale transform> */

	/* <rotate transform> */
	template<typename T> inline
		RotateTransform_<T>::RotateTransform_(_in(Quaternion_<T>) _Quat)
		: _Mydata(_Quat) { }

	template<typename T> inline
		RotateTransform_<T>::RotateTransform_(_in(Vector3_<T>) _Axis, T _Radians)
		: _Mydata( quaternion_type(_Axis, _Radians) ) { }

	template<typename T> inline
		RotateTransform_<T>::RotateTransform_(_in(Vector3_<T>) _From, _in(Vector3_<T>) _To) 
		: _Mydata( quaternion_type(_From, _To) ){ }

	template<typename T> inline
		Mat_<4, 4, T> RotateTransform_<T>::to_matrix() const 
		{	// convert to Mat4x4
		return ( static_cast<Mat_<4, 4, T>>(_Mydata) );
		}

	template<typename T> inline
		Vector3_<T>& RotateTransform_<T>::transform(_inout(Vector3_<T>) _Input) const 
		{	// transform _Input itself
		return (_Input = _Mydata.rotate( _Input ));
		}

	template<typename T> inline
		Vector3_<T>  RotateTransform_<T>::transform_copy(_in(Vector3_<T>) _Input) const 
		{	// transform _Input's copy
		return ( _Mydata.rotate(_Input) );
		}
	/* </rotate transform> */

	/* <translate transform> */
	template<typename T> inline
		TranslateTransform_<T>::TranslateTransform_(_in(Vector3_<T>) _Vec)
			: _Mydata(_Vec[0], _Vec[1], _Vec[2]) { }

	template<typename T> inline
		TranslateTransform_<T>::TranslateTransform_(T _X, T _Y, T _Z)
			: _Mydata(_X, _Y, _Z) { }

	template<typename T> inline
		Mat_<4, 4, T> TranslateTransform_<T>::to_matrix() const 
		{	// convert to Mat4x4
		return (translation_matrix(_Mydata[0], _Mydata[1], _Mydata[2]));
		}

	template<typename T> inline
		Vector3_<T>& TranslateTransform_<T>::transform(_inout(Vector3_<T>) _Input) const 
		{	// transform _Input itself
		return (_Input += _Mydata);
		}

	template<typename T> inline
		Vector3_<T> TranslateTransform_<T>::transform_copy(_in(Vector3_<T>) _Input) const 
		{	// transform _Input's copy
		return (_Input + _Mydata);
		}
	/* </translate transform> */


	/* <SQT transform> */
	template<typename T> inline
		SQTTransform_<T>::SQTTransform_()
		: _Myscl(), _Myrot(), _Mymov() { }

	template<typename T> inline
		SQTTransform_<T>::SQTTransform_(_in(ScaleTransform_<T>) _Scale)
		: _Myscl(_Scale), _Myrot(), _Mymov() { }

	template<typename T> inline
		SQTTransform_<T>::SQTTransform_(_in(RotateTransform_<T>) _Rotate)
		: _Myscl(), _Myrot(_Rotate), _Mymov() { }

	template<typename T> inline
		SQTTransform_<T>::SQTTransform_(_in(TranslateTransform_<T>) _Translate)
		: _Myscl(), _Myrot(), _Mymov(_Translate) { }

	template<typename T> inline
		SQTTransform_<T>::SQTTransform_(_in(ScaleTransform_<T>) _Scale, _in(RotateTransform_<T>) _Rotate, _in(TranslateTransform_<T>) _Translate)
		: _Myscl(_Scale), _Myrot(_Rotate), _Mymov(_Translate) { }

	template<typename T> inline
		SQTTransform_<T>::SQTTransform_(_in(Mat4) _Matrix)
		{
		using namespace::DirectX;
		XMVECTOR _S;
		XMVECTOR _Q;
		XMVECTOR _T;
		XMMATRIX _M = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&_Matrix));
		XMMatrixDecompose(&_S, &_Q, &_T, _M);
		_Myscl = ScaleTransform_<T>::vector_type(_S.m128_f32[0], _S.m128_f32[1], _S.m128_f32[2]);
		_Myrot = RotateTransform_<T>::quaternion_type(_Q.m128_f32[3], _Q.m128_f32[0], _Q.m128_f32[1], _Q.m128_f32[2]);
		_Mymov = TranslateTransform_<T>::vector_type(_T.m128_f32[0], _T.m128_f32[1], _T.m128_f32[2]);
		}

	template<typename T> inline
		Mat_<4, 4, T> SQTTransform_<T>::to_matrix() const 
		{	// convert to Mat4x4<T>
		Mat_<4, 4, T> _Matrix = _Myrot.to_matrix() * _Myscl.to_matrix();
		_Matrix[3][0] = _Mymov._Mydata[0];
		_Matrix[3][1] = _Mymov._Mydata[1];
		_Matrix[3][2] = _Mymov._Mydata[2];
		return (_Matrix);
		}

	template<typename T> inline
		Mat_<3, 4, T> SQTTransform_<T>::to_matrix3x4() const 
		{/*
			 [ scl[0], scl[1], scl[2], 0 ]
			 [ rot.w, rot.x, rot.y, rot.z]
			 [ mov[0], mov[1], mov[2], 0 ]
		 */
		return (Mat_<3, 4, T>(
			_Myscl._Mydata[0], _Myscl._Mydata[1], _Myscl._Mydata[2], T(0),
			_Myrot._Mydata[0], _Myrot._Mydata[1], _Myrot._Mydata[2], _Myrot._Mydata[3],
			_Mymov._Mydata[0], _Mymov._Mydata[1], _Mymov._Mydata[2], T(0)));
		}

	template<typename T> inline
		Vector3_<T>& SQTTransform_<T>::transform(_inout(Vector3_<T>) _Input) const 
		{	// transform _Input->scale->rotate->translate
		return (
			_Mymov.transform(
				_Myrot.transform(
					_Myscl.transform(_Input))));
		}

	template<typename T> inline
		Vector3_<T>   SQTTransform_<T>::transform_copy(_in(Vector3_<T>) _Input) const
		{	// transform _Input.copy->scale->rotate->translate 
		return (
			_Mymov.transform_copy(
				_Myrot.transform_copy(
					_Myscl.transform_copy(_Input))));
		}


	/* </SQT transform> */


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
		Normal_<T>& MatrixTransform_<T>::transform(_inout(Normal_<T>) _Normal) const
		{	// Mat3x4(_Mymat) * _Normal
		_Normal = transform_normal(_Normal, _Mymat);
		return (_Normal);
		}

	template<typename T> inline
	Vector3_<T>& MatrixTransform_<T>::transform(_inout(Vector3_<T>) _Vector) const
		{	// Mat4x4(_Mymat) * _Vector
		_Vector = transform_coord(_Vector, _Mymat);
		return (_Vector);
		}

	template<typename T> inline
	Normal_<T>  MatrixTransform_<T>::transform_copy(_in(Normal_<T>) _Normal) const 
		{
		return ( transform_normal(_Normal, _Mymat) );
		}
	
	template<typename T> inline
	Vector3_<T> MatrixTransform_<T>::transform_copy(_in(Vector3_<T>) _Vector) const 
		{
		return ( transform_coord(_Vector, _Mymat) );
		}

	template<typename T> inline
	Normal_<T>&  MatrixTransform_<T>::inv_transform(_inout(Normal_<T>) _Normal) const
		{
		_Normal = transform_normal(_Normal, _Myinvmat);
		return (_Normal);
		}

	template<typename T> inline
	Vector3_<T>& MatrixTransform_<T>::inv_transform(_inout(Vector3_<T>) _Vector) const 
		{
		_Vector = transform_coord(_Vector, _Myinvmat);
		return (_Vector);
		}

	template<typename T> inline
	Normal_<T>  MatrixTransform_<T>::inv_transform_copy(_in(Normal_<T>) _Normal) const 
		{
		return (transform_normal(_Normal, _Myinvmat));
		}

	template<typename T> inline
	Vector3_<T> MatrixTransform_<T>::inv_transform_copy(_in(Vector3_<T>) _Vector) const 
		{
		return ( transform_coord(_Vector, _Myinvmat) );
		}


	//inline tbn::tbn(const Vec3& _P0, const Vec3& _P1, const Vec3& _P2, 
	//	const Vec2& _Texcoord0, const Vec2& _Texcoord1, const Vec2& _Texcoord2)
	//	{
	//	Vec3 _E0 = _P1 - _P0;
	//	Vec3 _E1 = _P2 - _P0;
	//	Vec2 _Duv0 = _Texcoord1 - _Texcoord0;
	//	Vec2 _Duv1 = _Texcoord2 - _Texcoord0;
	//	Mat_<2, 3, float> _Deltapos = Mat_<2, 3, float>(_E0[0], _E0[1], _E0[2], 
	//													_E1[0], _E1[1], _E1[2]);
	//	Mat_<2, 2, float> _Deltatexcoord = Mat_<2, 2, float>(_Duv0[0], _Duv0[1],
	//														 _Duv1[0], _Duv1[1]);
	//	
	///*	auto _Result = determinant(_Deltatexcoord) * (_Deltatexcoord * _Deltapos);
	//	tangent = vec3(_Result.at(0, 0), _Result.at(0, 1), _Result.at(0, 2));
	//	btangent = vec3(_Result.at(1, 0), _Result.at(1, 1), _Result.at(1, 2));*/
	//	auto _F = determinant(_Deltatexcoord);
	//	tangent[0] = _F * (_Duv1[1] * _E0[0] - _Duv0[1] * _E1[0]);
	//	tangent[1] = _F * (_Duv1[1] * _E0[1] - _Duv0[1] * _E1[1]);
	//	tangent[2] = _F * (_Duv1[1] * _E0[2] - _Duv0[1] * _E1[2]);
	//	btangent[0] = _F * (-_Duv1[0] * _E0[0] + _Duv0[0] * _E1[0]);
	//	btangent[1] = _F * (-_Duv1[0] * _E0[1] + _Duv0[0] * _E1[1]);
	//	btangent[2] = _F * (-_Duv1[0] * _E0[2] + _Duv0[0] * _E1[2]);
	//	tangent = normalize( tangent );
	//	btangent = normalize( btangent );
	//	_Mynormal = cross( tangent, btangent );
	//	}
}// namespace clmagic

#endif