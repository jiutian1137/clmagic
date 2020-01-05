#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH_COMPLEX___QUATERNION___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH_COMPLEX___QUATERNION___H__
#include "../complex.h"

namespace clmagic 
{
	template<typename T> inline
		Vector4_<T> to_quaternion_vec(_in(Normal_<T>) _Axis, _in(T) _Radians) 
		{	/* @_quaternion:
			         re         im         im         im     
				[ sin(¦È/2), cos(¦È/2), cos(¦È/2), cos(¦È/2) ]
			*/
		auto _Half = Real(0.5) * _Radians;
		auto _Quat = Vector4_<T>(sin(_Half), cos(_Half), cos(_Half), cos(_Half));
		return ( _Quat * Vector4_<T>(T(1), _Axis[0], _Axis[1], _Axis[2]) );
	/*	using namespace::DirectX;
		auto _Result = XMQuaternionRotationAxis(
			simd_128f::load(static_cast<const float*>(_Axis)),
			_Radians);*/
		//return ( reference_cast<Vec4_<T>>(_Result) );
		}

	// <construct>

	template<typename T> inline
	Quaternion_<T>::Quaternion_(_in(Mat_<3, 3, T>) _Matrix)
		{	// construct by rotation matrix
		using namespace::DirectX;
		auto _Vec128fx4 = XMLoadFloat3x3( reference_cast<const XMFLOAT3X3>(_Matrix) );
		auto _Tmp = reference_cast< Vector4_<T> >( XMQuaternionRotationMatrix( _Vec128fx4 ) );
		shuffle(_Mydata, _Tmp, 3, 0, 1, 2);
		}

	template<typename T> inline
	Quaternion_<T>::Quaternion_(_in(Mat_<4, 4, T>) _Matrix)
		{
		using namespace::DirectX;
		auto _Vec128fx4 = XMLoadFloat4x4(_Matrix);
		auto _Tmp = reference_cast< Vector4_<T> >( XMQuaternionRotationMatrix(_Vec128fx4) );
		shuffle(_Mydata, _Tmp, 3, 0, 1, 2);
		}

	template<typename T> inline
	Quaternion_<T>::Quaternion_(_in(Normal_<T>) _Axis, _in(T) _Radians)
		{
		auto _Tmp = to_quaternion_vec(_Axis, _Radians);
		shuffle(_Mydata, _Tmp, 0, 1, 2, 3);
		}
	// </construct>

	template<typename T> inline
	Mat_<4, 4, T> Quaternion_<T>::to_matrix() const
		{	// convert to mat4x4
		return ( Mat_<4, 4, T>(
			T(1) - T(2) * (y * y + z * z),        T(2) * (x * y + z * w),        T(2) * (x * z - y * w), T(0),
				   T(2) * (x * y - z * w), T(1) - T(2) * (x * x + z * z),        T(2) * (y * z + x * w), T(0),
				   T(2) * (x * z + y * w),        T(2) * (y * z - x * w), T(1) - T(2) * (x * x + y * y), T(0),
								     T(0),					        T(0),						   T(0), T(1) 
			) );
		/*
			[ 1 - 2(y^2 + z^2),     2(x*y + z*w),     2(x*z - y*w), 0 ]
			[     2(x*y - z*w), 1 - 2(x^2 + z^2),     2(y*z + x*w), 0 ]
			[     2(x*z + y*w),     2(y*z - x*w), 1 - 2(x^2 + y^2), 0 ]
			[                0,                0,                0, 1 ]
		*/
		}

	template<typename T> inline
	Mat_<3, 3, T> Quaternion_<T>::to_matrix3x3() const
		{
		return ( Mat_<3, 3, T>(
			T(1) - T(2) * (y * y + z * z),        T(2) * (x * y + z * w),        T(2) * (x * z - y * w),
				   T(2) * (x * y - z * w), T(1) - T(2) * (x * x + z * z),        T(2) * (y * z + x * w),
				   T(2) * (x * z + y * w),        T(2) * (y * z - x * w), T(1) - T(2) * (x * x + y * y) 
			) );
		/*
			[ 1 - 2(y^2 + z^2),     2(x*y + z*w),     2(x*z - y*w) ]
			[     2(x*y - z*w), 1 - 2(x^2 + z^2),     2(y*z + x*w) ]
			[     2(x*z + y*w),     2(y*z - x*w), 1 - 2(x^2 + y^2) ]
		*/
		}

	template<typename T> inline
	Vector3_<T>& Quaternion_<T>::rotate(_inout(Vector3_<T>) _Vec) const 
		{
		_Vec = rotate_copy(_Vec);
		return (_Vec);
		}

	template<typename T> inline
	Vector4_<T>& Quaternion_<T>::rotate(_inout(Vector4_<T>) _Vec) const 
		{
		_Vec = rotate_copy(_Vec);
		return (_Vec);
		}

	template<typename T> inline
	Vector3_<T> Quaternion_<T>::rotate_copy(_in(Vector3_<T>) _Vec) const
		{
		auto v = Quaternion_<T>(T(1), _Vec[0]);
		auto r = (*this) * v * conj(*this);
		return ( shuffle<Vector3_<T>>(r._Mydata, 1, 2, 3) );
		/*vec3 _T = T(2) * cross(this->xyz, _Vec);
		return ( this->xyz + _T * this->w + cross(this->xyz, _T) );*/
		}

	template<typename T> inline
	Vector4_<T> Quaternion_<T>::rotate_copy(_in(Vector4_<T>) _Vec) const
		{	// r = q * v * -q
		auto v = shuffle<Quaternion_<T>>(_Vec, 3, 0, 1, 2);
		auto r = (*this) * v * conj(*this);
		return ( shuffle<Vector4_<T>>(r._Mydata, 1, 2, 3, 0) );
		}

	template<typename T> inline 
		Quaternion_<T> slerp(_in(Quaternion_<T>) q1, _in(Quaternion_<T>) q2, _in(T) t) 
		{	// { (1/sin(¦È)) * ( q1*sin((1-t)*¦È) + q2*sin(t*¦È) ) } or { q1 * cos(¦È*t) + q¡Í * sin(¦È*t) }
		T _Cos = dot(q1, q2);
		if (equal(_Cos, T(1), T(0.000002))) 
			{	// q1 q2 parallel
			return ( normalize( lerp(q1, q2, t) ) );
			}
		else 
			{
			T _Theta = acos( maxval( T(0), _Cos ) );
			T _Theta_t = _Theta * t;
			return ( q1 * cos(_Theta_t) + normalize(q2 - q1 * _Cos) * sin(_Theta_t) );
			}
		}

}// namespace clmagic

#endif