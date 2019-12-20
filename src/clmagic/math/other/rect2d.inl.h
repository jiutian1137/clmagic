#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH_OTHER___RECT2D___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH_OTHER___RECT2D___H__

namespace clmagic 
{
	// <other>
	inline Vec2
		rectcoords_cast(RECT_COORD In_Coord)
	{
		switch (In_Coord)
		{
		case RECT_COORD::LEFTTOP:	  return Vec2(0.0f, 1.0f);
		case RECT_COORD::LEFTBOTTOM:  return Vec2(0.0f, 0.0f);
		case RECT_COORD::RIGHTTOP:    return Vec2(1.0f, 1.0f);
		case RECT_COORD::RIGHTBOTTOM: return Vec2(1.0f, 0.0f);
		case RECT_COORD::CENTER:	  return Vec2(0.5f, 0.5f);
		default: return Vec2(0.0f, 0.0f);
		}
	}

	inline std::string
		to_string(RECT_COORD _Rectcoord)
	{
		switch (_Rectcoord)
		{
		case clmagic::RECT_COORD::LEFTTOP:    return (std::string("LEFTTOP"));
		case clmagic::RECT_COORD::LEFTBOTTOM: return (std::string("LEFTBOTTOM"));
		case clmagic::RECT_COORD::RIGHTTOP:   return (std::string("RIGHTTOP"));
		case clmagic::RECT_COORD::RIGHTBOTTOM: return (std::string("RIGHTBOTTOM"));
		case clmagic::RECT_COORD::CENTER:	  return (std::string("CENTER"));
		default: return (std::string());
		}
	}
	// </other>

	// <rect2d>

	// <construct>
	inline rect2d::rect2d()
		: _Mypos(0.0f), _Mysize(0.0f)
	{	// construct zero rect
	}

	inline rect2d::rect2d(const Vec2& _Pos, const Vec2& _Uv, const Vec2& _Size)
	{
		_Mypos = _Pos - _Size * _Uv;
		_Mysize = _Size;
	}

	inline rect2d::rect2d(const Vec2& _Pos, RECT_COORD _Coord, const Vec2& _Size)
		: rect2d(_Pos, rectcoords_cast(_Coord), _Size)
	{
	}

	inline rect2d::rect2d(const Vec2& _Pos, RECT_COORD _Coord, float _Width, float _Height)
		: rect2d(_Pos, rectcoords_cast(_Coord), Vec2(_Width, _Height))
	{
	}
	// </construct>

	// <convert>
	inline rect2d::operator std::string() const
	{
		std::string _Str = "rect2d[\n";
		_Str += "pos[" + static_cast<std::string>(_Mypos) + "],\n";
		_Str += "size[" + static_cast<std::string>(_Mysize) + "]";
		_Str += "]";
		return (_Str);
	}
	// </convert>

	// <method>
	inline int
		rect2d::intersect(const Vec2 _Point) const
	{
		// 0.0,1.0    1.0,1.0
		//	lt------+-------rt
		//  |  2    |   1   |
		//	|_____center____|
		//	|       |       |
		//  |  3    |   4   |
		//	lb------+-------rb 1.0,0.0
		//0.0,0.0(_Mypos)
		//
		Vec2 _Local = _Point - _Mypos;
		Vec2 _Uv = _Local / _Mysize;

		if (_Uv.x > 1.0f || _Uv.x < 0.0f || _Uv.y > 1.0f || _Uv.y < 0.0f)
		{
			return (0);
		}

		if (_Uv.x >= 0.5f)
		{	// { 1, 4 }
			if (_Uv.y >= 0.5f)
			{
				return (1);
			}
			else
			{
				return (4);
			}
		}
		else
		{	// { 2, 3 }
			if (_Uv.y >= 0.5f)
			{
				return (2);
			}
			else
			{
				return (3);
			}
		}
	}

	inline 
	bool rect2d::intersect(const Vec2 _Point, _out(Hit_<float>) _Result) const {
		Vec2 _Local = _Point - _Mypos;
		Vec2 _Uv = _Local / _Mysize;

		if (_Uv.x > 1.0f || _Uv.x < 0.0f || _Uv.y > 1.0f || _Uv.y < 0.0f) {
			return (false);
		}

		_Result._Myresult = ShapeType::ePLANE;
		_Result._Myt = 0.f;
		_Result._Myorigin = Vec3(_Point, 0.f);
		_Result._Mynormal = cZ;
		_Result._Myuv = _Uv;
		return ( true );
	}

	inline Vec2
		rect2d::translate(float _dX, float _dY)
	{
		Vec2 _Oldpos = _Mypos;
		_Mypos = _Mypos + Vec2(_dX, _dY);
		return (_Oldpos);
	}

	inline Vec2
		rect2d::scale_step(float _sX, float _sY)
	{
		Vec2 _Oldsize = _Mysize;
		_Mysize = _Mysize + Vec2(_sX, _sY);
		_Mypos = _Mypos - Vec2(_sX * 0.5f, _sY * 0.5f);
		return (_Oldsize);
	}

	inline Vec2
		rect2d::scale_rate(float _sX, float _sY)
	{
		return scale_step((_sX - 1.0f) * width(), (_sY - 1.0f) * height());
	}

	template< typename _Ty > inline
		Vec2_< _Ty >
		rect2d::get_pos(const Vec2& _Uv) const
	{
		Vec2_<float> _Pos = _Mypos + _Mysize * _Uv;
		return Vec2_< _Ty >(static_cast<_Ty>(_Pos.x()), static_cast<_Ty>(_Pos.y()));
	}

	template<> inline
		Vec2_< float >
		rect2d::get_pos<float>(const Vec2& _Uv) const { return (_Mypos + _Mysize * _Uv); }

	template<typename _Ty> inline
		Vec2_< _Ty >
		rect2d::get_pos(float _U, float _V) const { return get_pos<_Ty>(Vec2(_U, _V)); }

	template<typename _Ty> inline Vec2_<_Ty> rect2d::leftbottom()  const { return Vec2_<_Ty>(static_cast<_Ty>(_Mypos.x()), static_cast<_Ty>(_Mypos.y())); }
	template<> inline Vec2_<float> rect2d::leftbottom<float>() const { return (_Mypos); }
	template<typename _Ty> inline Vec2_<_Ty> rect2d::lefttop()	   const { return get_pos<_Ty>(0.0f, 1.0f); }
	template<typename _Ty> inline Vec2_<_Ty> rect2d::righttop()	   const { return (get_pos< _Ty >(1.0f, 1.0f)); }
	template<typename _Ty> inline Vec2_<_Ty> rect2d::rightbottom() const { return (get_pos< _Ty >(1.0f, 0.0f)); }
	template<typename _Ty> inline Vec2_<_Ty> rect2d::center()	   const { return (get_pos< _Ty >(0.5f, 0.5f)); }

	template<typename _Ty> inline _Ty rect2d::left()   const { return static_cast<_Ty>(_Mypos.x); }
	template<typename _Ty> inline _Ty rect2d::bottom() const { return static_cast<_Ty>(_Mypos.y); }
	template<typename _Ty> inline _Ty rect2d::top()    const { return static_cast<_Ty>(_Mypos.y + height()); }
	template<typename _Ty> inline _Ty rect2d::right()  const { return static_cast<_Ty>(_Mypos.x + width()); }
	template<typename _Ty> inline _Ty rect2d::width()  const { return static_cast<_Ty>(_Mysize[0]); }
	template<typename _Ty> inline _Ty rect2d::height() const { return static_cast<_Ty>(_Mysize[1]); }

	inline void rect2d::set_pos(Vec2 _Newpos) { _Mypos = _Newpos; }
	inline void rect2d::set_size(float _Newwidth, float _Newheight) { _Mysize = Vec2(_Newwidth, _Newheight); }

	inline Vec2& rect2d::my_pos() { return (_Mypos); }
	inline Vec2& rect2d::my_size() { return (_Mysize); }
	inline const Vec2& rect2d::my_pos()  const { return (_Mypos); }
	inline const Vec2& rect2d::my_size() const { return (_Mysize); }
	// </method>

}// namespace clmagic

#endif