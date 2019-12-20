#pragma once
#ifndef _CLMAGIC_CORE_GEOMETRY_RECT2D_H_
#define _CLMAGIC_CORE_GEOMETRY_RECT2D_H_
#include "lapack.h"
#include "raytracing.h"

namespace clmagic 
{
	/* number to hex string */
	std::string to_hexstring(int _Hexnumber);


	/*
	@_Color_space_RGB:
	*/
	struct ColorRGB
	{
		constexpr ColorRGB() : red(0), green(0), blue(0) { /*empty*/ }

		constexpr ColorRGB( unsigned char _Red, unsigned char _Green, unsigned char _Blue )
			: red( _Red ), green( _Green ), blue( _Blue ) 
			{ /*empty*/ }
		
		constexpr ColorRGB( int _Red, int _Green, int _Blue )
			: red(  unsigned char(_Red) ),
			  green(unsigned char(_Green) ),
			  blue( unsigned char(_Blue) ) 
			{ /*empty*/ }

		template<typename T>
		constexpr ColorRGB( _in(Vec3_<T>) _Linearcolor )
			: red(  static_cast<unsigned char>(_Linearcolor[0] * 255.f) ),
			  green(static_cast<unsigned char>(_Linearcolor[1] * 255.f) ),
			  blue( static_cast<unsigned char>(_Linearcolor[2] * 255.f) )
			{ /*empty*/ }

		operator Vec3() const;
		explicit operator std::string() const;

		std::string to_hexstring() const;

		union
		{
			struct { unsigned char red, green, blue; };
			unsigned char data[3];
		};
	};

	template<typename T> constexpr
		Vec_<3, T> constexpr_to_vec_3(_in(ColorRGB) _Rgb)
		{
		const auto _Linearcolor = Vec_<3, T>(
			static_cast<T>(_Rgb.red)   * cvtfLinearcolor,
			static_cast<T>(_Rgb.green) * cvtfLinearcolor,
			static_cast<T>(_Rgb.blue)  * cvtfLinearcolor
			);
		return ( _Linearcolor );
		}

	constexpr ColorRGB cRGB_RED = ColorRGB(255, 0, 0);
	constexpr ColorRGB cRGB_GREEN = ColorRGB(0, 255, 0);
	constexpr ColorRGB cRGB_BLUE = ColorRGB(0, 0, 255);
	constexpr ColorRGB cRGB_BLACK = ColorRGB(0, 0, 0);
	constexpr ColorRGB cRGB_WHITE = ColorRGB(255, 255, 255);
	constexpr ColorRGB cRGB_PINK = ColorRGB(255, 192, 203);
	constexpr ColorRGB cRGB_HOTPINK = ColorRGB(255, 105, 180);
	constexpr ColorRGB cRGB_CRIMSON = ColorRGB(220, 20, 60);
	constexpr ColorRGB cRGB_LAVENDERBLUSH = ColorRGB(255, 240, 245);
	constexpr ColorRGB cRGB_PALEVIOLETRED = ColorRGB(219, 112, 147);
	constexpr ColorRGB cRGB_MEDIUMVIOLETRED = ColorRGB(199, 21, 133);
	constexpr ColorRGB cRGB_ORCHID = ColorRGB(218, 112, 214);
	//- - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - - - - -

	/*
	@_Color_space_HSV
	*/
	struct ColorHSV
	{
		constexpr ColorHSV() : hue(0.f), saturation(0.f), value(0.f) { /*empty*/ }

		constexpr ColorHSV(float _Hue, float _Saturation, float _Value) 
			: hue(constexpr_mod(_Hue, 360.0f)),
			  saturation(constexpr_mod(_Saturation, 1.f)),
			  value(constexpr_mod(_Value, 1.f))
			{ /*empty*/ }
		
		constexpr ColorHSV( _in(ColorRGB) _RGB ) : hue(0.f), saturation(0.f), value(0.f)
			{
			const auto _Linearcolor = constexpr_to_vec_3<float>(_RGB);

			const float _Max = constexpr_maxval(_Linearcolor[0], _Linearcolor[1], _Linearcolor[2]);
			const float _Min = constexpr_minval(_Linearcolor[0], _Linearcolor[1], _Linearcolor[2]);

			hue = _Max == _Min ? 0.0f
				: _Max == _Linearcolor[0] ? fmod((((_Linearcolor[1] - _Linearcolor[2]) / (_Max - _Min)) * 60.0f) + 360.0f, 360.0f)
				: _Max == _Linearcolor[1] ? (_Linearcolor[2] - _Linearcolor[0]) / (_Max - _Min) * 60.0f + 120.0f
				: _Max == _Linearcolor[2] ? (_Linearcolor[0] - _Linearcolor[1]) / (_Max - _Min) * 60.0f + 240.0f
				: 0.0f;
			saturation = (_Max != 0.0f ? (1.0f - _Min / _Max) : 0.0f);
			value = _Max;
			}

		operator Vec3() const;
		explicit operator std::string() const;

		union
		{
			struct { float hue, saturation, value; };
			float data[3];
		};
	};
	
	
	
	enum class RECT_COORD
	{
		LEFTTOP, LEFTBOTTOM, RIGHTTOP, RIGHTBOTTOM, CENTER
	};
	Vec2 rectcoords_cast(RECT_COORD In_Coord);
	std::string to_string(RECT_COORD _Rectcoord);


	class rect2d
	{
		// 0.0,1.0    1.0,1.0
		//	lt-------- rt
		//	|		   |
		//	|0.0,0.0   |
		//	lb-------- rb 1.0,0.0
		//
		// lb + size * uv = pos
		// lb = pos - size * uv
		//
	public:
		// <construct>
		rect2d();
		rect2d(const Vec2& _Pos, const Vec2& _Uv, const Vec2& _Size);
		rect2d(const Vec2& _Pos, RECT_COORD _Coord, const Vec2& _Size);
		rect2d(const Vec2& _Pos, RECT_COORD _Coord, float _Width, float _Height);
		// </construct>

		// <convert>
		operator std::string() const;
		// </convert>

		// <method>
		int intersect(const Vec2 _Point) const;
		bool intersect(const Vec2 _Point, _out(Hit_<float>)) const;
		Vec2 translate(float _dX, float _dY);
		Vec2 scale_step(float _sX, float _sY);
		Vec2 scale_rate(float _sX, float _sY);

		template<typename _Ty = float> Vec2_<_Ty> get_pos(const Vec2& _Uv) const;
		template<typename _Ty = float> Vec2_<_Ty> get_pos(float _U, float _V) const;

		template<typename _Ty = float> Vec2_<_Ty> leftbottom() const;
		template<typename _Ty = float> Vec2_<_Ty> lefttop() const;
		template<typename _Ty = float> Vec2_<_Ty> righttop() const;
		template<typename _Ty = float> Vec2_<_Ty> rightbottom() const;
		template<typename _Ty = float> Vec2_<_Ty> center() const;
		template<typename _Ty = float> _Ty left() const;
		template<typename _Ty = float> _Ty top() const;
		template<typename _Ty = float> _Ty right() const;
		template<typename _Ty = float> _Ty bottom() const;
		template<typename _Ty = float> _Ty width() const;
		template<typename _Ty = float> _Ty height() const;

		void set_pos(Vec2 _Newpos);
		void set_size(float _Newwidth, float _Newheight);

		Vec2& my_pos();
		Vec2& my_size();
		const Vec2& my_size() const;
		const Vec2& my_pos() const;
		// </method>

	private:
		Vec2 _Mypos;// left bottom
		Vec2 _Mysize;// width height
	};
}// namespace clmagic

#endif