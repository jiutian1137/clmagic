#pragma once
#ifndef _CLMAGIC_CORE_GEOMETRY_NOISE_CELLULAR_H_
#define _CLMAGIC_CORE_GEOMETRY_NOISE_CELLULAR_H_
#include "../fraction.h"

namespace clmagic 
{
	/*
	Based on the work of Stefan Gustavson and Ashima Arts on "webgl-noise":
	https://github.com/ashima/webgl-noise
	Following Stefan Gustavson's paper "Simplex noise demystified":
	http://www.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf
	*/

	template<typename _Ty> inline
		Vec2_<_Ty> cellular(Vec2_<_Ty> P, _Hash22_func_pointer<_Ty>) 
		{
		const Vec3_<_Ty> K  = Vec3_<_Ty>(_Ty(0.142857142857f)); // 1/7
		const Vec3_<_Ty> Ko = Vec3_<_Ty>(_Ty(0.428571428571f)); // 3/7
		const _Ty jitter = _Ty(1.0);	// Less gives more regular pattern
		Vec2_<_Ty> Pi = floor(P) % _Ty(289);
		Vec2_<_Ty> Pf = fract(P);
		Vec3_<_Ty> oi = Vec3(-1.0f, 0.0f, 1.0f);
		Vec3_<_Ty> of = Vec3(-0.5f, 0.5f, 1.5f);
		Vec3_<_Ty> px = permute(Vec3_<_Ty>(Pi.x) + oi);
		Vec3_<_Ty> p  = permute(Vec3_<_Ty>(px.x + Pi.y) + oi); // p11, p12, p13
		Vec3_<_Ty> ox = fract(p*K) - Ko;
		Vec3_<_Ty> oy = floor(p*K) % _Ty(7) * K - Ko;
		Vec3_<_Ty> dx = Vec3_<_Ty>(Pf.x + 0.5 + jitter) * ox;
		Vec3_<_Ty> dy = Vec3_<_Ty>(Pf.y) - of + jitter * oy;
		Vec3_<_Ty> d1 = dx * dx + dy * dy; // d11, d12 and d13, squared
		p = permute(Vec3_<_Ty>(px.y + Pi.y) + oi); // p21, p22, p23
		ox = fract(p*K) - Ko;
		oy = floor(p*K) % _Ty(7) * K - Ko;
		dx = Vec3_<_Ty>(Pf.x - 0.5) + jitter * ox;
		dy = Vec3_<_Ty>(Pf.y) - of + jitter * oy;
		Vec3_<_Ty> d2 = dx * dx + dy * dy; // d21, d22 and d23, squared
		p = permute(Vec3_<_Ty>(px.z + Pi.y) + oi); // p31, p32, p33
		ox = fract(p*K) - Ko;
		oy = floor(p*K) % _Ty(7) * K - Ko;
		dx = Vec3_<_Ty>(Pf.x - 1.5) + jitter * ox;
		dy = Vec3_<_Ty>(Pf.y) - of + jitter * oy;
		Vec3_<_Ty> d3 = dx * dx + dy * dy; // d31, d32 and d33, squared
		// Sort out the two smallest distances (F1, F2)
		Vec3_<_Ty> d1a = minval(d1, d2);
		d2 = maxval(d1, d2); // Swap to keep candidates for F2
		d2 = minval(d2, d3); // neither F1 nor F2 are now in d3
		d1 = minval(d1a, d2); // F1 is now in d1
		d2 = maxval(d1a, d2); // Swap to keep candidates for F2
		
		if (d1.x > d1.y) 
			{	// Swap if smaller
			d1.x = d1.y;
			d1.y = d1.x;
			}

		if (d1.x > d1.z)
			{	// F1 is in d1.x
			d1.x = d1.z;
			d1.z = d1.x;
			}

		d1.y = minval(d1.y, d2.y);
		d1.z = minval(d1.z, d2.z);// F2 is now not in d2.yz

		d1.y = minval(d1.y, d1.z); // nor in  d1.z
		d1.y = minval(d1.y, d2.x); // F2 is in d1.y, we're done.
		return ( sqrt( d1.V<2>("xy") ) );
		}

}// namespace clmagic 

#endif