#pragma once
#ifndef clmagic_calculation_cg_ENVIRONMENT_h_
#define clmagic_calculation_cg_ENVIRONMENT_h_
#include "../lapack/vector.h"

namespace clmagic {
	template<typename _Ts>
	struct environment {
		_Ts          Ap;// ratio of particles, absorbed light-intensity per 1[m]
		vector3<_Ts> Cp;// color of particles
	};
}

#endif