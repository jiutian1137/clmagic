#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_matH_LAPACK___TRANSFORM___H__
#define __CLMAGIC_CORE_GEOMETRY_matH_LAPACK___TRANSFORM___H__
#include "../lapack.h"
#include "../complex.h"

namespace clmagic 
{
	/* <scale transform> */
	template<typename T> inline
		scale_transform<T>::scale_transform(_in(vector3<T>) _Vec)
		: _Mydata(_Vec[0], _Vec[1], _Vec[2]) { }

	template<typename T> inline
		scale_transform<T>::scale_transform(T _sX, T _sY, T _sZ)
		: _Mydata(_sX, _sY, _sZ) { }

	template<typename T> inline
		matrix<4, 4, T> scale_transform<T>::to_matrix() const 
		{	// convert to mat4x4
		return (scaling_matrix(_Mydata[0], _Mydata[1], _Mydata[2])); 
		}

	template<typename T> inline
		vector3<T>& scale_transform<T>::transform(_inout(vector3<T>) _Input) const 
		{	// transform _Input itself
		return (_Input *= _Mydata);
		}

	template<typename T> inline
		vector3<T> scale_transform<T>::transform_copy(_in(vector3<T>) _Input) const 
		{	// transform _Input's copy
		return (_Input * _Mydata);
		}
	/* </scale transform> */

	/* <rotate transform> */
	template<typename T> inline
		rotate_transform<T>::rotate_transform(_in(Quaternion_<T>) _Quat)
		: _Mydata(_Quat) { }

	template<typename T> inline
		rotate_transform<T>::rotate_transform(_in(vector3<T>) _Axis, T _Radians)
		: _Mydata( quaternion_type(_Axis, _Radians) ) { }

	template<typename T> inline
		rotate_transform<T>::rotate_transform(_in(vector3<T>) _From, _in(vector3<T>) _To) 
		: _Mydata( quaternion_type(_From, _To) ){ }

	template<typename T> inline
		matrix<4, 4, T> rotate_transform<T>::to_matrix() const 
		{	// convert to mat4x4
		return ( static_cast<matrix<4, 4, T>>(_Mydata) );
		}

	template<typename T> inline
		vector3<T>& rotate_transform<T>::transform(_inout(vector3<T>) _Input) const 
		{	// transform _Input itself
		return (_Input = _Mydata.rotate( _Input ));
		}

	template<typename T> inline
		vector3<T>  rotate_transform<T>::transform_copy(_in(vector3<T>) _Input) const 
		{	// transform _Input's copy
		return ( _Mydata.rotate(_Input) );
		}
	/* </rotate transform> */

	/* <translate transform> */
	template<typename T> inline
		translate_transform<T>::translate_transform(_in(vector3<T>) _Vec)
			: _Mydata(_Vec[0], _Vec[1], _Vec[2]) { }

	template<typename T> inline
		translate_transform<T>::translate_transform(T _X, T _Y, T _Z)
			: _Mydata(_X, _Y, _Z) { }

	template<typename T> inline
		matrix<4, 4, T> translate_transform<T>::to_matrix() const 
		{	// convert to mat4x4
		return (translation_matrix(_Mydata[0], _Mydata[1], _Mydata[2]));
		}

	template<typename T> inline
		vector3<T>& translate_transform<T>::transform(_inout(vector3<T>) _Input) const 
		{	// transform _Input itself
		return (_Input += _Mydata);
		}

	template<typename T> inline
		vector3<T> translate_transform<T>::transform_copy(_in(vector3<T>) _Input) const 
		{	// transform _Input's copy
		return (_Input + _Mydata);
		}
	/* </translate transform> */


	/* <SQT transform> */
	template<typename T> inline
		SQT_transform<T>::SQT_transform()
		: _Myscl(), _Myrot(), _Mymov() { }

	template<typename T> inline
		SQT_transform<T>::SQT_transform(_in(scale_transform<T>) _Scale)
		: _Myscl(_Scale), _Myrot(), _Mymov() { }

	template<typename T> inline
		SQT_transform<T>::SQT_transform(_in(rotate_transform<T>) _Rotate)
		: _Myscl(), _Myrot(_Rotate), _Mymov() { }

	template<typename T> inline
		SQT_transform<T>::SQT_transform(_in(translate_transform<T>) _Translate)
		: _Myscl(), _Myrot(), _Mymov(_Translate) { }

	template<typename T> inline
		SQT_transform<T>::SQT_transform(_in(scale_transform<T>) _Scale, _in(rotate_transform<T>) _Rotate, _in(translate_transform<T>) _Translate)
		: _Myscl(_Scale), _Myrot(_Rotate), _Mymov(_Translate) { }

	template<typename T> inline
		SQT_transform<T>::SQT_transform(_in(matrix<4,4>) _matrix)
		{
		using namespace::DirectX;
		XMVECTOR _S;
		XMVECTOR _Q;
		XMVECTOR _T;
		XMMATRIX _M = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&_matrix));
		XMMatrixDecompose(&_S, &_Q, &_T, _M);
		_Myscl = scale_transform<T>::vector_type(_S.m128_f32[0], _S.m128_f32[1], _S.m128_f32[2]);
		_Myrot = rotate_transform<T>::quaternion_type(_Q.m128_f32[3], _Q.m128_f32[0], _Q.m128_f32[1], _Q.m128_f32[2]);
		_Mymov = translate_transform<T>::vector_type(_T.m128_f32[0], _T.m128_f32[1], _T.m128_f32[2]);
		}

	template<typename T> inline
		matrix<4, 4, T> SQT_transform<T>::to_matrix() const 
		{	// convert to mat4x4<T>
		matrix<4, 4, T> _matrix = _Myrot.to_matrix() * _Myscl.to_matrix();
		_matrix[3][0] = _Mymov._Mydata[0];
		_matrix[3][1] = _Mymov._Mydata[1];
		_matrix[3][2] = _Mymov._Mydata[2];
		return (_matrix);
		}

	template<typename T> inline
		matrix<3, 4, T> SQT_transform<T>::to_matrix3x4() const 
		{/*
			 [ scl[0], scl[1], scl[2], 0 ]
			 [ rot.w, rot.x, rot.y, rot.z]
			 [ mov[0], mov[1], mov[2], 0 ]
		 */
		return (matrix<3, 4, T>(
			_Myscl._Mydata[0], _Myscl._Mydata[1], _Myscl._Mydata[2], T(0),
			_Myrot._Mydata[0], _Myrot._Mydata[1], _Myrot._Mydata[2], _Myrot._Mydata[3],
			_Mymov._Mydata[0], _Mymov._Mydata[1], _Mymov._Mydata[2], T(0)));
		}

	template<typename T> inline
		vector3<T>& SQT_transform<T>::transform(_inout(vector3<T>) _Input) const 
		{	// transform _Input->scale->rotate->translate
		return (
			_Mymov.transform(
				_Myrot.transform(
					_Myscl.transform(_Input))));
		}

	template<typename T> inline
		vector3<T>   SQT_transform<T>::transform_copy(_in(vector3<T>) _Input) const
		{	// transform _Input.copy->scale->rotate->translate 
		return (
			_Mymov.transform_copy(
				_Myrot.transform_copy(
					_Myscl.transform_copy(_Input))));
		}


	/* </SQT transform> */


	template<typename T> inline
		matrix_transform<T>::matrix_transform( )
		: _Mymat( matrix<4, 4, T>::identity() ),
		_Myinvmat( matrix<4, 4, T>::identity() )
		{
		}

	template<typename T> inline
		matrix_transform<T>::matrix_transform( _in(matrix<4, 4, T>) _matrix )
		: _Mymat(_matrix), _Myinvmat(_matrix.inverse())
		{
		}

	template<typename T> inline
	unit_vector<3, T>& matrix_transform<T>::transform(_inout(unit_vector<3, T>) _Normal) const
		{	// mat3x4(_Mymat) * _Normal
		_Normal = transform_normal(_Normal, _Mymat);
		return (_Normal);
		}

	template<typename T> inline
	vector3<T>& matrix_transform<T>::transform(_inout(vector3<T>) _Vector) const
		{	// mat4x4(_Mymat) * _Vector
		_Vector = transform_coord(_Vector, _Mymat);
		return (_Vector);
		}

	template<typename T> inline
	unit_vector<3, T>  matrix_transform<T>::transform_copy(_in(unit_vector<3, T>) _Normal) const 
		{
		return ( transform_normal(_Normal, _Mymat) );
		}
	
	template<typename T> inline
	vector3<T> matrix_transform<T>::transform_copy(_in(vector3<T>) _Vector) const 
		{
		return ( transform_coord(_Vector, _Mymat) );
		}

	template<typename T> inline
	unit_vector<3, T>&  matrix_transform<T>::inv_transform(_inout(unit_vector<3, T>) _Normal) const
		{
		_Normal = transform_normal(_Normal, _Myinvmat);
		return (_Normal);
		}

	template<typename T> inline
	vector3<T>& matrix_transform<T>::inv_transform(_inout(vector3<T>) _Vector) const 
		{
		_Vector = transform_coord(_Vector, _Myinvmat);
		return (_Vector);
		}

	template<typename T> inline
	unit_vector<3, T>  matrix_transform<T>::inv_transform_copy(_in(unit_vector<3, T>) _Normal) const 
		{
		return (transform_normal(_Normal, _Myinvmat));
		}

	template<typename T> inline
	vector3<T> matrix_transform<T>::inv_transform_copy(_in(vector3<T>) _Vector) const 
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
	//	mat_<2, 3, float> _Deltapos = mat_<2, 3, float>(_E0[0], _E0[1], _E0[2], 
	//													_E1[0], _E1[1], _E1[2]);
	//	mat_<2, 2, float> _Deltatexcoord = mat_<2, 2, float>(_Duv0[0], _Duv0[1],
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