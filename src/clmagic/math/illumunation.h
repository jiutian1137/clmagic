#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH___ILLUMUNATION___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH___ILLUMUNATION___H__
#include "lapack.h"

namespace clmagic 
{
	template<typename T>
		T attenuation(_in(T) _Dis, _in(T) _Falloffstart, _in(T) _Falloffend);

	template<typename T> 
		Vec3_<T> sun_direction(_in(T) _Time);

	template<typename T> 
		T sun_amount(_in(Vec3_<T>) _Eyedir, _in(T) _Time);
	
	/*
	used to cloud illumunation
		@_Equaltion: (1/4¦°) * { (1-g^2) / pow(1+g^2 - 2*g*cos(¦È), 3/2) }
		@_Tolight as to light vector, require normalize vector
		@_Toeye as to eyepos vector,  require normalize vector
		@_g as eccentricity, example:0.2
	*/
	template<typename T>
		T HenyeyGreenstein(_in(Vec3_<T>) _Tolight, _in(Vec3_<T>) _Toeye, _in(T) g);
		
	/*
	used to illumunation
		@_Equaltion: max(0, cos(¦È))
		@_L as to light vector,      require normalize vector
		@_N as object normal vector, require normalize vector
	*/
	template<typename T>
		T Lambert_CosineLaw(_in(Vec3_<T>) L, _in(Vec3_<T>) N);

	/*
	used to BPR render 
		@_Equaltion: Rf + (1-Rf) * (1-cos(N,L)^5
		@_Rf as Reflection factor, require { 0 <= Rf <= 1 }, (1-Rf) refraction factor
		@_N as object norma,    require normalize vector
		@_L as to light vector, require normalize vector
	*/
	template<typename T>
		Vec3_<T> Schlick_Fresnel(_in(Vec3_<T>) Rf, _in(Vec3_<T>) N, _in(Vec3_<T>) L);

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
		Vec3_<T> BlinnPhong_specalbedo(
			_in(Vec3_<T>) _Obj_Rf, 
			_in(Vec3_<T>) _Obj_N, 
			_in(T)		  _Obj_Shininess, 
			_in(Vec3_<T>) _Tolight, 
			_in(Vec3_<T>) _Toeye);

	/*
	used to BPR render
		@_Eqaltion: _Lstrenth * ( _Objdiffuse + BlinnPhong_specalbedo(_Objr0, _Objnormal, _Objroughness, _Tolight, _Toeye) )
	*/
	template<typename T>
		Vec3_<T> BlinnPhong(
			_in(Vec3_<T>) _Lstrenth, 
			_in(Vec3_<T>) _Objr0, _in(Vec3_<T>) _Objnormal, _in(T) _Objroughness, _in(Vec3_<T>) _Objdiffuse,
			_in(Vec3_<T>) _Tolight, _in(Vec3_<T>) _Toeye);

}// namespace clmagic 

#endif