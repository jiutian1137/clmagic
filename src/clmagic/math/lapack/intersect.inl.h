#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH_LAPACK___INTERSECT___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH_LAPACK___INTERSECT___H__
#include "../lapack.h"

namespace clmagic 
{
	template<typename _Ty> inline
		bool intersect(_in(Vector2_<_Ty>) _Pos, _Ty _Left, _Ty _Right, _Ty _Bottom, _Ty _Top)
		{	// _Pos in [_Left,_Right,_Bottom,_Top]
		return (_Pos.x < _Left || _Pos.x > _Right || _Pos.y < _Bottom || _Pos.y > _Top);
		}

	template<typename T> inline
		int intersect(_in(Vector3_<T>) _Pos, _in(Vector3_<T>) _V0, _in(Vector3_<T>) _V1, _in(Vector3_<T>) _V2)
		{	// _Pos in triangle face
		Vector3_<T> v0 = _V1 - _V0;
		Vector3_<T> v1 = _V2 - _V0;
		Vector3_<T> v2 = _Pos - _V0;

		T dot00, dot01, dot02, dot11, dot12;
		dot00 = dot(v0, v0);
		dot01 = dot(v0, v1);
		dot02 = dot(v0, v2);
		dot11 = dot(v1, v1);
		dot12 = dot(v1, v2);

		T invD = 1.0f / (dot00 * dot11 - dot01 * dot01);
		T u = (dot11 * dot02 - dot01 * dot12) * invD;
		if (u < T(0) || u > T(1))
			return (0);

		T v = (dot00 * dot12 - dot01 * dot02) * invD;
		if (v <  T(0) || v + u > T(1))
			return (0);

		if (u + v > T(0.7))
			return (1); //CB
		else if (u > v)
			return (2); //AC
		else
			return (3); //AB
		}

	template<typename T> inline 
		bool intersect(_in(Vector3_<T>) _Origin, _in(Vector3_<T>) _Dir, _in(Vector3_<T>) _V0, _in(Vector3_<T>) _V1, _in(Vector3_<T>) _V2, _out(Vector3_<T>) _Point)
		{
		Vector3_<T> _U, _V, _N;
		Vector3_<T> _W0, _W;
		T _R, _A, _B;

		_U = _V1 - _V0;
		_V = _V2 - _V0;
		_N = cross3(_U, _V);
		if ( _N.length() < 0.1 )
			return (false);

		_W0 = _Origin - _V0;
		_A = -dot(_N, _W0);
		_B = dot(_N, _Dir);
		if ( abs(_B) < Real(0.1) )
			return (false);

		_R = _A / _B;
		if (_R < Real(0) || _R > Real(1))
			return (false);

		_Point = _Origin + _R * _Dir;

		T _UdotU, _UdotV, _VdotV, _WdotU, _WdotV, _InvD;

		_UdotU = dot(_U, _U);
		_UdotV = dot(_U, _V);
		_VdotV = dot(_V, _V);
		_W = _Point - _V0;
		_WdotU = dot(_W, _U);
		_WdotV = dot(_W, _V);
		_InvD = 1.0f / (_UdotV * _UdotV - _UdotU * _VdotV);

		T _S = (_UdotV * _WdotV - _VdotV * _WdotU) * _InvD;
		if ( _S < Real(0) || _S > Real(1) )
			return (false);

		T _T = (_UdotV * _WdotU - _UdotU * _WdotV) * _InvD;
		if ( _T < Real(0) || _T > Real(1) )
			return (false);

		return (true);
		}

}// namespace clmagic

#endif