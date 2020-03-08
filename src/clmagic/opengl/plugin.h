#pragma once
#ifndef _CLMAGIC_OPENGL_PLUGIN_H_
#define _CLMAGIC_OPENGL_PLUGIN_H_

#ifndef CLMAGIC_CORE_H_

#include "basic/basic.h"
#include "geometry/geometry.h"
#include "plugin/imgproc/improc.h"
#include "plugin/strproc/toolstring.h"

namespace clmagic
{
	enum class GPUBUF_TYPE
	{
		VBO,// vertex buffer, default
		EBO,// element buffer
		TBO,// texture buffer
		FBO // feedback buffer
	};

	enum class GPUBUF_USAGE
	{
		STATIC_DRAW,// default
		DYNAMIC_DRAW
	};

	enum class PRIMITIVE
	{
		POINTS,// default
		LINES,
		LINE_LOOP,
		LINE_STRIP,
		TRIANGLES,
		TRIANGLE_STRIP,
		TRIANGLE_FAN,
		QUADS,
		QUAD_STRIP,
		POLYGON
	};

	enum class TEXTURE_WRAP
	{
		REPEAT,// default
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER
	};

	enum class TEXTURE_FILTER
	{
		LINEAR,// default
		NEAREST
	};

	enum class UNIFORM_TYPE
	{
		UF_UNKNOWN,// default
		UF_BOOL,
		UF_INT32,
		UF_UINT32,
		UF_FLOAT,
		UF_DOUBLE,
		UF_SAMPLER2D,
		UF_VEC2F,
		UF_VEC2I,
		UF_VEC3F,
		UF_VEC3I,
		UF_VEC4F,
		UF_VEC4I,
		UF_mat2,
		UF_mat2X3,
		UF_mat2X4,
		UF_mat3,
		UF_mat3X2,
		UF_mat3X4,
		UF_mat4,
		UF_mat4X2,
		UF_mat4X3
	};

	struct blend_param
	{
		size_t op;
		size_t src;
		size_t dst;
	};

	struct ft_texture
	{
		size_t id;
		float  left;
		float  top;
		float  width;
		float  height;
		int  advance[2];
	};

}// namespace clmagic

#else
#include <../core/core.h>

#endif //#ifndef CLMAGIC_CORE_H_


#endif //#ifndef _CLMAGIC_OPENGL_PLUGIN_H_