#pragma once
#ifndef clmagic_math_COMPLEX_h_
#define clmagic_math_COMPLEX_h_
#include "real/real.h"
#include "lapack/vector.h"
#include "lapack/matrix.h"
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

	/*template<typename T> 
		vector4<T> to_quaternion_vec(_in(unit_vector3<T>) _Axis, _in(T) _Radians);*/

	/*
	@_William Rowan Hamilton (1805 - 1865)
	@_quaternion: xi + yj + zk + w
				
				 Struct:
				 cos(θ/2), xyz*sin(θ/2)
	                    
				 ?:
				 [ w-zi -x+yi ]
				 [ x+yi  w+zi ]

				 matrix3x3:
				 [ 1 - 2(y^2 + z^2),     2(x*y + z*w),     2(x*z - y*w) ]
				 [     2(x*y - z*w), 1 - 2(x^2 + z^2),     2(y*z + x*w) ]
			     [     2(x*z + y*w),     2(y*z - x*w), 1 - 2(x^2 + y^2) ]


	*/
	template<typename T = real_t, typename _Vtag = normal_vector_tag>
	struct quaternion {
		using scalar_type = T;

		// <construct>
		quaternion(const T& x = T(0), const T& y = T(0), const T& z = T(0), const T& w = T(1));
		explicit quaternion(const vector<T, 4, _Vtag>& xyzw);
		explicit quaternion(const matrix<T, 3, 3, false, _Vtag>& _Matrix);
		explicit quaternion(const matrix<T, 4, 4, false, _Vtag>& _Matrix);
		quaternion(const unit_vector3<T>& _Axis, const T& _Radians);

		quaternion(_in(vector3<T>) _From, _in(vector3<T>) _To)
			: quaternion(cross3(_From, _To), radians(_From, _To)) { }
		// </construct>

		quaternion<T>& conj() { /*x = -x; y = -y; z = -z;*/ return (*this); }
		T radians() const { return (T(2) * (T)acos(_Myvec[3])); }
		matrix<T, 4, 4> to_matrix() const;
		matrix<T, 3, 3> to_matrix3x3() const;
		vector3<T>& rotate(_inout(vector3<T>) _Vec) const;
		vector4<T>& rotate(_inout(vector4<T>) _Vec) const;
		vector3<T>  rotate_copy(_in(vector3<T>) _Vec) const;
		vector4<T>  rotate_copy(_in(vector4<T>) _Vec) const;

		friend quaternion operator+(_in(quaternion) q1, _in(quaternion) q2) { return (quaternion(q1.w + q2.w, q1.xyz + q2.xyz)); }
		friend quaternion operator-(_in(quaternion) q1, _in(quaternion) q2) { return (quaternion(q1.w - q2.w, q1.xyz - q2.xyz)); }
		friend quaternion operator*(_in(quaternion) q1, _in(quaternion) q2) {
			/*
			(q1.w, q1.x, q1.y, q1.z) * (q2.w, q2.x, q2.y, q2.z)
			  w = q1.w*q2.w - (q1.xyz¡¤q2.xyz)
			xyz = q1.xyz X q2.xyz + q1.w*q2.xyz + q2.w*q1.xyz
			*/
			/*return (quaternion<T>(q1.w * q2.w - dot(q1.xyz, q2.xyz),
				cross3(q1.xyz, q2.xyz) + q1.w * q2.xyz + q2.w * q1.xyz));*/
			return quaternion<T>();
		}
		friend quaternion operator/(_in(quaternion) q1, _in(quaternion) q2) { return (q1 * conj(q2)); }
		friend T dot(_in(quaternion) _A, _in(quaternion) _B) { return (_A.w * _B.w + dot(_A.xyz, _B.xyz)); }

		vector<T, 4, _Vtag> _Myvec;// IM:xyz RE:w
	};


	template<typename T> inline
		quaternion<T> conj(_in(quaternion<T>) _A) { return (quaternion<T>(_A.w, -_A.x, -_A.y, -_A.z)); }

	template<typename T> 
		quaternion<T> slerp(_in(quaternion<T>) q1, _in(quaternion<T>) q2, _in(T) t);

}// namespace clmagic

#endif