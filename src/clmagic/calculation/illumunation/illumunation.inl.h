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
		vector3<T> sun_direction(_in(T) _Time)
		{
		return (normalize(vector3<T>(T(0), T(sin(_Time)), T(cos(_Time)))));
		}

	template<typename T> inline
		T sun_amount(_in(vector3<T>) _Eyedir, _in(T) _Time)
		{
		return (dot(_Eyedir, sun_direction(_Time)));
		}

	template<typename T> inline
		T HenyeyGreenstein(_in(vector3<T>) _Tolight, _in(vector3<T>) _Toeye, _in(T) g)
		{	// (1/4¦°) * { (1-g^2) / pow(1+g^2 - 2*g*cos(¦È), 3/2) }
		return ( T(0.0795774693) /* 1/4¦° */
			   * (T(1) - g * g)  /* (1 - g^2) */
			   / pow(T(1) + g * g - T(2) * g * dot(_Tolight, _Toeye), T(3) / T(2)) /* pow(1+g^2 - 2*g*cos(¦È), 3/2) */
			   );
		}

	template<typename T> inline
		T Lambert_CosineLaw(_in(vector3<T>) L, _in(vector3<T>) N)
		{	// max(0, cos(¦È))
		return ( maxval(T(0), dot(L, N)) );
		}

	template<typename T> inline
		vector3<T> Schlick_Fresnel(_in(vector3<T>) Rf, _in(vector3<T>) N, _in(vector3<T>) L)
		{	// Rf + (1-Rf) * (1-cos(N,L)^5
		return ( Rf + (T(1) - Rf) * pow(T(1) - Lambert_CosineLaw(L, N), 5) );
		}

	template<typename T> inline
		vector3<T> BlinnPhong_specalbedo(_in(vector3<T>) _Obj_Rf, _in(vector3<T>) _Obj_N, _in(T) _Obj_Shininess,
			_in(vector3<T>) _Tolight, _in(vector3<T>) _Toeye)
		{	// Schlick_Fresnel(Rf, N, L) * { ((m+8)/8) * Lambert(H, N)^m }
		vector3<T> _Halfvec = normalize(_Tolight + _Toeye);
		T _Microfactor = _Obj_Shininess * T(256);

		return ( Schlick_Fresnel(_Obj_Rf, _Obj_N, _Tolight) /* Schlick_Fresnel(Rf, N, L) */
			 * (_Microfactor + T(8)) / T(8) * pow(Lambert_CosineLaw(_Halfvec, _Obj_N), _Microfactor) /* ((m+8)/8) * Lambert(H, N)^m */
				);
		}

	template<typename T> inline
		vector3<T> BlinnPhong( _in(vector3<T>) _Lstrenth, _in(vector3<T>) _Objr0, _in(vector3<T>) _Objnormal, _in(T) _Objroughness, _in(vector3<T>) _Objdiffuse,
			_in(vector3<T>) _Tolight, _in(vector3<T>) _Toeye)
		{	// _Lstrenth * (_Objdiffuse + BlinnPhong_specalbedo(_Objr0, _Objnormal, _Objroughness, _Tolight, _Toeye))
		vector3<T> _Specalbedo = BlinnPhong_specalbedo(_Objr0, _Objnormal, _Objroughness, _Tolight, _Toeye);
		_Specalbedo /= (_Specalbedo + T(1));
		return ( _Lstrenth * (_Objdiffuse + _Specalbedo) );
		}

}// namespace clmagic

#endif