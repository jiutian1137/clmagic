#pragma once
#ifndef clmagic_calculation_cg_LIGHT_SOURCE_h_
#define clmagic_calculation_cg_LIGHT_SOURCE_h_
#include "../lapack/vector.h"

namespace clmagic {
	template<typename _Ts>
	struct light_source {
		// 1. physic_light_attribs
		vector3<_Ts> color;
		_Ts          intensity;// W/sr
		vector3<_Ts> position;
		// 2. non_physic_light_attribs
		_Ts          umbra    = constant_Pi<_Ts>;// 2Pi = spherical_light
		vector3<_Ts> direction;
		_Ts          penumbra = umbra;
	};

}// namespace clmagic

#endif