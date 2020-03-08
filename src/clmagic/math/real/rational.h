#pragma once
#ifndef clmagic_math_real_RATIONAL_h_
#define clmagic_math_real_RATIONAL_h_
#include "../general/bitset.h"
#include <ratio>

namespace clmagic {

	inline intmax_t _Gcdx(intmax_t _Ax, intmax_t _Bx) {// computes greatest common divisor of _Ax and _Bx
		if (_Bx == 0) {
			return (_Ax);
		} else {
			return _Gcdx(_Bx, _Ax % _Bx);
		}
	}

	inline intmax_t gcd(intmax_t _Ax, intmax_t _Bx) {// computes greatest common divisor of _Ax and _Bx
		if (_Ax == 0 && _Bx == 0) {
			return (1);
		}

		if (_Bx > _Ax) {
			std::swap(_Ax, _Bx);
		}
		return _Gcdx(_Ax < 0 ? -_Ax : _Ax, _Bx < 0 ? -_Bx : _Bx);
	}


	struct rational {
		void assign(intmax_t _Nx, intmax_t _Dx);

		constexpr rational() : _Num(0), _Den(0) {}
		constexpr rational(std::initializer_list<intmax_t> _Ilist) : _Num(*_Ilist.begin()), _Den(*(_Ilist.begin() + 1)) {}
		rational(intmax_t _Nx, intmax_t _Dx);
		
		template<typename _Fp = float> constexpr _Fp to_floating() const {
			return static_cast<_Fp>(_Num) / static_cast<_Fp>(_Den);
		}

#ifdef clmagic_math_general_BITSET_h_
		template<typename _Ty, size_t _En, size_t _Mn> 
		void assign(floating_point<_Ty, _En, _Mn> _Fp) {
			using _BitTy = decltype(_Fp.exponent());
			const auto _Hide     = mask_at<_BitTy>(_Mn);
			auto       _Exp      = make_int32(_Fp.exponent() - mask_at<_BitTy>(_En - 1)) + 1;
			auto       _Mantissa = _Fp.significant() | _Hide;
			if (_Mn >= _Exp) {// _Mn - _Exp >= 0
				this->_Num = make_uintmax(_Mantissa);
				this->_Den = intmax_t(1) << (_Mn - _Exp);
			} else {
				this->_Num = make_uintmax(_Mantissa) << (_Exp - _Mn);
				this->_Den = 1;
			}

			if (make_int32(_Fp.sign()) == 1) {
				this->_Num = -this->_Num;
			}

			this->reduce();

			/*
			0 00000001 10101101011010110101000
			1.10101101011010110101000 = 1.0000000000000000000000 | 0.10101101011010110101000

			_Nx     110101101011010110101000
			---- = -------------------------- * pow(2, _Exp)
			_Dx           1 << _Mn

			 110101101011010110101000 << 1
			----------------------------------- * pow(2, _Exp-1)
					 1 << _Mn

			 110101101011010110101000 << 1 >> 1
			----------------------------------- * pow(2, _Exp-1)
				   1 << _Mn >> 1

			  110101101011010110101000
			----------------------------- * pow(2, _Exp-1)
				   1 << (_Mn - 1)

			  110101101011010110101000
			----------------------------- * 1
				  1 << (_Mn - _Exp)
			*/
		}
		template<typename _Ty, size_t _En, size_t _Mn> 
		rational(floating_point<_Ty, _En, _Mn> _Fp) { this->assign(_Fp); }
#endif

		intmax_t& operator[](size_t _Pos) {
			return (_Pos == 0 ? _Num : _Den); }
		const intmax_t& operator[](size_t _Pos) const { 
			return (_Pos == 0 ? _Num : _Den); }

		bool operator==(const rational& _Right) const;
		bool operator!=(const rational& _Right) const;
		bool operator< (const rational& _Right) const;
		bool operator> (const rational& _Right) const;
		bool operator>=(const rational& _Right) const;
		bool operator<=(const rational& _Right) const;
		rational operator+(const rational& _Right) const;
		rational operator-(const rational& _Right) const;
		rational operator*(const rational& _Right) const;
		rational operator/(const rational& _Right) const;
		rational& operator+=(const rational& _Right);
		rational& operator-=(const rational& _Right);
		rational& operator*=(const rational& _Right);
		rational& operator/=(const rational& _Right);

		void reduce();

		friend std::string to_string(const rational& _R) {
			return (std::to_string(_R[0]) + "/" + std::to_string(_R[1]));
		}

		friend std::ostream& operator<<(std::ostream& _Ostr, const rational& _R) {
			return (_Ostr << to_string(_R));
		}

	private:
		intmax_t _Num;
		intmax_t _Den;
	};

}// namespace clmagic


namespace clmagic {

	inline void rational::assign(intmax_t _Nx, intmax_t _Dx) {
		const auto _Gx = gcd(_Nx, _Dx);
		this->_Num = _Nx / _Gx;
		this->_Den = _Dx / _Gx;
	}

	inline rational::rational(intmax_t _Nx, intmax_t _Dx) { this->assign(_Nx, _Dx); }


	inline bool rational::operator==(const rational& _Right) const {
		return (this->_Num * _Right._Den == _Right._Num * this->_Den);
	}

	inline bool rational::operator!=(const rational& _Right) const {
		return !((*this) == _Right);
	}

	inline bool rational::operator<(const rational& _Right) const {
		return std::_Ratio_less((*this)[0], (*this)[1], _Right[0], _Right[1]);
	}

	inline bool rational::operator>(const rational& _Right) const {
		return (_Right < (*this));
	}

	inline bool rational::operator>=(const rational& _Right) const {
		return !((*this) < _Right);
	}

	inline bool rational::operator<=(const rational& _Right) const {
		return !((*this) > _Right);
	}
	
	inline rational rational::operator+(const rational & _Right) const {
		const auto _N1 = (*this)[0], _D1 = (*this)[1], _N2 = _Right[0], _D2 = _Right[1];
		const auto _Gx = gcd(_D1, _D2);
		return rational(_N1 * (_D2 / _Gx) + _N2 * (_D1 / _Gx),
						_D1 * (_D2 / _Gx));
		/*
		 N1*D2 + N2*D1
		--------------
		   D1*D2
		*/
	}

	inline rational rational::operator-(const rational & _Right) const {
		const auto _N1 = (*this)[0], _D1 = (*this)[1], _N2 = _Right[0], _D2 = _Right[1];
		const auto _Gx = gcd(_D1, _D2);
		return rational(_N1 * (_D2 / _Gx) - _N2 * (_D1 / _Gx),
						_D1 * (_D2 / _Gx));
		/*
		 N1*D2 - N2*D1
		---------------
		   D1*D2
		*/
	}

	inline rational rational::operator*(const rational & _Right) const {
		const auto _N1 = (*this)[0], _D1 = (*this)[1], _N2 = _Right[0], _D2 = _Right[1];
		const auto _Gn = gcd(_N1, _N2);
		const auto _Gd = gcd(_D1, _D2);
		return rational((_N1 / _Gn) * (_N2 / _Gn),
						(_D1 / _Gd) * (_D2 / _Gd));
		/*
		 N1*N2
		-------
		 D1*D2
		*/
	}

	inline rational rational::operator/(const rational & _Right) const {
		const auto _N1 = (*this)[0], _D1 = (*this)[1], _N2 = _Right[0], _D2 = _Right[1];
		const auto _Gn = gcd(_N1, _N2);
		const auto _Gd = gcd(_D1, _D2);
		return rational((_N1 / _Gn) * (_D2 / _Gd),
						(_D1 / _Gd) * (_N2 / _Gn));
		/*
		 N1*D2
		-------
		 D1*N2
		 */
	}
	
	inline rational& rational::operator+=(const rational & _Right) {
		(*this) = (*this) + _Right;
		return (*this);
	}

	inline rational& rational::operator-=(const rational & _Right) {
		(*this) = (*this) - _Right;
		return (*this);
	}

	inline rational& rational::operator*=(const rational & _Right) {
		(*this) = (*this) * _Right;
		return (*this);
	}

	inline rational& rational::operator/=(const rational & _Right) {
		(*this) = (*this) / _Right;
		return (*this);
	}

	inline void rational::reduce() {
		const auto _Gx = gcd(_Num, _Den);
		_Num = _Num / _Gx;
		_Den = _Den / _Gx;
	}

}// namespace clmagic

#endif