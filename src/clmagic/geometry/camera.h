#pragma once
#ifndef _CLMAGIC_CORE_CAMERA_H_
#define _CLMAGIC_CORE_CAMERA_H_
#include "math.h"
#include <thread>
#include <string>
#include <iostream>



namespace clmagic
{
	enum class CAMERA_MOVE 
	{
		Noupdate, Liveupdate, Slowupdate
	};


	enum class CAMERA_STATE 
	{
		NORMAL, MOVED, MOVING
	};

	/*
	@_visualcamera
	*/
	class VirtualCamera
	{
	public:
		constexpr static const float PercentageTwo = 2.0f / 100.0f;
		constexpr static const float NegativePercentageTwo = -PercentageTwo;

		VirtualCamera(const std::string& _Name, const Vec3 _EyePos = Vec3(0.0f, 0.0f, 1.0f), CAMERA_MOVE _Movemode = CAMERA_MOVE::Liveupdate);
		virtual ~VirtualCamera();

		virtual explicit operator std::string() const = 0;

		// <interface>
		virtual void update_viewmatrix() = 0;
		virtual Ray  compute_ray(float _MouseX, float _MouseY) const = 0;
		virtual void arbitrary_move(const Vec3& _Dir, float _Length) = 0;
		// </interface>

		void reset_name(const std::string& _Name);
		void reset_movemode(CAMERA_MOVE _Movemode);
		void eyepos_goto(const Vec3& _Pos);
		void eyepos_goto(float _Px, float _Py, float _Pz);
		
		const Vec3& eyepos()				 const { return (_Myeye); }
		const mat4& matrix_view()			 const { return (_Myview); }
		const mat4& matrix_projection()		 const { return (_Myproj); }
		mat4	    matrix_view_projection() const { return (_Myview * _Myproj); }

		std::string& name() { return (_Myname); }
		Vec3& eyepos() { return (_Myeye); }

		const std::string& name() const { return (_Myname); }
		CAMERA_MOVE movemode() const { return (_Mymv);		}
		CAMERA_STATE state()   const { return (_Mystate);   }

	protected:
		friend void _Slow_move_eyepos(VirtualCamera& _Cmr, const Vec3& _Target);
		
		std::string _Myname;
		CAMERA_MOVE  _Mymv;
		CAMERA_STATE _Mystate;
		Vec3 _Myeye;
		mat4 _Myview;
		mat4 _Myproj;
	};


	/*
	@_perspectivecamera
	*/
	class PerspectiveCamera : public VirtualCamera
	{
	public:
		PerspectiveCamera();
		virtual ~PerspectiveCamera() {}

		virtual explicit operator std::string() const override;

		virtual void update_viewmatrix() override;
		virtual Ray  compute_ray(float _MouseX, float _MouseY) const override;
		virtual void arbitrary_move(const Vec3& _Dir, float _Length) override;

		void walk(float distance);
		void strafe(float distance);
		void pitch(float angle);
		void yaw(float angle);
		void zoom(float _Percentage);
		void set_perspective(float _Fov, float _Width, float _Height, float _Nearplane, float _Farplane);

		inline const Vec3& frontvector() const { return (v3_Myfront); }
		inline const Vec3& upvector()    const { return (v3_Myup);	  }
		inline const Vec3& rightvector() const { return (v3_Myright); }
		inline const mat4& projmatrix_flipped() const { return (m4_Myprojflip); }
		inline float fov()		 const { return (f_Myfov);    }
		inline float width()	 const { return (f_Mywidth);  }
		inline float height()	 const { return (f_Myheight); }
		inline float nearplane() const { return (f_Mynear);   }
		inline float farplane()  const { return (f_Myfar);	  }
		
	private:
		Vec3 v3_Myfront;
		Vec3 v3_Myup;
		Vec3 v3_Myright;
		mat4 m4_Myprojflip;
		float f_Myfov;
		float f_Mywidth;
		float f_Myheight;
		float f_Mynear;
		float f_Myfar;
	};


	/*
	@_orthocamera
	*/
	class OrthoCamera : public VirtualCamera
	{
	public:
		OrthoCamera();
		virtual ~OrthoCamera() {}

		virtual explicit operator std::string() const override;

		virtual void update_viewmatrix() override;
		virtual Ray  compute_ray(float _MouseX, float _MouseY) const override;
		virtual void arbitrary_move(const Vec3& _Direction, float _Distane) override;

		void walk(float distance);
		void strafe(float distance);
		void zoom(float _Percentage);
		void set_ortho(float _Left, float _Right, float _Bottom, float _Top, float _Nearplane, float _Farplane);

		inline float left()	  const { return (f_Myleft);   }
		inline float right()  const { return (f_Myright);  }
		inline float bottom() const { return (f_Mybottom); }
		inline float top()	  const { return (f_Mytop);	   }
		inline float zoom_rate() const { return (f_Myscale); }
		

	private:
		Vec3 v3_Myfront;
		Vec3 v3_Myup;
		Vec3 v3_Myright;
		float f_Myleft;
		float f_Myright;
		float f_Mybottom;
		float f_Mytop;
		float f_Mynear;
		float f_Myfar;
		float f_Myscale;
	};


	inline void camera_rotate(PerspectiveCamera& _Camera, float _Yaw, float _Pitch, float _Roll = 0.0f)
		{
		_Camera.yaw(_Yaw);
		_Camera.pitch(_Pitch);
		if (_Roll != 0.0f)
			{	// ...
			}
		_Camera.update_viewmatrix();
		}



	template<typename T>
		Ray_<T> compute_ray(
			_in(Vec3_<T>) _Localpoint,
			_in(Vec3_<T>) _Cmrorigin,
			_in(Vec3_<T>) _Cmrfwd,
			_in(Vec3_<T>) _Cmrup);

}// namespace clmagic

#include "camera.inl.h"

#endif