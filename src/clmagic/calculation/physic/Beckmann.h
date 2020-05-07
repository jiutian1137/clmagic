//--------------------------------------------------------------------------------------
// Copyright (c) Petr-Beckmann(1924-1993)
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_calculation_physic_BECKMANN_h_
#define clmagic_calculation_physic_BECKMANN_h_
#include "../lapack/vector.h"

namespace Beckmann {
	/*<Reference> The-Scattering-of-Electromagnetic-Waves-from-Rough-Surfaces.p331 </Reference>*/
	template<typename _Ts, typename _Tb>
	_Ts isotropic_normal_distribution(clmagic::unit_vector3<_Ts, _Tb> m, clmagic::unit_vector3<_Ts, _Tb> n, _Ts roughness) {
		/*<formula>
		 pow(theta_n_cos,2) - 1
		-------------------------------------
		 pow(roughness,2)*pow(theta_n_cos,2)

		     1 - 1/pow(theta_n_cos,2)
		=> -------------------------
		       pow(roughness,2)

		     1 - pow(theta_sec,2)
		=> -------------------------
		       pow(roughness,2)

			 - pow(theta_tan,2)
		=> -------------------------
			   pow(roughness,2)
		<formula>*/
		using clmagic::dot;
		using clmagic::pow;
		using clmagic::exp;
		using clmagic::is_positive;
		const _Ts theta_n_cos = dot(n, m);
		const _Ts Pi        = static_cast<_Ts>(3.141592);
		const _Ts One       = static_cast<_Ts>(1);
		const _Ts Two       = static_cast<_Ts>(2);
		const _Ts Four      = static_cast<_Ts>(4);
		return	      is_positive(theta_n_cos)
		/*--------------------------------------------*/ * exp( ( pow(theta_n_cos,Two) - One ) / ( pow(roughness,Two)*pow(theta_n_cos,Two) ) )
		 / ( Pi*pow(roughness,Two)*pow(theta_n_cos,Four) );
	}

	template<typename _Ts, typename _Tb>
	_Ts isotropic_lambda(clmagic::unit_vector3<_Ts, _Tb> v, clmagic::unit_vector3<_Ts, _Tb> n, _Ts roughness) {
		using clmagic::erf;
		using clmagic::exp;
		using clmagic::dot;
		using clmagic::pow;
		using clmagic::sqrt;
		const _Ts Pi  = static_cast<_Ts>(3.141592);
		const _Ts One = static_cast<_Ts>(1);
		const _Ts Two = static_cast<_Ts>(2);

		const _Ts theta_o_cos = dot(v, n);
		const _Ts a           = theta_o_cos 
			/*------------------------------------------------*/
			/ ( roughness * sqrt(One - pow(theta_o_cos,Two)) );// 1/(roughness*tan(theta_o))

		return (erf(a) - One)/Two + One/(Two * a * sqrt(Pi)) * exp(-pow(a, Two));
	}

}// namespace Beckmann

#endif