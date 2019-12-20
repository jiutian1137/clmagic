#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH___COMPLEX___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH___COMPLEX___H__
#include "general.h"
#include "lapack.h"
#include <complex>

namespace clmagic 
{
	inline std::complex<float> to_complex(float _R, float _Radians)
		{
		/*
		cos(_Theta) = cos(_Radians);
		sin(_Theta) = sin(_Radians);

		real = R * cos(_Theta)
		imag = R * sin(_Theta)
		*/
		float _Cos = std::cos(_Radians);
		float _Sin = std::sin(_Radians);
		return (std::complex<float>(_R * _Cos, _R * _Sin));
		}


	template<typename T> 
		Vec4_<T> to_quaternion_vec(_in(Vec3_<T>) _Axis, _in(T) _Radians);

	/*
	@_William Rowan Hamilton
	@_quaternion: w + xi + yj + zk
				 cos(¦È), xyz*sin(¦È)
	*/
	template<typename T>
	struct Quaternion_
	{
		// <construct>
		constexpr Quaternion_() : ptr{ T(1), T(0), T(0), T(0) } { /* constexpr */ }
		constexpr Quaternion_(_in(T) _W, _in(T) _X, _in(T) _Y, _in(T) _Z) : ptr{ _W, _X, _Y, _Z } { /* constexpr */ }
		Quaternion_(_in(Vec4_<T>) _V);
		Quaternion_(_in(T) _W, _in(Vec3_<T>) _XYZ);

		explicit Quaternion_(_in(Mat_<3, 3, T>) _Matrix);
		explicit Quaternion_(_in(Mat_<4, 4, T>) _Matrix);
		Quaternion_(_in(Vec3_<T>) _Axis, _in(T) _Radians);
		Quaternion_(_in(Vec3_<T>) _From, _in(Vec3_<T>) _To);
		// </construct>

		// <convert>
		explicit operator Mat_<3, 3, T>() const;
		explicit operator Mat_<4, 4, T>() const;
		explicit operator Vec4_<T>() const;
		explicit operator std::string() const;

		T& operator[] (size_t _Pos);
		const T& operator[] (size_t _Pos) const;
		// </convert>

		// <method>
		Vec3_<T> rotate(_in(Vec3_<T>) _Vec) const;
		Vec4_<T> rotate(_in(Vec4_<T>) _Vec) const;
		Quaternion_<T> conj() const;
		T radians() const;
		T angle() const;
		// </method>

		union 
		{ 
			struct 
			{
				T w; 
				union 
				{ 
					Vec3_<T> xyz; 
					struct { T x, y, z; };
				};
			};
			T ptr[4];
		};
	};

	using Quaternion = Quaternion_<float>;

	template<typename T> Quaternion_<T> operator- (_in(Quaternion_<T>) _Lhs);
	template<typename T> Quaternion_<T> operator+ (_in(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs);
	template<typename T> Quaternion_<T> operator- (_in(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs);
	template<typename T> Quaternion_<T> operator* (_in(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs);
	template<typename T> Quaternion_<T> operator/ (_in(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs);
	template<typename T> Quaternion_<T> operator* (_in(Quaternion_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Quaternion_<T> operator/ (_in(Quaternion_<T>) _Lhs, _in(T) _Rhs);

	template<typename T> Quaternion_<T>& operator+= (_inout(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs);
	template<typename T> Quaternion_<T>& operator-= (_inout(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs);
	template<typename T> Quaternion_<T>& operator*= (_inout(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs);
	template<typename T> Quaternion_<T>& operator/= (_inout(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs);
	template<typename T> Quaternion_<T>& operator*= (_inout(Quaternion_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Quaternion_<T>& operator/= (_inout(Quaternion_<T>) _Lhs, _in(T) _Rhs);

	template<typename T> std::ostream& operator<< (_inout(std::ostream) _Ostr, _in(Quaternion_<T>) _Lhs);

	template<typename T> T dot(_in(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs);
	
	template<typename T> T length(_in(Quaternion_<T>) _Lhs);
	template<typename T> T length(_in(Quaternion_<T>) _Lhs, _in(Quaternion_<T>) _Rhs);

	template<typename T> Quaternion_<T> normalize(_in(Quaternion_<T>) _Lhs);

	template<typename T> Quaternion_<T> slerp(_in(Quaternion_<T>) q1, _in(Quaternion_<T>) q2, _in(T) t);

}// namespace clmagic

#endif