//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_math_general_UNIT_h_h
#define clmagic_math_general_UNIT_h_h
#include "rational.h"
#include <ratio>

namespace clmagic {
	/*
	@_Unit: unit value wrapper class
	*/
	template<typename _Ti, typename _Ratio>
	class unit_ : public rational<_Ti> {
		using _Mybase = rational<_Ti>;

	public:
		using ratio         = _Ratio;
		using integral_type = _Ti;

		unit_(_Ti _Val) : _Mybase(_Val) {}
		unit_(const rational<_Ti>& _Rt) : _Mybase(_Rt) {}
		template<typename _Ti2, typename _Ratio2>
		unit_(const unit_<_Ti2, _Ratio2>& _Right) : _Mybase(static_cast<_Ti>(_Right._Num), static_cast<_Ti>(_Right._Den)) {
			using _Cvt = std::ratio_divide<_Ratio, _Ratio2>;
			static_cast<_Mybase&>(*this) *= rational<_Ti>(static_cast<_Ti>(_Cvt::num), static_cast<_Ti>(_Cvt::den));// otherUnit / stdUnit
			//static_cast<_Mybase&>(*this) /= rational<_Ti>(static_cast<_Ti>(_Cvt::num), static_cast<_Ti>(_Cvt::den));// stdUnit / otherUnit
			/*
				  1000m          1km          1km       X
			Xm * ------- = Xm * ------ = X * ------ = ------ km
				   1km           1000m        1000     1000

				  1000m        1000m
			Xkm * ------ = X * ------ = X*1000m
				   1km           1

				   1Am      1Bm          50       1
			XAm * ------ * ------ = X * ----- * ----Bm = (X * BmCvtf / AmCvtf)[Bm]
					50m      6m           1       6
			*/
		}

		unit_& operator=(const unit_&) = default;
		unit_& operator=(_Ti _Val) {
			static_cast<_Mybase&>(*this) = _Val;
			return *this;
		}
		template<typename _Ti2, typename _Ratio2>
		unit_& operator=(const unit_<_Ti2, _Ratio2>& _Right) {
			_Mybase::_Num = static_cast<_Ti>(_Right._Num);
			_Mybase::_Den = static_cast<_Ti>(_Right._Den);

			using _Cvt = std::ratio_divide<_Ratio, _Ratio2>;
			static_cast<_Mybase&>(*this) *= rational<_Ti>(static_cast<_Ti>(_Cvt::num), static_cast<_Ti>(_Cvt::den));// otherUnit / stdUnit
			//static_cast<_Mybase&>(*this) /= rational<_Ti>(static_cast<_Ti>(_Cvt::num), static_cast<_Ti>(_Cvt::den));// stdUnit / otherUnit
			return (*this);
		}
	
		explicit operator float() const { return this->to_floating<float>(); }
		explicit operator double() const { return this->to_floating<double>(); }

		unit_ operator+(_Ti _Integral) const {
			return unit_(static_cast<const _Mybase&>(*this) + _Integral);
		}
		unit_ operator-(_Ti _Integral) const {
			return unit_(static_cast<const _Mybase&>(*this) - _Integral);
		}
		unit_ operator*(_Ti _Integral) const {
			return unit_(static_cast<const _Mybase&>(*this) * _Integral);
		}
		unit_ operator/(_Ti _Integral) const {
			return unit_(static_cast<const _Mybase&>(*this) / _Integral);
		}
		unit_& operator+=(_Ti _Integral) {
			static_cast<_Mybase&>(*this) += _Integral;
			return *this;
		}
		unit_& operator-=(_Ti _Integral) {
			static_cast<_Mybase&>(*this) -= _Integral;
			return *this;
		}
		unit_& operator*=(_Ti _Integral) {
			static_cast<_Mybase&>(*this) *= _Integral;
			return *this;
		}
		unit_& operator/=(_Ti _Integral) {
			static_cast<_Mybase&>(*this) /= _Integral;
			return *this;
		}
		friend unit_ operator+(_Ti _Integral, const unit_& _Right) {
			return unit_(_Integral + static_cast<const _Mybase&>(_Right));
		}
		friend unit_ operator-(_Ti _Integral, const unit_& _Right) {
			return unit_(_Integral - static_cast<const _Mybase&>(_Right));
		}
		friend unit_ operator*(_Ti _Integral, const unit_& _Right) {
			return unit_(_Integral * static_cast<const _Mybase&>(_Right));
		}
		friend unit_ operator/(_Ti _Integral, const unit_& _Right) {
			return unit_(_Integral / static_cast<const _Mybase&>(_Right));
		}
		unit_& operator+=(const unit_& _Right) {
			static_cast<_Mybase&>(*this) += static_cast<const rational<_Ti>&>(_Right);
			return *this;
		}
		unit_& operator-=(const unit_& _Right) {
			static_cast<_Mybase&>(*this) -= static_cast<const rational<_Ti>&>(_Right);
			return *this;
		}
		unit_& operator*=(const unit_& _Right) {
			static_cast<_Mybase&>(*this) *= static_cast<const rational<_Ti>&>(_Right);
			return *this;
		}
		unit_& operator/=(const unit_& _Right) {
			static_cast<_Mybase&>(*this) /= static_cast<const rational<_Ti>&>(_Right);
			return *this;
		}
		template< typename _Ti2, typename _Ratio2>
		auto operator+(const unit_<_Ti2, _Ratio2>& _Right) const;
		template< typename _Ti2, typename _Ratio2>
		auto operator-(const unit_<_Ti2, _Ratio2>& _Right) const;
		template< typename _Ti2, typename _Ratio2>
		auto operator*(const unit_<_Ti2, _Ratio2>& _Right) const;
		template< typename _Ti2, typename _Ratio2>
		auto operator/(const unit_<_Ti2, _Ratio2>& _Right) const;

		friend std::string to_string(const unit_& _Right) {
			return std::to_string(_Right.to_floating());
		}
		friend std::ostream& operator<<(std::ostream& _Ostr, const unit_& _Right) {
			return (_Ostr << to_string(_Right));
		}
	};

	using degrees = unit_<int64_t, std::ratio<1>>;
	using radians = unit_<int64_t, std::ratio<3141'59/*rad*/, 18'0000'00/*deg*/>>;

	using meter_ratio = std::ratio<1, 1>/*m*/;
	using meters      = unit_<int64_t, meter_ratio>;
	using millimeters = unit_<int64_t, std::ratio_multiply<meter_ratio, std::ratio<1000/*1mm*/, 1/*m*/>>>;
	using centimeters = unit_<int64_t, std::ratio_multiply<meter_ratio, std::ratio<100/*cm*/,   1/*m*/>>>;
	using kilometers  = unit_<int64_t, std::ratio_multiply<meter_ratio, std::ratio<1/*km*/,  1000/*m*/>>>;

	
	template<typename _To, typename _Ty, typename _Ratio>
	_To unit_cast(const unit_<_Ty, _Ratio>& _Unit) {
		return static_cast<_To>(_Unit);
	}

	// two unit operation
	template<typename _Ti1, typename _Ti2, typename _Ratio1, typename _Ratio2>
	struct _Unit_operation {
		using first_argument_type  = unit_<_Ti1, _Ratio1>;
		using second_argument_type = unit_<_Ti2, _Ratio2>;
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
	auto unit_<_Ti, _Ratio>::operator+(const unit_<_Ti2, _Ratio2>& _Right) const {
		return _Unit_operation<_Ti, _Ti2, _Ratio, _Ratio2>()(*this, _Right, std::plus<>());
	}
	template<typename _Ti, typename _Ratio> template< typename _Ti2, typename _Ratio2> inline
	auto unit_<_Ti, _Ratio>::operator-(const unit_<_Ti2, _Ratio2>& _Right) const {
		return _Unit_operation<_Ti, _Ti2, _Ratio, _Ratio2>()(*this, _Right, std::minus<>());
	}
	template<typename _Ti, typename _Ratio> template< typename _Ti2, typename _Ratio2> inline
	auto unit_<_Ti, _Ratio>::operator*(const unit_<_Ti2, _Ratio2>& _Right) const {
		return _Unit_operation<_Ti, _Ti2, _Ratio, _Ratio2>()(*this, _Right, std::multiplies<>());
	}
	template<typename _Ti, typename _Ratio> template< typename _Ti2, typename _Ratio2> inline
	auto unit_<_Ti, _Ratio>::operator/(const unit_<_Ti2, _Ratio2>& _Right) const {
		return _Unit_operation<_Ti, _Ti2, _Ratio, _Ratio2>()(*this, _Right, std::divides<>());
	}

}// namespace clmagic

#endif