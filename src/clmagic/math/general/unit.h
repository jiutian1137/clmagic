//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_math_general_UNIT_h_h
#define clmagic_math_general_UNIT_h_h
#include "../real/rational.h"
#include <ratio>

namespace clmagic {
	/*
	@_Unit: unit value wrapper class
	*/
	template<typename _Ty, typename _Ratio>
	class unit_ : public any_class<_Ty>::type {
		using _Mybase = typename any_class<_Ty>::type;

	public:
		using ratio = _Ratio;
		using scalar_type = clmagic::scalar_type<_Ty>;
		constexpr static rational cvtf = rational{ ratio::num, ratio::den };

		template<typename ..._Tys>
		unit_(const _Tys&... _Args) : _Mybase(_Args...) { }

		template<typename _Rt>
		unit_(const unit_<_Ty, _Rt>& _Right) : _Mybase(_Right) {
			auto _Cvtf = this->cvtf / _Right.cvtf;
			(*this) *= _Cvtf.to_floating<scalar_type>();// otherUnit / stdUnit
			//(*this) /= _Cvtf.to_floating<scalar_type>();// stdUnit / otherUnit
			/*
				  1000m          1km          1km       X
			Xm * ------- = Xm * ------ = X * ------ = ------ km
				   1km           1000m        1000     1000

				  1000m        1000m
			Xkm * ------ = X * ------ = X*1000m
				   1km           1

				   1Am      1Bm          50       1
			XAm * ------ * ------ = X * ----- * ----Bm = X * Bm_Cvtf / AmCvtf
					50m      6m           1       6
			*/
		}

		//unit_& operator=(const unit_&) = default;

		/*template<typename _Ty2>
		unit_& operator=(const _Ty2& _Right) {
			(*this) = _Right;
			return (*this);
		}*/
	};

	template<typename _Ty> using standard = unit_<_Ty, std::ratio<1>>;

	template<typename _Ty> using degrees = standard<_Ty>;
	template<typename _Ty> using radians = unit_<_Ty, std::ratio<3141'5926/*rad*/, 18'0000'0000/*deg*/>>;

	using meter_ratio = std::ratio<1, 1/*m*/>;
	template<typename _Ty> using meters      = unit_<_Ty, meter_ratio>;
	template<typename _Ty> using millimeters = unit_<_Ty, std::ratio_multiply<meter_ratio, std::ratio<1000/*1mm*/, 1/*m*/>>>;
	template<typename _Ty> using centimeters = unit_<_Ty, std::ratio_multiply<meter_ratio, std::ratio<100/*cm*/,   1/*m*/>>>;
	template<typename _Ty> using kilometers  = unit_<_Ty, std::ratio_multiply<meter_ratio, std::ratio<1/*km*/,  1000/*m*/>>>;
}// namespace clmagic

#endif