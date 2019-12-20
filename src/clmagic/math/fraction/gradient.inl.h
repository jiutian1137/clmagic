#pragma once
#ifndef _CLMAGIC_CORE_GEOMETRY_NOISE_GRADIENT_H_
#define _CLMAGIC_CORE_GEOMETRY_NOISE_GRADIENT_H_
#include "../fraction.h"


namespace clmagic
{
	/* Copyright @ 2017 Inigo Quilez */

	template<typename _Ty> inline
		Vec2_<_Ty> g_hash22(Vec2_<_Ty> _X)  // replace this by something better
		{
		Vec2_<_Ty> k = Vec2(0.3183099f, 0.3678794f);
		_X = _X * k + Vec2(k.y, k.x);
		return ( Vec2_<_Ty>(-1) 
			  + _Ty(2) * fract( _Ty(16) * k * fract( _X.x*_X.y*(_X.x+_X.y) ) ) );
		}

	template<typename _Ty> inline
		Vec3_<_Ty> g_hash33(Vec3_<_Ty> p) // replace this by something better. really. do
		{
		p = Vec3(dot(p, Vec3(_Ty(127.1), _Ty(311.7), _Ty(74.7))),
				 dot(p, Vec3(_Ty(269.5), _Ty(183.3), _Ty(246.1))),
				 dot(p, Vec3(_Ty(113.5), _Ty(271.9), _Ty(124.6))));

		return (Vec3_<_Ty>(-1) + _Ty(2) * fract( sin(p) * _Ty(43758.5453123) ) );
		}


	template<typename _Ty> inline
		_Ty gradient2(Vec2_<_Ty> _Pos, _Hash22_func_pointer<_Ty> _Hash)
		{	// return gradient noise
		/*
		(0,0)	  (1,0)
		A-----------B
		|		    |
		|		    |
		|			|
		C-----------D
		(1,0)		(1,1)
		*/
		Vec2 _Pi = floor(_Pos);
		Vec2 _Pf = fract(_Pos);

		// if <quintic interpolation>
		Vec2 u = fade(_Pf);
		// else if <cubic interpolation >
		//vec2 u = s_curve(_Pf);

		Vec2_<_Ty> ga = _Hash(_Pi + Vec2_<_Ty>(0.0f, 0.0f));
		Vec2_<_Ty> gb = _Hash(_Pi + Vec2_<_Ty>(1.0f, 0.0f));
		Vec2_<_Ty> gc = _Hash(_Pi + Vec2_<_Ty>(0.0f, 1.0f));
		Vec2_<_Ty> gd = _Hash(_Pi + Vec2_<_Ty>(1.0f, 1.0f));

		_Ty _A = dot(ga, _Pf - Vec2_<_Ty>(0.0f, 0.0f));
		_Ty _B = dot(gb, _Pf - Vec2_<_Ty>(1.0f, 0.0f));
		_Ty _C = dot(gc, _Pf - Vec2_<_Ty>(0.0f, 1.0f));
		_Ty _D = dot(gd, _Pf - Vec2_<_Ty>(1.0f, 1.0f));

		_Ty _Value = lerp(lerp(_A, _B, u.x),
						  lerp(_C, _D, u.x),
						  u.y);

		return (_Value);
		}

	template<typename _Ty> inline
		Vec3_<_Ty> gradientd2(Vec2_<_Ty> _Pos, _Hash22_func_pointer<_Ty> _Hash)
		{	// return gradient noise (in x) and its derivatives (in yz)
		Vec2 _Pi = floor(_Pos);
		Vec2 _Pf = fract(_Pos);

		// if <quintic interpolation>
		Vec2 u = fade(_Pf);
		Vec2 du = faded(_Pf);
		// else if <cubic interpolation >
		//vec2 u = s_curve(_Pf);
		//vec2 du = s_curved(_Pf);

		Vec2_<_Ty> ga = _Hash( _Pi + Vec2_<_Ty>(0.0f, 0.0f) );
		Vec2_<_Ty> gb = _Hash( _Pi + Vec2_<_Ty>(1.0f, 0.0f) );
		Vec2_<_Ty> gc = _Hash( _Pi + Vec2_<_Ty>(0.0f, 1.0f) );
		Vec2_<_Ty> gd = _Hash( _Pi + Vec2_<_Ty>(1.0f, 1.0f) );

		_Ty _A = dot( ga, _Pf - Vec2_<_Ty>(0.0f, 0.0f) );
		_Ty _B = dot( gb, _Pf - Vec2_<_Ty>(1.0f, 0.0f) );
		_Ty _C = dot( gc, _Pf - Vec2_<_Ty>(0.0f, 1.0f) );
		_Ty _D = dot( gd, _Pf - Vec2_<_Ty>(1.0f, 1.0f) );

		float _Value = lerp(lerp(_A, _B, u.x),
							lerp(_C, _D, u.x),
							u.y);

		Vec2_<_Ty> _Derivatives = ga 
								+ u.x * (gb - ga) 
								+ u.y * (gc - ga) 
								+ u.x * u.y * (ga - gb - gc + gd)
								+ du * ( u.V<2>("YX") * (_A - _B - _C + _D)
									   + Vec2_<_Ty>(_B, _C) - Vec2_<_Ty>(_A) );

		return ( Vec3_<_Ty>(_Value, _Derivatives) );
		}
	
	template<typename _Ty> inline
		_Ty gradient3(Vec3_<_Ty> _Pos, _Hash33_func_pointer<_Ty> _Hash)
		{
		// grid
		Vec3_<_Ty> _Pi = floor(_Pos);
		Vec3_<_Ty> _Pf = fract(_Pos);

		// if <quintic interpolation>
		Vec3_<_Ty> u = fade(_Pf);
		// else if <cubic interpolation >
		//vec3 u = s_curve(_Pf);

		// gradients
		Vec3_<_Ty> ga = _Hash(_Pi + Vec3_<_Ty>(0.f, 0.f, 0.f));
		Vec3_<_Ty> gb = _Hash(_Pi + Vec3_<_Ty>(1.f, 0.f, 0.f));
		Vec3_<_Ty> gc = _Hash(_Pi + Vec3_<_Ty>(0.f, 1.f, 0.f));
		Vec3_<_Ty> gd = _Hash(_Pi + Vec3_<_Ty>(1.f, 1.f, 0.f));
		Vec3_<_Ty> ge = _Hash(_Pi + Vec3_<_Ty>(0.f, 0.f, 1.f));
		Vec3_<_Ty> gf = _Hash(_Pi + Vec3_<_Ty>(1.f, 0.f, 1.f));
		Vec3_<_Ty> gg = _Hash(_Pi + Vec3_<_Ty>(0.f, 1.f, 1.f));
		Vec3_<_Ty> gh = _Hash(_Pi + Vec3_<_Ty>(1.f, 1.f, 1.f));

		// projections
		_Ty _A = dot(ga, _Pf - Vec3_<_Ty>(0.f, 0.f, 0.f));
		_Ty _B = dot(gb, _Pf - Vec3_<_Ty>(1.f, 0.f, 0.f));
		_Ty _C = dot(gc, _Pf - Vec3_<_Ty>(0.f, 1.f, 0.f));
		_Ty _D = dot(gd, _Pf - Vec3_<_Ty>(1.f, 1.f, 0.f));
		_Ty _E = dot(ge, _Pf - Vec3_<_Ty>(0.f, 0.f, 1.f));
		_Ty _F = dot(gf, _Pf - Vec3_<_Ty>(1.f, 0.f, 1.f));
		_Ty _G = dot(gg, _Pf - Vec3_<_Ty>(0.f, 1.f, 1.f));
		_Ty _H = dot(gh, _Pf - Vec3_<_Ty>(1.f, 1.f, 1.f));

		_Ty _Value = lerp(lerp(lerp(_A, _B, u.x),
							   lerp(_C, _D, u.x),
							   u.y),
						  lerp(lerp(_E, _F, u.x),
							   lerp(_G, _H, u.x),
							   u.y),
						  u.z);
		return (_Value);
	}

	template<typename _Ty> inline
		Vec4_<_Ty> gradientd3(Vec3_<_Ty> _Pos, _Hash33_func_pointer<_Ty> _Hash)
		{
		// grid
		Vec3_<_Ty> _Pi = floor(_Pos);
		Vec3_<_Ty> _Pf = fract(_Pos);

		// if <quintic interpolation>
		Vec3_<_Ty> u = fade(_Pf);
		Vec3_<_Ty> du = faded(_Pf);
		// else if <cubic interpolation >
		//vec3 u = s_curve(_Pf);
		//vec3 du = s_curved(_Pf);

		// gradients
		Vec3_<_Ty> ga = _Hash(_Pi + Vec3_<_Ty>(0.f, 0.f, 0.f));
		Vec3_<_Ty> gb = _Hash(_Pi + Vec3_<_Ty>(1.f, 0.f, 0.f));
		Vec3_<_Ty> gc = _Hash(_Pi + Vec3_<_Ty>(0.f, 1.f, 0.f));
		Vec3_<_Ty> gd = _Hash(_Pi + Vec3_<_Ty>(1.f, 1.f, 0.f));
		Vec3_<_Ty> ge = _Hash(_Pi + Vec3_<_Ty>(0.f, 0.f, 1.f));
		Vec3_<_Ty> gf = _Hash(_Pi + Vec3_<_Ty>(1.f, 0.f, 1.f));
		Vec3_<_Ty> gg = _Hash(_Pi + Vec3_<_Ty>(0.f, 1.f, 1.f));
		Vec3_<_Ty> gh = _Hash(_Pi + Vec3_<_Ty>(1.f, 1.f, 1.f));

		// projections
		_Ty _A = dot(ga, _Pf - Vec3_<_Ty>(0.f, 0.f, 0.f));
		_Ty _B = dot(gb, _Pf - Vec3_<_Ty>(1.f, 0.f, 0.f));
		_Ty _C = dot(gc, _Pf - Vec3_<_Ty>(0.f, 1.f, 0.f));
		_Ty _D = dot(gd, _Pf - Vec3_<_Ty>(1.f, 1.f, 0.f));
		_Ty _E = dot(ge, _Pf - Vec3_<_Ty>(0.f, 0.f, 1.f));
		_Ty _F = dot(gf, _Pf - Vec3_<_Ty>(1.f, 0.f, 1.f));
		_Ty _G = dot(gg, _Pf - Vec3_<_Ty>(0.f, 1.f, 1.f));
		_Ty _H = dot(gh, _Pf - Vec3_<_Ty>(1.f, 1.f, 1.f));

		_Ty _K1 = _A;
		_Ty _K2 = _B - _A;
		_Ty _K3 = _C - _A;
		_Ty _K4 = _E - _A;
		_Ty _K5 = _A - _B - _C + _D;
		_Ty _K6 = _A - _C - _E + _G;
		_Ty _K7 = _A - _B - _E + _F;
		_Ty _K8 = -_A + _B + _C - _D + _E - _F - _G + _H;

		_Ty _Value = _K1
				   + _K2 * u.x
				   + _K3 * u.y
				   + _K4 * u.z
				   + _K5 * u.x * u.y
				   + _K6 * u.y * u.z
				   + _K7 * u.z * u.x
				   + _K8 * u.x * u.y * u.z;

		Vec3_<_Ty> _Derivatives = ga
								+ u.x * (gb - ga)
								+ u.y * (gc - ga)
								+ u.z * (ge - ga)
								+ u.x * u.y * (ga - gb - gc + gd)
								+ u.y * u.z * (ga - gc - ge + gg)
								+ u.z * u.x * (ga - gb - ge + gf)
								+ u.x * u.y * u.z * (-ga + gb + gc - gd + ge - gf - gg + gh)
								+ du * (Vec3_<_Ty>(_K2, _K3, _K4)
									  + Vec3_<_Ty>(_K5, _K6, _K7) * u.V<3>("yzx")
									  + Vec3_<_Ty>(_K7, _K5, _K6) * u.V<3>("zxy")
									  + _K8 * u.V<3>("yzx") * u.V<3>("zxy"));

		return ( Vec4_<_Ty>(_Value, _Derivatives) );
		}

}// namespace clmagic

#endif