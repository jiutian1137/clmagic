#pragma once
#ifndef _CLMAGIC_CORE_GEOMETRY_NOISE_WORLEY_H_
#define _CLMAGIC_CORE_GEOMETRY_NOISE_WORLEY_H_
#include "../fraction.h"

namespace clmagic
{
	template<typename _Ty> inline
		_Ty worley2(Vec2_<_Ty> _Pos, _Hash22_func_pointer<_Ty> _Hash)
		{
		using std::sqrt;
		Vec2_<_Ty> _Pi = floor(_Pos);
		Vec2_<_Ty> _Pf = fract(_Pos);

		_Ty _Result = _Ty( 8 );
		for (int j = -1; j <= 1; j++)
			for (int i = -1; i <= 1; i++)
			{
			Vec2_<_Ty> _Randpoint = Vec2_<_Ty>(_Ty(i), _Ty(j));
			Vec2_<_Ty> r = _Randpoint - _Pf + _Hash(_Pi + _Randpoint) * _Ty(0.33);
			_Ty d = dot(r, r);

			_Result = minval( _Result, d );
			}
		return ( sqrt(_Result) );
		}

	template<typename _Ty> inline
		_Ty worley3(Vec3_<_Ty> _Pos, _Hash33_func_pointer<_Ty> _Hash)
		{
		using std::sqrt;
		Vec3_<_Ty> _Pi = floor(_Pos);
		Vec3_<_Ty> _Pf = fract(_Pos);

		_Ty _Result = _Ty( 8 );
		for (int j = -1; j <= 1; j++)
			for (int i = -1; i <= 1; i++)
				for (int k = -1; k <= 1; k++)
				{
				Vec3_<_Ty> _Point = Vec3_<_Ty>(_Ty(k), _Ty(i), _Ty(j));
				Vec3_<_Ty>  r = _Point - _Pf + _Hash(_Pi + _Point) * _Ty(0.33);
				_Ty d = dot(r, r);

				_Result = minval(_Result, d);
				}
		return ( sqrt(_Result) );
		}
	
}// namespace clmagic

#endif