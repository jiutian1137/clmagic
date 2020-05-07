//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
// Apache Licene 2.0
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_calculation_general_UNIT_CAST_h_
#define clmagic_calculation_general_UNIT_CAST_h_
#include "rational.h"
#include <ratio>

namespace clmagic {

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
	template<typename _FromRatio, typename _ToRatio, typename _Ty>
	_Ty unit_cast(const _Ty& x) {
		using _Ts       = scalar_type<_Ty>;
		using _CvtRatio = std::ratio_divide<_ToRatio, _FromRatio>;
		return x * ( static_cast<_Ts>(_CvtRatio::num) / static_cast<_Ts>(_CvtRatio::den) );// otherUnit / stdUnit
		/*<another>
		return x / ( static_cast<Ts>(_CvtRatio::num) / static_cast<Ts>(_CvtRatio::den) );// stdUnit / otherUnit
		</another>*/
	}

	template<typename _Ty, typename _Ti>
	_Ty unit_cast(const _Ty& x, rational<_Ti> _Ratio, rational<_Ti> _Dest_ratio) {
		using _Ts = scalar_type<_Ty>;
		return x * static_cast<_Ts>(_Dest_ratio / _Ratio);
		/*<another>
		return x / static_cast<_Ts>(_Dest_ratio / _Ratio);
		</another>*/
	}

	using seconds_ratio = std::ratio<1/*s*/,    1/*s*/>; //SI unit
	using minuts_ratio  = std::ratio<1/*m*/,   60/*s*/>;
	using hours_ratio   = std::ratio<1/*h*/, 3600/*s*/>;

	using grams_ratio     = std::ratio<1000/*g*/, 1/*kg*/>;
	using kilograms_ratio = std::ratio<1/*kg*/,   1/*kg*/>; //SI unit
	using tons_ratio      = std::ratio<1/*t*/, 1000/*kg*/>;

	using meters_ratio      = std::ratio<1/*m*/,      1/*m*/>; //SI unit
	using millimeters_ratio = std::ratio<1000/*1mm*/, 1/*m*/>;
	using centimeters_ratio = std::ratio<100/*cm*/,   1/*m*/>;
	using kilometers_ratio  = std::ratio<1/*km*/,  1000/*m*/>;

	using radians_ratio    = std::ratio<3141'59/*rad*/, 18'0000'00/*deg*/>;
	using degrees_ratio    = std::ratio<      1/*deg*/,          1/*deg*/>;
	using revolution_ratio = std::ratio<      1/*rev*/,        360/*deg*/>;

}// namespace clmagic

#endif