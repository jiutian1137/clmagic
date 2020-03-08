#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH_COMPLEX___QUATERNION___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH_COMPLEX___QUATERNION___H__
#include "../complex.h"

namespace clmagic {
	template<typename T> inline
		vector4<T> to_quaternion_vec(_in(unit_vector3<T>) _Axis, _in(T) _Radians)
		{	/* @_quaternion:
			         re         im         im         im     
				[ cos(¦È/2), sin(¦È/2), sin(¦È/2), sin(¦È/2) ]
			*/
		auto _Half = T(0.5) * _Radians;
		auto _Quat = vector4<T>(cos(_Half), sin(_Half), sin(_Half), sin(_Half));
		return ( _Quat * vector4<T>(T(1), _Axis[0], _Axis[1], _Axis[2]) );
		}

	// <construct>

#define quaternion_T_V template<typename T, size_t V>

	template<typename T, size_t V> inline
	quaternion<T, V>::quaternion(const T& x, const T& y, const T& z, const T& w)
		: _Myvec(x, y, z, w) {
		// 
	}

	template<typename T, size_t V> inline
	quaternion<T, V>::quaternion(const vector<4, T, V>& xyzw)
		: _Myvec(xyzw) {
		//
	}

	template<typename T, size_t V> inline
	quaternion<T, V>::quaternion(const matrix<3, 3, T, false, V>& _Matrix) {// construct by rotation matrix
		DirectX::XMQuaternionRotationMatrix( _Vec128fx4 )
			DirectX::XMQuaternionRotationMatrix
	}

	template<typename T, size_t V> inline
	quaternion<T, V>::quaternion(const matrix<4, 4, T, false, V>& _Matrix)
		{
		using namespace::DirectX;
		auto _Vec128fx4 = XMLoadFloat4x4(_matrix);
		auto _Tmp = reference_cast< vector4<T> >( XMQuaternionRotationmatrix(_Vec128fx4) );
		shuffle(_Mydata, _Tmp, 3, 0, 1, 2);
		}

	template<typename T, size_t V> inline
	quaternion<T, V>::quaternion(const unit_vector3<T, V>& _Axis, const T& _Radians) {
		auto _Tmp = to_quaternion_vec(_Axis, _Radians);
		shuffle(_Mydata, _Tmp, 0, 1, 2, 3);
	}
	// </construct>

	quaternion_T_V inline
	matrix<4, 4, T> quaternion<T, V>::to_matrix() const
		{	// convert to mat4x4
		return ( matrix<4, 4, T>(
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

	quaternion_T_V inline
	matrix<3, 3, T> quaternion<T, V>::to_matrix3x3() const
		{
		return ( matrix<3, 3, T>(
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

	quaternion_T_V inline
	vector3<T>& quaternion<T, V>::rotate(_inout(vector3<T>) _Vec) const
		{
		_Vec = rotate_copy(_Vec);
		return (_Vec);
		}

	quaternion_T_V inline
	vector4<T>& quaternion<T, V>::rotate(_inout(vector4<T>) _Vec) const
		{
		_Vec = rotate_copy(_Vec);
		return (_Vec);
		}

	quaternion_T_V inline
	vector3<T> quaternion<T, V>::rotate_copy(_in(vector3<T>) _Vec) const
		{
		auto v = Quaternion_<T>(T(1), _Vec);
		auto r = (*this) * v * clmagic::conj(*this);
		return ( shuffle<vector3<T>>(r._Mydata, 1, 2, 3) );
		/*vec3 _T = T(2) * cross(this->xyz, _Vec);
		return ( this->xyz + _T * this->w + cross(this->xyz, _T) );*/
		}

	quaternion_T_V inline
	vector4<T> quaternion<T, V>::rotate_copy(_in(vector4<T>) _Vec) const
		{	// r = q * v * -q
		auto v = shuffle<Quaternion_<T>>(_Vec, 3, 0, 1, 2);
		auto r = (*this) * v * clmagic::conj(*this);
		return ( shuffle<vector4<T>>(r._Mydata, 1, 2, 3, 0) );
		}

	quaternion_T_V inline
		quaternion<T, V> slerp(_in(quaternion<T, V>) q1, _in(quaternion<T, V>) q2, _in(T) t)
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