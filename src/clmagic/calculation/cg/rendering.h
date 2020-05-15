#pragma once
#include "../lapack/vector.h"

namespace clmagic {

	template<typename _Ts, typename _Tb = _Ts>
	struct environment {
		SCALAR  particles_ratio;// absorbed light-intensity per 1[m]
		VECTOR3 particles_color;
	};

	template<typename _Ts, typename _Tb = _Ts>
	struct light_source {
		// physic_light_attribs
		VECTOR3 color;
		SCALAR  intensity;// [W/sr]
		VECTOR3 position;
		// non_physic_light_attribs
		SCALAR  umbra;// 2Pi = spherical_light
		VECTOR3 direction;
		SCALAR  penumbra;
	};
	
	template<typename _Ts, typename _Tb = _Ts>
	struct surface {
		VECTOR3 subsurface_albedo;
		SCALAR  roughness_x;
		VECTOR3 reflect_value;
		SCALAR  roughness_y;// anisotripic
	};

}// namespace clmagic


//------------------------------------------
// Copyright (c) tri-Ace 
// All Rights Reserved
//------------------------------------------
namespace tri_Ace {
	/* isotripic-BRDF
		vector3 Pss = subsurface_albedo
		scalar  n   = surface_shininess;
		vector3 f0  = surface_reflectance;
		vector3 N   = surface_normal;
		vector3 E   = eye_vector;
		vector3 L   = light_vector;*/
	template<typename _Ts, typename _Tb>
	VECTOR3 BRDF(VECTOR3 Pss, SCALAR n, VECTOR3 F0, VECTOR3 N, VECTOR3 E, VECTOR3 L) {
		using clmagic::max;
		using clmagic::dot;
		using clmagic::pow;
		using clmagic::normalize;
		SCALAR  Pi = 3.14159;
		VECTOR3 H  = normalize(E + L);
    
		VECTOR3 Fdiff = F0 + (1 - F0) * pow(1 - max(0, dot(N, L)), 5);
		VECTOR3 Fspec = F0 + (1 - F0) * pow(1 - max(0, dot(E, H)), 2);
		return (Pss/Pi)*(1-Fdiff) +               (n + 2)              *  Fspec * pow(max(0,dot(N, H)),n)    
									/*------------------------------*/  /*-------------------------------*/
										/ (4*Pi * (2 - pow(2,-n/2)))        / max(dot(N, L), dot(N, E));
	}
}// namespace tri_Ace

