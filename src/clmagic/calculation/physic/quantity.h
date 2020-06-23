//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
// Apache Licene 2.0
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_calculation_physic_QUANTITY_h_
#define clmagic_calculation_physic_QUANTITY_h_
#include <iostream>
#include <ratio>

namespace clmagic {

	template<typename _ToRatio, typename _FromRatio, typename _Ty> inline
	_Ty ratio_cast(const _Ty& x) {
		using _Ts       = scalar_t<_Ty>;
		using _CvtRatio = std::ratio_divide<_ToRatio, _FromRatio>;
		return x * ( static_cast<float_t>(_CvtRatio::num) / static_cast<float_t>(_CvtRatio::den) );// otherUnit / stdUnit
		/*<another>
		return x / ( static_cast<Ts>(_CvtRatio::num) / static_cast<Ts>(_CvtRatio::den) );// stdUnit / otherUnit
		</another>*/
	/*<unit-cast>
		<idea>unit-convert</idea>
		<example>
					1000[m]          1[km]         1[km]     X
			X[m] * ------- = X[m] * ------- = X * ------ = ------ [km]
					1[km]           1000[m]        1000     1000

					1000[m]       1000[m]
			X[km] * ------- = X * ------- = X*1000[m]
						1[km]           1
		</example>
		<formula>
			1[Lm] = 50[m] => std::ratio<1,50> = _FromRatio
			1[Rm] = 6[m]  => std::ratio<1,6>  = _ToRatio
					 1[Lm]     1[Rm]         50     1[Rm]
			X[Lm] * ------- * ------ = X * ----- * -------
					 50[m]     6[m]          1        6

			=> (X * (1/_FromRatio) * _ToRatio)[Rm]
			=> X * (_ToRatio/_FromRatio)[Rm]
		</formula>
	</unit-cast>*/
	}

#ifdef clmagic_calculation_general_RATIONAL_h_
	template<typename _Ty, typename _Ti>
	_Ty ratio_cast(const _Ty& x, rational<_Ti> _Ratio, rational<_Ti> _Dest_ratio) {
		using _Ts = scalar_t<_Ty>;
		return x * static_cast<_Ts>(_Dest_ratio / _Ratio);
		/*<another>
		return x / static_cast<_Ts>(_Dest_ratio / _Ratio);
		</another>*/
	}
#endif

	using meters_ratio      = std::ratio<1/*m*/,        1/*m*/>; //SI unit
	using micrometers_ratio = std::ratio<1000000/*um*/, 1/*m*/>;
	using millimeters_ratio = std::ratio<1000/*mm*/,    1/*m*/>;
	using centimeters_ratio = std::ratio<100/*cm*/,     1/*m*/>;
	using kilometers_ratio  = std::ratio<1/*km*/,    1000/*m*/>;

	using grams_ratio     = std::ratio<1000/*g*/,     1/*kg*/>;
	using kilograms_ratio = std::ratio<1/*kg*/,       1/*kg*/>; //SI unit
	using tons_ratio      = std::ratio<10/*ton*/,  9072/*kg*/>;// 1[ton] = 907.2[kg]
	using pounds_ratio    = std::ratio<4536/*t*/, 10000/*kg*/>;// 453.6[lb] = 1000[kg]
	
	using seconds_ratio = std::ratio<1/*s*/,       1/*s*/>; //SI unit
	using minuts_ratio  = std::ratio<1/*m*/,      60/*s*/>;
	using hours_ratio   = std::ratio<1/*h*/,    3600/*s*/>;
	using days_ratio    = std::ratio<1/*d*/, 24*3600/*s*/>;
	using years_ratio   = std::ratio<1/*y*/, static_cast<int>(365.25*24*3600)/*s*/>;

	using radians_ratio     = std::ratio<10/*rad*/, 573/*deg*/>;// SI_unit, 1[rad] = 57.3[deg]
	using degrees_ratio     = std::ratio< 1/*deg*/,   1/*deg*/>;
	using revolutions_ratio = std::ratio< 1/*rev*/, 360/*deg*/>;

#define DEFINE_UNIT(UNIT_NAME, SYMBOL)    \
	struct UNIT_NAME {                    \
		using ratio = UNIT_NAME##s_ratio; \
		constexpr static const char* symbol = SYMBOL; \
	}

	// [unit numbers, "2020.03.27, LongJiangnan"]
	template<typename _Ty, typename _Unit>
	struct quantity {
		using _Myunit = _Unit;

		quantity() = default;
		quantity(_Ty _Val) : num(_Val) {}
		template<typename _FromUnit>
		quantity(quantity<_Ty, _FromUnit> _Right) : num(ratio_cast<_Myunit::ratio, _FromUnit::ratio>(_Right.num)) {}

		quantity& operator=(_Ty _Val) {
			this->num = _Val;
			return *this;
		}
		template<typename _FromUnit>
		quantity& operator=(quantity<_Ty, _FromUnit> _Right) {
			this->num = ratio_cast<_Myunit::ratio, _FromUnit::ratio>(_Right.num);
			return *this;
		}

		operator _Ty() const {
			return num;
		}
		std::string to_string() const {
			using std::to_string;
			return (to_string(this->num) + _Myunit::symbol);
		}

		quantity operator-() const {
			return quantity( -num );
		}
		quantity operator+(quantity _Right) const {
			return quantity(num + _Right.num);
		}
		quantity operator-(quantity _Right) const {
			return quantity(num - _Right.num);
		}
		template<typename _TyRatio>
		quantity operator*(_TyRatio _Right) const {// multiplies a ratio_number
			return quantity(num * _Right);
		}
		_Ty      operator/(quantity _Right) const {// ratio of this and _Right
			return (num / _Right.num);
		}
		/*quantity operator%(quantity _Right) const {
			return quantity(num % _Right.num);
		}*/
		
		quantity& operator+=(quantity _Right) {
			num += _Right.num;
			return *this;
		}
		quantity& operator-=(quantity _Right) {
			num -= _Right.num;
			return *this;
		}
		template<typename _TyRatio>
		quantity& operator*=(_TyRatio _Right) {
			num *= _Right;
			return *this;
		}
		// not have assign_divides<>()
		/*quantity& operator%=(quantity _Right) {
			num %= _Right.num;
			return *this;
		}*/

		friend std::ostream& operator<<(std::ostream& _Ostr, quantity _Left) {
			return (_Ostr << _Left.to_string());
		}

		_Ty num;
	};

	template<typename _ToUnit, typename _FromUnit, typename _Ty> inline
	quantity<_Ty, _ToUnit> unit_cast(quantity<_Ty, _FromUnit> _Src) {
		return quantity<_Ty, _ToUnit>( ratio_cast<_ToUnit::ratio, _FromUnit::ratio, _Ty>(_Src.num) );
	}

	template<typename _Ty, typename _Unit> inline
	quantity<_Ty, _Unit> fmod(quantity<_Ty, _Unit> a, quantity<_Ty, _Unit> b) {
		return quantity<_Ty, _Unit>( fmod(a.num, b.num) );
	}

	// [length unit]
	DEFINE_UNIT(micrometer, "[um]");

	// [length unit]
	DEFINE_UNIT(millimeter, "[mm]");
	
	// [length unit]
	DEFINE_UNIT(centimeter, "[cm]");
	
	// [length unit]
	DEFINE_UNIT(meter,      "[m]");
	
	// [length unit]
	DEFINE_UNIT(kilometer,  "[km]");

	// [mass unit]
	DEFINE_UNIT(gram,     "[g]");

	// [mass unit]
	DEFINE_UNIT(kilogram, "[kg]");

	// [mass unit]
	DEFINE_UNIT(ton,      "[ton]");

	// [mass unit]
	DEFINE_UNIT(pound,    "[lb]");

	// [time unit]
	DEFINE_UNIT(second, "[s]");

	// [time unit]
	DEFINE_UNIT(minut,  "[m]");

	// [time unit]
	DEFINE_UNIT(hour,   "[h]");

	// [time unit]
	DEFINE_UNIT(day,    "[d]");

	// [time unit]
	DEFINE_UNIT(year,   "[y]");

	// [angle unit]
	DEFINE_UNIT(radian,     "[rad]");
	
	// [angle unit]
	DEFINE_UNIT(degree,     "[deg]");

	// [angle unit]
	DEFINE_UNIT(revolution, "[rev]");

	// [force unit]
	struct Newton {
		using ratio = std::ratio_multiply< std::ratio_divide<std::ratio_divide<meters_ratio, seconds_ratio>, seconds_ratio>, 
			                               kilograms_ratio >;
		constexpr static const char* symbol = "[N]";
	};

	template<typename _Ty>
		using millimeters = quantity<_Ty, millimeter>;
	template<typename _Ty>
		using centimeters = quantity<_Ty, centimeter>;
	template<typename _Ty>
		using meters      = quantity<_Ty, meter>;
	template<typename _Ty>
		using kilometers  = quantity<_Ty, kilometer>;

	template<typename _Ty>
		using grams     = quantity<_Ty, gram>;
	template<typename _Ty>
		using kilograms = quantity<_Ty, kilogram>;
	template<typename _Ty>
		using tons      = quantity<_Ty, ton>;

	template<typename _Ty>
		using seconds = quantity<_Ty, second>;
	template<typename _Ty>
		using minuts  = quantity<_Ty, minut>;
	template<typename _Ty>
		using hours   = quantity<_Ty, hour>;

	template<typename _Ty>
		using radians     = quantity<_Ty, radian>;
	template<typename _Ty>
		using degrees     = quantity<_Ty, degree>;
	template<typename _Ty>
		using revolutions = quantity<_Ty, revolution>;

	#define MILLIMETERS ::clmagic::millimeters<_Ts>
	#define CENTIMETERS ::clmagic::centimeters<_Ts>
	#define METERS      ::clmagic::meters<_Ts>
	#define KILOMETERS  ::clmagic::kilometers<_Ts>

	#define GRAMS     ::clmagic::grams<_Ts>
	#define KILOGRAMS ::clmagic::kilograms<_Ts>
	#define TONS      ::clmagic::tons<_Ts>

	#define SECONDS ::clmagic::seconds<_Ts>
	#define MINUTS  ::clmagic::minuts<_Ts>
	#define HOURS   ::clmagic::hours<_Ts>

	#define RADIANS ::clmagic::radians<_Ts>
	#define DEGREES ::clmagic::degrees<_Ts>

}// namespace clmagic

#endif