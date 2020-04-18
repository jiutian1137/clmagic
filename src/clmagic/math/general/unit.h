//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_math_general_UNIT_h_
#define clmagic_math_general_UNIT_h_
#include "rational.h"
#include <ratio>

namespace clmagic {
	/*
	@_Unit: unit value wrapper class
	*/
	template<typename _Ti, typename _Ratio>
	class rational_duration : public rational<_Ti> {
		using _Mybase = rational<_Ti>;

	public:
		using ratio         = _Ratio;
		using integral_type = _Ti;
		using rational_type = rational<_Ti>;

		rational_duration(integral_type _Val) : _Mybase(_Val) {}
		
		rational_duration(rational_type _Rt) : _Mybase(_Rt) {}
	
		template<typename _Ti2, typename _Lratio>
		rational_duration(const rational_duration<_Ti2, _Lratio>& _Right)
			: _Mybase(static_cast<_Ti>(_Right._Num), static_cast<_Ti>(_Right._Den)) {
			/*<describ>
				<idea>unit-convert</idea>
				<example>
						   1000[m]           1[km]         1[km]     X
					X[m] * ------- = X[m] * ------- = X * ------ = ------ [km]
						    1[km]           1000[m]        1000     1000

						    1000[m]       1000[m]
					X[km] * ------- = X * ------- = X*1000[m]
						     1[km]           1
				</example>
				<formula>
					1[Lm] = 50[m] => std::ratio<1,50> = Lratio
					1[Rm] = 6[m]  => std::ratio<1,6>  = Rratio
						     1[Lm]     1[Rm]         50     1[Rm]
					X[Lm] * ------- * ------ = X * ----- * -------
						     50[m]     6[m]          1        6

					=> (X * (1/Lratio) * Rratio)[Rm]
					=> X * (Rratio/Lratio)[Rm]
				</formula>
			  </describ>*/
			using _Cvt_ratio = std::ratio_divide<_Ratio, _Lratio>;
			static_cast<_Mybase&>(*this) *= rational<_Ti>(_Cvt_ratio());
			//static_cast<_Mybase&>(*this) /= rational<_Ti>(_Cvt_ratio);// stdUnit / otherUnit
		}

		rational_duration& operator=(const rational_duration&) = default;
		
		rational_duration& operator=(_Ti _Val) {
			static_cast<_Mybase&>(*this) = _Val;
			return *this;
		}
		
		template<typename _Ti2, typename _Lratio>
		rational_duration& operator=(const rational_duration<_Ti2, _Lratio>& _Right) {
			_Mybase::_Num = static_cast<_Ti>(_Right._Num);
			_Mybase::_Den = static_cast<_Ti>(_Right._Den);

			using _Cvt_ratio = std::ratio_divide<_Ratio, _Lratio>;
			static_cast<_Mybase&>(*this) *= rational<_Ti>(_Cvt_ratio());// otherUnit / stdUnit
			//static_cast<_Mybase&>(*this) /= rational<_Ti>(_Cvt_ratio);// stdUnit / otherUnit
			return (*this);
		}
	
		explicit operator float() const { return this->to_floating<float>(); }
		explicit operator double() const { return this->to_floating<double>(); }

		rational_duration operator+(_Ti _Integral) const {
			return rational_duration(static_cast<const _Mybase&>(*this) + _Integral);
		}
		rational_duration operator-(_Ti _Integral) const {
			return rational_duration(static_cast<const _Mybase&>(*this) - _Integral);
		}
		rational_duration operator*(_Ti _Integral) const {
			return rational_duration(static_cast<const _Mybase&>(*this) * _Integral);
		}
		rational_duration operator/(_Ti _Integral) const {
			return rational_duration(static_cast<const _Mybase&>(*this) / _Integral);
		}
		rational_duration& operator+=(_Ti _Integral) {
			static_cast<_Mybase&>(*this) += _Integral;
			return *this;
		}
		rational_duration& operator-=(_Ti _Integral) {
			static_cast<_Mybase&>(*this) -= _Integral;
			return *this;
		}
		rational_duration& operator*=(_Ti _Integral) {
			static_cast<_Mybase&>(*this) *= _Integral;
			return *this;
		}
		rational_duration& operator/=(_Ti _Integral) {
			static_cast<_Mybase&>(*this) /= _Integral;
			return *this;
		}
		friend rational_duration operator+(_Ti _Integral, const rational_duration& _Right) {
			return rational_duration(_Integral + static_cast<const _Mybase&>(_Right));
		}
		friend rational_duration operator-(_Ti _Integral, const rational_duration& _Right) {
			return rational_duration(_Integral - static_cast<const _Mybase&>(_Right));
		}
		friend rational_duration operator*(_Ti _Integral, const rational_duration& _Right) {
			return rational_duration(_Integral * static_cast<const _Mybase&>(_Right));
		}
		friend rational_duration operator/(_Ti _Integral, const rational_duration& _Right) {
			return rational_duration(_Integral / static_cast<const _Mybase&>(_Right));
		}
		rational_duration& operator+=(const rational_duration& _Right) {
			static_cast<_Mybase&>(*this) += static_cast<const rational<_Ti>&>(_Right);
			return *this;
		}
		rational_duration& operator-=(const rational_duration& _Right) {
			static_cast<_Mybase&>(*this) -= static_cast<const rational<_Ti>&>(_Right);
			return *this;
		}
		rational_duration& operator*=(const rational_duration& _Right) {
			static_cast<_Mybase&>(*this) *= static_cast<const rational<_Ti>&>(_Right);
			return *this;
		}
		rational_duration& operator/=(const rational_duration& _Right) {
			static_cast<_Mybase&>(*this) /= static_cast<const rational<_Ti>&>(_Right);
			return *this;
		}
		template< typename _Ti2, typename _Ratio2>
		auto operator+(const rational_duration<_Ti2, _Ratio2>& _Right) const;
		template< typename _Ti2, typename _Ratio2>
		auto operator-(const rational_duration<_Ti2, _Ratio2>& _Right) const;
		template< typename _Ti2, typename _Ratio2>
		auto operator*(const rational_duration<_Ti2, _Ratio2>& _Right) const;
		template< typename _Ti2, typename _Ratio2>
		auto operator/(const rational_duration<_Ti2, _Ratio2>& _Right) const;

		friend std::string to_string(const rational_duration& _Right) {
			return std::to_string(_Right.to_floating());
		}
		friend std::ostream& operator<<(std::ostream& _Ostr, const rational_duration& _Right) {
			return (_Ostr << to_string(_Right));
		}
	};

	using second_ratio = std::ratio<1, 1>/*s*/;
	using seconds      = rational_duration<int64_t, second_ratio>;
	using minuts       = rational_duration<int64_t, std::ratio_multiply<second_ratio, std::ratio<1/*m*/, 60/*s*/>>>;
	using hours        = rational_duration<int64_t, std::ratio_multiply<second_ratio, std::ratio<1/*h*/, 3600/*s*/>>>;

	using kilogram_ratio = std::ratio<1, 1>/*kg*/;
	using grams          = rational_duration<int64_t, std::ratio_multiply<kilogram_ratio, std::ratio<1000/*g*/, 1/*kg*/>>>;
	using kilograms      = rational_duration<int64_t, kilogram_ratio>;
	using tons           = rational_duration<int64_t, std::ratio_multiply<kilogram_ratio, std::ratio<1/*t*/, 1000/*kg*/>>>;

	using meter_ratio = std::ratio<1, 1>/*m*/;
	using meters      = rational_duration<int64_t, meter_ratio>;
	using millimeters = rational_duration<int64_t, std::ratio_multiply<meter_ratio, std::ratio<1000/*1mm*/, 1/*m*/>>>;
	using centimeters = rational_duration<int64_t, std::ratio_multiply<meter_ratio, std::ratio<100/*cm*/,   1/*m*/>>>;
	using kilometers  = rational_duration<int64_t, std::ratio_multiply<meter_ratio, std::ratio<1/*km*/,  1000/*m*/>>>;

	using degrees = rational_duration<int64_t, std::ratio<1>>;
	using radians = rational_duration<int64_t, std::ratio<3141'59/*rad*/, 18'0000'00/*deg*/>>;

	using watt_ratio = std::ratio<1, 1>/*W*/;
	using watts      = rational_duration<int64_t, watt_ratio>;

	using kelvin_ratio = std::ratio<1, 1>/*K*/;
	using kelvins      = rational_duration<int64_t, kelvin_ratio>;
	
	using newton_ratio = std::ratio_divide<std::ratio_multiply<kilogram_ratio, meter_ratio>, 
										   std::ratio_multiply<second_ratio, second_ratio>>/*kg * m/s²*/;
	using newtons = rational_duration<int64_t, newton_ratio>;

	
	template<typename _To, typename _Ty, typename _Ratio>
	_To unit_cast(const rational_duration<_Ty, _Ratio>& _Unit) {
		return static_cast<_To>(_Unit);
	}

	// two unit operation
	template<typename _Ti1, typename _Ti2, typename _Ratio1, typename _Ratio2>
	struct _Unit_operation {
		using first_argument_type  = rational_duration<_Ti1, _Ratio1>;
		using second_argument_type = rational_duration<_Ti2, _Ratio2>;
		using result_type = std::conditional_t<std::ratio_greater_v<_Ratio1, _Ratio2>,
			first_argument_type, second_argument_type>;
		using common_operation_type = rational<typename result_type::integral_type>;

		template<typename _Fn>
		result_type operator()(const first_argument_type& _Left, const second_argument_type& _Right, _Fn _Func) {
			const auto _Common_left  = unit_cast<result_type>(_Left);
			const auto _Common_right = unit_cast<result_type>(_Right);
			return result_type(_Func(static_cast<const common_operation_type&>(_Common_left), static_cast<const common_operation_type&>(_Common_right)));
		}
	};

	template<typename _Ti, typename _Ratio> template< typename _Ti2, typename _Ratio2> inline
	auto rational_duration<_Ti, _Ratio>::operator+(const rational_duration<_Ti2, _Ratio2>& _Right) const {
		return _Unit_operation<_Ti, _Ti2, _Ratio, _Ratio2>()(*this, _Right, std::plus<>());
	}
	template<typename _Ti, typename _Ratio> template< typename _Ti2, typename _Ratio2> inline
	auto rational_duration<_Ti, _Ratio>::operator-(const rational_duration<_Ti2, _Ratio2>& _Right) const {
		return _Unit_operation<_Ti, _Ti2, _Ratio, _Ratio2>()(*this, _Right, std::minus<>());
	}
	template<typename _Ti, typename _Ratio> template< typename _Ti2, typename _Ratio2> inline
	auto rational_duration<_Ti, _Ratio>::operator*(const rational_duration<_Ti2, _Ratio2>& _Right) const {
		return _Unit_operation<_Ti, _Ti2, _Ratio, _Ratio2>()(*this, _Right, std::multiplies<>());
	}
	template<typename _Ti, typename _Ratio> template< typename _Ti2, typename _Ratio2> inline
	auto rational_duration<_Ti, _Ratio>::operator/(const rational_duration<_Ti2, _Ratio2>& _Right) const {
		return _Unit_operation<_Ti, _Ti2, _Ratio, _Ratio2>()(*this, _Right, std::divides<>());
	}

}// namespace clmagic

#endif