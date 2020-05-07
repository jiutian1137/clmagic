#pragma once
#ifndef clmagic_calculation_cg_RENDERING_h_
#define clmagic_calculation_cg_RENDERING_h_
#include "../lapack/vector.h"

namespace clmagic {
	template<typename _Ts>
	struct environment {
		_Ts          particles_ratio;// absorbed light-intensity per 1[m]
		vector3<_Ts> particles_color;
	};

	template<typename _Ts>
	struct surface {
		vector3<_Ts> subsurface_albedo;
		vector3<_Ts> reflect_value;
		_Ts          roughness;
	};

	template<typename _Ts>
	struct light_source {
		// physic_light_attribs
		vector3<_Ts> color;
		_Ts          intensity;// [W/sr]
		vector3<_Ts> position;
		// non_physic_light_attribs
		_Ts          umbra;// 2Pi = spherical_light
		vector3<_Ts> direction;
		_Ts          penumbra;
	};
}

#endif