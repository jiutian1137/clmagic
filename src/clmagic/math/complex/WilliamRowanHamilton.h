#pragma once
#ifndef clmagic_math_complex_WILLIAM_ROWAN_HAMILTON_h_
#define clmagic_math_complex_WILLIAM_ROWAN_HAMILTON_h_
#include "../lapack/vector.h"
#include "../lapack/matrix.h"
#include "../general/unit.h"
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
	template<typename _SclTy, typename _BlkTy = _SclTy>
	struct __declspec(align(std::alignment_of_v<_BlkTy>)) quaternion {
		using real_type = _SclTy;
		using imag_type = clmagic::vector3<_SclTy, _BlkTy>;

		quaternion() = default;

		quaternion(const _SclTy& _Real, const _SclTy& _ImX, const _SclTy& _ImY, const _SclTy& _ImZ)
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
		quaternion operator*(const _SclTy& _Scalar) const {
			return quaternion(real() * _Scalar, imag() * _Scalar);
		}
		quaternion operator/(const _SclTy& _Scalar) const {
			return quaternion(real() / _Scalar, imag() / _Scalar);
		}
		friend quaternion operator*(const _SclTy& _Scalar, const quaternion& _Right) {
			return quaternion(_Scalar * _Right.real(), _Scalar * _Right.imag());
		}
		friend quaternion operator/(const _SclTy& _Scalar, const quaternion& _Right) {
			return quaternion(_Scalar / _Right.real(), _Scalar / _Right.imag());
		}

		quaternion operator()(const quaternion& p) const {// theta/2 in polar(...)
			const auto& q     = *this;
			const auto  q_inv = quaternion(real(), -imag());// conj(*this);
			return (q * p * q_inv);
		}
		clmagic::vector4<_SclTy, _BlkTy> operator()(const clmagic::vector4<_SclTy, _BlkTy>& p) const {
			const auto qpq = (*this)( reinterpret_cast<const quaternion&>(p) );
			return reinterpret_cast<const clmagic::vector4<_SclTy, _BlkTy>&>(qpq);
		}
		clmagic::vector3<_SclTy, _BlkTy> operator()(const clmagic::vector3<_SclTy, _BlkTy>& p) const {
			const auto qpq = (*this)(quaternion(static_cast<_SclTy>(1), p[0], p[1], p[2]));
			return reinterpret_cast<const clmagic::vector3<_SclTy, _BlkTy>&>(qpq);
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
		_SclTy _Mydata[4];
	};


	template<typename _SclTy, typename _BlkTy> inline
	quaternion<_SclTy, _BlkTy> identity() {// (0, i*1, j*1, k*1)
		return quaternion<_SclTy, _BlkTy>(static_cast<_SclTy>(0), 
			static_cast<_SclTy>(1), static_cast<_SclTy>(1), static_cast<_SclTy>(1));
	}

	template<typename _SclTy, typename _BlkTy> inline
	quaternion<_SclTy, _BlkTy> polar(const clmagic::unit_vector3<_SclTy, _BlkTy>& _Axis, clmagic::radians _Angle) {
		const auto theta = static_cast<_SclTy>(_Angle / 2);// div 2, because p*q*inverse(q) is rotate 2*theta
		return quaternion<_SclTy, _BlkTy>(cos(theta), sin(theta) * _Axis);
	}
	
	// 1. conj(conj(q)) = q
	// 2. conj(q + r) = conj(q) + conj(r)
	// 3. conj(q*r)  = conj(r) * conj(q)
	template<typename _SclTy, typename _BlkTy> inline
	quaternion<_SclTy, _BlkTy> conj(const quaternion<_SclTy, _BlkTy>& _Left) {
		return quaternion<_SclTy, _BlkTy>(_Left.real(), -_Left.imag());
	}

	template<typename _SclTy, typename _BlkTy> inline
	_SclTy dot(const quaternion<_SclTy, _BlkTy>& _Left, const quaternion<_SclTy, _BlkTy>& _Right) {
		return dot(_Left.imag(), _Right.imag()) + _Left.real() + _Right.real();
	}

	template<typename _SclTy, typename _BlkTy> inline
	_SclTy norm(const quaternion<_SclTy, _BlkTy>& _Left) {// sqrt( q * conj(q) ) = sqrt( dot(q.imag(), conj(q).imag() + pow(q.real(),2) )
		return sqrt( dot(_Left, _Left) );
	}
	
	template<typename _SclTy, typename _BlkTy> inline
	quaternion<_SclTy, _BlkTy> inverse(const quaternion<_SclTy, _BlkTy>& _Left) {// 1/norm(q)*conj(q)
		return static_cast<_SclTy>(1)
			 /*------------------*/ * conj(_Left)
			     / norm(_Left);
	}

	template<typename _SclTy, typename _BlkTy> inline
	quaternion<_SclTy, _BlkTy> normalize(const quaternion<_SclTy, _BlkTy>& _Left) {
		const auto _Dot = dot(_Left, _Left);
		return clmagic::approach_equal(_Dot, static_cast<_SclTy>(1), std::numeric_limits<_SclTy>::epsilon()) ?
			_Left :
			_Left / sqrt(_Dot);
	}

	template<typename _SclTy, typename _BlkTy> inline
	quaternion<_SclTy, _BlkTy> slerp(const quaternion<_SclTy, _BlkTy>& q1, const quaternion<_SclTy, _BlkTy>& q2, const _SclTy& t) {
		// from Book<<pbrt>> -2.9.2 Quaternion Interpolation
		auto cos_theta = dot(q1, q2);
		if (cos_theta > static_cast<_SclTy>(0.9995f)) {	// q1 q2 parallel
			return normalize(clmagic::lerp(q1, q2, t));
		} else {
			const auto theta  = clmagic::acos( clmagic::clamp(cos_theta, static_cast<_SclTy>(-1), static_cast<_SclTy>(1)) );
			const auto thetap = theta * t;
			quaternion<_SclTy, _BlkTy> qperp = normalize(q2 - q1 * cos_theta);
			return ( q1 * clmagic::cos(thetap) + qperp * clmagic::sin(thetap) );
		}
	}

	// q = cos(theta) + axis*sin(theta) = pow(e, axis*theta). 


	template<typename _SclTy, size_t _Rows, typename _BlkTy, bool _Major = clmagic::_COL_MAJOR_>
	struct rotation {/*default matrix3x3*/
		static_assert(_Rows == 3 || _Rows == 4, "->[::Euler::rotation<_SclTy, _Rows, ...>]");

		using matrix_type = clmagic::square_matrix<_SclTy, _Rows, _BlkTy, _Major>;

		static matrix_type get_matrix(const quaternion<_SclTy, _BlkTy>& q) {
			const auto qx = q.imag()[0];
			const auto qy = q.imag()[1];
			const auto qz = q.imag()[2];
			const auto qw = q.real();
			const auto s  = static_cast<_SclTy>(2)/(qx*qx + qy*qy + qz*qz + qw*qw);
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					_SclTy(1)-s*(qy*qy+qz*qz),        s*(qx*qy-qw*qz),        s*(qx*qz+qw*qy),
					       s*(qx*qy+qw*qz), _SclTy(1)-s*(qx*qx+qz*qz),        s*(qy*qz-qw*qx),
						   s*(qx*qz-qw*qy),		   s*(qy*qz+qw*qx), _SclTy(1)-s*(qx*qx+qy*qy) };
			} else {
				return matrix_type{
					_SclTy(1)-s*(qy*qy+qz*qz),        s*(qx*qy+qw*qz),        s*(qx*qz-qw*qy),
					       s*(qx*qy-qw*qz), _SclTy(1)-s*(qx*qx+qz*qz),        s*(qy*qz+qw*qx),
						   s*(qx*qz+qw*qy),		   s*(qy*qz-qw*qx), _SclTy(1)-s*(qx*qx+qy*qy) };
			}
		}
	
		static quaternion<_SclTy, _BlkTy> get_quaternion(const matrix_type& M) {
			/*
			s = 2/pow(norm(q),2)

			tr(M) = 4-2s(qx*qx+qy*qy+qz*qz), 
			      = 4-4*(qx*qx+qy*qy+qz*qz)/pow(norm(q),2)
				  = 4 * (1 - (qx*qx+qy*qy+qz*qz)/pow(norm(q),2))
				  = 4 * (1 - (qx*qx+qy*qy+qz*qz)/(qx*qx+qy*qy+qz*qz+qw*qw))
				  = 4 * ((qw*qw)/(qx*qx+qy*qy+qz*qz+qw*qw))
				  = 4*qw*qw / pow(norm(q),2)
			4*qw*qw = tr(M)
			qw = sqrt( tr(M)/4 ) <------------

			M[2,0]-M[0,2] = s*(qx*qz+qw*qy)-s*(qx*qz-qw*qy)
			              = s*qx*qz + s*qw*qy - s*qx*qz + s*qw*qy
					      = 2*s*qw*qy
			qy = (M[2,0]-M[0,2])/(2*s*qw)
			   = (M[2,0]-M[0,2])/(4/pow(norm(q),2)*qw)
			qy = (M[2,0]-M[0,2])/(4*qw) <------------

			M[0,1]-M[1,0] = s*(qx*qy+qw*qz) - s*(qx*qy-qw*qz)
			              = s*qx*qy + s*qw*qz - s*qx*qy + s*qw*qz
						  = 2*s*qw*qz
			qz = (M[0,1]-M[1,0])/(2*s*qw)
			   = (M[0,1]-M[1,0])/(4/pow(norm(q),2)*qw)
			qz = (M[0,1]-M[1,0])/(4*qw) <------------

			M[1,2]-M[2,1] = s*(qy*qz+qw*qx)-s*(qy*qz-qw*qx)
			              = s*qy*qz + s*qw*qx - s*qy*qz + s*qw*qx
						  = 2*s*qw*qx
			qx = (M[1,2]-M[2,1]) / (2*s*qw)
			   = (M[1,2]-M[2,1]) / (4*qw) <------------
			*/
			const auto M00 = M.at(0, 0);
			const auto M11 = M.at(1, 1);
			const auto M22 = M.at(2, 2);
			const auto qw  = sqrt( M00+M11+M22+(_SclTy)1 ) / (_SclTy)2;

			if ( qw > std::numeric_limits<_SclTy>::epsilon() ) {
				const auto qw_mul4 = qw * (_SclTy)4;
				if _CONSTEXPR_IF(matrix_type::col_major()) {
					const auto qx = (M.at(2,1) - M.at(1,2)) / qw_mul4;
					const auto qy = (M.at(0,2) - M.at(2,0)) / qw_mul4;
					const auto qz = (M.at(1,0) - M.at(0,1)) / qw_mul4;
					return quaternion<_SclTy, _BlkTy>(qw, qx, qy, qz);
				} else {
					const auto qx = (M.at(1,2) - M.at(2,1)) / qw_mul4;
					const auto qy = (M.at(2,0) - M.at(0,2)) / qw_mul4;
					const auto qz = (M.at(0,1) - M.at(1,0)) / qw_mul4;
					return quaternion<_SclTy, _BlkTy>(qw, qx, qy, qz);
				}
			} else {
				const auto M33 = (_SclTy)1;
				const auto qx = sqrt( (+M00 - M11 - M22 + M33) / (_SclTy)4 );
				const auto qy = sqrt( (-M00 + M11 - M22 + M33) / (_SclTy)4 );
				const auto qz = sqrt( (-M00 - M11 + M22 + M33) / (_SclTy)4 );
				return quaternion<_SclTy, _BlkTy>(qw, qx, qy, qz);
			}
		}
	};

	template<typename _SclTy, typename _BlkTy, bool _Major>
	struct rotation<_SclTy, 4, _BlkTy, _Major> {/*matrix4x4*/
		using matrix_type = clmagic::square_matrix<_SclTy, 4, _BlkTy, _Major>;
		
		static matrix_type get_matrix(const quaternion<_SclTy, _BlkTy>& q) {
			const auto qx = q.imag()[0];
			const auto qy = q.imag()[1];
			const auto qz = q.imag()[2];
			const auto qw = q.real();
			const auto s  = static_cast<_SclTy>(2)/(qx*qx + qy*qy + qz*qz + qw*qw);
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					_SclTy(1)-s*(qy*qy+qz*qz),        s*(qx*qy-qw*qz),        s*(qx*qz+qw*qy), (_SclTy)0,
					       s*(qx*qy+qw*qz), _SclTy(1)-s*(qx*qx+qz*qz),        s*(qy*qz-qw*qx), (_SclTy)0,
						   s*(qx*qz-qw*qy),		   s*(qy*qz+qw*qx), _SclTy(1)-s*(qx*qx+qy*qy), (_SclTy)0,
					       (_SclTy)0,                 (_SclTy)0,                 (_SclTy)0,          (_SclTy)1 };
			} else {
				return matrix_type{
					_SclTy(1)-s*(qy*qy+qz*qz),        s*(qx*qy+qw*qz),        s*(qx*qz-qw*qy), (_SclTy)0,
					       s*(qx*qy-qw*qz), _SclTy(1)-s*(qx*qx+qz*qz),        s*(qy*qz+qw*qx), (_SclTy)0,
						   s*(qx*qz+qw*qy),		   s*(qy*qz-qw*qx), _SclTy(1)-s*(qx*qx+qy*qy), (_SclTy)0,
						   (_SclTy)0,                 (_SclTy)0,                 (_SclTy)0,          (_SclTy)1 };
			}
		}
	
		static quaternion<_SclTy, _BlkTy> get_quaternion(const matrix_type& M) {
			const auto M00 = M.at(0, 0);
			const auto M11 = M.at(1, 1);
			const auto M22 = M.at(2, 2);
			const auto qw  = sqrt( ( M00+M11+M22+(_SclTy)1 ) / (_SclTy)4 );

			if ( qw > std::numeric_limits<_SclTy>::epsilon() ) {
				const auto qw_mul4 = qw * (_SclTy)4;
				if _CONSTEXPR_IF(matrix_type::col_major()) {
					const auto qx = (M.at(2,1) - M.at(1,2)) / qw_mul4;
					const auto qy = (M.at(0,2) - M.at(2,0)) / qw_mul4;
					const auto qz = (M.at(1,0) - M.at(0,1)) / qw_mul4;
					return quaternion<_SclTy, _BlkTy>(qw, qx, qy, qz);
				} else {
					const auto qx = (M.at(1,2) - M.at(2,1)) / qw_mul4;
					const auto qy = (M.at(2,0) - M.at(0,2)) / qw_mul4;
					const auto qz = (M.at(0,1) - M.at(1,0)) / qw_mul4;
					return quaternion<_SclTy, _BlkTy>(qw, qx, qy, qz);
				}
			} else {
				const auto M33 = (_SclTy)1;
				const auto qx = sqrt( (+M00 - M11 - M22 + M33) / (_SclTy)4 );
				const auto qy = sqrt( (-M00 + M11 - M22 + M33) / (_SclTy)4 );
				const auto qz = sqrt( (-M00 - M11 + M22 + M33) / (_SclTy)4 );
				return quaternion<_SclTy, _BlkTy>(qw, qx, qy, qz);
			}

		}
	};

}// WilliamRowanHamilton

#endif