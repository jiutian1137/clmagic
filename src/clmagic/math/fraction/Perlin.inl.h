#pragma once
#ifndef _CLMAGIC_CORE_GEOMETRY_NOISE_PERLIN_H_
#define _CLMAGIC_CORE_GEOMETRY_NOISE_PERLIN_H_
#include "../fraction.h"

namespace clmagic
{
	/*
	Based on the work of Stefan Gustavson and Ashima Arts on "webgl-noise":
	https://github.com/ashima/webgl-noise
	Following Stefan Gustavson's paper "Simplex noise demystified":
	http://www.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf
	*/

	// Classic Perlin noise
	template<typename T> inline
	T cnoise2( vector2<T> _Pos ) {
		/*
		+---------------1(Pi[2],Pi[3])
		|               |
		|               |
		|               |
		|               |
		0---------------+
		(Pi[0],Pi[1])
		*/
			  vector4<T> _Pi = floor(_Pos.shuffle(0, 1, 0, 1)) + vector4<T>(T(0), T(0), T(1), T(1));
		const vector4<T> _Pf = fract(_Pos.shuffle(0, 1, 0, 1)) - vector4<T>(T(0), T(0), T(1), T(1));
		                 _Pi = mod(_Pi, T(289)); // To avoid truncation effects in permutation
		const vector4<T> ix  = _Pi.shuffle(0,2,0,2);//_Pi.xzxz
		const vector4<T> iy  = _Pi.shuffle(1,3,1,3);//_Pi.yyww
		const vector4<T> fx  = _Pf.shuffle(0,2,0,2);//_Pf.xzxz
		const vector4<T> fy  = _Pf.shuffle(1,3,1,3);//_Pf.yyww

		const vector4<T> i   = permute(permute(ix) + iy);

		vector4<T> gx  = T(2) * fract(i / T(41)) - T(1);
		vector4<T> gy  = abs(gx) - T(0.5);
		vector4<T> tx  = floor(gx + T(0.5));
		           gx  = gx - tx;

		vector2<T> g00 = vector2<T>(gx[0], gy[0]);
		vector2<T> g10 = vector2<T>(gx[1], gy[1]);
		vector2<T> g01 = vector2<T>(gx[2], gy[2]);
		vector2<T> g11 = vector2<T>(gx[3], gy[3]);

		vector4<T> norm = taylor_invsqrt( vector4<T>(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)) );
		           g00 *= norm[0];
		           g01 *= norm[1];
		           g10 *= norm[2];
		           g11 *= norm[3];

		const T n00 = dot(g00, vector2<T>(fx[0], fy[0]));
		const T n10 = dot(g10, vector2<T>(fx[1], fy[1]));
		const T n01 = dot(g01, vector2<T>(fx[2], fy[2]));
		const T n11 = dot(g11, vector2<T>(fx[3], fy[3]));

		vector4<T> uv = fade(_Pf);
		return ( T(2.3) * lerp( lerp(n00, n01, uv[0]), 
			                    lerp(n10, n11, uv[1]), 
			                    uv[1] ) );
		}

	//// Classic Perlin noise, periodic variant
	//template<typename _Ty> inline 
	//	_Ty pnoise2(Vec2_<_Ty> _Pos, Vec2_<_Ty> _Rep)
	//	{
	//	Vec4_<_Ty> _Pi = floor(_Pos.V<4>("xyxy")) + Vec4_<_Ty>(_Ty(0), _Ty(0), _Ty(1), _Ty(1));
	//	Vec4_<_Ty> _Pf = fract(_Pos.V<4>("xyxy")) - Vec4_<_Ty>(_Ty(0), _Ty(0), _Ty(1), _Ty(1));
	//	_Pi = _Pi % Vec4_<_Ty>(_Rep.x, _Rep.y, _Rep.x, _Rep.y);
	//	_Pi = _Pi % _Ty(289); // To avoid truncation effects in permutation
	//	Vec4_<_Ty> ix = _Pi.V<4>("xzxz");
	//	Vec4_<_Ty> iy = _Pi.V<4>("yyww");
	//	Vec4_<_Ty> fx = _Pf.V<4>("xzxz");
	//	Vec4_<_Ty> fy = _Pf.V<4>("yyww");

	//	Vec4_<_Ty> i = permute(permute(ix) + iy);

	//	Vec4_<_Ty> gx = fract(i / _Ty(41)) * _Ty(2) - Vec4_<_Ty>(_Ty(1));
	//	Vec4_<_Ty> gy = abs(gx) - Vec4_<_Ty>(_Ty(0.5));
	//	Vec4_<_Ty> tx = floor(gx + Vec4_<_Ty>(_Ty(0.5)));
	//	gx = gx - tx;

	//	Vec2_<_Ty> g00 = Vec2_<_Ty>(gx.x, gy.x);
	//	Vec2_<_Ty> g10 = Vec2_<_Ty>(gx.y, gy.y);
	//	Vec2_<_Ty> g01 = Vec2_<_Ty>(gx.z, gy.z);
	//	Vec2_<_Ty> g11 = Vec2_<_Ty>(gx.w, gy.w);

	//	Vec4 norm = taylor_invsqrt(Vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
	//	g00 *= norm.x;
	//	g01 *= norm.y;
	//	g10 *= norm.z;
	//	g11 *= norm.w;

	//	_Ty n00 = dot(g00, Vec2_<_Ty>(fx.x, fy.x));
	//	_Ty n10 = dot(g10, Vec2_<_Ty>(fx.y, fy.y));
	//	_Ty n01 = dot(g01, Vec2_<_Ty>(fx.z, fy.z));
	//	_Ty n11 = dot(g11, Vec2_<_Ty>(fx.w, fy.w));

	//	Vec2_<_Ty> n_x = mix(Vec2_<_Ty>(n00, n01), Vec2_<_Ty>(n10, n11), fade(_Pf.x));
	//	_Ty n_xy = mix(n_x.x, n_x.y, fade(_Pf.y));
	//	return ( _Ty(2.3) * n_xy );
	//	}

	//// Classic Perlin noise
	//template<typename _Ty> inline
	//	_Ty cnoise3(Vec3_<_Ty> _Pos)
	//	{
	//	const Vec4_<_Ty> _Zero = Vec4_<_Ty>( _Ty(0) );
	//	const Vec4_<_Ty> _V0o5 = Vec4_<_Ty>( _Ty(0.5) );
	//	const Vec4_<_Ty> _K = Vec4_<_Ty>( _Ty(0.142857142857) ); // 1/7

	//	Vec3_<_Ty> _Pi0 = floor( _Pos );// Integer part for indexing
	//	Vec3_<_Ty> _Pi1 = _Pi0 + Vec3_<_Ty>( _Ty(1) ); // Integer part + 1
	//	_Pi0 = _Pi0 % _Ty(289);
	//	_Pi1 = _Pi1 % _Ty(289);
	//	Vec3_<_Ty> _Pf0 = fract( _Pos );// Fractional part for interpolation
	//	Vec3_<_Ty> _Pf1 = _Pf0 - _Ty(1); // Fractional part - 1.0
	//	Vec4_<_Ty> ix = Vec4_<_Ty>( _Pi0.x, _Pi1.x, _Pi0.x, _Pi1.x );
	//	Vec4_<_Ty> iy = Vec4_<_Ty>( _Pi0.y, _Pi0.y, _Pi1.y, _Pi1.y );
	//	Vec4_<_Ty> iz0 = Vec4_<_Ty>( _Pi0.z );// _Pi0.zzzz
	//	Vec4_<_Ty> iz1 = Vec4_<_Ty>( _Pi1.z );// _Pi1.zzzz

	//	Vec4_<_Ty> ixy = permute( permute( ix ) + iy );
	//	Vec4_<_Ty> ixy0 = permute( ixy + iz0 );
	//	Vec4_<_Ty> ixy1 = permute( ixy + iz1 );

	//	Vec4_<_Ty> gx0 = ixy0 * _K;
	//	Vec4_<_Ty> gy0 = fract( floor( gx0 ) * _K ) - _V0o5;
	//	gx0 = fract( gx0 );
	//	Vec4_<_Ty> gz0 = _V0o5 - abs( gx0 ) - abs( gy0 );
	//	Vec4_<_Ty> sz0 = step(gz0, _Zero);
	//	gx0 -= sz0 * ( step(_Zero, gx0) - _V0o5 );
	//	gy0 -= sz0 * ( step(_Zero, gy0) - _V0o5 );

	//	Vec4_<_Ty> gx1 = ixy1 * _K;
	//	Vec4_<_Ty> gy1 = fract( floor( gx1 ) * _K ) - _V0o5;
	//	gx1 = fract( gx1 );
	//	Vec4_<_Ty> gz1 = _V0o5 - abs( gx1 ) - abs( gy1 );
	//	Vec4_<_Ty> sz1 = step( gz1, _Zero );
	//	gx1 -= sz1 * ( step(_Zero, gx1) - _V0o5 );
	//	gy1 -= sz1 * ( step(_Zero, gy1) - _V0o5 );

	//	Vec3_<_Ty> g000 = Vec3_<_Ty>(gx0.x, gy0.x, gz0.x);
	//	Vec3_<_Ty> g100 = Vec3_<_Ty>(gx0.y, gy0.y, gz0.y);
	//	Vec3_<_Ty> g010 = Vec3_<_Ty>(gx0.z, gy0.z, gz0.z);
	//	Vec3_<_Ty> g110 = Vec3_<_Ty>(gx0.w, gy0.w, gz0.w);
	//	Vec3_<_Ty> g001 = Vec3_<_Ty>(gx1.x, gy1.x, gz1.x);
	//	Vec3_<_Ty> g101 = Vec3_<_Ty>(gx1.y, gy1.y, gz1.y);
	//	Vec3_<_Ty> g011 = Vec3_<_Ty>(gx1.z, gy1.z, gz1.z);
	//	Vec3_<_Ty> g111 = Vec3_<_Ty>(gx1.w, gy1.w, gz1.w);

	//	Vec4_<_Ty> norm0 = taylor_invsqrt(Vec4_<_Ty>(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
	//	g000 *= norm0.x;
	//	g010 *= norm0.y;
	//	g100 *= norm0.z;
	//	g110 *= norm0.w;
	//	Vec4_<_Ty> norm1 = taylor_invsqrt(Vec4_<_Ty>(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
	//	g001 *= norm1.x;
	//	g011 *= norm1.y;
	//	g101 *= norm1.z;
	//	g111 *= norm1.w;

	//	_Ty n000 = dot(g000, _Pf0);
	//	_Ty n100 = dot(g100, Vec3_<_Ty>(_Pf1.x, _Pf0.y, _Pf0.z));
	//	_Ty n010 = dot(g010, Vec3_<_Ty>(_Pf0.x, _Pf1.y, _Pf0.z));
	//	_Ty n110 = dot(g110, Vec3_<_Ty>(_Pf1.x, _Pf1.y, _Pf0.z));
	//	_Ty n001 = dot(g001, Vec3_<_Ty>(_Pf0.x, _Pf0.y, _Pf1.z));
	//	_Ty n101 = dot(g101, Vec3_<_Ty>(_Pf1.x, _Pf0.y, _Pf1.z));
	//	_Ty n011 = dot(g011, Vec3_<_Ty>(_Pf0.x, _Pf1.y, _Pf1.z));
	//	_Ty n111 = dot(g111, _Pf1);

	//	/*
	//	lerp(lerp(lerp(n000, n100, x), 
	//			  lerp(n010, n110, x),
	//			  y),
	//		 lerp(lerp(n001, n101, x),
	//			  lerp(n011, n111, x),
	//			  y),
	//		 z)
	//	14add+7mul
	//	*/
	//	Vec4 n_z   = lerp( Vec4_<_Ty>(n000, n100, n010, n110), Vec4_<_Ty>(n001, n101, n011, n111), fade(_Pf0.z) );
	//	Vec2 n_yz  = lerp( Vec2_<_Ty>(n_z.x, n_z.y), Vec2_<_Ty>(n_z.z, n_z.w), fade(_Pf0.y) );
	//	_Ty  n_xyz = lerp( n_yz.x, n_yz.y, fade(_Pf0.x) );// 14add+7mul
	//	return ( _Ty(2.2) * n_xyz );
	//	}

	//// Classic Perlin noise, periodic variant
	//template<typename _Ty> inline
	//	_Ty pnoise3(Vec3_<_Ty> _Pos, Vec3_<_Ty> _Rep)
	//	{
	//	const Vec4_<_Ty> _Zero = Vec4_<_Ty>( _Ty(0) );
	//	const Vec4_<_Ty> _V0o5 = Vec4_<_Ty>( _Ty(0.5) );
	//	const Vec4_<_Ty> _K = Vec4_<_Ty>( _Ty(0.142857142857) ); // 1/7

	//	Vec3_<_Ty> _Pi0 = floor( _Pos ) % _Rep;// Integer part for indexing
	//	Vec3_<_Ty> _Pi1 = (_Pi0 + Vec3_<_Ty>( _Ty(1) )) % _Rep; // Integer part + 1
	//	_Pi0 = _Pi0 % _Ty(289);
	//	_Pi1 = _Pi1 % _Ty(289);
	//	Vec3_<_Ty> _Pf0 = fract( _Pos );// Fractional part for interpolation
	//	Vec3_<_Ty> _Pf1 = _Pf0 - Vec3_<_Ty>( _Ty(1) ); // Fractional part - 1.0
	//	Vec4_<_Ty> ix = Vec4_<_Ty>( _Pi0.x, _Pi1.x, _Pi0.x, _Pi1.x );
	//	Vec4_<_Ty> iy = Vec4_<_Ty>( _Pi0.y, _Pi0.y, _Pi1.y, _Pi1.y );
	//	Vec4_<_Ty> iz0 = Vec4_<_Ty>( _Pi0.z );// _Pi0.zzzz
	//	Vec4_<_Ty> iz1 = Vec4_<_Ty>( _Pi1.z );// _Pi1.zzzz

	//	Vec4_<_Ty> ixy = permute( permute( ix ) + iy );
	//	Vec4_<_Ty> ixy0 = permute( ixy + iz0 );
	//	Vec4_<_Ty> ixy1 = permute( ixy + iz1 );

	//	Vec4_<_Ty> gx0 = ixy0 * _K;
	//	Vec4_<_Ty> gy0 = fract( floor( gx0 ) * _K ) - _V0o5;
	//	gx0 = fract( gx0 );
	//	Vec4_<_Ty> gz0 = _V0o5 - abs( gx0 ) - abs( gy0 );
	//	Vec4_<_Ty> sz0 = step(gz0, _Zero);
	//	gx0 -= sz0 * ( step(_Zero, gx0) - _V0o5 );
	//	gy0 -= sz0 * ( step(_Zero, gy0) - _V0o5 );

	//	Vec4_<_Ty> gx1 = ixy1 * _K;
	//	Vec4_<_Ty> gy1 = fract( floor( gx1 ) * _K ) - _V0o5;
	//	gx1 = fract( gx1 );
	//	Vec4_<_Ty> gz1 = _V0o5 - abs( gx1 ) - abs( gy1 );
	//	Vec4_<_Ty> sz1 = step( gz1, _Zero );
	//	gx1 -= sz1 * ( step(_Zero, gx1) - _V0o5 );
	//	gy1 -= sz1 * ( step(_Zero, gy1) - _V0o5 );

	//	Vec3_<_Ty> g000 = Vec3_<_Ty>(gx0.x, gy0.x, gz0.x);
	//	Vec3_<_Ty> g100 = Vec3_<_Ty>(gx0.y, gy0.y, gz0.y);
	//	Vec3_<_Ty> g010 = Vec3_<_Ty>(gx0.z, gy0.z, gz0.z);
	//	Vec3_<_Ty> g110 = Vec3_<_Ty>(gx0.w, gy0.w, gz0.w);
	//	Vec3_<_Ty> g001 = Vec3_<_Ty>(gx1.x, gy1.x, gz1.x);
	//	Vec3_<_Ty> g101 = Vec3_<_Ty>(gx1.y, gy1.y, gz1.y);
	//	Vec3_<_Ty> g011 = Vec3_<_Ty>(gx1.z, gy1.z, gz1.z);
	//	Vec3_<_Ty> g111 = Vec3_<_Ty>(gx1.w, gy1.w, gz1.w);

	//	Vec4_<_Ty> norm0 = taylor_invsqrt(Vec4_<_Ty>(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
	//	g000 *= norm0.x;
	//	g010 *= norm0.y;
	//	g100 *= norm0.z;
	//	g110 *= norm0.w;
	//	Vec4_<_Ty> norm1 = taylor_invsqrt(Vec4_<_Ty>(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
	//	g001 *= norm1.x;
	//	g011 *= norm1.y;
	//	g101 *= norm1.z;
	//	g111 *= norm1.w;

	//	_Ty n000 = dot(g000, _Pf0);
	//	_Ty n100 = dot(g100, Vec3_<_Ty>(_Pf1.x, _Pf0.y, _Pf0.z));
	//	_Ty n010 = dot(g010, Vec3_<_Ty>(_Pf0.x, _Pf1.y, _Pf0.z));
	//	_Ty n110 = dot(g110, Vec3_<_Ty>(_Pf1.x, _Pf1.y, _Pf0.z));
	//	_Ty n001 = dot(g001, Vec3_<_Ty>(_Pf0.x, _Pf0.y, _Pf1.z));
	//	_Ty n101 = dot(g101, Vec3_<_Ty>(_Pf1.x, _Pf0.y, _Pf1.z));
	//	_Ty n011 = dot(g011, Vec3_<_Ty>(_Pf0.x, _Pf1.y, _Pf1.z));
	//	_Ty n111 = dot(g111, _Pf1);

	//	/*
	//	lerp(lerp(lerp(n000, n100, x), 
	//			  lerp(n010, n110, x),
	//			  y),
	//		 lerp(lerp(n001, n101, x),
	//			  lerp(n011, n111, x),
	//			  y),
	//		 z)
	//	14add+7mul
	//	*/
	//	Vec4 n_z   = lerp( Vec4_<_Ty>(n000, n100, n010, n110), Vec4_<_Ty>(n001, n101, n011, n111), fade(_Pf0.z) );
	//	Vec2 n_yz  = lerp( Vec2_<_Ty>(n_z.x, n_z.y), Vec2_<_Ty>(n_z.z, n_z.w), fade(_Pf0.y) );
	//	_Ty  n_xyz = lerp( n_yz.x, n_yz.y, fade(_Pf0.x) );// 14add+7mul
	//	return ( _Ty(2.2) * n_xyz );
	//	}

}// namespace clmagic

#endif