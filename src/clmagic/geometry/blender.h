#pragma once
#ifndef _CLMAGIC_CORE_GEOMETRY_BLENDER_H_
#define _CLMAGIC_CORE_GEOMETRY_BLENDER_H_
#include "math.h"

namespace clmagic 
{
	using blend_value_type = const Vec4&;

	class blender 
	{
	public:
		typedef Vec4(*_Factorfunc)(blend_value_type _Src, blend_value_type _Dst);
		typedef Vec4(*_Operator)(blend_value_type _Src, blend_value_type _Dst);
	
		_Operator _Myop;
		_Factorfunc _Mysrc;
		_Factorfunc _Mydst;

		Vec4 operator() (blend_value_type _Src, blend_value_type _Dst) 
			{
			return ( _Myop(_Src * _Mysrc(_Src, _Dst), _Dst * _Mydst(_Src, _Dst)) );
			}

#ifndef blend
		Vec4 blend(blend_value_type _Src, blend_value_type _Dst)
			{	// not recommended
			return ( _Myop(_Mysrc(_Src, _Dst), _Mydst(_Src, _Dst)) );
			}
#endif
	};


	inline Vec4 blend_operator_none(blend_value_type _Src, blend_value_type _Dst)
		{
		return ( Vec4(_Src[0], _Src[1], _Src[2], _Src[3]) );
		}

	inline Vec4 blend_operator_add(blend_value_type _Src, blend_value_type _Dst)
		{
		return (_Src + _Dst);
		}

	inline Vec4 blend_operator_sub(blend_value_type _Src, blend_value_type _Dst)
		{
		return (_Src - _Dst);
		}

	inline Vec4 blend_operator_sub_rv(blend_value_type _Src, blend_value_type _Dst)
		{
		return (_Dst - _Src);
		}

	inline Vec4 blend_operator_min(blend_value_type _Src, blend_value_type _Dst)
		{
		return ( for_each(_Src, _Dst, [](float a, float b) { return (minval(a, b)); } ) );
		}

	inline Vec4 blend_operator_max(blend_value_type _Src, blend_value_type _Dst)
		{
		return ( for_each(_Src, _Dst, [](float a, float b) { return (maxval(a, b)); }) );
		}

	inline Vec4 blend_factor_one(blend_value_type _Src, blend_value_type _Dst)
		{
		return (Vec4(1.0f));
		}

	inline Vec4 blend_factor_zero(blend_value_type _Src, blend_value_type _Dst)
		{
		return (Vec4(0.0f));
		}

	inline Vec4 blend_factor_src_alpha(blend_value_type _Src, blend_value_type _Dst)
		{
		return ( Vec4( _Src[3] ) );
		}

	inline Vec4 blend_factor_dst_alpha(blend_value_type _Src, blend_value_type _Dst)
		{
		return ( Vec4(_Dst[3]) );
		}

	inline Vec4 blend_factor_src_color(blend_value_type _Src, blend_value_type _Dst)
		{
		return ( Vec4(_Src[0], _Src[1], _Src[2], 1.0f) );
		}

	inline Vec4 blend_factor_dst_color(blend_value_type _Src, blend_value_type _Dst)
		{
		return ( Vec4(_Dst[0], _Dst[1], _Dst[2], 1.0f) );
		}

	inline Vec4 blend_factor_one_minus_src_alpha(blend_value_type _Src, blend_value_type _Dst)
		{
		return ( Vec4(1.0f - _Src[3]) );
		}

	inline Vec4 blend_factor_one_minus_dst_alpha(blend_value_type _Src, blend_value_type _Dst)
		{
		return ( Vec4(1.0f - _Dst[3]) );
		}

	inline Vec4 blend_factor_one_minus_src_color(blend_value_type _Src, blend_value_type _Dst)
		{
		return ( Vec4( 1.0f - _Src[0], 1.0f - _Src[1], 1.0f - _Src[2], 1.0f ) );
		}

	inline Vec4 blend_factor_one_minus_dst_color(blend_value_type _Src, blend_value_type _Dst)
		{
		return ( Vec4(1.0f - _Dst[0], 1.0f - _Dst[1], 1.0f - _Dst[2], 1.0f) );
		}

	inline blender noblend()
		{
		return (blender{ blend_operator_none });
		}

	inline blender transparent() 
		{
		return ( blender{ blend_operator_add, blend_factor_src_alpha, blend_factor_one_minus_src_alpha } );
		}

	inline blender composition() 
		{
		return ( blender{ blend_operator_add, blend_factor_one, blend_factor_one } );
		}

}// namespace clmagic 

#endif