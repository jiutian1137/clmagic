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
	clmagic::vector3<_Ts, _Tb> Fresnel_reflectance(clmagic::vector3<_Ts, _Tb> F0, _Ts N_dot_L) {
		using clmagic::pow;
		using clmagic::positive;
		return F0 + (1 - F0) * pow(1-positive(N_dot_L), 5);
	}

	template<typename _Ts, typename _Tb>
	clmagic::vector3<_Ts, _Tb> Fresnel_reflectance(clmagic::vector3<_Ts, _Tb> F0, clmagic::vector3<_Ts, _Tb> F90, _Ts p, _Ts N_dot_L) {
		using clmagic::pow;
		using clmagic::positive;
		return F0 + (F90 - F0) * pow(1 - positive(N_dot_L), p);
	}
}

#endif