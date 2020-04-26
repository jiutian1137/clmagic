#pragma once
#ifndef _CLMAGIC_CORE_GEOMETRY_NOISE_GRADIENT_H_
#define _CLMAGIC_CORE_GEOMETRY_NOISE_GRADIENT_H_
#include "../fraction.h"


namespace clmagic
{
	/* Copyright @ 2017 Inigo Quilez */

	template<typename _Ty> inline
	vector2<_Ty> g_hash22(vector2<_Ty> _X)  // replace this by something better
		{
		auto k = vector2<_Ty>(R(0.3183099), R(0.3678794f));
		_X = _X * k + vector2<_Ty>(k[1], k[0]);
		return ( incr(R(2) * fract(R(16) * k * fract( _X[0]*_X[1]*(_X[0]+_X[1]) ) ),
					  R(-1)) );
		}

	template<typename _Ty> inline
	vector3<_Ty> g_hash33(vector3<_Ty> p) // replace this by something better. really. do
		{
		p = vector3<_Ty>(
			dot(p, vector3<_Ty>(_Ty(127.1), _Ty(311.7), _Ty(74.7))),
			dot(p, vector3<_Ty>(_Ty(269.5), _Ty(183.3), _Ty(246.1))),
			dot(p, vector3<_Ty>(_Ty(113.5), _Ty(271.9), _Ty(124.6))));

		return ( incr(R(2) * fract( sin(p) * R(43758.5453123) ), 
					  R(-1)) );
		}


	template<typename _Ty, typename _Fn> inline
	_Ty gradient2(vector2<_Ty> _Pos, _Fn _Hash22)
		{	// return gradient noise
		/*
		(0,0)	  (1,0)
		A-----------B
		|		    |
		|		    |
		|			|
		C-----------D
		(0,1)		(1,1)
		*/
		const auto _Pi = floor(_Pos);
		const auto _Pf = fract(_Pos);

		// if <quintic interpolation>
		const auto u   = fade(_Pf);
		// else if <cubic interpolation >
		//auto u = s_curve(_Pf);

		const auto _00 = vector2<_Ty>(_Ty(0), _Ty(0));
		const auto _10 = vector2<_Ty>(_Ty(1), _Ty(0));
		const auto _01 = vector2<_Ty>(_Ty(0), _Ty(1));
		const auto _11 = vector2<_Ty>(_Ty(1), _Ty(1));

		const auto ga  = _Hash22(_Pi + _00);
		const auto gb  = _Hash22(_Pi + _10);
		const auto gc  = _Hash22(_Pi + _01);
		const auto gd  = _Hash22(_Pi + _11);

		const _Ty  _A  = dot2(ga, _Pf - _00);
		const _Ty  _B  = dot2(gb, _Pf - _10);
		const _Ty  _C  = dot2(gc, _Pf - _01);
		const _Ty  _D  = dot2(gd, _Pf - _11);

		_Ty _Value = lerp(lerp(_A, _B, u[0]),
						  lerp(_C, _D, u[0]),
						  u[1]);

		return (_Value);
		}

	template<typename _Ty, typename _Fn> inline
	vector3<_Ty> gradientd2(vector3<_Ty> _Pos, _Fn _Hash22)
		{	// return gradient noise (in x) and its derivatives (in yz)
		const auto _Pi = floor(_Pos);
		const auto _Pf = fract(_Pos);

		// if <quintic interpolation>
		const auto u   = fade(_Pf);
		const auto du  = faded(_Pf);
		// else if <cubic interpolation >
		//auto u = s_curve(_Pf);
		//auto du = s_curved(_Pf);
		const auto _00 = vector2<_Ty>(_Ty(0), _Ty(0));
		const auto _10 = vector2<_Ty>(_Ty(1), _Ty(0));
		const auto _01 = vector2<_Ty>(_Ty(0), _Ty(1));
		const auto _11 = vector2<_Ty>(_Ty(1), _Ty(1));

		const auto ga  = _Hash22( _Pi + _00);
		const auto gb  = _Hash22( _Pi + _10);
		const auto gc  = _Hash22( _Pi + _01);
		const auto gd  = _Hash22( _Pi + _11);

		const _Ty _A   = dot( ga, _Pf - _00);
		const _Ty _B   = dot( gb, _Pf - _10);
		const _Ty _C   = dot( gc, _Pf - _01);
		const _Ty _D   = dot( gd, _Pf - _11);

		_Ty _Value = lerp(lerp(_A, _B, u.x),
						  lerp(_C, _D, u.x),
						  u.y);
		
		vector2<_Ty> _Derivatives = ga 
								+ u[0] * (gb - ga) 
								+ u[1] * (gc - ga) 
								+ u[0] * u[1] * (ga - gb - gc + gd)
								+ du * ( u.shuffle(1, 0) * (_A - _B - _C + _D) + vector2<_Ty>(_B, _C) - vector2<_Ty>(_A) );

		return ( vector3<_Ty>(_Value, _Derivatives[0], _Derivatives[1]) );
		}
	
	template<typename _Ty, typename _Fn> inline
	_Ty gradient3(vector3<_Ty> _Pos, _Fn _Hash33)
		{
		const auto _Pi  = floor(_Pos);
		const auto _Pf  = fract(_Pos);

		// if <quintic interpolation>
		const auto u    = fade(_Pf);
		// else if <cubic interpolation >
		//vec3 u = s_curve(_Pf);

		const auto _000 = vector3<_Ty>(_Ty(0), _Ty(0), _Ty(0));
		const auto _100 = vector3<_Ty>(_Ty(1), _Ty(0), _Ty(0));
		const auto _010 = vector3<_Ty>(_Ty(0), _Ty(1), _Ty(0));
		const auto _110 = vector3<_Ty>(_Ty(1), _Ty(1), _Ty(0));
		const auto _001 = vector3<_Ty>(_Ty(0), _Ty(0), _Ty(1));
		const auto _101 = vector3<_Ty>(_Ty(1), _Ty(0), _Ty(1));
		const auto _011 = vector3<_Ty>(_Ty(0), _Ty(1), _Ty(1));
		const auto _111 = vector3<_Ty>(_Ty(1), _Ty(1), _Ty(1));

		// gradients
		const auto ga   = _Hash33(_Pi + _000);
		const auto gb   = _Hash33(_Pi + _100);
		const auto gc   = _Hash33(_Pi + _010);
		const auto gd   = _Hash33(_Pi + _110);
		const auto ge   = _Hash33(_Pi + _001);
		const auto gf   = _Hash33(_Pi + _101);
		const auto gg   = _Hash33(_Pi + _011);
		const auto gh   = _Hash33(_Pi + _111);

		// projections
		const _Ty  _A   = dot(ga, _Pf - _000);
		const _Ty  _B   = dot(gb, _Pf - _100);
		const _Ty  _C   = dot(gc, _Pf - _010);
		const _Ty  _D   = dot(gd, _Pf - _110);
		const _Ty  _E   = dot(ge, _Pf - _001);
		const _Ty  _F   = dot(gf, _Pf - _101);
		const _Ty  _G   = dot(gg, _Pf - _011);
		const _Ty  _H   = dot(gh, _Pf - _111);

		_Ty _Value = lerp(lerp(lerp(_A, _B, u[0]),
							   lerp(_C, _D, u[0]),
							   u[1]),
						  lerp(lerp(_E, _F, u[0]),
							   lerp(_G, _H, u[0]),
							   u[1]),
						  u[2]);
		return (_Value);
	}

	template<typename _Ty, typename _Fn> inline
	vector4<_Ty> gradientd3(vector3<_Ty> _Pos, _Fn _Hash33)
		{
		// grid
		const auto _Pi  = floor(_Pos);
		const auto _Pf  = fract(_Pos);

		// if <quintic interpolation>
		const auto u    = fade(_Pf);
		const auto du   = faded(_Pf);
		// else if <cubic interpolation >
		//vec3 u = s_curve(_Pf);
		//vec3 du = s_curved(_Pf);

		const auto _000 = vector3<_Ty>(_Ty(0), _Ty(0), _Ty(0));
		const auto _100 = vector3<_Ty>(_Ty(1), _Ty(0), _Ty(0));
		const auto _010 = vector3<_Ty>(_Ty(0), _Ty(1), _Ty(0));
		const auto _110 = vector3<_Ty>(_Ty(1), _Ty(1), _Ty(0));
		const auto _001 = vector3<_Ty>(_Ty(0), _Ty(0), _Ty(1));
		const auto _101 = vector3<_Ty>(_Ty(1), _Ty(0), _Ty(1));
		const auto _011 = vector3<_Ty>(_Ty(0), _Ty(1), _Ty(1));
		const auto _111 = vector3<_Ty>(_Ty(1), _Ty(1), _Ty(1));

		// gradients
		const auto ga   = _Hash33(_Pi + _000);
		const auto gb   = _Hash33(_Pi + _100);
		const auto gc   = _Hash33(_Pi + _010);
		const auto gd   = _Hash33(_Pi + _110);
		const auto ge   = _Hash33(_Pi + _001);
		const auto gf   = _Hash33(_Pi + _101);
		const auto gg   = _Hash33(_Pi + _011);
		const auto gh   = _Hash33(_Pi + _111);

		// projections
		const _Ty  _A   = dot(ga, _Pf - _000);
		const _Ty  _B   = dot(gb, _Pf - _100);
		const _Ty  _C   = dot(gc, _Pf - _010);
		const _Ty  _D   = dot(gd, _Pf - _110);
		const _Ty  _E   = dot(ge, _Pf - _001);
		const _Ty  _F   = dot(gf, _Pf - _101);
		const _Ty  _G   = dot(gg, _Pf - _011);
		const _Ty  _H   = dot(gh, _Pf - _111);

		const _Ty  _K1  = _A;
		const _Ty  _K2  = _B - _A;
		const _Ty  _K3  = _C - _A;
		const _Ty  _K4  = _E - _A;
		const _Ty  _K5  = _A - _B - _C + _D;
		const _Ty  _K6  = _A - _C - _E + _G;
		const _Ty  _K7  = _A - _B - _E + _F;
		const _Ty  _K8  = -_A + _B + _C - _D + _E - _F - _G + _H;

		_Ty _Value = _K1
				   + _K2 * u.x
				   + _K3 * u.y
				   + _K4 * u.z
				   + _K5 * u.x * u.y
				   + _K6 * u.y * u.z
				   + _K7 * u.z * u.x
				   + _K8 * u.x * u.y * u.z;

		vector3<_Ty> _Derivatives = ga
								+ u.x * (gb - ga)
								+ u.y * (gc - ga)
								+ u.z * (ge - ga)
								+ u.x * u.y * (ga - gb - gc + gd)
								+ u.y * u.z * (ga - gc - ge + gg)
								+ u.z * u.x * (ga - gb - ge + gf)
								+ u.x * u.y * u.z * (-ga + gb + gc - gd + ge - gf - gg + gh)
								+ du * (vector3<_Ty>(_K2, _K3, _K4)
									  + vector3<_Ty>(_K5, _K6, _K7) * u.shuffle(1, 2, 0)
									  + vector3<_Ty>(_K7, _K5, _K6) * u.shuffle(2, 0, 1)
									  + _K8 * u.shuffle(1, 2, 0) * u.shuffle(2, 0, 1));

		return ( vector4<_Ty>(_Value, _Derivatives[0], _Derivatives[1], _Derivatives[2]) );
		}

}// namespace clmagic

#endif