#pragma once
#ifndef _CLMAGIC_CORE_GEOMETRY_NOISE_VALUE_H_
#define _CLMAGIC_CORE_GEOMETRY_NOISE_VALUE_H_
#include "../fraction.h"

/*
@ Copy right from value_noise article...
*/

namespace clmagic 
{
	template<typename _Ty> inline
		Vec2_<_Ty> v_hash22(Vec2_<_Ty> p)
		{
		constexpr auto _C_0o1031 = _Ty(0.1031);
		constexpr auto _C_0o1030 = _Ty(0.1030);
		constexpr auto _C_0o0973 = _Ty(0.0973);
		constexpr auto _C_19o19 = _Ty(19.19);

		Vec3_<_Ty> p3 = fract( p.V<3>("xyx") * Vec3_<_Ty>(_C_0o1031,_C_0o1030,_C_0o0973) );

		p3 += Vec3_<_Ty>( dot(p3, p3.V<3>("yzx") + Vec3_<_Ty>(_C_19o19)) );

		return ( Vec2_<_Ty>(fract((p3.x + p3.y) * p3.z), _Ty(0)) );
		}

	template<typename _Ty> inline
		_Ty value2(Vec2_<_Ty> p, _Hash22_func_pointer<_Ty> _Hash)
		{
		Vec2_<_Ty> _Pi = floor(p);
		Vec2_<_Ty> _Pf = fract(p);
		Vec2_<_Ty> u = fade(_Pf);

		auto _A = _Hash(_Pi + Vec2_<_Ty>(0.f, 0.f)).x;
		auto _B = _Hash(_Pi + Vec2_<_Ty>(1.f, 0.f)).x;
		auto _C = _Hash(_Pi + Vec2_<_Ty>(0.f, 1.f)).x;
		auto _D = _Hash(_Pi + Vec2_<_Ty>(1.f, 1.f)).x;

		auto _Result = lerp(lerp(_A, _B, u.x),
							lerp(_C, _D, u.x),
							u.y);
		return (_Result);
		}

	template<typename _Ty> inline
		_Ty value3(Vec3_<_Ty> x, _Hash33_func_pointer<_Ty> _Hash)
		{
		Vec3_<_Ty> _Pi = floor(x);
		Vec3_<_Ty> _Pf = fract(x);
		Vec3_<_Ty> u = fade(_Pf);

		_Ty _A = _Hash(_Pi + Vec3_<_Ty>(0.f, 0.f, 0.f)).x;
		_Ty _B = _Hash(_Pi + Vec3_<_Ty>(1.f, 0.f, 0.f)).x;
		_Ty _C = _Hash(_Pi + Vec3_<_Ty>(0.f, 1.f, 0.f)).x;
		_Ty _D = _Hash(_Pi + Vec3_<_Ty>(1.f, 1.f, 0.f)).x;

		_Ty _E = _Hash(_Pi + Vec3_<_Ty>(0.f, 0.f, 1.f)).x;
		_Ty _F = _Hash(_Pi + Vec3_<_Ty>(1.f, 0.f, 1.f)).x;
		_Ty _G = _Hash(_Pi + Vec3_<_Ty>(0.f, 1.f, 1.f)).x;
		_Ty _H = _Hash(_Pi + Vec3_<_Ty>(1.f, 1.f, 1.f)).x;

		_Ty _Value = lerp(lerp(lerp(_A, _B, u.x), 
							   lerp(_C, _D, u.x),
							   u.y),
						  lerp(lerp(_E, _F, u.x), 
							   lerp(_G, _H, u.x),
							   u.y), 
						  u.z);
		return ( _Value );
		}

	template<typename _Ty> inline
		Vec4_<_Ty> valued3(Vec3_<_Ty> x, _Hash33_func_pointer<_Ty>)
		{
		Vec3_<_Ty> _Pi = floor(x);
		Vec3_<_Ty> _Pf = fract(x);
		Vec3_<_Ty> u = s_curve(_Pf);
		Vec3_<_Ty> du = s_curved(_Pf);

		_Ty n = _Pi.x + _Pi.y * _Ty(157) + _Pi.z * _Ty(113);

		auto _Hash = [](_Ty n) { return fract(std::sin(n) * _Ty(753.5453123)); };

		_Ty a = _Hash(n + _Ty(0));
		_Ty b = _Hash(n + _Ty(1));
		_Ty c = _Hash(n + _Ty(157));
		_Ty d = _Hash(n + _Ty(158));
		_Ty e = _Hash(n + _Ty(113));
		_Ty f = _Hash(n + _Ty(114));
		_Ty g = _Hash(n + _Ty(270));
		_Ty h = _Hash(n + _Ty(271));

		_Ty k0 = a;
		_Ty k1 = b - a;
		_Ty k2 = c - a;
		_Ty k3 = e - a;
		_Ty k4 = a - b - c + d;
		_Ty k5 = a - c - e + g;
		_Ty k6 = a - b - e + f;
		_Ty k7 = -a + b + c - d + e - f - g + h;

		_Ty _Value = k0
				   + k1 * u.x
				   + k2 * u.y
				   + k3 * u.z
				   + k4 * u.x * u.y
				   + k5 * u.y * u.z
				   + k6 * u.z * u.x
				   + k7 * u.x * u.y * u.z;

		Vec3_<_Ty> _Derivative = du * (Vec3(k1, k2, k3)
								     + Vec3(k4, k5, k6) * u.V<3>("YZX")
								     + Vec3(k6, k4, k5) * u.V<3>("ZXY")
								     + k7 * u.V<3>("YZX") * u.V<3>("ZXY"));
		return ( Vec4_<_Ty>(_Value, _Derivative) );
		}

}// namespace clmagic

#endif