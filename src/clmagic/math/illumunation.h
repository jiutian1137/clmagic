#pragma once
#ifndef clmagic_math_ILLUMUNATION_h_
#define clmagic_math_ILLUMUNATION_h_
#include "lapack.h"

namespace clmagic 
{
	template<typename T>
		T attenuation(_in(T) _Dis, _in(T) _Falloffstart, _in(T) _Falloffend);

	template<typename T> 
		vector3<T> sun_direction(_in(T) _Time);

	template<typename T> 
		T sun_amount(_in(vector3<T>) _Eyedir, _in(T) _Time);
	
	/*
	used to cloud illumunation
		@_Equaltion: (1/4¦°) * { (1-g^2) / pow(1+g^2 - 2*g*cos(¦È), 3/2) }
		@_Tolight as to light vector, require normalize vector
		@_Toeye as to eyepos vector,  require normalize vector
		@_g as eccentricity, example:0.2
	*/
	template<typename T>
		T HenyeyGreenstein(_in(vector3<T>) _Tolight, _in(vector3<T>) _Toeye, _in(T) g);
		
	/*
	used to illumunation
		@_Equaltion: max(0, cos(¦È))
		@_L as to light vector,      require normalize vector
		@_N as object normal vector, require normalize vector
	*/
	template<typename T>
		T Lambert_CosineLaw(_in(vector3<T>) L, _in(vector3<T>) N);

	/*
	used to BPR render 
		@_Equaltion: Rf + (1-Rf) * (1-cos(N,L)^5
		@_Rf as Reflection factor, require { 0 <= Rf <= 1 }, (1-Rf) refraction factor
		@_N as object norma,    require normalize vector
		@_L as to light vector, require normalize vector
	*/
	template<typename T>
		vector3<T> Schlick_Fresnel(_in(vector3<T>) Rf, _in(vector3<T>) N, _in(vector3<T>) L);

	/*
	used to BPR render
		@_Equaltion: Schlick_Fresnel(Rf, N, L) * { ((m+8)/8) * Lambert(H, N)^m }, m=shiness*256
		@_Obj_Rf as object's reflection factor, require [0, 1]
		@_Obj_N  as object's normal,  require normalize vector
		@_Obj_Shininess as object's shininess,  require [0, 1]
		@_Tolight as to light vector, require normalize vector
		@_Toeye   as to eye vector,   require normalize vector
	*/
	template<typename T>
		vector3<T> BlinnPhong_specalbedo(
			_in(vector3<T>) _Obj_Rf,
			_in(vector3<T>) _Obj_N,
			_in(T)		     _Obj_Shininess,
			_in(vector3<T>) _Tolight,
			_in(vector3<T>) _Toeye);

	/*
	used to BPR render
		@_Eqaltion: _Lstrenth * ( _Objdiffuse + BlinnPhong_specalbedo(_Objr0, _Objnormal, _Objroughness, _Tolight, _Toeye) )
	*/
	template<typename T>
		vector3<T> BlinnPhong(
			_in(vector3<T>) _Lstrenth,
			_in(vector3<T>) _Objr0, _in(vector3<T>) _Objnormal, _in(T) _Objroughness, _in(vector3<T>) _Objdiffuse,
			_in(vector3<T>) _Tolight, _in(vector3<T>) _Toeye);

	template<typename _Ty>
	_Ty hdr(_in(_Ty) L, _in(_Ty) white) {
		auto r1 = static_cast<_Ty>(1);
		return ( L + L*L/(white*white)
			 /*-----------------------*/
			  / ( L + r1 )
			);
		/*
		L + L^2 / white^2
		---------------------
		   L + 1
		*/
	}

}// namespace clmagic 

#endif