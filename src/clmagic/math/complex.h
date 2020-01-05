#pragma once
#ifndef clmagic_math_COMPLEX_h_
#define clmagic_math_COMPLEX_h_
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
		Vector4_<T> to_quaternion_vec(_in(Normal_<T>) _Axis, _in(T) _Radians);

	/*
	@_William Rowan Hamilton (1805 - 1865)
	@_quaternion: w + xi + yj + zk
				
				 Struct:
				 cos(¦È/2), xyz*sin(¦È/2)
	                    
				 ?:
				 [ w-zi -x+yi ]
				 [ x+yi  w+zi ]

				 Mat3x3:
				 [ 1 - 2(y^2 + z^2),     2(x*y + z*w),     2(x*z - y*w) ]
				 [     2(x*y - z*w), 1 - 2(x^2 + z^2),     2(y*z + x*w) ]
			     [     2(x*z + y*w),     2(y*z - x*w), 1 - 2(x^2 + y^2) ]
	*/
	template<typename T>
#if defined(clmagic_using_SIMDMath)
	struct _declspec(align(16)) Quaternion_ {
#else
	struct Quaternion_ {
#endif
		// <construct>
		constexpr Quaternion_(_in(T) W = T(1), _in(T) X = T(0), _in(T) Y = T(0), _in(T) Z = T(0)) 
			: _Mydata{ W, X, Y, Z } { }
		Quaternion_(_in(T) W, _in(Vector3_<T>) XYZ)
			: _Mydata{ W, XYZ[0], XYZ[1], XYZ[2] } {  }

		explicit Quaternion_(_in(Mat_<3, 3, T>) _Matrix);
		explicit Quaternion_(_in(Mat_<4, 4, T>) _Matrix);
		Quaternion_(_in(Normal_<T>) _Axis, _in(T) _Radians);

		Quaternion_(_in(Vector3_<T>) _From, _in(Vector3_<T>) _To)
			: Quaternion_(cross3(_From, _To), radians(_From, _To)) { }
		// </construct>

		Quaternion_<T>& conj() { x = -x; y = -y; z = -z; return (*this); }
		T radians() const { return (T(2) * (T)acos(w)); }
		Mat_<4, 4, T> to_matrix() const;
		Mat_<3, 3, T> to_matrix3x3() const;
		Vector3_<T>& rotate(_inout(Vector3_<T>) _Vec) const;
		Vector4_<T>& rotate(_inout(Vector4_<T>) _Vec) const;
		Vector3_<T>  rotate_copy(_in(Vector3_<T>) _Vec) const;
		Vector4_<T>  rotate_copy(_in(Vector4_<T>) _Vec) const;


		friend Quaternion_ conj(_in(Quaternion_) _A) { return (Quaternion_(_A.w, -_A.x, -_A.y, -_A.z)); }
		friend Quaternion_ operator+(_in(Quaternion_) q1, _in(Quaternion_) q2) { return (Quaternion_(q1.w + q2.w, q1.xyz + q2.xyz)); }
		friend Quaternion_ operator-(_in(Quaternion_) q1, _in(Quaternion_) q2) { return (Quaternion_(q1.w - q2.w, q1.xyz - q2.xyz)); }
		friend Quaternion_ operator*(_in(Quaternion_) q1, _in(Quaternion_) q2) { 
			/*
			(q1.w, q1.x, q1.y, q1.z) * (q2.w, q2.x, q2.y, q2.z)
			  w = q1.w*q2.w - (q1.xyz¡¤q2.xyz)
			xyz = q1.xyz X q2.xyz + q1.w*q2.xyz + q2.w*q1.xyz
			*/
			return (Quaternion_<T>(q1.w * q2.w - dot(q1.xyz, q2.xyz),
				cross(q1.xyz, q2.xyz) + q1.w * q2.xyz + q1.w * q2.xyz));
		}
		friend Quaternion_ operator/(_in(Quaternion_) q1, _in(Quaternion_) q2) { return (q1 * conj(q2)); }
		friend T dot(_in(Quaternion_) _A, _in(Quaternion_) _B) { return (_A.w * _B.w + dot(_A.xyz, _B.xyz)); }

		union { 
			struct {
				T w; 
				union { 
					Vec_<3, T> xyz; 
					struct { T x, y, z; };
				};
			};
			T _Mydata[4];
		};
	};

	using Quaternion = Quaternion_<float>;

	template<typename T> 
		Quaternion_<T> slerp(_in(Quaternion_<T>) q1, _in(Quaternion_<T>) q2, _in(T) t);

}// namespace clmagic

#endif