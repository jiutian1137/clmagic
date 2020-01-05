#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH_RAYTRACING___INTERSECT___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH_RAYTRACING___INTERSECT___H__
#include "../raytracing.h"

namespace clmagic 
{
	template<typename T> inline
	bool intersect(_in(Ray_<T>) _Ray, _in(Plane_<T>) _Plane, _out(Hit_<T>) _Result) 
		{/*
			(o + td - p0)，N = 0
			t = ((p0 - o)，N)/(d，N)
		 */
		const auto& o  = _Ray.o;
		const auto& d  = _Ray.d;
		const auto& p0 = _Plane.origin;
		const auto& N  = _Plane.normal;
		T t = dot( (p0 - o), N ) / dot( d, N );
		if ( _Ray._Mysight.exclude( t ) )
			return ( false );// direction different, or out of sight

		Vector3_<T> impact = o + t * d;			

		_Result._Myresult = ShapeType::ePLANE;
		_Result._Myt      = t;
		_Result._Myorigin = impact;
		_Result._Mynormal = faceforward(N, d, N);
		_Result._Myuv     = Vector2_<T>(impact[0], impact[2]);// [x,z]
		return ( true );
		}

	/*
	arguments: [test condition] [test data] [test result]  
	*/

	template<typename T> inline
		bool _Clip_test_spherical(_in(Ray_<T>) _Ray, _in(Sphere_<T>) _Sphere,
			/*in*/T t0, /*in*/T t1, _in(Vector3_<T>) o, _in(Vector3_<T>) d,
			_out(T) t, _out(Vector3_<T>) impack, _out(T) phi, _out(T) theta)
		{
		sort(t0, t1);
		t      = t0;
		impack = o + t * d;
		phi    = atan2(impack.y, impack.x); if (phi < T(0)) phi += T(c2Pi);
		theta  = acos(impack.z / _Sphere.r);
		if ( _Ray.sight_range.exclude(t) || _Sphere.phi_range.exclude(phi) || _Sphere.theta_range.exclude(theta) )
			{
			t      = t1;
			impack = o + t * d;
			phi    = atan2(impack.y, impack.x); if (phi < T(0)) phi += T(c2Pi);
			theta  = acos(impack.z / _Sphere.r);
			if ( _Ray.sight_range.exclude(t) || _Sphere.phi_range.exclude(phi) || _Sphere.theta_range.exclude(theta) )
				{
				return ( false );
				}
			}

		return ( true );
		}

	template<typename T> inline
		bool intersect(_in(Ray_<T>) _Ray, _in(Sphere_<T>) _Sphere, _out(Hit_<T>) _Result) {
			// 1. transform to object
			const auto o = _Sphere.world.inv_transform_copy(_Ray.o);
			const auto d = _Sphere.world.inv_transform_copy(Normal_<T>(_Ray.d));
			T t0, t1;
			T t; Vector3_<T> impack; T phi; T theta;

			if ( spherical_equation(o, d, _Sphere.r, t0, t1 ) ) {
				if ( _Clip_test_spherical(_Ray, _Sphere, t0, t1, o, d, /*out..*/t, impack, phi, theta) ) {
					_Result._Myresult = ShapeType::eSPHERE;
					_Result._Myt      = t;
					_Result._Myorigin = _Sphere.world.transform_copy(impack);
					_Result._Mynormal = _Sphere.world.transform_copy(Normal_<T>(impack / _Sphere.r));
					_Result._Myuv[0]  = phi / _Sphere.phi_range.range();
					_Result._Myuv[1]  = _Sphere.theta_range.remap(theta);
					return ( true );
				}
			}

			return ( false );
		}

	//template<typename T> inline
	//	bool intersect(_in(Ray_<T>) _Ray, _in(Cylinder_<T>) _Cylinder, _out(Hit_<T>) _Result) {
	//		// 1. transform to object
	//		const auto r = _Cylinder._Myradius;
	//		const auto o = _Cylinder._Myworld.inv_transform(_Ray.o);
	//		const auto d = _Cylinder._Myworld.inv_transform(_Ray.d, _Tag_normal());

	//		/*
	//		x^2 + z^2 - r^2 = 0
	//		(o.x+t*d.x)^2 + (o.z+t*d.z)^2 - r = 0
	//		o.x^2 + t^2*d.x^2 + 2*o.x*t*d.x + o.z^2 + t^2*d.z^2 + 2*o.z*t*d.z - r^2 = 0
	//		t^2*d.x^2 + t^2*d.z^2 + 2*o.x*t*d.x + 2*o.z*t*d.z + o.x^2 + o.z^2 - r^2 = 0
	//		t^2*(d.x^2 + d.z^2) + t*(2*(o.x*d.x + o.z*d.z)) + (o.x^2 + o.z^2 - r^2) = 0
	//		*/

	//		// 2. collision test
	//		T A = (d.x * d.x + d.z * d.z);
	//		T B = T(2) * (o.x * d.x + o.z * d.z);
	//		T C = o.x * o.x + o.z * o.z - r * r;
	//		T t0, t1;
	//		if (!quadratic(A, B, C, t0, t1)) {// is the equaltion solvable
	//			return (false);
	//		}

	//		// 3. clip test
	//		auto _Check = [&_Ray, &_Cylinder](_in(T) t, _in(T) h, _in(T) phi) -> bool
	//			{	// out of sight || out of height || out of phi
	//			return (_Ray._Mysight.exclude(t)
	//				|| h < T(0) || h > _Cylinder._Myheight
	//				|| _Cylinder._Myphi.exclude(phi));
	//			};

	//		auto t      = t0;
	//		auto impact = o + t * d;
	//		auto phi    = atan( impact.z, impact.x ); if (phi < T(0)) phi += T(6.28);
	//		if (_Check( t, impact.y, phi )) {
	//			t      = t1;
	//			impact = o + t * d;
	//			phi    = atan( impact.z, impact.x ); if (phi < T(0)) phi += T(6.28);
	//			if (_Check( t, impact.y, phi )) {
	//				return ( false );// out of shape clipping properties
	//			}
	//		}

	//		// 4. initial hit result
	//		_Result._Myresult = ShapeType::CYLINDER;
	//		_Result._Myt = t;
	//		_Result._Myorigin = _Cylinder._Myworld.transform(impact);
	//		_Result._Mynormal = _Cylinder._Myworld.transform(Vec3(cos(phi), T(0), sin(phi)), _Tag_normal());
	//		_Result._Myuv[0] = _Cylinder._Myphi.remap(phi);
	//		_Result._Myuv[1] = impact.y / _Cylinder._Myheight;
	//		return ( true );
	//	}

	//template<typename T> inline
	//	bool intersect(_in(Ray_<T>) _Ray, _in(Disk_<T>) _Disk, _out(Hit_<T>) _Result) {
	//		const auto o = _Disk._Myworld.inv_transform( _Ray.o );
	//		const auto d = _Disk._Myworld.inv_transform( _Ray.d, _Tag_normal() );
	//		const auto t = -o.z / d.z;

	//		if ( _Ray._Mysight.exclude(t) ) {// out of sight
	//			return (false);
	//		}

	//		auto _Pow2 = [](_in(T) _Val) { return (_Val * _Val); };

	//		auto _Impact = o + t * d;
	//		auto _Phi    = atan( _Impact.y, _Impact.x ); if (_Phi < 0.0) _Phi += 6.28f;
	//		auto _Dis2   = _Pow2(_Impact.y) + _Pow2(_Impact.x);
	//		
	//		if ( _Disk._Myphi.exclude(_Phi) || _Disk._Myradius.exclude(_Dis2, _Pow2, _Pow2) ) {// out of phi || out of radius
	//			return ( false );
	//		}
	//		
	//		_Result._Myresult = ShapeType::DISK;
	//		_Result._Myt      = t;
	//		_Result._Myorigin = _Disk._Myworld.transform(_Impact);
	//		_Result._Mynormal = _Disk._Myworld.transform(normalize(_Impact), _Tag_normal() );
	//		_Result._Myuv[0]  = _Disk._Myphi.remap(_Phi);
	//		_Result._Myuv[1]  = T(1) - (sqrt(_Dis2) - _Disk._Myradius._Min) / (_Disk._Myradius._Max / _Disk._Myradius._Min);
	//		return ( true );
	//	}

	//template<typename T> inline
	//	bool intersect(_in(Ray_<T>) _Ray, _in(Cone_<T>) _Cone, _out(Hit_<T>) _Result) {
	//		// 1. transform to object space
	//		const auto o = inv_transform(_Cone._Myworld, _Ray.o);
	//		const auto d = inv_transform(_Cone._Myworld, _Ray.d, _Tag_normal());
	//	
	//		// 2. collision test
	//		auto k = _Cone._Myradius / _Cone._Myheight;
	//		k = k * k;
	//		T A = d.x * d.x + d.y * d.y - k * d.z * d.z;
	//		T B = T(2) * (o.x * d.x + o.y * d.y - k * (o.z - _Cone._Myheight) * d.z);
	//		T C = o.x * o.x + o.y * o.y - k * pow(o.z - _Cone._Myheight, 2);
	//		T t0, t1;
	//		if ( !quadratic(A, B, C, t0, t1) ) {// is the equaltion solvable
	//			return ( false );
	//		}

	//		// 3. clip test
	//		auto _Check = [&_Ray, &_Cone](_in(T) t, _in(T) z, _in(T) phi)-> bool 
	//			{	// out of sight || out of height | out of phi
	//			return ( _Ray._Mysight.exclude(t) 
	//				|| z < T(0) || z > _Cone._Myheight 
	//				|| _Cone._Myphi.exclude(phi) );
	//			};

	//		auto t	    = t0;
	//		auto impact = o + t * d;
	//		auto phi	= atan(impact.y, impact.x); if (phi < 0.0) phi += T(6.283185482);
	//		if ( _Check(t, impact.z, phi) ) {
	//			t	   = t1;
	//			impact = o + t * d;
	//			phi	   = atan(impact.y, impact.x); if (phi < 0.0) phi += T(6.283185482);
	//			if ( _Check(t, impact.z, phi) ) {
	//				return ( false );
	//			}
	//		}

	//		// 4. initial hit result
	//		_Result._Myresult = ShapeType::CONE;
	//		_Result._Myt	  = t;
	//		_Result._Myorigin = transform(_Cone._Myworld, impact);
	//		_Result._Mynormal = transform(_Cone._Myworld, normalize(impact), _Tag_normal());
	//		_Result._Myuv	  = Vec2_<T>(_Cone._Myphi.remap(phi), impact.z / _Cone._Myheight);
	//		return ( true );
	//	}
	//
	//template<typename T> inline
	//	bool intersect(_in(Ray_<T>) _Ray, _in(Paraboloid_<T>) _Paraboloid, _out(Hit_<T>) _Result) {
	//		// 1. transform to object space
	//		auto o = inv_transform(_Paraboloid._Myworld, _Ray.o);
	//		auto d = inv_transform(_Paraboloid._Myworld, _Ray.d, _Tag_normal());

	//		// 2. 
	//		auto k = _Paraboloid._Myheight / (_Paraboloid._Myradius * _Paraboloid._Myradius);
	//		T A = k * (d.x * d.x + d.y * d.y);
	//		T B = 2.0 * k * (d.x * o.x + d.y * o.y) - d.z;
	//		T C = k * (o.x * o.x + o.y * o.y) - o.z;
	//		T t0, t1;
	//		if (!quadratic(A, B, C, t0, t1)) {// is the equaltion solvable
	//			return (false);
	//		}
	//			
	//		// 3. 
	//		auto _Check = [&_Ray, &_Paraboloid](_in(T) t, _in(T) h, _in(T) phi)-> bool
	//			{	// out of sight || out of height || out of phi
	//			return ( _Ray._Mysight.exclude(t)
	//				|| h < T(0) || h > _Paraboloid._Myheight
	//				|| _Paraboloid._Myphi.exclude(phi) );
	//			};

	//		auto t      = t0;
	//		auto impact = o + t * d;
	//		auto phi    = atan(impact.y, impact.x); if (phi < 0.0) phi += 6.283185482;
	//		if ( _Check(t, impact.z, phi) ) {
	//			t      = t1;
	//			impact = o + t * d;
	//			phi    = atan(impact.y, impact.x); if (phi < 0.0) phi += 6.283185482;
	//			if ( _Check(t, impact.z, phi) ) {
	//				return ( false );
	//			}
	//		}

	//		// 4.
	//		_Result._Myresult = ShapeType::PARABOLOID;
	//		_Result._Myt	  = t;
	//		_Result._Myorigin = transform(_Paraboloid._Myworld, impact);
	//		_Result._Mynormal = transform(_Paraboloid._Myworld, normalize(impact), _Tag_normal());
	//		_Result._Myuv.x   = _Paraboloid._Myphi.remap(phi);
	//		_Result._Myuv.y   = impact.z / _Paraboloid._Myheight;
	//		return (true);
	//	}

	template<typename T> inline
		bool intersect(_in(Ray_<T>) _Ray, _in(Triangle_<T>) _Triangle, _out(Hit_<T>) _Result) {
			/*
			e1 = p2-p1, e2 = p3-p1, s = o-p1

			[t ]         1		 [(s X e1)，e2]
			[b1] = --------------[(d X e2)，s ]
			[b2]    (d X e2)，e1 [(s X e1)，d ]

			s1 = (d X e2), s2= (s X e1)

			[t ]         1		 [s2，e2]
			[b1] = --------------[s1，s ]
			[b2]       s1，e1    [s2，d ]
			*/
			Vector3_<T> _e1 = _Triangle._Myp1 - _Triangle._Myp0;
			Vector3_<T> _e2 = _Triangle._Myp2 - _Triangle._Myp0;
			Vector3_<T> _s  = _Ray.o - _Triangle._Myp0;

			Vector3_<T> _s1 = cross3(_Ray.d, _e2);
			T _Invdet = dot(_s1, _e1); if (equal(_Invdet, T(0), T(0.00002))) return ( false );
			_Invdet = T(1) / _Invdet;

			T _b1 = _Invdet * dot(_s1, _s);
			if (_b1 < T(0) || _b1 > T(1))
				return ( false );

			Vector3_<T> _s2 = cross3(_s, _e1);
			T _b2 = _Invdet * dot(_s2, _Ray.d);
			if (_b2 < T(0) || _b2 + _b1 > T(1))
				return ( false );

			T _t = _Invdet * dot(_s2, _e2);
			if ( _Ray.sight_range.exclude(_t) )
				return ( false );// direction different, or out of sight

			_Result._Myresult = ShapeType::TRIANGLE;
			_Result._Myt      = _t;
			_Result._Myorigin = _Ray.o + _t * _Ray.d;
			_Result._Mynormal = normalize(cross3(_Triangle._Myp1 - _Triangle._Myp0, _Triangle._Myp2 - _Triangle._Myp0));
			_Result._Myuv     = Vector2_<T>(_b1, _b2);
			return ( true );
		}


	template<typename T> inline
	PerspectiveRay_<T>::PerspectiveRay_()
		: _Myview( Mat_<4, 4, T>::identity() ),
		_Myproj( Mat_<4, 4, T>::identity() ),
		_Mylook{ Vector3_<T>(T(0)), -cZ },
		_Myup(T(0), T(1), T(0)),
		_Myright(T(1), T(0), T(0)),
		_Myfov(   to_radians( T(60) ) ),
		_Myaspect( T(1200.0 / 860.0) ),
		_Mynear(   T(0.0001) ),
		_Myfar(    T(10000.0) ) {
		this->update_viewmatrix();
		this->reset_perspective(_Myfov, _Myaspect, _Mynear, _Myfar);
	}

	template<typename T> inline
	void PerspectiveRay_<T>::reset_perspective(_in(T) _Fov, _in(T) _Aspect, _in(T) _Near, _in(T) _Far) {
		_Myfov = _Fov;
		_Myaspect = _Aspect;
		_Mynear = _Near;
		_Myfar = _Far;
		_Myproj = Perspective(_Myfov, _Myaspect, _Mynear, _Myfar);
	}

	template<typename T> inline
	void PerspectiveRay_<T>::walk(_in(T) _Distance) {
		_Mylook.o += _Distance * _Mylook.d;
	}

	template<typename T> inline
	void PerspectiveRay_<T>::strafe(_in(T) _Distance) {
		_Mylook.o += _Distance * _Myright;
	}

	template<typename T> inline
	void PerspectiveRay_<T>::pitch(_in(T) _Radians) {
		Quaternion_<T> _Rotator = Quaternion_<T>(_Myright, _Radians);
		_Myup = _Rotator.rotate(_Myup);
		_Mylook.d = _Rotator.rotate(_Mylook.d);
	}

	template<typename T> inline
	void PerspectiveRay_<T>::yaw(_in(T) _Radians) {
		Quaternion_<T> _Rotator = Quaternion_<T>(cY, _Radians);
		_Myup = _Rotator.rotate(_Myup);
		_Myright = _Rotator.rotate(_Myright);
		_Mylook.d = _Rotator.rotate(_Mylook.d);
	}

	template<typename T> inline
	void PerspectiveRay_<T>::zoom(_in(T) _Percentage) {
		_Myfov += _Percentage;
		_Myproj = Perspective(_Myfov, _Myaspect, _Mynear, _Myfar);
	}
	
	template<typename T> inline
	void PerspectiveRay_<T>::update_viewmatrix() {
		_Myview = Lookat(_Mylook.o, _Mylook.d, _Myup);
	}

}// namespace clmagic

#endif