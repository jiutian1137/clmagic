//--------------------------------------------------------------------------------------
// Copyright (c) 16.October.1843 William-Rowan-Hamilton(1805–1865)
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_calculation_complex_WILLIAM_ROWAN_HAMILTON_h_
#define clmagic_calculation_complex_WILLIAM_ROWAN_HAMILTON_h_
#include "../lapack/vector.h"
#include "../lapack/matrix.h"
#include "../real/radians.h"

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
		using imag_type = ::clmagic::vector<_Ts, 3, _Tb>;

		quaternion() = default;
		quaternion(const _Ts& _Real, const _Ts& _ImX, const _Ts& _ImY, const _Ts& _ImZ)
			: _Mydata{ _ImX, _ImY, _ImZ, _Real } {}
		quaternion(const real_type& _Real, const imag_type& _Imag)
			: _Mydata{ _Imag[0], _Imag[1], _Imag[2], _Real } {}

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
		std::string to_string() const {
			std::array<char, 256> _Temp;
			auto _Last_index = snprintf(_Temp.data(), _Temp.size() - 1,
				"[%s + i*%s + j*%s + k*%s]",
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
			const auto& q_real = this->real();
			const auto& q_imag = this->imag();
			const auto& r_real = _Right.real();
			const auto& r_imag = _Right.imag();
			return quaternion(q_real * r_real - dot(q_imag, r_imag),
				cross3(q_imag, r_imag) + q_real*r_imag + r_real*q_imag);
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

		quaternion operator()(const quaternion& p) const {// theta/2 in polar(...)
			const auto& q     = *this;
			const auto  q_inv = quaternion(real(), -imag());// conj(*this);
			return (q * p * q_inv);
		}
		VECTOR4    operator()(VECTOR4 p) const {
			const auto qpq = (*this)( reinterpret_cast<const quaternion&>(p) );
			return reinterpret_cast<const VECTOR4&>(qpq);
		}
		VECTOR3    operator()(VECTOR3 p) const {
			const auto qpq = (*this)(quaternion(static_cast<_Ts>(1), p[0], p[1], p[2]));
			return reinterpret_cast<const VECTOR3&>(qpq);
		}

		friend std::ostream& operator<<(std::ostream& _Ostr, const quaternion& _Quat) {
			return (_Ostr << _Quat.to_string());
		}
	
	private:
		_Ts _Mydata[4];
	};

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
	quaternion<_Ts, _Tb> polar(clmagic::unit_vector3<_Ts, _Tb> _Axis, clmagic::radians<_Ts> _Angle) {
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
		const auto _Dot = dot(_Left, _Left);
		return clmagic::approach_equal(_Dot, static_cast<_Ts>(1), std::numeric_limits<_Ts>::epsilon()) ?
			_Left :
			_Left / sqrt(_Dot);
	}

	template<typename _Ts, typename _Tb> inline
	quaternion<_Ts, _Tb> slerp(const quaternion<_Ts, _Tb>& q1, const quaternion<_Ts, _Tb>& q2, const _Ts& t) {
		// from Book<<pbrt>> -2.9.2 Quaternion Interpolation
		auto cos_theta = dot(q1, q2);
		if (cos_theta > static_cast<_Ts>(0.9995f)) {	// q1 q2 parallel
			return normalize(clmagic::lerp(q1, q2, t));
		} else {
			const auto theta  = clmagic::acos( clmagic::clamp(cos_theta, static_cast<_Ts>(-1), static_cast<_Ts>(1)) );
			const auto thetap = theta * t;
			quaternion<_Ts, _Tb> qperp = normalize(q2 - q1 * cos_theta);
			return ( q1 * clmagic::cos(thetap) + qperp * clmagic::sin(thetap) );
		}
	}

	// q = cos(theta) + axis*sin(theta) = pow(e, axis*theta). 

	/*<Reference>RealTimeRendering-4th-Edition.</Reference>*/
	template<typename _Tm>
	struct rotation {// default common-function
		template<typename _Tq, typename _Ts/*auto*/, typename _TTm/*auto*/>
		static _Tq _Get_quaternion(_Ts qw, const _TTm& M) {
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
			const auto qw_mul4 = qw * static_cast<_Ts>(4);

			if _CONSTEXPR_IF( M.col_major() ) {
				const auto qx = (M.at(2,1) - M.at(1,2)) / qw_mul4;
				const auto qy = (M.at(0,2) - M.at(2,0)) / qw_mul4;
				const auto qz = (M.at(1,0) - M.at(0,1)) / qw_mul4;
				return _Tq(qw, qx, qy, qz);
			} else {
				const auto qx = (M.at(1,2) - M.at(2,1)) / qw_mul4;
				const auto qy = (M.at(2,0) - M.at(0,2)) / qw_mul4;
				const auto qz = (M.at(0,1) - M.at(1,0)) / qw_mul4;
				return _Tq(qw, qx, qy, qz);
			}
		}
		
		template<typename _Tq, typename _Ts/*auto*/>
		static _Tq _Get_quaternion_0qw(_Ts qw, _Ts M00, _Ts M11, _Ts M22) {
			using clmagic::sqrt;
			const auto M33 = static_cast<_Ts>(1);
			const auto qx  = sqrt((+M00 - M11 - M22 + M33) / static_cast<_Ts>(4));
			const auto qy  = sqrt((-M00 + M11 - M22 + M33) / static_cast<_Ts>(4));
			const auto qz  = sqrt((-M00 - M11 + M22 + M33) / static_cast<_Ts>(4));
			return _Tq(qw, qx, qy, qz);
		}
	};

	template<typename _Ts, typename _Tb, bool _Major>
	struct rotation< clmagic::matrix3x3<_Ts, _Tb, _Major> > {// matrix3x3
		
		using matrix_type     = clmagic::matrix3x3<_Ts, _Tb, _Major>;
		using quaternion_type = WilliamRowanHamilton::quaternion<_Ts, _Tb>;

		static matrix_type get_matrix(const quaternion_type& q) {
			const auto qx = q.imag()[0];
			const auto qy = q.imag()[1];
			const auto qz = q.imag()[2];
			const auto qw = q.real();
			const auto s  = static_cast<_Ts>(2)/(qx*qx + qy*qy + qz*qz + qw*qw);
			
			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					_Ts(1)-s*(qy*qy+qz*qz),        s*(qx*qy-qw*qz),        s*(qx*qz+qw*qy),
					       s*(qx*qy+qw*qz), _Ts(1)-s*(qx*qx+qz*qz),        s*(qy*qz-qw*qx),
						   s*(qx*qz-qw*qy),		   s*(qy*qz+qw*qx), _Ts(1)-s*(qx*qx+qy*qy) };
			} else {
				return matrix_type{
					_Ts(1)-s*(qy*qy+qz*qz),        s*(qx*qy+qw*qz),        s*(qx*qz-qw*qy),
					       s*(qx*qy-qw*qz), _Ts(1)-s*(qx*qx+qz*qz),        s*(qy*qz+qw*qx),
						   s*(qx*qz+qw*qy),		   s*(qy*qz-qw*qx), _Ts(1)-s*(qx*qx+qy*qy) };
			}
		}
	
		static quaternion_type get_quaternion(const matrix_type& M) {
			const auto M00 = M.at(0, 0);
			const auto M11 = M.at(1, 1);
			const auto M22 = M.at(2, 2);
			const auto qw  = clmagic::sqrt( ( M00+M11+M22+static_cast<_Ts>(1) ) / static_cast<_Ts>(4) );

			if ( qw > std::numeric_limits<_Ts>::epsilon() ) {
				return rotation<void>::_Get_quaternion<quaternion_type>(qw, M);
			} else {
				return rotation<void>::_Get_quaternion_0qw<quaternion_type>(qw, M00, M11, M22);
			}
		}
	};

	template<typename _Ts, typename _Tb, bool _Major>
	struct rotation< clmagic::matrix4x4<_Ts, _Tb, _Major> > {// matrix4x4
		using matrix_type     = clmagic::matrix4x4<_Ts, _Tb, _Major>;
		using quaternion_type = WilliamRowanHamilton::quaternion<_Ts, _Tb>;

		static matrix_type get_matrix(const quaternion_type& q) {
			const auto qx = q.imag()[0];
			const auto qy = q.imag()[1];
			const auto qz = q.imag()[2];
			const auto qw = q.real();
			const auto s  = static_cast<_Ts>(2)/(qx*qx + qy*qy + qz*qz + qw*qw);

			if _CONSTEXPR_IF(matrix_type::col_major()) {
				return matrix_type{
					_Ts(1)-s*(qy*qy+qz*qz),        s*(qx*qy-qw*qz),        s*(qx*qz+qw*qy), (_Ts)0,
					       s*(qx*qy+qw*qz), _Ts(1)-s*(qx*qx+qz*qz),        s*(qy*qz-qw*qx), (_Ts)0,
						   s*(qx*qz-qw*qy),		   s*(qy*qz+qw*qx), _Ts(1)-s*(qx*qx+qy*qy), (_Ts)0,
					                (_Ts)0,                 (_Ts)0,                 (_Ts)0, (_Ts)1 };
			} else {
				return matrix_type{
					_Ts(1)-s*(qy*qy+qz*qz),        s*(qx*qy+qw*qz),        s*(qx*qz-qw*qy), (_Ts)0,
					       s*(qx*qy-qw*qz), _Ts(1)-s*(qx*qx+qz*qz),        s*(qy*qz+qw*qx), (_Ts)0,
						   s*(qx*qz+qw*qy),		   s*(qy*qz-qw*qx), _Ts(1)-s*(qx*qx+qy*qy), (_Ts)0,
						            (_Ts)0,                 (_Ts)0,                 (_Ts)0, (_Ts)1 };
			}
		}
	
		static quaternion<_Ts, _Tb> get_quaternion(const matrix_type& M) {
			const auto M00 = M.at(0, 0);
			const auto M11 = M.at(1, 1);
			const auto M22 = M.at(2, 2);
			const auto qw  = clmagic::sqrt( ( M00+M11+M22+static_cast<_Ts>(1) ) / static_cast<_Ts>(4) );

			if ( qw > std::numeric_limits<_Ts>::epsilon() ) {
				return rotation<void>::_Get_quaternion<quaternion_type>(qw, M);
			} else {
				return rotation<void>::_Get_quaternion_0qw<quaternion_type>(qw, M00, M11, M22);
			}
		}
	};

}// WilliamRowanHamilton

#endif