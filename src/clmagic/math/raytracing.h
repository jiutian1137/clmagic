#pragma once
#ifndef clmagic_math_RAYTRACING_h_
#define clmagic_math_RAYTRACING_h_
#include "lapack.h"

namespace clmagic 
{
	enum ShapeType {
		eUNKNOWN,
		eRAY,
		ePLANE,
		eSPHERE,
		eCYLINDER,
		eDISK,
		eCONE,
		ePARABOLOID,
		eTRIANGLE
	};
	template<typename T> struct Hit_;
	template<typename T> struct Ray_;
	template<typename T> struct Plane_;
	template<typename T> struct Sphere_;
	template<typename T> struct Cylinder_;
	template<typename T> struct Disk_;
	template<typename T> struct Cone_;
	template<typename T> struct Paraboloid_;
	template<typename T> struct Triangle_;
	

	/*
	@_Equation: x^2 + y^2 - r^2 = 0
	*/
	template<typename T> inline
		bool circle_equation(_in(Vector2_<T>) o, _in(Vector2_<T>) d, _in(T) r, _out(T) t0, _out(T) t1)
		{
		/*
			(ox+t*dx)^2 + (oy+t*dy)^2 - r^2 = 0
			ox^2 + t^2*dx^2 + 2*t*ox*dx + oy^2 + t^2*dy^2 + 2*t*oy*dy - r^2 = 0
			t^2*dx^2 + t^2*dy^2 + 2*t*ox*dx + 2*t*oy*dy + ox^2 + oy^2 - r^2 = 0(again)
			(dx^2 + dy^2)t^2 + (2*ox*dx + 2*oy*dy)t + ox^2 + oy^2 - r^2 = 0
		*/
		return ( quadratic(/*A*/dot2(d, d), /*B*/T(2) * dot2(d, o), /*C*/dot2(o, o) - r * r, t0, t1) );
		}

	/* 
	@_Equation: x^2 + y^2 + z^2 - r^2 = 0
	@_Note: local coordinate system
	*/
	template<typename T> inline
		bool spherical_equation(_in(Vector3_<T>) o, _in(Vector3_<T>) d, _in(T) r, _out(T) t0, _out(T) t1)
		{
		/*
			(ox+t*dx)^2 + (oy+t*dy)^2 + (oz+t*dz)^2 - r^2 = 0
			ox^2 + (t*dx)^2 + 2*t*ox*dx + oy^2 + (t*dy)^2 + 2*t*oy*dy + oz^2 + (t*dz)^2 + 2*t*oz*dz - r^2 = 0
			t^2*dx^2 + t^2*dy^2 + t^2*dz^2 + ox^2 + oy^2 + oz^2 + t*2*ox*dx + t*2*oy*dy + t*2*oz*dz - r^2 = 0
			(dx^2 + dy^2 + dz^2)t^2 + (2*ox*dx + 2*oy*dy + 2*oz*dz)t + (-r^2 + ox^2 + oy^2 + oz^2) = 0
		*/
		return ( quadratic(/*A*/dot3(d, d), /*B*/T(2) * dot3(d, o), /*C*/dot3(o, o) - r * r, t0, t1) );
		}

	/* 
	@_Equation: x^2 + y^2 - r^2 = 0
	@_Note: ignore height, local coordinate system 
	*/
	template<typename T> inline
		bool cylindrical_equation(_in(Vector3_<T>) o, _in(Vector3_<T>) d, _in(T) r, _out(T) t0, _out(T) t1)
		{
		return ( quadratic(dot2(d, d), T(2) * dot2(d, o), dot2(o) - r * r, t0, t1) );
		}


	template<typename T> inline
		bool intersect(_in(Ray_<T>) _Ray, _in(Plane_<T>) _Plane, _out(Hit_<T>) _Result);
	template<typename T> inline
		bool intersect(_in(Ray_<T>) _Ray, _in(Sphere_<T>) _Sphere, _out(Hit_<T>) _Result);
	template<typename T> inline
		bool intersect(_in(Ray_<T>) _Ray, _in(Cylinder_<T>) _Cylinder, _out(Hit_<T>) _Result);
	template<typename T> inline
		bool intersect(_in(Ray_<T>) _Ray, _in(Disk_<T>) _Disk, _out(Hit_<T>) _Result);
	template<typename T> inline
		bool intersect(_in(Ray_<T>) _Ray, _in(Cone_<T>) _Cone, _out(Hit_<T>) _Result);
	template<typename T> inline
		bool intersect(_in(Ray_<T>) _Ray, _in(Paraboloid_<T>) _Paraboloid, _out(Hit_<T>) _Result);
	template<typename T> inline
		bool intersect(_in(Ray_<T>) _Ray, _in(Triangle_<T>) _Triangle, _out(Hit_<T>) _Result);
	


	template<typename T>
	struct Hit_ {
		ShapeType   _Myresult;
		T		    _Myt;
		Vector3_<T> _Myorigin;
		Vector3_<T> _Mynormal;
		Vector2_<T> _Myuv;

		static Hit_ no_hit();
	};
	using Hit = Hit_<real_t>;


	template<typename T>
	struct Plane_ {
	/*
	@_Equaltion:
		normal��(POINT - origin) = 0
	@_Parametric_equaltion:
		...
	*/
		Vector3_<T> origin;
		Vector3_<T> normal;
	};
	using Plane = Plane_<real_t>;
	

	template<typename T>
	struct Sphere_ {
	/*
	@_Equaltion:
		x^2 + y^2 + z^2 - r^2 = 0
	@_Parametric_equaltion:
		u, v
		phi   = u * phi_max
		theta = thetamin + v(theta_max - theta_min)
		x = r * sin(theta) * cos(phi)
		y = r * sin(theta) * sin(phi)
		z = r * cos(theta)
	*/
		MatrixTransform_<T> world = MatrixTransform(Mat4::identity());
		T		  r;
		Range_<T> phi_range   = Range_<T>(T(0), T(c2Pi));// phimin, phimax
		Range_<T> theta_range = Range_<T>(T(0), T(c2Pi));// thetamin, thetamax
	};
	using Sphere = Sphere_<real_t>;

	
	template<typename T>
	struct Cylinder_ {
	/*
	@_Equaltion:
		x^2 + z^2 - r^2 = 0, { y | [ymin, ymax] }
	@_Parametric equaltion:
		u, v
		phi = u * phi_max
		x   = r * cos(phi)
		z   = r * sin(phi)
		y   = ymin + v * (height)
	*/
		MatrixTransform_<T> _Myworld;
		T		  _Myradius;
		T		  _Myheight;
		Range_<T> _Myphi;// phimin, phimax
	};
	using Cylinder = Cylinder_<real_t>;


	template<typename T>
	struct Disk_ {
	/*
	@_Parametric equaltion:
		u, v
		phi = u * phi_max
		r = (1-v)*outer + v*inner
		x = r * cos(phi)
		y = r * sin(phi)
		z   = 0
	*/
		MatrixTransform_<T> _Myworld;
		Range_<T> _Myradius;// inner, outer
		Range_<T> _Myphi;// phimin, phimax
	};
	using Disk = Disk_<real_t>;


	template<typename T>
	struct Cone_ {
	/*
	@_Equaltion:
		(h*x/r)^2 + (h*z/r)^2 - (y-h)^2 = 0
	@_Parametric equaltion:
		u, v
		phi = u * phi_max
		x = (1-v) * r * cos(phi)
		z = (1-v) * r * sin(phi)
		y = v * height
	*/
		MatrixTransform_<T> _Myworld;
		T		  _Myradius;
		T	      _Myheight;
		Range_<T> _Myphi;// phimin, phimax
	};
	using Cone = Cone_<real_t>;


	template<typename T>
	struct Paraboloid_ {
	/*
	@_Equaltion:
		h*x^2/r^2 + h*z^2/r^2 - z = 0
	@_Parametric equaltion:
		u, v
		phi = u * phi_max
		y = v(height)
		r = r * ��(y/height)
		x = r * cos(phi)
		z = r * sin(phi)
	*/
		MatrixTransform_<T> _Myworld;
		T		  _Myradius;
		T         _Myheight;
		Range_<T> _Myphi;// phimin, phimax
	};
	using Paraboloid = Paraboloid_<real_t>;

	
	template<typename T>
	struct Triangle_ {
	/*
	@_Equaltion:
		...
	@_Parametric equaltion:
		u, v
		0 < u + v < 1
		x = ( 1-u-v ) * p0.x + u * p1.x + v * p2.x
		y = ( 1-u-v ) * p0.y + u * p1.y + v * p2.y
		z = ( 1-u-v ) * p0.z + u * p1.z + v * p2.z
	*/
		Vector3_<T> _Myp0;
		Vector3_<T> _Myp1;
		Vector3_<T> _Myp2;
	};
	using Triangle = Triangle_<real_t>;


	template<typename T>
	struct Ray_ {
		/*
		@_Equaltion: o + t * d
		*/
		Vector3_<T> o;
		Vector3_<T> d;
		mutable Range_<T> sight_range = Range_<T>(T(0), T(cGiga));
	};
	using Ray = Ray_<real_t>;



	template<typename T>
	struct PerspectiveRay_
	{
	public:
		PerspectiveRay_();

		void reset_perspective(_in(T) _Fov, _in(T) _Aspect, _in(T) _Near, _in(T) _Far);
		void walk(_in(T) distance);
		void strafe(_in(T) distance);
		void pitch(_in(T) angle);
		void yaw(_in(T) angle);
		void zoom(_in(T) _Percentage);
		void update_viewmatrix();

		Vector3_<T>& forward_vector() { return (_Mylook.d); }
		Vector3_<T>& up_vector() { return (_Myup); }
		Vector3_<T>& right_vector() { return (_Myright); }
		Mat_<4, 4, T>& view_matrix() { return (_Myview); }
		Mat_<4, 4, T>& projection_matrix() { return (_Myproj); }
		const Vector3_<T>& forward_vector() const { return (_Mylook.d); }
		const Vector3_<T>& up_vector() const { return (_Myup); }
		const Vector3_<T>& right_vector() const { return (_Myright); }
		const Mat_<4, 4, T>& view_matrix() const { return (_Myview); }
		const Mat_<4, 4, T>& projection_matrix() const { return (_Myproj); }

		Mat_<4, 4, T> _Myview;
		Mat_<4, 4, T> _Myproj;
		Ray_<T>		  _Mylook;
		Vector3_<T>   _Myup;
		Vector3_<T>   _Myright;
		T _Myfov;
		T _Myaspect;
		T _Mynear;
		T _Myfar;
	};

}// namespace clmagic

#endif