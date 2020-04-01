#pragma once
#ifndef clmagic_math_complex_WILLIAM_ROWAN_HAMILTON_h_
#define clmagic_math_complex_WILLIAM_ROWAN_HAMILTON_h_
#include "../lapack/vector.h"
#include "../lapack/matrix.h"
#include "../general//unit.h"
#include <string>
#include <iostream>

namespace WilliamRowanHamilton {

	/*
	@_Note: memory:{imag, real}, because this can accelerates
	@_William Rowan Hamilton (1805 - 1865)
	@_quaternion: w + xi + yj + zk
		(sin(theta), cos(theta)*axis), theta in [0, Pi], note polar(...)

		matrix3x3:
		[ 1 - 2(y^2 + z^2),     s(x*y + z*w),     s(x*z - y*w) ]
		[     s(x*y - z*w), 1 - 2(x^2 + z^2),     s(y*z + x*w) ]
		[     s(x*z + y*w),     s(y*z - x*w), 1 - 2(x^2 + y^2) ], s = 2/pow(norm(q),2)


	*/
	template<typename _Ty, typename _Block>
	struct __declspec(align(std::alignment_of_v<_Block>)) quaternion {
		using real_type = _Ty;
		using imag_type = clmagic::vector3<_Ty, _Block>;

		quaternion() = default;

		quaternion(const _Ty& _Real, const _Ty& _ImX, const _Ty& _ImY, const _Ty& _ImZ)
			: _Mydata{ _ImX, _ImY, _ImZ, _Real } {}

		quaternion(const real_type& _Real, const imag_type& _Imag)
			: quaternion(_Real, _Imag[0], _Imag[1], _Imag[2]) {}

		real_type& real() {
			return _Mydata[3];
		}
		imag_type& imag() {
			return ( * reinterpret_cast<imag_type*>(_Mydata) );
		}
		const real_type& real() const {
			return _Mydata[3];
		}
		const imag_type& imag() const {
			return ( * reinterpret_cast<const imag_type*>(_Mydata) );
		}

		quaternion operator+(const quaternion& _Right) const {
			return quaternion(this->real() + _Right.real(), this->imag() + _Right.imag());
		}
		quaternion operator-(const quaternion& _Right) const {
			return quaternion(this->real() - _Right.real(), this->imag() - _Right.imag());
		}
		quaternion operator*(const quaternion& _Right) const {
			/* (RE1 + i*IMx1 + j*IMy1 + k*IMz1) * (RE2 + i*IMx2 + j*IMy2 + k*IMz2)
			= RE1*RE2 + i*RE1*IMx2 + j*RE1*IMy2 + k*RE1*IMz2
			  + i*IMx1*RE2 + pow(i,2)*IMx1*IMx2 + i*j*IMx1*IMy2 + i*k*IMx1*IMz2
			  + j*IMy1*RE2 + j*i*IMy1*IMx2 + pow(j,2)*IMy1*IMy2 + j*k*IMy1*IMz2
			  + k*IMz1*RE2 + k*i*IMz1*IMx2 + k*j*IMz1*IMy2 + pow(k,2)*IMz1*IMz2
			
			= RE1*RE2 -(IMx1*IMx2) -(IMy1*IMy2) -(IMz1*IMz2)
			  + i*RE1*IMx2 + j*RE1*IMy2 + k*RE1*IMz2
			  + i*IMx1*RE2 + i*j*IMx1*IMy2 + i*k*IMx1*IMz2
			  + j*IMy1*RE2 + j*i*IMy1*IMx2 + j*k*IMy1*IMz2
			  + k*IMz1*RE2 + k*i*IMz1*IMx2 + k*j*IMz1*IMy2

			= RE1*RE2 -(IMx1*IMx2) -(IMy1*IMy2) -(IMz1*IMz2)
			  + i*(RE1*IMx2 + IMx1*RE2)
			  + j*(RE1*IMy2 + IMy1*RE2)
			  + k*(RE1*IMz2 + IMz1*RE2)
			  + i*j*IMx1*IMy2 + i*k*IMx1*IMz2
			  + j*i*IMy1*IMx2 + j*k*IMy1*IMz2
			  + k*i*IMz1*IMx2 + k*j*IMz1*IMy2

			= RE1*RE2 -(IMx1*IMx2) -(IMy1*IMy2) -(IMz1*IMz2)
			  + i*(RE1*IMx2 + IMx1*RE2 - IMx1*IMy2 - IMy1*IMx2)
			  + j*(RE1*IMy2 + IMy1*RE2 - IMy1*IMz2 - IMz1*IMy2)
			  + k*(RE1*IMz2 + IMz1*RE2 - IMx1*IMz2 - IMz1*IMx2)

			= RE1*RE2 - dot([IMx1 IMy1 IMz1], [IMx2 IMy2 IMz2])
			  + i*(RE1*IMx2 + IMx1*RE2 + (-IMy1*IMx2 - IMx1*IMy2))
			  + j*(RE1*IMy2 + IMy1*RE2 + (-IMz1*IMy2 - IMy1*IMz2))
			  + k*(RE1*IMz2 + IMz1*RE2 + (-IMz1*IMx2 - IMx1*IMz2))
			
			= ...

			q2 = q*r
			q2.real() = q.real()*r.real() - dot(q.imag(), r.imag())
			q2.imag() = (cross3(q.imag(), r.imag()) + q.real()*r.imag() + r.real()*q.imag())
			*/
			const auto& q_real = this->real();
			const auto& q_imag = this->imag();
			const auto& r_real = _Right.real();
			const auto& r_imag = _Right.imag();
			return quaternion(q_real * r_real - dot(q_imag, r_imag),
				cross3(q_imag, r_imag) + q_real*r_imag + r_real*q_imag);
		}
		quaternion operator*(const _Ty& _Scalar) const {
			return quaternion(real() * _Scalar, imag() * _Scalar);
		}
		quaternion operator/(const _Ty& _Scalar) const {
			return quaternion(real() / _Scalar, imag() / _Scalar);
		}
		friend quaternion operator*(const _Ty& _Scalar, const quaternion& _Right) {
			return quaternion(_Scalar * _Right.real(), _Scalar * _Right.imag());
		}

		quaternion operator()(const quaternion& p) const {// theta/2 in polar(...)
			const auto& q     = *this;
			const auto  q_inv = quaternion(real(), -imag());// conj(*this);
			return (q * p * q_inv);
		}
		clmagic::vector4<_Ty, _Block> operator()(const clmagic::vector4<_Ty, _Block>& p) const {
			const auto qpq = (*this)( reinterpret_cast<const quaternion&>(p) );
			return reinterpret_cast<const clmagic::vector4<_Ty, _Block>&>(qpq);
		}
		clmagic::vector3<_Ty, _Block> operator()(const clmagic::vector3<_Ty, _Block>& p) const {
			const auto qpq = (*this)(quaternion(static_cast<_Ty>(1), p[0], p[1], p[2]));
			return reinterpret_cast<const clmagic::vector3<_Ty, _Block>&>(qpq);
		}

		friend std::string to_string(const quaternion& _Quat) {
			std::array<char, 256> _Temp;
			auto _Last_index = snprintf(_Temp.data(), _Temp.size() - 1,
				"[%s + i*%s + j*%s + k*%s]",
				std::to_string(_Quat.real()).c_str(),
				std::to_string(_Quat.imag()[0]).c_str(),
				std::to_string(_Quat.imag()[1]).c_str(),
				std::to_string(_Quat.imag()[2]).c_str() );
			return std::string(&_Temp[0], &_Temp[_Last_index]);
		}
		friend std::ostream& operator<<(std::ostream& _Ostr, const quaternion& _Quat) {
			return (_Ostr << to_string(_Quat));
		}
	
	private:
		_Ty _Mydata[4];
	};


	template<typename _Ty, typename _Block> inline
	_Ty real(const quaternion<_Ty, _Block>& _Left) {
		return _Left.real();
	}

	template<typename _Ty, typename _Block> inline
	clmagic::vector3<_Ty, _Block> imag(const quaternion<_Ty, _Block>& _Left) {
		return _Left.imag();
	}

	template<typename _Ty, typename _Block> inline
	quaternion<_Ty, _Block> identity() {// (0, i*1, j*1, k*1)
		return quaternion<_Ty, _Block>(static_cast<_Ty>(0), 
			static_cast<_Ty>(1), static_cast<_Ty>(1), static_cast<_Ty>(1));
	}

	// 1. conj(conj(q)) = q
	// 2. conj(q + r) = conj(q) + conj(r)
	// 3. conj(q*r)  = conj(r) * conj(q)
	template<typename _Ty, typename _Block> inline
	quaternion<_Ty, _Block> conj(const quaternion<_Ty, _Block>& _Left) {
		return quaternion<_Ty, _Block>(_Left.real(), -_Left.imag());
	}

	template<typename _Ty, typename _Block> inline
	quaternion<_Ty, _Block> inverse(const quaternion<_Ty, _Block>& _Left) {
		/*
		inverse(q) = 1/norm(q)*conj(q)
		*/
		return static_cast<_Ty>(1)
			 /*------------------*/ * conj(_Left)
			     / norm(_Left);
	}

	template<typename _Ty, typename _Block> inline
	quaternion<_Ty, _Block> polar(const clmagic::unit_vector3<_Ty, _Block>& _Axis, clmagic::radians _Angle) {
		const auto theta = static_cast<_Ty>(_Angle / 2);// div 2, because p*q*inverse(q) is rotate 2*theta
		return quaternion<_Ty, _Block>(cos(theta), sin(theta) * _Axis);
	}

	template<typename _Ty, typename _Block> inline
	_Ty norm(const quaternion<_Ty, _Block>& _Left) {// sqrt(q*conj(q))
		return sqrt(_Left.real() * _Left.real() + normL2_square(_Left.imag()));
	}

	// q = cos(theta) + axis*sin(theta) = pow(e, axis*theta). 


	template<typename _Ty, size_t _Rows, typename _Block, bool _Major = clmagic::_COL_MAJOR_>
	struct rotation {/*default matrix3x3*/
		static_assert(_Rows == 3 || _Rows == 4, "->[::Euler::rotation<_Ty, _Rows, ...>]");

		using matrix_type = clmagic::square_matrix<_Ty, _Rows, _Block, _Major>;

		static matrix_type get_matrix(const quaternion<_Ty, _Block>& q) {
			const auto qx = q.imag()[0];
			const auto qy = q.imag()[1];
			const auto qz = q.imag()[2];
			const auto qw = q.real();
			const auto s  = static_cast<_Ty>(2)/(qx*qx + qy*qy + qz*qz + qw*qw);
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					_Ty(1)-s*(qy*qy+qz*qz),        s*(qx*qy-qw*qz),        s*(qx*qz+qw*qy),
					       s*(qx*qy+qw*qz), _Ty(1)-s*(qx*qx+qz*qz),        s*(qy*qz-qw*qx),
						   s*(qx*qz-qw*qy),		   s*(qy*qz+qw*qx), _Ty(1)-s*(qx*qx+qy*qy) };
			} else {
				return matrix_type{
					_Ty(1)-s*(qy*qy+qz*qz),        s*(qx*qy+qw*qz),        s*(qx*qz-qw*qy),
					       s*(qx*qy-qw*qz), _Ty(1)-s*(qx*qx+qz*qz),        s*(qy*qz+qw*qx),
						   s*(qx*qz+qw*qy),		   s*(qy*qz-qw*qx), _Ty(1)-s*(qx*qx+qy*qy) };
			}
		}
	};

	template<typename _Ty, typename _Block, bool _Major>
	struct rotation<_Ty, 4, _Block, _Major> {/*matrix4x4*/
		using matrix_type = clmagic::square_matrix<_Ty, 4, _Block, _Major>;
		
		static matrix_type get_matrix(const quaternion<_Ty, _Block>& q) {
			const auto qx = q.imag()[0];
			const auto qy = q.imag()[1];
			const auto qz = q.imag()[2];
			const auto qw = q.real();
			const auto s  = static_cast<_Ty>(2)/(qx*qx + qy*qy + qz*qz + qw*qw);
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					_Ty(1)-s*(qy*qy+qz*qz),        s*(qx*qy-qw*qz),        s*(qx*qz+qw*qy), (_Ty)0,
					       s*(qx*qy+qw*qz), _Ty(1)-s*(qx*qx+qz*qz),        s*(qy*qz-qw*qx), (_Ty)0,
						   s*(qx*qz-qw*qy),		   s*(qy*qz+qw*qx), _Ty(1)-s*(qx*qx+qy*qy), (_Ty)0,
					       (_Ty)0,                 (_Ty)0,                 (_Ty)0,          (_Ty)1 };
			} else {
				return matrix_type{
					_Ty(1)-s*(qy*qy+qz*qz),        s*(qx*qy+qw*qz),        s*(qx*qz-qw*qy), (_Ty)0,
					       s*(qx*qy-qw*qz), _Ty(1)-s*(qx*qx+qz*qz),        s*(qy*qz+qw*qx), (_Ty)0,
						   s*(qx*qz+qw*qy),		   s*(qy*qz-qw*qx), _Ty(1)-s*(qx*qx+qy*qy), (_Ty)0,
						   (_Ty)0,                 (_Ty)0,                 (_Ty)0,          (_Ty)1 };
			}
		}
	};

}// WilliamRowanHamilton

#endif