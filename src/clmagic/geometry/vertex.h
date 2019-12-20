#pragma once
#ifndef _CLMAGIC_CORE_GEOMETRY_VERTEX_H_
#define _CLMAGIC_CORE_GEOMETRY_VERTEX_H_
#include "math.h"
#include "variable.h"
#include <map>
#include <array>
#include <vector>


namespace clmagic 
{
	struct attribute_descriptor 
	{
		size_t	  size;
		BASE_TYPE type;
		bool	  normalize;
		size_t	  stride;
		size_t	  offset;
	};

	using vertex_descriptor = std::vector<attribute_descriptor>;

	using shader_variant = _STD pair<const char*, const char*>;



	/*
	one component vertex
	*/
	template<typename _Ty1>
	struct vertex_component1
	{
		using attrib1_type = _Ty1;

		explicit vertex_component1( const _Ty1& _Attr1 = _Ty1() );
		explicit operator std::string( ) const;

		static vertex_descriptor descriptor( );

		_Ty1 first;
	};


	/*
	two component vertex
	*/
	template<typename _Ty1, typename _Ty2>
	struct vertex_component2
	{
		using attrib1_type = _Ty1;
		using attrib2_type = _Ty2;

		vertex_component2(const _Ty1& _Attr1 = _Ty1(), const _Ty2& _Attr2 = _Ty2());
		explicit operator std::string() const;

		static vertex_descriptor descriptor();

		_Ty1 first;
		_Ty2 second;
	};


	/*
	three component vertex
	*/
	template<typename _Ty1, typename _Ty2, typename _Ty3>
	struct vertex_component3
	{
		using attrib1_type = _Ty1;
		using attrib2_type = _Ty2;
		using attrib3_type = _Ty3;

		vertex_component3(const _Ty1& _Attr1 = _Ty1(), const _Ty2& _Attr2 = _Ty2(), const _Ty3& _Attr3 = _Ty3());
		explicit operator std::string() const;

		static vertex_descriptor descriptor();

		_Ty1 first;
		_Ty2 second;
		_Ty3 third;
	};	// struct Vertex 


	/*
	four component vertex
	*/
	template<typename _Ty1, typename _Ty2, typename _Ty3, typename _Ty4>
	struct vertex_component4
	{
		using attrib1_type = _Ty1;
		using attrib2_type = _Ty2;
		using attrib3_type = _Ty3;
		using attrib4_type = _Ty4;

		vertex_component4(const _Ty1& _Attr1 = _Ty1(), const _Ty2& _Attr2 = _Ty2(), const _Ty3& _Attr3 = _Ty3(), const _Ty4& _Attr4 = _Ty4());
		explicit operator std::string() const;

		static vertex_descriptor descriptor();

		attrib1_type first;
		attrib2_type second;
		attrib3_type third;
		attrib4_type fourth;
	};
	

	struct vertex_pos3
		: public vertex_component1<Vec3> 
	{
		vertex_pos3(const Vec3& _Pos = Vec3(0.0f))
			: vertex_component1<Vec3>(_Pos)
			{
			}
	};

	struct vertex_pos3_weight
		: public vertex_component2<Vec3, float>
	{
		vertex_pos3_weight(const Vec3& _Pos = Vec3(0.0f), float _Weight = 0.0f)
			: vertex_component2<Vec3, float>(_Pos, _Weight)
			{
			}
	};

	/*
	Position: x, y, z
	Normal:   x, y, z
	Uv:		  x, y
	*/
	struct vertex_pos3_norm3_uv2 : public vertex_component3<Vec3, Vec3, Vec2>
	{
		vertex_pos3_norm3_uv2()
			{	// empty constructor
			}

		vertex_pos3_norm3_uv2(const Vec3& _Pos, const Vec3& _Norm, const Vec2& _Uv)
			: vertex_component3<Vec3, Vec3, Vec2>(_Pos, _Norm, _Uv)
			{
			}
	};
	using vertex3d = vertex_pos3_norm3_uv2;


	/*
	Position: x, y, z
	Normal:   x, y, z
	TexCoord: x, y, z
	*/
	struct vertex_pos3_norm3_uv3 : public vertex_component3<Vec3, Vec3, Vec3> 
	{
		vertex_pos3_norm3_uv3()
			{	// empty constructor
			}

		vertex_pos3_norm3_uv3(const Vec3& _Pos, const Vec3& _Norm, const Vec3& _Uv)
			: vertex_component3<Vec3, Vec3, Vec3>(_Pos, _Norm, _Uv)
			{
			}
	};
	using vertex_skybox = vertex_pos3_norm3_uv3;


	/*
	Position:  x, y, z
	Normal:    x, y, z
	TexCoord:  x, y
	TextureId: id
	*/
	struct vertex_pos3_norm3_uv3_id1 : public vertex_component4< Vec3, Vec3, Vec2, variable32bit> 
	{
		vertex_pos3_norm3_uv3_id1()
			{	// empty constructor
			}

		vertex_pos3_norm3_uv3_id1(const Vec3& _Pos, const Vec3& _Norm, const Vec2& _Uv, int _Texid)
			: vertex_component4< Vec3, Vec3, Vec2, variable32bit>(_Pos, _Norm, _Uv, static_cast<float>(_Texid))
			{
			}
	};
	using vertex_terrain = vertex_pos3_norm3_uv3_id1;

	/*
	Position: x, y
	Texcoord: x, y
	*/
	struct vertex_pos2_uv2 : public vertex_component2<Vec2, Vec2> 
	{
		vertex_pos2_uv2() 
			{	// empty constructor
			}

		vertex_pos2_uv2(const Vec2& _Pos, const Vec2& _Uv) 
			: vertex_component2<Vec2, Vec2>(_Pos, _Uv)
			{
			}
	};
	using vertex2d = vertex_pos2_uv2;

}// namespace clmagic 

#include "vertex.inl.h"

#endif