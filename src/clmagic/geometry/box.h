#pragma once
#ifndef _CLMAGIC_CORE_BOX_H_
#define _CLMAGIC_CORE_BOX_H_
#include "math.h"
#include <vector>


namespace clmagic 
{
	struct spherebox 
	{
		Vec3 center = Vec3(0.0f);
		float radius = 0.0f;
		
		void translate(float _X, float _Y, float _Z);
		void set_center(const Vec3& _Center);
		void set_radius(const Vec3& _Pos);
		void set_radius(float _Radius);
	};

	inline void spherebox::translate(float _X, float _Y, float _Z)
		{
		center = center + Vec3(_X, _Y, _Z);
		}

	inline void spherebox::set_center(const Vec3& _Center)
		{
		center = _Center;
		}

	inline void spherebox::set_radius(const Vec3& _Pos)
		{
		float _Length = length(center, _Pos);
		if (_Length > radius) 
			{
			radius = _Length;
			}
		}

	inline void spherebox::set_radius(float _Radius)
		{
		radius = _Radius;
		}




	struct aabb 
	{
		Vec3 min;
		Vec3 max;
	};

	inline spherebox gen_sphere_box(const std::vector<Vec3>& _Pos) 
		{
		spherebox _Box;
		for (const Vec3& _P : _Pos) 
			{
			_Box.center = _Box.center + _P;
			}
		_Box.center = _Box.center / static_cast<float>(_Pos.size());

		for (const Vec3& _P : _Pos)
			{
			float _Length = length(_P, _Box.center);
			if (_Box.radius < _Length)
				{
				_Box.radius = _Length;
				}
			}

		return (_Box);
		}

	inline aabb gen_aabb_box(const std::vector<Vec3>& _Pos)
		{
		aabb _Box;
		for (const Vec3& _P : _Pos) 
			{
			if (_P.x < _Box.min.x) 
				{
				_Box.min.x = _P.x;
				}

			if (_P.y < _Box.min.y)
				{
				_Box.min.y = _P.y;
				}

			if (_P.z < _Box.min.z)
				{
				_Box.min.z = _P.z;
				}

			if (_P.x > _Box.max.x)
				{
				_Box.max.x = _P.x;
				}

			if (_P.y > _Box.max.y)
				{
				_Box.max.y = _P.y;
				}

			if (_P.z > _Box.max.z)
				{
				_Box.max.z = _P.z;
				}
			}
		return (_Box);
		}

}// namespace clmagic 

#endif