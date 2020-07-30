//--------------------------------------------------------------------------------------
// Copyright (c) 16.October.1843 William-Rowan-Hamilton(1805–1865)
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_calculation_complex_WILLIAM_ROWAN_HAMILTON_h_
#define clmagic_calculation_complex_WILLIAM_ROWAN_HAMILTON_h_
#include "../lapack/vector.h"
#include "../lapack/matrix.h"
#include "../physics/fundamental.h"

namespace WilliamRowanHamilton {
	/*<Biographie>http://mathshistory.st-andrews.ac.uk/Biographies/Hamilton.html</Biographie>*/

	/*
	<Memory-order>[imag[0], imag[1], imag[2], real]</Memory-order> because this can accelerates
	<Describ>
		<mean>w + xi + yj + zk</mean>
		<mean>[sin(theta), cos(theta)*axis], theta in [0, Pi]</mean> note polar(...)
		<matrix>
			matrix3x3:
			[ 1 - 2(y^2 + z^2),     s(x*y + z*w),     s(x*z - y*w) ]
			[     s(x*y - z*w), 1 - 2(x^2 + z^2),     s(y*z + x*w) ]
			[     s(x*z + y*w),     s(y*z - x*w), 1 - 2(x^2 + y^2) ], s = 2/pow(norm(q),2)
		<matrix>
	</Describ>
	*/
	template<typename _Ts, typename _Tb = _Ts>
	struct __declspec(align(std::alignment_of_v<_Tb>)) quaternion {
		using real_type = _Ts;
		using imag_type = ::clmagic::vector<3, _Ts, _Tb>;

		quaternion() = default;
		quaternion(const _Ts& _Real, const _Ts& _ImX, const _Ts& _ImY, const _Ts& _ImZ)
			: _Mydata{ _ImX, _ImY, _ImZ, _Real } {}
		quaternion(const real_type& _Real, const imag_type& _Imag)
			: _Mydata{ _Imag[0], _Imag[1], _Imag[2], _Real } {}

		const real_type& real() const {
			return _Mydata[3];
		}
		const imag_type& imag() const {
			return ( * reinterpret_cast<const imag_type*>(_Mydata) );
		}
		std::string to_string() const {
			std::array<char, 256> _Temp;
			auto _Last_index = snprintf(_Temp.data(), _Temp.size() - 1,
				"\"quaternion\":{ %s + i*%s + j*%s + k*%s }",
				std::to_string(this->real()).c_str(),
				std::to_string(this->imag()[0]).c_str(),
				std::to_string(this->imag()[1]).c_str(),
				std::to_string(this->imag()[2]).c_str());
			return std::string(&_Temp[0], &_Temp[_Last_index]);
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
			const auto& qw = this->real();
			const auto& qv = this->imag();
			const auto& rw = _Right.real();
			const auto& rv = _Right.imag();
			return quaternion(qw*rw - dot(qv, rv), cross3(qv, rv) + rw*qv + qw*rv);
		}
		
		quaternion operator*(const _Ts& _Scalar) const {
			return quaternion(real() * _Scalar, imag() * _Scalar);
		}
		quaternion operator/(const _Ts& _Scalar) const {
			return quaternion(real() / _Scalar, imag() / _Scalar);
		}
		friend quaternion operator*(const _Ts& _Scalar, const quaternion& _Right) {
			return quaternion(_Scalar * _Right.real(), _Scalar * _Right.imag());
		}
		friend quaternion operator/(const _Ts& _Scalar, const quaternion& _Right) {
			return quaternion(_Scalar / _Right.real(), _Scalar / _Right.imag());
		}

		quaternion rotate(quaternion p) const {// theta/2 in polar(...)
			const auto& q     = *this;
			const auto  q_inv = quaternion(real(), -imag());// conj(*this);
			return (q * p * q_inv);
		}
		quaternion operator()(quaternion p) const {// theta/2 in polar(...)
			return this->rotate(p);
		}
		VECTOR4    operator()(VECTOR4 p) const {
			const auto qpq = this->rotate( quaternion(static_cast<_Ts>(0), p[0], p[1], p[2]) );
			return reinterpret_cast<const VECTOR4&>(qpq);
		}
		VECTOR3    operator()(VECTOR3 p) const {
			const auto qpq = this->rotate( quaternion(static_cast<_Ts>(0), p[0], p[1], p[2]) );
			return reinterpret_cast<const VECTOR3&>(qpq);
		}

		friend std::ostream& operator<<(std::ostream& _Ostr, const quaternion& _Quat) {
			return (_Ostr << _Quat.to_string());
		}
		friend std::istream& operator>>(std::istream& _Istr, quaternion& _Quat) {
			_Istr >> _Quat.real() >> _Quat.imag();
			return _Istr;
		}
	private:
		_Ts _Mydata[4];
	};

#define QUATERNION ::WilliamRowanHamilton::quaternion<_Ts, _Tb>

	template<typename _Ts, typename _Tb> inline
	std::string to_string(const quaternion<_Ts, _Tb>& q) {
		return q.to_string();
	}

	template<typename _Ts, typename _Tb> inline
	quaternion<_Ts, _Tb> identity() {// (0, i*1, j*1, k*1)
		return quaternion<_Ts, _Tb>(static_cast<_Ts>(0), 
			static_cast<_Ts>(1), static_cast<_Ts>(1), static_cast<_Ts>(1));
	}

	template<typename _Ts, typename _Tb> inline
	quaternion<_Ts, _Tb> polar(clmagic::unit_vector3<_Ts, _Tb> _Axis, RADIANS _Angle) {
		const auto theta = static_cast<_Ts>(_Angle) / 2;// div 2, because p*q*inverse(q) is rotate 2*theta
		return quaternion<_Ts, _Tb>(cos(theta), sin(theta) * _Axis);
	}
	

	template<typename _Ts, typename _Tb> inline
	_Ts dot(const quaternion<_Ts, _Tb>& _Left, const quaternion<_Ts, _Tb>& _Right) {
		return dot(_Left.imag(), _Right.imag()) + _Left.real() + _Right.real();
	}

	template<typename _Ts, typename _Tb> inline
	_Ts norm(const quaternion<_Ts, _Tb>& _Left) {// sqrt( q * conj(q) ) = sqrt( dot(q.imag(), conj(q).imag() + pow(q.real(),2) )
		return sqrt( dot(_Left, _Left) );
	}
	
	// 1. conj(conj(q)) = q
	// 2. conj(q + r) = conj(q) + conj(r)
	// 3. conj(q*r)  = conj(r) * conj(q)
	template<typename _Ts, typename _Tb> inline
	quaternion<_Ts, _Tb> conj(const quaternion<_Ts, _Tb>& _Left) {
		return quaternion<_Ts, _Tb>(_Left.real(), -_Left.imag());
	}
	
	template<typename _Ts, typename _Tb> inline
	quaternion<_Ts, _Tb> inverse(const quaternion<_Ts, _Tb>& _Left) {// 1/norm(q)*conj(q)
		return ( static_cast<_Ts>(1)
			 /*------------------*/ * conj(_Left)
			     / norm(_Left) );
	}

	template<typename _Ts, typename _Tb> inline
	quaternion<_Ts, _Tb> normalize(const quaternion<_Ts, _Tb>& _Left) {
		const auto _Length_sqr = dot(_Left, _Left);
		const auto _Error      = _Length_sqr - static_cast<_Ts>(1);
		return (abs(_Error) < std::numeric_limits<_Ts>::epsilon()) ? _Left : _Left/sqrt(_Length_sqr);
	}

	template<typename _Ts, typename _Tb> inline
	quaternion<_Ts, _Tb> slerp(const quaternion<_Ts, _Tb>& q1, const quaternion<_Ts, _Tb>& q2, const _Ts& t) {
		// <Reference> Book<pbrt> Section<2.9.2 Quaternion Interpolation> </Reference>
		using clmagic::lerp;
		using clmagic::clamp;
		using clmagic::acos;
		using clmagic::cos;
		using clmagic::sin;
		auto cos_theta = dot(q1, q2);
		if (cos_theta > 0.9995f) {// q1 q2 parallel
			return normalize(lerp(q1, q2, t));
		} else {
			const auto theta  = acos( clamp(cos_theta, static_cast<_Ts>(-1), static_cast<_Ts>(1)) );
			const auto thetap = theta * t;
			quaternion<_Ts, _Tb> qperp = normalize(q2 - q1 * cos_theta);
			return ( q1 * cos(thetap) + qperp * sin(thetap) );
		}
	}

	// q = cos(theta) + axis*sin(theta) = pow(e, axis*theta). 

}// namespace WilliamRowanHamilton

#endif