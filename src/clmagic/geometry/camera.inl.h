#pragma once
#ifndef _CLMAGIC_CORE_CAMERA_INLINE_H_
#define _CLMAGIC_CORE_CAMERA_INLINE_H_

namespace clmagic
{
	// local function
	inline void _Slow_move_eyepos(VirtualCamera& _Cmr, const Vec3& _Target)
		{
		int _Step = 100;
		std::chrono::milliseconds _Internal{ 3000LL / _Step };
		Vec3 _Stride = _Target / static_cast<float>(_Step);

		for (int i = 0; i != _Step; ++i)
			{
			std::this_thread::sleep_for(_Internal);
			_Cmr._Myeye = _Cmr._Myeye + _Stride;
			_Cmr.update_viewmatrix();
			}
		}

	// <VirtualCamera>
	inline VirtualCamera::VirtualCamera( const std::string& _Name, const Vec3 _EyePos, CAMERA_MOVE _Movemode)
		: _Myname(_Name),
		_Mymv(_Movemode),
		_Mystate(CAMERA_STATE::MOVED),
		_Myeye(_EyePos),
		_Myview(mat4::identity()),
		_Myproj(mat4::identity())
		{	// ...
		}

	inline VirtualCamera::~VirtualCamera() 
		{
		}

	inline void VirtualCamera::reset_name(const std::string& _Name)
		{ _Myname = _Name; }

	inline void VirtualCamera::eyepos_goto(const Vec3& _Pos)
		{
		switch (_Mymv)
			{
			case clmagic::CAMERA_MOVE::Noupdate:
				this->_Myeye = _Pos;
				this->_Mystate = CAMERA_STATE::MOVED;
				break;
			case clmagic::CAMERA_MOVE::Liveupdate:
				this->_Myeye = _Pos;
				this->update_viewmatrix();
				break;
			case clmagic::CAMERA_MOVE::Slowupdate:
				{
				std::thread _Thr(_Slow_move_eyepos, std::ref(*this), _Pos);
				if (_Thr.joinable())
					_Thr.detach();
				this->_Mystate = CAMERA_STATE::MOVING;
				}
			break;
			default: break;
			}
		}

	inline void VirtualCamera::eyepos_goto(float _Px, float _Py, float _Pz) 
		{ eyepos_goto(Vec3(_Px, _Py, _Pz)); }

	inline void VirtualCamera::reset_movemode(CAMERA_MOVE _Movemode) 
		{ _Mymv = _Movemode; }
	// </VirtualCamera>

	//-------------------------------------------------------------------------------------
	inline PerspectiveCamera::PerspectiveCamera()
		: VirtualCamera("PerspectiveCamera"),
		v3_Myfront( -cZ ), v3_Myup( cY ), v3_Myright( cX ),
		m4_Myprojflip(),
		f_Myfov(0.0f),
		f_Mywidth(0.0f),
		f_Myheight(0.0f),
		f_Mynear(0.0f),
		f_Myfar(0.0f)
		{
		// Right hand coordinate, Because look is -1.0
		// (do nothing)
		}

	inline PerspectiveCamera::operator std::string() const
		{
		std::string _Str = "PerspectiveCamera[\n";
		_Str += "front[" + static_cast<std::string>(v3_Myfront) + "]\n";
		_Str += "   up[" + static_cast<std::string>(v3_Myup) + "]\n";
		_Str += "right[" + static_cast<std::string>(v3_Myright) + "]\n";
		_Str += "]";
		/*
		PerspectiveCamera[
		front[ ... ]
		   up[ ... ]
		right[ ... ]
		]
		*/
		return (_Str);
		}

	inline void PerspectiveCamera::update_viewmatrix()
		{
		Vec3 _Target = _Myeye + v3_Myfront;
		_Myview = Lookat(_Myeye, _Target, v3_Myup);
		}

	inline Ray PerspectiveCamera::compute_ray(float _MouseX, float _MouseY) const
		{	// mouse coordinate at left bottom
		float normalized_x = (_MouseX / f_Mywidth * 2.0f) - 1.0f;// Region[-1.0, 1.0]
		float normalized_y = (_MouseY / f_Myheight * 2.0f) - 1.0f;// Region[-1.0, 1.0]

		/*
		mat4 inverse_view = inverse(m4_Myview);
		mat4 inverse_proj = inverse(m4_Myproj);

		Vec4 _Direction = normal_cast<Vec4>(normalized_x, normalized_y, 1.0f, 1.0f);
		_Direction = transformCoord(_Direction, normal_cast<mat128x4>(inverse_proj));
		_Direction = transformNormal(_Direction, normal_cast<mat128x4>(inverse_view));
		_Direction = normalize(_Direction);
		*/

		Ray _Ray;
		_Ray._Mydirection = screen_to_world(Vec3(normalized_x, normalized_y, 1.0f), _Myview, _Myproj);
		_Ray._Myorigin = _Myeye;
		return (_Ray);
		}

	inline void PerspectiveCamera::arbitrary_move(const Vec3& _Dir, float _Length)
		{
		_Myeye = _Dir * _Length + _Myeye;
		}

	inline void PerspectiveCamera::walk(float _Length)
		{
		_Myeye = v3_Myfront * _Length + _Myeye;
		}

	inline void PerspectiveCamera::strafe(float _Length)
		{
		_Myeye = v3_Myright * _Length + _Myeye;
		}

	inline void PerspectiveCamera::pitch(float angle)
		{
		Quaternion _Rot{ v3_Myright, angle };
		v3_Myup = _Rot.rotate(v3_Myup);
		v3_Myfront = _Rot.rotate(v3_Myfront);
		}

	inline void PerspectiveCamera::yaw(float angle)
		{
		Quaternion _Rot{ cY, angle };
		v3_Myright = _Rot.rotate(v3_Myright);
		v3_Myup = _Rot.rotate(v3_Myup);
		v3_Myfront = _Rot.rotate(v3_Myfront);
		}

	inline void PerspectiveCamera::zoom(float _Percentage)
		{
		f_Myfov += _Percentage;
		_Myproj = Perspective(f_Myfov, f_Mywidth / f_Myheight, f_Mynear, f_Myfar);
		m4_Myprojflip = Perspective(f_Myfov, f_Mywidth / f_Myheight, f_Myfar, f_Mynear);
		}

	inline void PerspectiveCamera::set_perspective(float _Fov, float _Width, float _Height, float _Nearplane, float _Farplane)
		{
		f_Myfov = _Fov;
		f_Mywidth = _Width;
		f_Myheight = _Height;
		f_Mynear = _Nearplane;
		f_Myfar = _Farplane;
		_Myproj = Perspective(_Fov, _Width / _Height, _Nearplane, _Farplane);
		m4_Myprojflip = Perspective(_Fov, _Width / _Height, _Farplane, _Nearplane);
		}



	//float* lookat()
	//	{
	//	static float result[4][4]
	//	{   { 1.0f, 0.0f, 0.0f,0.0f },
	//		{ 0.0f, 1.0f, 0.0f,0.0f },
	//		{ 0.0f, 0.0f, 1.0f,0.0f },
	//		{ 0.0f, 0.0f, 0.0f,1.0f }
	//	};
	//
	//	cmr::vec3 f(m_front);
	//
	//	cmr::vec3 r = cross(f, m_up);
	//	r.normalizing();
	//
	//	cmr::vec3 u = cross(r, f);
	//	u.normalizing();
	//
	//	result[0][0] = r.x;
	//	result[1][0] = r.y;
	//	result[2][0] = r.z;
	//	result[0][1] = u.x;
	//	result[1][1] = u.y;
	//	result[2][1] = u.z;
	//	result[0][2] = -f.x;
	//	result[1][2] = -f.y;
	//	result[2][2] = -f.z;
	//	result[3][0] = -(m_eye.dot(r));
	//	result[3][1] = -(m_eye.dot(u));
	//	result[3][2] = m_eye.dot(f);
	//	/*
	//	* [ r.x   u.x   -f.x   0 ]
	//	* [ r.y   u.y   -f.y   0 ]
	//	* [ r.z   u.z   -f.z   0 ]
	//	* [ -e.r -e.u    e.f   1 ]
	//	*/
	//
	//	return (&(result[0][0]));
	//	}


	//-------------------------------------------------------------------------------------
	inline OrthoCamera::OrthoCamera()
		: VirtualCamera("OrthoCamera"),
		v3_Myfront( -cZ ),
		v3_Myup( cY ),
		v3_Myright( cX ),
		f_Myleft(0.0f),
		f_Myright(256.0f),
		f_Mybottom(0.0f),
		f_Mytop(256.0f),
		f_Mynear(1.0f),
		f_Myfar(-1.0f),
		f_Myscale(1.0f)
		{	// ...
		}

	inline OrthoCamera::operator std::string() const
		{
		std::string _Str = "ortho camera[";
		_Str += "front[" + static_cast<std::string>(v3_Myfront) + "]\n";
		_Str += "   up[" + static_cast<std::string>(v3_Myup) + "]\n";
		_Str += "right[" + static_cast<std::string>(v3_Myright) + "]\n";
		_Str += "projection[" + std::to_string(f_Myleft) + " " + std::to_string(f_Myright) + " "
			+ std::to_string(f_Mybottom) + " " + std::to_string(f_Mytop) + " "
			+ std::to_string(f_Mynear) + " " + std::to_string(f_Myfar) + "]\n";
		_Str += "]";
		return (_Str);
		}

	inline void OrthoCamera::update_viewmatrix()
		{
		_Myview = Lookat(_Myeye, v3_Myfront, v3_Myup);
		}

	inline Ray OrthoCamera::compute_ray(float _MouseX, float _MouseY) const
		{
		//     ________screen_____________
		//    |	    ___viewport_____      |
		//    |	   |	 _mouse	    |     |
		//   lef1 lef2  /\-  	    |     |
		//    |	   |	  \			|     |
		//    |	    ----------------+	  |
		//    +---------------------------+
		//				  | |
		float _Width = f_Myright - f_Myleft;
		float _Height = f_Mytop - f_Mybottom;
		_MouseX += _Myeye.x * _Width * 0.5f;
		_MouseY += _Myeye.y * _Height * 0.5f;

		float _InvZoom = 1.0f / f_Myscale;
		// lef2 - lef1
		_MouseX = _MouseX * _InvZoom + (f_Myleft * _InvZoom - f_Myleft);
		_MouseY = _MouseY * _InvZoom + (f_Mybottom * _InvZoom - f_Mybottom);

		Ray _Ray;
		_Ray._Myorigin = Vec3(_MouseX, _MouseY, 1.0f);
		_Ray._Mydirection = Vec3(_MouseX, _MouseY, -1.0f);
		return ( _Ray );
		}

	inline void OrthoCamera::arbitrary_move(const Vec3& _Dir, float _Length)
		{
		_Myeye = _Dir * _Length + _Myeye;
		}

	inline void OrthoCamera::walk(float _Length)
		{
		_Myeye = v3_Myup * _Length + _Myeye;
		}

	inline void OrthoCamera::strafe(float _Length)
		{
		_Myeye = v3_Myright * _Length + _Myeye;
		}

	inline void OrthoCamera::zoom(float _Percentage)
		{
		f_Myscale += _Percentage;
		float _InvZoom = 1.0f / f_Myscale;
		_Myproj = Ortho(f_Myleft * _InvZoom, f_Myright * _InvZoom, f_Mybottom * _InvZoom, f_Mytop * _InvZoom, f_Mynear, f_Myfar);
		}

	inline void OrthoCamera::set_ortho(float _Left, float _Right, float _Bottom, float _Top, float _Nearplane, float _Farplane)
		{
		f_Myleft = _Left;
		f_Myright = _Right;
		f_Mybottom = _Bottom;
		f_Mytop = _Top;
		_Myproj = Ortho(_Left, _Right, _Bottom, _Top, _Nearplane, _Farplane);
		}


	template<typename T> inline
		Ray_<T> compute_ray( _in(Vec3_<T>) _Localpoint, _in(Vec3_<T>) _Cmrorigin, _in(Vec3_<T>) _Cmrfwd, _in(Vec3_<T>) _Cmrup )
		{
		Vec3 _Cmrright = cross(_Cmrfwd, _Cmrup);

		Ray_<T> _Ray;
		_Ray._Myorigin = _Cmrorigin;
		_Ray._Mydirection = normalize(_Cmrfwd + _Cmrup * _Localpoint.y + _Cmrright * _Localpoint.x);

		return _Ray;
		}

} // namespace

#endif