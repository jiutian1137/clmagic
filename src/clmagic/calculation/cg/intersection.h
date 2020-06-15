#pragma once
#include "../lapack/vector.h"
#include "../lapack/matrix.h"
#include "../lapack/geometry.h"

namespace clmagic {
	enum class Shape_Type {
		Unknown,
		Ray,
		Plane,
		Sphere,
		Cylinder,
		Disk,
		Cone,
		Paraboloid,
		Box,
		Triangle
	};
	
	enum class Hit_Type {
		No    = 0,
		Once  = 1,// entry point only
		Twice = 2,// entry point, exit point
		More  = 3 //
	};


	template<typename _Ts, typename _Tb = _Ts>
	struct ray {
		VECTOR3      _Myorg;
		SCALAR       _Mynear;
		UNIT_VECTOR3 _Mydir;
		SCALAR       _Myfar;

		ray() = default;
		ray(VECTOR3 o, UNIT_VECTOR3 d)
			: _Myorg(o), _Mydir(d), _Mynear(0), _Myfar(std::numeric_limits<SCALAR>::max()) {}
		ray(VECTOR3 o, UNIT_VECTOR3 d, SCALAR n, SCALAR f)
			: _Myorg(o), _Mydir(d), _Mynear(n), _Myfar(f) {}
	};

	template<typename _Ts, typename _Tb>
	struct intersection {
		Hit_Type     _Myresult;
		VECTOR3      _Mypos;
		UNIT_VECTOR3 _Mynormal;
		VECTOR2      _Myuv;
	};

	template<typename _Ts, typename _Tb = _Ts>
	struct intersection_ray : public intersection<_Ts, _Tb> {
		VECTOR3      _Myorg;
		SCALAR       _Myentry;
		UNIT_VECTOR3 _Mydir;
		SCALAR       _Myexit;
	};

	//template<typename _Ts, typename _Tb = _Ts>
	//struct _Plane {
	//	UNIT_VECTOR3 _Mynormal;
	//	SCALAR       _Mydistance;
	//};
	//
	//template<typename _Ts, typename _Tb = _Ts>
	//struct _Sphere {
	//	/*
	//	@_Equaltion:
	//		x^2 + y^2 + z^2 - r^2 = 0
	//	@_Parametric_equaltion:
	//		zenith, azimuth
	//		x = r * cos(zenith) * cos(azimuth)
	//		y = r * sin(zenith)
	//		z = r * cos(zenith) * sin(azimuth)
	//	*/
	//	MATRIX4x4 _Myworld;
	//	VECTOR3 _Myorigin;
	//	SCALAR	_Myradius;
	//	SCALAR  _Myzenith_min;
	//	SCALAR  _Myzenith_max;
	//	SCALAR  _Myazimuth_min;
	//	SCALAR  _Myazimuth_max;
	//};
	//
	//template<typename _Ts, typename _Tb = _Ts>
	//struct _Cylinder {
	//	/*
	//	@_Equaltion:
	//		x^2 + z^2 - r^2 = 0, { y | [ymin, ymax] }
	//	@_Parametric equaltion:
	//		u, v
	//		phi = u * phi_max
	//		x   = r * cos(phi)
	//		z   = r * sin(phi)
	//		y   = 0 + v * (height)
	//	*/
	//	MATRIX4x4 _Myworld;
	//	SCALAR _Myradius;
	//	SCALAR _Myheight;
	//	SCALAR _Myazimuth_min;
	//	SCALAR _Myazimuth_max;
	//};

	//template<typename _Ts, typename _Tb = _Ts>
	//struct _Disk {
	//	/*
	//	@_Parametric equaltion:
	//		u, v
	//		phi = u * phi_max
	//		r = (1-v)*outer + v*inner
	//		x = r * cos(phi)
	//		y = r * sin(phi)
	//		z = 0
	//	*/
	//	MATRIX4x4 _Myworld;
	//	SCALAR _Myradius_inner;
	//	SCALAR _Myradius_outer;
	//	SCALAR _Myazimuth_min;
	//	SCALAR _Myazimuth_max;
	//};

	//template<typename _Ts, typename _Tb = _Ts>
	//struct _Cone {
	//	/*
	//	@_Equaltion:
	//		(h*x/r)^2 + (h*z/r)^2 - (y-h)^2 = 0
	//	@_Parametric equaltion:
	//		u, v
	//		phi = u * phi_max
	//		x = (1-v) * r * cos(phi)
	//		z = (1-v) * r * sin(phi)
	//		y = v * height
	//	*/
	//	MATRIX4x4 _Myworld;
	//	SCALAR _Myradius;
	//	SCALAR _Myheight;
	//	SCALAR _Myazimuth_min;
	//	SCALAR _Myazimuth_max;
	//};

	//template<typename _Ts, typename _Tb = _Ts>
	//struct _Paraboloid {
	//	/*
	//	@_Equaltion:
	//		h*x^2/r^2 + h*z^2/r^2 - z = 0
	//	@_Parametric equaltion:
	//		u, v
	//		phi = u * phi_max
	//		y = v(height)
	//		r = r * ¡Ì(y/height)
	//		x = r * cos(phi)
	//		z = r * sin(phi)
	//	*/
	//	MATRIX4x4 _Myworld;
	//	SCALAR _Myradius;
	//	SCALAR _Myheight;
	//	SCALAR _Myazimuth_min;
	//	SCALAR _Myazimuth_max;
	//};
	
	template<typename _Ts, typename _Tb = _Ts>
	struct _Triangle {
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
		VECTOR3 _Mypoint0;
		VECTOR3 _Mypoint1;
		VECTOR3 _Mypoint2;
	};

	template<typename _Ts, typename _Tb = _Ts>
	struct box {
		VECTOR3 _Mymin;/* minimum of xyz */
		VECTOR3 _Mymax;/* maximum of xyz */

		VECTOR3 diff() const {
			return _Mymax - _Mymin;
		}
	};


	template<typename _Ts, typename _Tb>
	bool intersect(const ray<_Ts,_Tb> _Ray, const box<_Ts,_Tb> _Box, intersection_ray<_Ts,_Tb>& _Result) {
		/*
			  |         |__
			  |   ___---|
			__|---      |
			  |         |
			  tn        tf

			(y1 - y0) / (x1 - x0) = m
			(Pbox[i] - Pray[i]) / m = (x1 - x0)
			∵ x0 = 0
			∴ (Pbox[i] - Pray[i]) / m = x1 = t
		*/
		auto t0 = std::numeric_limits<_Ts>::min();
		auto t1 = std::numeric_limits<_Ts>::max();
		for (size_t axis = 0; axis != 3; ++axis) {
			auto tn = (_Box._Mymin[axis] - _Ray._Myorg[axis]) / _Ray._Mydir[axis];// (Pmin-Pray)·n / (Vray · n), n:(1,0,0) | (0,1,0) | (0,0,1)
			auto tf = (_Box._Mymax[axis] - _Ray._Myorg[axis]) / _Ray._Mydir[axis];
			
			if (tn > tf) std::swap(tn, tf);
			t0 = maxval(t0, tn);
			t1 = minval(t1, tf);

			if (t0 > t1) {// Faild
				return (false);
			}
		}

		_Result._Myorg   = _Ray._Myorg;
		_Result._Mydir   = _Ray._Mydir;
		_Result._Myentry = t0;
		_Result._Myexit  = t1;

		_Result._Myresult = Hit_Type::Twice;
		_Result._Mypos    = _Result._Myorg * _Result._Myentry;
		return (true);
	}



	template<typename _Ts, typename _Tb = _SIMD4_t<_Ts>, matrix_major _Major = DEFAULT_MAJOR>
	struct viewer {
		RADIANS fov() const {
			return perspective_fov(_My_proj_matrix);
		}
		SCALAR  aspect() const {
			return perspective_aspect(_My_proj_matrix);
		}
		MATRIX4x4 view_matrix() const {
			return _My_view_matrix;
		}
		MATRIX4x4 proj_matrix() const {
			return _My_proj_matrix;
		}

		MATRIX4x4 _My_view_matrix;
		MATRIX4x4 _My_proj_matrix;
	};

	template<typename _Ts, typename _Tb = _SIMD4_t<_Ts>, matrix_major _Major = DEFAULT_MAJOR, coordinate_system _Cs = DEFAULT_HAND>// because this type have forward direction
	struct perspective_viewer : viewer<_Ts, _Tb, _Major> {
		using _Mybase = viewer<_Ts, _Tb, _Major>;
		enum lock_axis {
			RIGHT_AXIS = 0,
			UP_AXIS    = 1,
			FWARD_AXIS = 2
		};

		perspective_viewer()
			: perspective_viewer(VECTOR3{ 0, 1.7f, 0 }, UNIT_VECTOR3{ 0, 0, (_Cs == LEFT_HAND ? 1 : -1) }, DEGREES(60)) {}

		perspective_viewer(VECTOR3 _Position, UNIT_VECTOR3 _Direction, RADIANS _Fov) {
			_My_position = _Position;
			_My_f_vector = _Direction;
			_My_u_vector = { 0, 1, 0 };
			this->look();// update _My_r_vector
			this->reset_perspective(_Fov, 16.0f/9.0f, 0.4f, 1000.f);
		}

		// deferred change position
		void walk(METERS dis) {// forward
			_My_position += _My_f_vector * dis;
		}
		void strafe(METERS dis) {// go to right
			_My_position += _My_r_vector * dis;
		}
		// deferred change direction
		void rotate(UNIT_VECTOR3 axis, RADIANS angle) {// direction: arbitrary
			QUATERNION _Rotator = polar(axis, angle);
			_My_r_vector = _Rotator(_My_r_vector);
			_My_u_vector = _Rotator(_My_u_vector);
			_My_f_vector = _Rotator(_My_f_vector);
		}
		void yaw(RADIANS angle) {// direction: right to left
			this->rotate(_My_u_vector, angle);
		}
		void pitch(RADIANS angle) {// direction: down to up
			this->rotate(_My_r_vector, angle);
		}
		void roll(RADIANS angle) {// direction: right to left
			this->rotate(_My_f_vector, angle);
		}
		void surround(VECTOR3 Ptarget, UNIT_VECTOR3 axis, RADIANS angle) {
			QUATERNION _Rotator = polar(axis, angle);
			_My_position -= Ptarget;
			_My_position = _Rotator(_My_position);// surround Ptarget-center rotate
			_My_position += Ptarget;
		}
		void yaw_surround(VECTOR3 Ptarget, RADIANS angle) {
			this->surround(Ptarget, UNIT_VECTOR3{ 0, 1, 0 }, angle);
		}
		void pitch_surround(VECTOR3 Ptarget, RADIANS angle) {
			this->surround(Ptarget, UNIT_VECTOR3{ 1, 0, 0 }, angle);
		}
		// immediate change direction
		void look() {// update view matrix
			_Mybase::_My_view_matrix = clmagic::look_to<_Ts, _Tb, _Major, _Cs>(_My_position, _My_f_vector, _My_u_vector);
		}
		void look_at(VECTOR3 Ptarget) {
			_My_f_vector = UNIT_VECTOR3(Ptarget - _My_position);
			this->look();
		}
		void look_to(UNIT_VECTOR3 f_vector) {
			_My_f_vector = f_vector;
			this->look();
		}
		void reset_perspective(RADIANS fov, SCALAR r, SCALAR n, SCALAR f) {
			_Mybase::_My_proj_matrix = perspective<_Ts, _Tb, _Major, _Cs>(fov, r, n, f);
		}

		UNIT_VECTOR3 forward_vector() const {
			return _My_f_vector;
		}
		UNIT_VECTOR3 right_vector() const {
			return _My_r_vector;
		}
		UNIT_VECTOR3 up_vector() const {
			return _My_u_vector;
		}
		VECTOR3 position() const {
			return _My_position;
		}
		std::string to_string() const {
			std::string _Str = "\"_My_position\": " + clmagic::to_string(_My_position) + '\n';
			_Str += "\"_My_r_vector\": " + clmagic::to_string(_My_r_vector) + '\n';
			_Str += "\"_My_u_vector\": " + clmagic::to_string(_My_u_vector) + '\n';
			_Str += "\"_My_f_vector\": " + clmagic::to_string(_My_f_vector) + '\n';
			return _Str;
		}

		VECTOR3      _My_position;
		UNIT_VECTOR3 _My_r_vector;// x
		UNIT_VECTOR3 _My_u_vector;// y
		UNIT_VECTOR3 _My_f_vector;// z
	};

	template<typename _Ts, typename _Tb = _SIMD4_t<_Ts>, matrix_major _Major = DEFAULT_MAJOR, coordinate_system _Cs = DEFAULT_HAND>
	struct perspective_surround_viewer {

		//spherical_coordinate<_Ts> 
	};


	/*template<typename _Ts, typename _Tb = _Ts, bool _Major = COL_MAJOR>
	struct perspective_camera {
		using scalar_type       = _Ts;
		using unit_vector3_type = unit_vector3<_Ts, _Tb>;
		using matrix4x4_type    = matrix4x4<_Ts, _Tb>;
		using quaternion_type   = WilliamRowanHamilton::quaternion<_Ts, _Tb>;

		perspective_camera()
			: _Myview((_Ts)1), _Myproj((_Ts)1), 
			_Mylook(VECTOR3{ 0.f, 1.7f, 0.f }, UNIT_VECTOR3{ 0.f, 0.f, 1.f }, 1.f, 10000.f),
			_Myup_vector{0.f, 1.f, 0.f},
			_Myright_vector{ 1.f, 0.f, 0.f }, 
			_Myfov(1.0), 
			_Myaspect(1600.f/900.f) {
			reset_perspective(_Myfov, _Myaspect, _Mylook._Mynear, _Mylook._Myfar);
			update_viewmatrix();
		}

		void reset_perspective(radians<scalar_type> _Fov, scalar_type _Aspect, scalar_type _Near, scalar_type _Far) {
			_Myfov          = _Fov;
			_Myaspect       = _Aspect;
			_Mylook._Mynear = _Near;
			_Mylook._Myfar  = _Far;
			_Myproj = perspective<matrix4x4_type>::get_matrix(_Fov, _Aspect, _Near, _Far);
		}
		void walk(scalar_type _Distance) {
			_Mylook._Myorg += _Distance * _Mylook._Mydir;
		}
		void strafe(scalar_type _Distance) {
			_Mylook._Myorg += _Distance * _Myright_vector;
		}
		void pitch(radians<scalar_type> _Angle) {
			quaternion_type _Rotator = WilliamRowanHamilton::polar(_Myright_vector, _Angle);
			_Myup_vector             = _Rotator(_Myup_vector);
			_Mylook._Mydir           = _Rotator(_Mylook._Mydir);
		}
		void yaw(radians<scalar_type> _Angle) {
			quaternion_type _Rotator = WilliamRowanHamilton::polar(unit_vector3_type{ 0.f, 1.f, 0.f }, _Angle);
			_Myup_vector    = _Rotator(_Myup_vector);
			_Myright_vector = _Rotator(_Myright_vector);
			_Mylook._Mydir  = _Rotator(_Mylook._Mydir);
		}
		void zoom(scalar_type _Percentage) {
		}
		void update_viewmatrix() {
			_Myview = look_at<matrix4x4_type>::get_matrix(_Mylook._Myorg, _Mylook._Mydir, _Myup_vector);
		}

		VECTOR3 eye_position() const {
			return _Mylook._Myorg; }
		UNIT_VECTOR3 forward_vector() const { 
			return _Mylook._Mydir; }
		UNIT_VECTOR3 up_vector() const {
			return _Myup_vector; }
		UNIT_VECTOR3 right_vector() const {
			return _Myright_vector; }
		MATRIX4x4 view_matrix() const {
			return _Myview; }
		MATRIX4x4 projection_matrix() const {
			return _Myproj; }

		MATRIX4x4    _Myview;
		MATRIX4x4    _Myproj;
		ray<_Ts, _Tb> _Mylook;
		unit_vector3_type _Myup_vector;
		unit_vector3_type _Myright_vector;
		scalar_type _Myfov;
		scalar_type _Myaspect;
	};*/

}// namespace clmagic
