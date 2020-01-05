#pragma once
#ifndef clmagic_math_OTHER_h_
#define clmagic_math_OTHER_h_
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
		constexpr ColorRGB( _in(Vector3_<T>) _Linearcolor )
			: red(  static_cast<unsigned char>(_Linearcolor[0] * 255.f) ),
			  green(static_cast<unsigned char>(_Linearcolor[1] * 255.f) ),
			  blue( static_cast<unsigned char>(_Linearcolor[2] * 255.f) )
			{ /*empty*/ }

		operator Vector3_<real_t>() const;
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

		operator Vector3_<real_t>() const;
		explicit operator std::string() const;

		union
		{
			struct { float hue, saturation, value; };
			float data[3];
		};
	};
	
	
	
	enum class RECT_COORD {
		LEFTTOP,
		LEFTBOTTOM,
		RIGHTTOP, 
		RIGHTBOTTOM, 
		CENTER
	};

	template<typename T>
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
#if defined(clmagic_using_SIMDstructure)
		using position_type = SIMDVec_<2, T>;
		using region_type   = SIMDVec_<2, T>;
#else
		using position_type = Vec_<2, T>;
		using region_type   = Vec_<2, T>;
#endif

		static position_type rectcoords_cast(RECT_COORD In_Coord);
		static std::string to_string(RECT_COORD _Rectcoord);

		// <construct>
		rect2d() : _Mypos(T(0)), _Mysize(T(0)) { }
		rect2d(const position_type& _Pos, const position_type& _Uv, const region_type& _Size) : _Mypos(_Pos - _Size * _Uv), _Mysize(_Size) { }
		rect2d(const position_type& _Pos, RECT_COORD _Coord, const region_type& _Size) : rect2d(_Pos, rectcoords_cast(_Coord), _Size){ }
		rect2d(const position_type& _Pos, RECT_COORD _Coord, float _Width, float _Height) : rect2d(_Pos, rectcoords_cast(_Coord), region_type(_Width, _Height)) { }
		// </construct>

		// <method>
		int intersect(const position_type& _Point) const;
		bool intersect(const position_type& _Point, _out(Hit_<T>)) const;
		position_type translate(T _dX, T _dY);
		region_type   scale_step(T _sX, T _sY);
		region_type   scale_rate(T _sX, T _sY);

		template<typename _Ty = float> Vector2_<_Ty> get_pos(const position_type& _Uv) const;
		template<typename _Ty = float> Vector2_<_Ty> get_pos(T _U, T _V) const;

		template<typename _Ty = float> Vector2_<_Ty> leftbottom() const { return Vector2_<_Ty>((_Ty)_Mypos[0], (_Ty)_Mypos[1]); }
		template<typename _Ty = float> Vector2_<_Ty> lefttop()    const { return get_pos<_Ty>(T(0), T(1)); }
		template<typename _Ty = float> Vector2_<_Ty> righttop()   const { return get_pos<_Ty>(T(1), T(1)); }
		template<typename _Ty = float> Vector2_<_Ty> rightbottom() const{ return get_pos<_Ty>(T(1), T(0)); }
		template<typename _Ty = float> Vector2_<_Ty> center()     const { return get_pos<_Ty>(T(0.5), T(0.5)); }
		template<typename _Ty = float> _Ty left() const   { return static_cast<_Ty>(_Mypos[0]); }
		template<typename _Ty = float> _Ty bottom() const { return static_cast<_Ty>(_Mypos[1]); }
		template<typename _Ty = float> _Ty right() const  { return static_cast<_Ty>(_Mypos[0] + _Mysize[0]); }
		template<typename _Ty = float> _Ty top() const    { return static_cast<_Ty>(_Mypos[1] + _Mysize[1]); }
		template<typename _Ty = float> _Ty width() const  { return ((_Ty)_Mysize[0]); }
		template<typename _Ty = float> _Ty height() const { return ((_Ty)_Mysize[1]); }

		void set_pos(const position_type& _Newpos) { _Mypos = _Newpos; }
		void set_size(float _Newwidth, float _Newheight) { _Mysize = region_type(_Newwidth, _Newheight); }

		position_type& my_pos() { return (_Mypos); }
		region_type& my_size()  { return (_Mysize); }
		const position_type& my_pos() const { return (_Mypos); }
		const region_type& my_size()  const { return (_Mysize); }
		// </method>

	private:
		position_type _Mypos;
		region_type   _Mysize;
	};
}// namespace clmagic

#endif