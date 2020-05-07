#pragma once
#ifndef clmagic_calculation_raytracing_MICROFACET_h_
#define clmagic_calculation_raytracing_MICROFACET_h_
#include "../lapack/vector.h"


/*- - - - - - - - - - - - - - - - - - - - - - - normal_distribution - - - - - - - - - - - - - - - - - - - - - - -*/

//------------------------------------------
// Copyright (c) Blinn-Phong
// All Rights Reserved
//------------------------------------------
namespace TrowbridgeReitz{
	template<typename _Ts>
	_Ts isotropic_normal_distribution(_Ts m_dot_n, _Ts r) {// shape-invariant
		// <r> pow(roughness,2) </r>
		using clmagic::is_positive;
		const _Ts Pi              = static_cast<_Ts>(3.141592);
		const _Ts thetaN_cos_pow2 = m_dot_n * m_dot_n;
		const _Ts r_pow2          = r * r;

		const _Ts D = r_pow2 / ( Pi * ( 1 + thetaN_cos_pow2*(r_pow2 - 1) ) );
		return D * is_positive(m_dot_n);
		/*<formula>
			                  is_positive(cos(thetaN))
			--------------------------------------------------------------
			                                          pow(tan(thetaN),2)
			Pi * r_pow2 * pow(cos(thetaN),4) * ( 1 + -------------------)
			                                               r_pow2
		<formula>*/
	}

	template<typename _Ts>
	_Ts isotropic_lambda(_Ts v_dot_n, _Ts r) {
		using clmagic::sqrt;
		const _Ts v_dot_n_pow2 = v_dot_n * v_dot_n;
		const _Ts a_pow2       = v_dot_n_pow2 / (r * r * (1 - v_dot_n_pow2));
		return ( -1 + sqrt(1 + 1/a_pow2) ) / 2;
		/*<a_pow2>        
						 v_dot_n
			pow(-----------------------------, 2)
				 r*sqrt(1 - pow(v_dot_n,2))
					 pow(v_dot_n,2)
			=> -----------------------------
				pow(r,2) * (1-pow(v_dot_n,2))
		</a_pow2>*/
	}
}

//------------------------------------------
// Copyright (c) Blinn-Phong
// All Rights Reserved
//------------------------------------------
namespace Blinn {
	template<typename _Ts>
	_Ts isotropic_normal_distribution(_Ts m_dot_n, _Ts r) {
		//<r> pow(8192, [0,1]) or 2*pow(Beckmann_roughness,-2)-2 </r>
		using clmagic::pow;
		using clmagic::is_positive;
		const _Ts Pi  = static_cast<_Ts>(3.141592);
		const _Ts Two = static_cast<_Ts>(2);
		const _Ts D   = (r + Two)
		              /*----------*/ * pow(m_dot_n, r)
			          / (Two * Pi);
		return  D * is_positive(m_dot_n);
	}

	/*<isotropic_lambda> 
		not exist lambda-function, using the Backmann::isotropic_lambda(..)
		<Reference>RealTimeRender-4th-Edition.p340</Reference>
	</isotropic_lambda>*/
}

//------------------------------------------
// Copyright (c) Petr-Beckmann(1924-1993)
// All Rights Reserved
//------------------------------------------
namespace Beckmann {
	template<typename _Ts>
	_Ts isotropic_normal_distribution(_Ts m_dot_n, _Ts r) {
		// shape-invariant
		// <Reference> <<The Scattering of Electromagnetic Waves from Rough Surfaces>>.p331 </Reference>
		using clmagic::exp;
		using clmagic::is_positive;
		const _Ts Pi              = static_cast<_Ts>(3.141592);
		const _Ts One             = static_cast<_Ts>(1);
		const _Ts thetaN_cos_pow2 = m_dot_n * m_dot_n;
		const _Ts thetaN_cos_pow4 = thetaN_cos_pow2 * thetaN_cos_pow2;
		const _Ts r_pow2          = r * r;

		/*<formula>
				is_positive(cos(thetaN))                pow(tan(thetaN),2)
			------------------------------- * exp( - ---------------------- )
			 Pi * r_pow2 * thetaN_cos_pow4                 r_pow2
		<formula>*/
		const _Ts D = exp( (thetaN_cos_pow2 - One) / (r_pow2 * thetaN_cos_pow2) )
		             /*----------------------------------------------------------*/
		                    / ( Pi * r_pow2 * thetaN_cos_pow4 );
		return D * is_positive(m_dot_n);
	}

	template<typename _Ts>
	_Ts isotropic_lambda(_Ts v_dot_n, _Ts r) {
		using clmagic::erf;
		using clmagic::exp;
		using clmagic::sqrt;
		const _Ts Pi  = static_cast<_Ts>(3.141592);
		const _Ts One = static_cast<_Ts>(1);
		const _Ts Two = static_cast<_Ts>(2);

		const _Ts a   = v_dot_n / ( r * sqrt(One - v_dot_n*v_dot_n) );// 1/(r*tan(thetaO))

		return (erf(a) - One)/Two + One/(Two * a * sqrt(Pi)) * exp(-a * a);
	}

	template<typename _Ts>
	_Ts isotropic_lambda_approximation(_Ts a) {
		using clmagic::lerp;
		const auto a_pow2 = a * a;
		return lerp(0.0, (1.0 - 1.259*a + 0.396*a_pow2) / (3.535*a + 2.181*a_pow2), 
			a < 1.6);
	}
}


//------------------------------------------
// Copyright (c) BrentBurley
// All Rights Reserved
//------------------------------------------
namespace Burley {
	template<typename _Ts>
	_Ts isotropic_normal_distribution(_Ts m_dot_n, _Ts r0, _Ts r1) {
		
	}
}

//------------------------------------------
// Copyright (c) KarisBrian
// All Rights Reserved
//------------------------------------------
namespace Karis {
	template<typename _Ts>
	_Ts Smith_masking(_Ts v_dot_n, _Ts r) {
		const _Ts Two = static_cast<_Ts>(2);
		return     Two * v_dot_n 
			/*--------------------------*/
			/ (v_dot_n * (Two - r) + r);
	}
}


// F(mu) = F0 + (1-F0)*(1-mu)^5*mu
//       = F0 + (mu-F0*mu)*(1 + (-mu) + (-mu)^2 + (-mu)^3 + (-mu)^4 + (-mu)^5)
//       = F0 + mu + mu*-mu + mu*(-mu)^2 + mu*(-mu)^3 + mu*(-mu)^4 + mu*(-mu)^5 
//            -F0*mu - F0*mu*(-mu) - F0*mu*(-mu)^2 - F0*mu*(-mu)^3 - F0*mu*(-mu)^4 - F0*mu*(-mu)^5)
//       = F0 + mu - mu^2 + mu^3 - mu^4 + mu^5 - mu^6
//            - F0*mu + F0*mu^2 - F0*mu^3 + F0*mu^4 - F0*mu^5 + F0*mu^6 
//       = F0 + (1-F0)*mu - (1-F0)*mu^2 + (1-F0)*mu^3 - (1-F0)*mu^4 + (1-F0)*mu^5 - (1-F0)*mu^6
// antiderivative(F(mu)) = F0*mu + (1-F0)/2*mu^2 - (1-F0)/3*mu^3 + (1-F0)/4*mu^4 - (1-F0)/5*mu^5 + (1-F0)/6*mu^6 - (1-F0)/7*mu^7
// integrate(F(mu), 0, 1) = [F0*mu + (1-F0)/2*mu^2 - (1-F0)/3*mu^3 + (1-F0)/4*mu^4 - (1-F0)/5*mu^5 + (1-F0)/6*mu^6 - (1-F0)/7*mu^7]( 0 to 1 )
// integrate(F(mu), 0, 1) = F0 + (1-F0)/2 - (1-F0)/3 + (1-F0)/4 - (1-F0)/5 + (1-F0)/6 - (1-F0)/7
// integrate(F(mu), 0, 1) = F0 + (1-F0)*2520/5040 - (1-F0)*1680/5040 + (1-F0)*1260/5040 - (1-F0)*1008/5040 + (1-F0)*840/5040 - (1-F0)*720/5040
// integrate(F(mu), 0, 1) = F0 + (1-F0)*101/420
// integrate(F(mu), 0, 1) = 101/420 + (1 - 101/420)*F0

#endif