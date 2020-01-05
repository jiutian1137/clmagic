#pragma once
#ifndef _CLMAGIC_CORE_GEOMETRY_MATH_ILLUMUNATION_INLINE_H_
#define _CLMAGIC_CORE_GEOMETRY_MATH_ILLUMUNATION_INLINE_H_
#include "../illumunation.h"

namespace clmagic 
{
	template<typename T> inline
		T attenuation(_in(T) _Dis, _in(T) _Falloffstart, _in(T) _Falloffend)
		{
		return ( clamp((_Falloffend - _Dis) / (_Falloffstart - _Falloffend), T(0), T(1)) );
		}

	template<typename T> inline
		Vector3_<T> sun_direction(_in(T) _Time)
		{
		return (normalize(Vector3_<T>(T(0), T(sin(_Time)), T(cos(_Time)))));
		}

	template<typename T> inline
		T sun_amount(_in(Vector3_<T>) _Eyedir, _in(T) _Time)
		{
		return (dot(_Eyedir, sun_direction(_Time)));
		}

	template<typename T> inline
		T HenyeyGreenstein(_in(Vector3_<T>) _Tolight, _in(Vector3_<T>) _Toeye, _in(T) g)
		{	// (1/4¦°) * { (1-g^2) / pow(1+g^2 - 2*g*cos(¦È), 3/2) }
		return ( T(0.0795774693) /* 1/4¦° */
			   * (T(1) - g * g)  /* (1 - g^2) */
			   / pow(T(1) + g * g - T(2) * g * dot(_Tolight, _Toeye), T(3) / T(2)) /* pow(1+g^2 - 2*g*cos(¦È), 3/2) */
			   );
		}

	template<typename T> inline
		T Lambert_CosineLaw(_in(Vector3_<T>) L, _in(Vector3_<T>) N)
		{	// max(0, cos(¦È))
		return ( maxval(T(0), dot(L, N)) );
		}

	template<typename T> inline
		Vector3_<T> Schlick_Fresnel(_in(Vector3_<T>) Rf, _in(Vector3_<T>) N, _in(Vector3_<T>) L)
		{	// Rf + (1-Rf) * (1-cos(N,L)^5
		return ( Rf + (T(1) - Rf) * pow(T(1) - Lambert_CosineLaw(L, N), 5) );
		}

	template<typename T> inline
		Vector3_<T> BlinnPhong_specalbedo(_in(Vector3_<T>) _Obj_Rf, _in(Vector3_<T>) _Obj_N, _in(T) _Obj_Shininess,
			_in(Vector3_<T>) _Tolight, _in(Vector3_<T>) _Toeye)
		{	// Schlick_Fresnel(Rf, N, L) * { ((m+8)/8) * Lambert(H, N)^m }
		Vector3_<T> _Halfvec = normalize(_Tolight + _Toeye);
		T _Microfactor = _Obj_Shininess * T(256);

		return ( Schlick_Fresnel(_Obj_Rf, _Obj_N, _Tolight) /* Schlick_Fresnel(Rf, N, L) */
			 * (_Microfactor + T(8)) / T(8) * pow(Lambert_CosineLaw(_Halfvec, _Obj_N), _Microfactor) /* ((m+8)/8) * Lambert(H, N)^m */
				);
		}

	template<typename T> inline
		Vector3_<T> BlinnPhong( _in(Vector3_<T>) _Lstrenth, _in(Vector3_<T>) _Objr0, _in(Vector3_<T>) _Objnormal, _in(T) _Objroughness, _in(Vector3_<T>) _Objdiffuse,
			_in(Vector3_<T>) _Tolight, _in(Vector3_<T>) _Toeye)
		{	// _Lstrenth * (_Objdiffuse + BlinnPhong_specalbedo(_Objr0, _Objnormal, _Objroughness, _Tolight, _Toeye))
		Vector3_<T> _Specalbedo = BlinnPhong_specalbedo(_Objr0, _Objnormal, _Objroughness, _Tolight, _Toeye);
		_Specalbedo /= (_Specalbedo + T(1));
		return ( _Lstrenth * (_Objdiffuse + _Specalbedo) );
		}

}// namespace clmagic

#endif