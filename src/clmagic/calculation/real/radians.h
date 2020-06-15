//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
// Apache Licene 2.0
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_calculation_real_RADIANS_h_
#define clmagic_calculation_real_RADIANS_h_
#include "unit_cast.h"

namespace clmagic {

	// 2Pi[radians] = 360[degrees] = 1[revolution]
	template<typename _Ty, typename _Ratio>
	struct _Angle {
		_Angle() = default;
		_Angle(int _Val) : num(static_cast<_Ty>(_Val)) {}
		_Angle(float _Val) : num(static_cast<_Ty>(_Val)) {}
		_Angle(double _Val) : num(static_cast<_Ty>(_Val)) {}
		template<typename _FromRatio>
		_Angle(_Angle<_Ty, _FromRatio> _Val) : num(unit_cast<_FromRatio, _Ratio>(_Val.num)) { }

		operator _Ty() const {
			return num;
		}

		_Angle operator-() const {
			return _Angle( -num );
		}
		_Angle operator+(_Angle _Right) const {
			return _Angle(num + _Right.num);
		}
		_Angle operator-(_Angle _Right) const {
			return _Angle(num - _Right.num);
		}
		_Angle operator*(_Angle _Right) const {
			return _Angle(num * _Right.num);
		}
		_Angle operator/(_Angle _Right) const {
			return _Angle(num / _Right.num);
		}
		_Angle operator%(_Angle _Right) const {
			return _Angle(num % _Right.num);
		}
		_Angle& operator+=(_Angle _Right) {
			num += _Right.num;
			return *this;
		}
		_Angle& operator-=(_Angle _Right) {
			num -= _Right.num;
			return *this;
		}
		_Angle& operator*=(_Angle _Right) {
			num *= _Right.num;
			return *this;
		}
		_Angle& operator/=(_Angle _Right) {
			num /= _Right.num;
			return *this;
		}
		_Angle& operator%=(_Angle _Right) {
			num %= _Right.num;
			return *this;
		}

		_Ty num;
	};

	template<typename _Ty>
	using radians = _Angle<_Ty, radians_ratio>;
	template<typename _Ty>
	using degrees = _Angle<_Ty, degrees_ratio>;
	template<typename _Ty>
	using revolution = _Angle<_Ty, revolution_ratio>;

	template<typename _Ty>
	using millimeters = _Angle<_Ty, millimeters_ratio>;
	template<typename _Ty>
	using centimeters = _Angle<_Ty, centimeters_ratio>;
	template<typename _Ty>
	using meters = _Angle<_Ty, meters_ratio>;
	template<typename _Ty>
	using kilometers = _Angle<_Ty, kilometers_ratio>;

	#define RADIANS ::clmagic::radians<_Ts>
	#define DEGREES ::clmagic::degrees<_Ts>

	#define MILLIMETERS ::clmagic::millimeters<_Ts>
	#define CENTIMETERS ::clmagic::centimeters<_Ts>
	#define METERS      ::clmagic::meters<_Ts>
	#define KILOMETERS  ::clmagic::kilometers<_Ts>

	template<typename _Ty, typename _Ratio>
	_Angle<_Ty, _Ratio> fmod(_Angle<_Ty, _Ratio> a, _Angle<_Ty, _Ratio> b) {
		return _Angle<_Ty, _Ratio>( fmod(a.num, b.num) );
	}

}// namespace clmagic

#endif