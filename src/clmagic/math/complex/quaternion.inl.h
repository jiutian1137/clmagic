#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH_COMPLEX___QUATERNION___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH_COMPLEX___QUATERNION___H__
#include "../complex.h"

namespace clmagic 
{
	template<typename T> inline
		Vec4_<T> to_quaternion_vec(_in(Vec3_<T>) _Axis, _in(T) _Radians) 
		{
		using namespace::DirectX;
		auto _Result = XMQuaternionRotationAxis(
			simd_128f::load(static_cast<const float*>(_Axis)),
			_Radians);
		return ( reference_cast<Vec4_<T>>(_Result) );
		}

	// <construct>
	template<typename T> inline 
		Quaternion_<T>::Quaternion_(_in(Vec4_<T>) _V)
		: w(_V.w), xyz(_V.x, _V.y, _V.z) 
		{ /* constexpr */ }

	template<typename T> inline
		Quaternion_<T>::Quaternion_(_in(T) _W, _in(Vec3_<T>) _XYZ)
		: w(_W), xyz(_XYZ.x, _XYZ.y, _XYZ.z) 
		{ /* constexpr */ }

	template<typename T> inline
		Quaternion_<T>::Quaternion_(_in(Mat_<3, 3, T>) _Matrix)
		{	// construct by rotation matrix
		using namespace::DirectX;
		auto _Vec128fx4 = XMLoadFloat3x3( reference_cast<const XMFLOAT3X3>(_Matrix) );
		auto _Tmp = reference_cast< Vec4_<T> >( XMQuaternionRotationMatrix( _Vec128fx4 ) );
		this->w = _Tmp.w; this->x = _Tmp.x; this->y = _Tmp.y; this->z = _Tmp.z;
		}

	template<typename T> inline
		Quaternion_<T>::Quaternion_(_in(Mat_<4, 4, T>) _Matrix)
		{
		using namespace::DirectX;
		auto _Vec128fx4 = XMLoadFloat4x4(_Matrix);
		auto _Tmp = reference_cast<Vec4_<T>>( XMQuaternionRotationMatrix(_Vec128fx4) );
		this->w = _Tmp.w; this->x = _Tmp.x; this->y = _Tmp.y; this->z = _Tmp.z;
		}

	template<typename T> inline
		Quaternion_<T>::Quaternion_(_in(Vec3_<T>) _Axis, _in(T) _Radians)
		{
		auto _Tmp = to_quaternion_vec(_Axis, _Radians);
		this->w = _Tmp.w; this->x = _Tmp.x; this->y = _Tmp.y; this->z = _Tmp.z;
		}

	template<typename T> inline
		Quaternion_<T>::Quaternion_(_in(Vec3_<T>) _From, _in(Vec3_<T>) _To)
		{
		/*
						      _Nto
							    |
							    |
							    |
							    |
							    |
							    |    acos( dot(_Nfrom, _Nto) )
							    |\ ¨L
		 					    | \ _Radians
	    ------------------------*-------------------->-->-- _Nfrom
						      / |
						    /	|
					      /		|
cross(_Nfrom, _Nto)	->  /		|
					  /			|
					/			|
				_Axis			|
								|
		*/
		Vec3_<T> _Axis = cross(_From, _To);
		T _Radians = radians(_From, _To);
		auto _Tmp = to_quaternion_vec(_Axis, _Radians);
		this->w = _Tmp.w; this->x = _Tmp.x; this->y = _Tmp.y; this->z = _Tmp.z;
		}
	// </construct>

	// <convert>
	template<typename T> inline
		Quaternion_<T>::operator Mat_<3, 3, T>() const 
		{
		/*
		[ 1 - 2(y^2 + z^2),     2(x*y + z*w),     2(x*z - y*w) ]
		[     2(x*y - z*w), 1 - 2(x^2 + z^2),     2(y*z + x*w) ]
		[     2(x*z + y*w),     2(y*z - x*w), 1 - 2(x^2 + y^2) ]
		*/
		Mat_<3, 3, T> _Result(
			T(1) - T(2) * (y * y + z * z),        T(2) * (x * y + z * w),        T(2) * (x * z - y * w),
				   T(2) * (x * y - z * w), T(1) - T(2) * (x * x + z * z),        T(2) * (y * z + x * w),
				   T(2) * (x * z + y * w),        T(2) * (y * z - x * w), T(1) - T(2) * (x * x + y * y) );
		return ( _Result );
		}

	template<typename T> inline
		Quaternion_<T>::operator Mat_<4, 4, T>() const
		{	// convert to mat4
		/*
		[ 1 - 2(y^2 + z^2),     2(x*y + z*w),     2(x*z - y*w), 0 ]
		[     2(x*y - z*w), 1 - 2(x^2 + z^2),     2(y*z + x*w), 0 ]
		[     2(x*z + y*w),     2(y*z - x*w), 1 - 2(x^2 + y^2), 0 ]
		[                0,                0,                0, 1 ]
		*/
		Mat_<4, 4, T> _Result(
			T(1) - T(2) * (y * y + z * z),        T(2) * (x * y + z * w),        T(2) * (x * z - y * w), T(0),
				   T(2) * (x * y - z * w), T(1) - T(2) * (x * x + z * z),        T(2) * (y * z + x * w), T(0),
				   T(2) * (x * z + y * w),        T(2) * (y * z - x * w), T(1) - T(2) * (x * x + y * y), T(0),
								     T(0),					        T(0),						   T(0), T(1) );
		return ( _Result );
		}

	template<typename T> inline
		Quaternion_<T>::operator Vec4_<T>() const
		{	// convert to vec4
		return ( Vec4_<T>(this->xyz, this->w) );
		}

	template<typename T> inline
		Quaternion_<T>::operator std::string() const
		{	// convert to string
		std::string _Str = "Quaternion[" + std::to_string(w) + ", " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + "]";
		return ( _Str );
		}
	// </convert>

	template<typename T> inline
		T& Quaternion_<T>::operator[] (size_t _Pos)
		{ return ( this->ptr[_Pos] ); }

	template<typename T> inline
		const T& Quaternion_<T>::operator[] (size_t _Pos) const
		{ return ( this->ptr[_Pos] ); }

	template<typename T> inline
		Vec3_<T> Quaternion_<T>::rotate(_in(Vec3_<T>) _Vec) const
		{
		/*vec3 _T = T(2) * cross(this->xyz, _Vec);
		return ( this->xyz + _T * this->w + cross(this->xyz, _T) );*/
		return ( Vec3_<T>(this->rotate( Vec4_<T>(_Vec, T(1)) ) ) );
		}

	template<typename T> inline
		Vec4_<T> Quaternion_<T>::rotate(_in(Vec4_<T>) _Vec) const
		{
		return ( Vec4_<T>( (*this) * Quaternion(_Vec) * this->conj() ) );
		}

	template<typename T> inline
		Quaternion_<T> Quaternion_<T>::conj() const
		{
		return ( Quaternion_<T>(w, -x, -y, -z) );
		}

	template<typename T> inline
		T Quaternion_<T>::radians() const
		{	// 2*acos(w)
		return ( T(2) * std::acos( w ) );
		}

	template<typename T> inline
		T Quaternion_<T>::angle() const
		{	// (180/3.14) * 2*acos(w)
		return ( to_degrees(this->radians()) );
		}


	template<typename T> inline 
		Quaternion_<T> operator- (_in(Quaternion_<T>) _Lhs) 
		{
		}

	template<typename T> inline 
		Quaternion_<T> operator+ (_in(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs) 
		{
		return ( Quaternion_<T>(_Lhs.w + _Rhs.w, _Lhs.xyz + _Rhs.xyz) );
		}

	template<typename T> inline 
		Quaternion_<T> operator- (_in(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs) 
		{
		return ( Quaternion_<T>(_Lhs.w - _Rhs.w, _Lhs.xyz - _Rhs.xyz) );
		}

	template<typename T> inline 
		Quaternion_<T> operator* (_in(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs) 
		{
		/*
		(q1.w, q1.x, q1.y, q1.z) * (q2.w, q2.x, q2.y, q2.z)
		  w = q1.w*q2.w - (q1.xyz¡¤q2.xyz)
		xyz = q1.xyz X q2.xyz + q1.w*q2.xyz + q2.w*q1.xyz
		*/
		return ( Quaternion_<T>( _Lhs.w * _Rhs.w - dot(_Lhs.xyz, _Rhs.xyz), 
								 cross(_Lhs.xyz, _Rhs.xyz) + _Lhs.w*_Rhs.xyz + _Rhs.w*_Lhs.xyz ) );
		}

	template<typename T> inline 
		Quaternion_<T> operator/ (_in(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs) 
		{
		return ( _Lhs * (_Rhs.conj()) );
		}

	template<typename T> inline 
		Quaternion_<T> operator* (_in(Quaternion_<T>) _Lhs, _in(T) _Rhs) 
		{
		return ( Quaternion_<T>(_Lhs.w * _Rhs, _Lhs.xyz * _Rhs) );
		}

	template<typename T> inline 
		Quaternion_<T> operator/ (_in(Quaternion_<T>) _Lhs, _in(T) _Rhs) 
		{
		return ( Quaternion_<T>(_Lhs.w / _Rhs, _Lhs.xyz / _Rhs) );
		}


	template<typename T> inline 
		Quaternion_<T>& operator+= (_inout(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs) 
		{
		_Lhs = _Lhs + _Rhs;
		return ( _Lhs );
		}

	template<typename T> inline 
		Quaternion_<T>& operator-= (_inout(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs) 
		{
		return (_Lhs = _Lhs - _Rhs, _Lhs );
		}

	template<typename T> inline
		Quaternion_<T>& operator*= (_inout(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs)
		{
		_Lhs = _Lhs * _Rhs;
		return ( _Lhs );
		}

	template<typename T> inline 
		Quaternion_<T>& operator/= (_inout(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs) 
		{
		_Lhs = _Lhs / _Rhs;
		return (_Lhs);
		}

	template<typename T> inline 
		Quaternion_<T>& operator*= (_inout(Quaternion_<T>) _Lhs, _in(T) _Rhs) 
		{
		_Lhs = _Lhs * _Rhs;
		return ( _Lhs );
		}

	template<typename T> inline 
		Quaternion_<T>& operator/= (_inout(Quaternion_<T>) _Lhs, _in(T) _Rhs) 
		{
		_Lhs = _Lhs / _Rhs;
		return ( _Lhs );
		}

	template<typename T> inline 
		std::ostream& operator<< (_inout(std::ostream) _Ostr, _in(Quaternion_<T>) _Lhs) 
		{
		_Ostr << std::string(_Lhs);
		return ( _Ostr );
		}

	template<typename T> inline 
		T dot(_in(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs) 
		{
		return ( dot(_Lhs.xyz, _Rhs.xyz) + _Lhs.w * _Rhs.w );
		}

	template<typename T> inline 
		T length(_in(Quaternion_<T>) _Lhs) 
		{
		return ( sqrt( dot(_Lhs, _Lhs) ) );
		}

	template<typename T> inline 
		T length(_in(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs) 
		{
		return ( length(_Lhs - _Rhs) );
		}

	template<typename T> inline 
		Quaternion_<T> normalize(_in(Quaternion_<T>) _Lhs) 
		{
		T _Length = length( _Lhs );
		if ( equals(_Length, T(1), T(0.00002)) ) 
			{
			return ( _Lhs );
			}
		else 
			{
			return ( _Lhs / _Length );
			}
		}

	template<typename T> inline 
		Quaternion_<T> slerp(_in(Quaternion_<T>) q1, _in(Quaternion_<T>) q2, _in(T) t) 
		{	// { (1/sin(¦È)) * ( q1*sin((1-t)*¦È) + q2*sin(t*¦È) ) } or { q1 * cos(¦È*t) + q¡Í * sin(¦È*t) }
		T _Cos = dot(q1, q2);
		if (equals(_Cos, T(1), T(0.000002))) 
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