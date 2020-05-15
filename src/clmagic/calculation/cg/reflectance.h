#pragma once
#ifndef clmagic_calculation_raytracing_REFLECTANCE_h_
#define clmagic_calculation_raytracing_REFLECTANCE_h_
#include "../lapack/vector.h"

//--------------------------------------------------------------------------------------
// Copyright (c) 1994 Christophe-Schlick
// All Rights Reserved
//--------------------------------------------------------------------------------------
namespace Schlick {
	template<typename _Ts, typename _Tb>
	VECTOR3 Fresnel_reflectance(VECTOR3 F0, SCALAR theta_cos) {
		using clmagic::pow;
		using clmagic::positive;
		return F0 + (1 - F0) * pow(1-positive(theta_cos), 5);
	}

	template<typename _Ts, typename _Tb>
	VECTOR3 Fresnel_reflectance(VECTOR3 F0, VECTOR3 F90, SCALAR p, SCALAR theta_cos) {
		using clmagic::pow;
		using clmagic::positive;
		return F0 + (F90 - F0) * pow(1 - positive(theta_cos), p);
	}
}

#endif