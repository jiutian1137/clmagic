#pragma once
#ifndef _CLMAGIC_OPENGL_OPENGL_FUNCTION_H_
#define _CLMAGIC_OPENGL_OPENGL_FUNCTION_H_
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <ft2build.h>
#include <freetype/freetype.h>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"

#include "plugin.h"

namespace clmagic
{
	constexpr GLuint OPENGL_INVALIDE_UINT = -1;
	constexpr GLint OPENGL_INVALIDE_INT = -1;

	// <kernel>
	void gl_draw(GLuint _Vao, GLuint _Vbo, GLuint _Ebo, GLenum _Primtive, GLsizei _Off, GLsizei _Count);
	// </kernel>

	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	// <primitive>
	GLenum gl_convert_primitive(PRIMITIVE _Type);
	PRIMITIVE gl_convert_primitive(GLenum _Type);
	// </primitive>

	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	// <uniform>
	UNIFORM_TYPE gl_convert_uniform_type(const _STD string& _Type);
	_STD string gl_convert_uniform_type(UNIFORM_TYPE _Type);

	void gl_set_uniform(GLint _Location, UNIFORM_TYPE _Type, void* _Data);
	void gl_set_uniform(GLint _Location, int _Val);
	void gl_set_uniform(GLint _Location, unsigned int _Val);
	void gl_set_uniform(GLint _Location, float _Val);
	void gl_set_uniform(GLint _Location, double _Val);
	void gl_set_uniform(GLint _Location, const Vec2& _Data);
	void gl_set_uniform(GLint _Location, const Vec3& _Data);
	void gl_set_uniform(GLint _Location, const Vec4& _Data);
	void gl_set_uniform(GLint _Location, const Vec2i& _Data);
	void gl_set_uniform(GLint _Location, const Vec3i& _Data);
	void gl_set_uniform(GLint _Location, const Vec4i& _Data);
	void gl_set_uniform(GLint _Location, const Mat2& _Data);
	void gl_set_uniform(GLint _Location, const Mat3& _Data);
	void gl_set_uniform(GLint _Location, const Mat4& _Data);
	GLint gl_find_uniform_location(GLuint _Program, const char* _Uname);
	// </uniform>

	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	// <gpubuf>
	GLenum gl_convert_gpubuf_type(GPUBUF_TYPE _Type);
	GPUBUF_TYPE gl_convert_gpubuf_type(GLenum _Type);
	GLenum gl_convert_gpubuf_usage(GPUBUF_USAGE _Usage);
	GPUBUF_USAGE gl_convert_gpubuf_usage(GLenum _Usage);

	void gl_create_gpubuf(GLuint& _Dst, GLenum _Target, GLenum _Usage, void* _Data, GLsizeiptr _Size, GLsizeiptr _Capcity);
	void gl_create_renderbuffer(GLuint& _Dst, size_t _Width, size_t _Height);
	void gl_create_framebuffer(GLuint& _Dst, size_t _Width, size_t _Height, const _STD vector<GLuint>& _Colortex);
	void gl_memcpy_gpubuf(GLuint _Dst, GLuint _Src, GLsizeiptr _Size);
	void gl_delete_gpubuf(GLuint& _Buffer);
	void gl_delete_renderbuffer(GLuint& _Rbo);
	void gl_delete_framebuffer(GLuint& _Fbo);
	// </gpubuf>

	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	// <vertex attrib>
	GLenum gl_convert_base_type(BASE_TYPE _Type);
	BASE_TYPE gl_convert_base_type(GLenum _Type);
	void gl_set_current_vertex_object(GLuint _Index, GLint _Size, GLenum _Type, GLboolean _Isnomal, GLsizei _Stride, GLvoid* _Off);

	void gl_delete_vertex_array_object(GLuint& _Array);
	void gl_create_vertex_array_object(GLuint& _Dst, const vertex_descriptor& _Vdesc, GLuint _Vbo);
	// </vertex attrib>

	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	// <blend>
	GLenum gl_convert_blend_operator(blender::_Operator _Op);
	GLenum gl_convert_blend_factorfunc(blender::_Factorfunc _Func);
	blender::_Operator gl_convert_blend_operator(GLenum _Op);
	blender::_Factorfunc gl_convert_blend_factorfunc(GLenum _Func);
	bool gl_exist_blend_operator(GLenum _Op);
	bool gl_exist_blend_factorfunc(GLenum _Func);

	// <inline>
	blend_param gl_convert_blender(const blender& _Blender);
	blender gl_convert_blender(const blend_param& _Blender);
	void gl_set_blend(const blender& _Blender);
	void gl_set_blend(const blend_param& _Fblend);
	void gl_enable_blend(const blender& _Blender);
	void gl_enable_blend(const blend_param& _Fblend);
	void gl_disable_blend();
	// </inline>
	// </blend>

	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	// <texture>
	GLenum gl_convert_texture_wrap(TEXTURE_WRAP _Wrap);
	GLenum gl_convert_texture_filter(TEXTURE_FILTER _Filter);
	void gl_create_texture(GLuint& _Dst, void* _Data, size_t _Width, size_t _Height, GLenum _Internalformat, GLenum _Format, GLenum _Xwrap, GLenum _Ywrap, GLenum _Fltmag, GLenum _Fltmin, _STD initializer_list<GLint> _Swizzles = {});
	void gl_create_ft_texture(ft_texture& _Dst, int _Charactor, const _STD string& _Ftfamily, size_t _Ftsize, bool _Isdeostrylib = true);

	// <inline>
	void gl_delete_texture(GLuint& _Dst);
	void gl_create_texture(GLuint& _Dst, void* _Data, size_t _Width, size_t _Height, size_t _Channels, GLenum _Xwrap, GLenum _Ywrap, GLenum _Fltmag, GLenum _Fltmin, _STD initializer_list<GLint> _Swizzles = {});
	void gl_create_texture(GLuint& _Dst, const cv::Mat& _Src, GLenum _Xwrap, GLenum _Ywrap, GLenum _Fltmag, GLenum _Fltmin);
	void gl_create_texture_dds(GLuint& _Dst, const std::string& _Filename, /* void* _Data, size_t _Width, size_t _Height, unsigned long In_Compressformat, */GLenum _Xwrap, GLenum _Ywrap, GLenum _Fltmag, GLenum _Fltmin);
	// </inline>
	// </texture>

	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	void gl_read_frame(cv::OutputArray _Dst, Vec2i _Winsize);

}// namespace clmagic

#endif