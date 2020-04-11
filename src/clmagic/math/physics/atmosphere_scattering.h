#pragma once
#ifndef clmagic_math_PHYSICS_h_
#define clmagic_math_PHYSICS_h_
#include "../lapack/vector.h"

namespace clmagic {

	template<typename T>
	struct scattering_coefficient {
		vector3<T> extinction;
		vector3<T> total_scattering;// scattering coefficient 
		vector3<T> angular_scattering;// !!precision problem

		static scattering_coefficient<T> Rayleigh() {
			const auto _Wavelengths = std::array<double, 3>{ 680e-9, 550e-9, 440e-9 };
			      auto _Sctr        = scattering_coefficient<T>();

			double n  = 1.0003;
			double N  = 2.545e+25;
			double Pn = 0.035;
			double C  = 8.0 * pow(3.14159, 3) * pow(n * n - 1.0, 2) * (6.0 + 3.0 * Pn)
				      /*-------------------------------------------------------------*/
				      / ((3.0 * N) * (6.0 - 7.0 * Pn));

			for (size_t i = 0; i < 3; ++i) {
				double _Lambda2  = pow(_Wavelengths[i], 2);
				double _Lambda4  = pow(_Wavelengths[i], 4);
				double _Sctrcoef = C / _Lambda4;
				_Sctr.total_scattering[i] = static_cast<T>(_Sctrcoef);

				// Angular scattering coefficient is essentially volumetric scattering coefficient multiplied by the
				// normalized phase function
				// p(Theta) = 3/(16*Pi) * (1 + cos^2(Theta))
				// aR contains all the terms exepting 1 + cos^2(Theta):
				_Sctr.angular_scattering[i] = (T)(3 /* (1+cos^2(θ)) igonore */
											 /*--------------*/ * _Sctrcoef
											 / (16 * 3.14159));
			}
			_Sctr.extinction = _Sctr.total_scattering;

			return ( _Sctr );
		}

		static scattering_coefficient<T> Mie(bool _Preetham_method = false, T _Turbidity = T(1.02)) {
			const auto _Wavelengths = std::array<double, 3>{ 680e-9, 550e-9, 440e-9 };
			      auto _Sctr        = scattering_coefficient<T>();

			if (_Preetham_method) {
				double K[] = {
					 0.68455,					/* K[650nm] */
					 0.678781,					/* K[570nm] */
					(0.668532 + 0.669765) / 2.0 /* (K[470nm]+K[480nm])/2 */
				};

				assert(_Turbidity >= 1.f);

				// Beta is an Angstrom's turbidity coefficient and is approximated by:
				//float beta = 0.04608365822050f * m_fTurbidity - 0.04586025928522f; ???????
				double c = (0.6544 * _Turbidity - 0.6510) * 1E-16; // concentration factor
				double v = 4; // Junge's exponent

				double dTotalMieBetaTerm = 0.434 * c * 3.14159 * pow(2.0 * 3.14159, v - 2);

				for (size_t i = 0; i < 3; ++i) {
					double Lambdav_minus_2    = pow(_Wavelengths[i], v - 2);
					_Sctr.total_scattering[i] = static_cast<T>(dTotalMieBetaTerm * K[i] / Lambdav_minus_2);
				}
			} else {
				const T _Mie_beta = 2e-5f * 1.f/*g_post_proces_attrib.m_fAerosolDensityScale*/;
				_Sctr.total_scattering = vector3<T>(_Mie_beta, _Mie_beta, _Mie_beta);
			}

			for (size_t i = 0; i != 3; ++i) {
				// Normalized to unity Cornette-Shanks phase function has the following form:
				// F(theta) = 1/(4*PI) * 3*(1-g^2) / (2*(2+g^2)) * (1+cos^2(theta)) / (1 + g^2 - 2g*cos(theta))^(3/2)
				// The angular scattering coefficient is the volumetric scattering coefficient multiplied by the phase 
				// function. 1/(4*PI) is baked into the aM, the other terms are baked into f4CS_g
				_Sctr.angular_scattering[i] = _Sctr.total_scattering[i] / static_cast<T>(4.f * 3.14159f);
				// [BN08] also uses slight absorption factor which is 10% of scattering
				_Sctr.extinction[i] = _Sctr.total_scattering[i] * (T(1.0) * T(0.1)/*g_post_proces_attrib.m_fAerosolAbsorbtionScale*/);
			}
			return ( _Sctr );
		}
	};// struct scattering_coefficient<>

}// namespace clmagic

#endif