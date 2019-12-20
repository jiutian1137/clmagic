#pragma once
#ifndef __CLMAGIC_CORE_OPENGL___GLSL___H__
#define __CLMAGIC_CORE_OPENGL___GLSL___H__
#include "clmagic_basic.h"
#include "clmagic_math.h"
#include <string>
#include <vector>
#include <algorithm>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"

namespace clmagic
{
	inline void gl_delete_shader(GLuint& _Shader) {
		if (glIsShader(_Shader)) {
			glDeleteShader(_Shader);
		}
		_Shader = -1;
	}

	inline GLuint gl_compile_shader(GLenum _Type, const char* _Source, size_t _Length) {
		GLuint _Result = glCreateShader(_Type);
		glShaderSource(_Result, 1, &_Source, 0);
		glCompileShader(_Result);
		return (_Result);
	}

	inline bool gl_shader_compile_status(GLuint _Shader) {
		GLint _Result = GL_FALSE;
		glGetShaderiv(_Shader, GL_COMPILE_STATUS, &_Result);
		return (_Result == GL_FALSE ? false : true);
	}

	inline std::string gl_shader_infolog(GLuint _Shader) {
		std::string _Result;
		GLsizei _Errsize = 0;

		glGetShaderiv(_Shader, GL_INFO_LOG_LENGTH, &_Errsize);
		_Result.resize(static_cast<size_t>(_Errsize));
		glGetShaderInfoLog(_Shader, _Errsize, &_Errsize, (GLchar*)(_Result.data()));
		return (_Result);
	}

	inline void gl_delete_program(GLuint& _Shaderprogram) {
		if (glIsProgram(_Shaderprogram)) {
			glDeleteProgram(_Shaderprogram);
		}
		_Shaderprogram = -1;
	}

	inline bool gl_program_link_status(GLuint _Shaderprogram) {
		GLint _Result = GL_FALSE;
		glGetProgramiv(_Shaderprogram, GL_LINK_STATUS, &_Result);
		return (_Result == GL_FALSE ? false : true);
	}

	inline std::string gl_program_infolog(GLuint _Shaderprogram) {
		std::string _Result;
		GLint _Errsize = 0;

		glGetProgramiv(_Shaderprogram, GL_INFO_LOG_LENGTH, &_Errsize);
		_Result.resize(static_cast<size_t>(_Errsize));
		glGetProgramInfoLog(_Shaderprogram, _Errsize, &_Errsize, (GLchar*)(_Result.data()));
		return (_Result);
	}

	inline std::vector<GLuint> gl_program_attached_shaders(GLuint _Shaderprogram) {
		const GLint _Maxcount = 10;
		GLuint _Shaders[_Maxcount];
		GLsizei _Size = 0;
		glGetAttachedShaders(_Shaderprogram, _Maxcount, &_Size, _Shaders);
		return ( std::vector<GLuint>(_Shaders, _Shaders + _Size) );
	}

	inline GLuint gl_create_shader(GLenum _Type, const char* _Source, size_t _Length) {
		GLuint _Result = gl_compile_shader(_Type, _Source, _Length);
		if (gl_shader_compile_status(_Result) == GL_FALSE) {// compile failure
			logcout << std_mes("error", gl_shader_infolog(_Result), "gl_create_shader(GLenum, const char*, size_t)", __FILE__, __LINE__);
			gl_delete_shader(_Result);
		}
		return (_Result);
	}

	inline GLuint gl_create_program(GLuint* _Shaders, size_t _Size) {
		GLuint _Program = glCreateProgram();
		while (_Size--) {
			glAttachShader(_Program, _Shaders[_Size]);
		}

		glLinkProgram(_Program);
		if (gl_program_link_status(_Program) == GL_FALSE) {
			logcout << std_mes("error", gl_program_infolog(_Program), "GLuint gl_create_program(GLuint*, size_t)", __FILE__, __LINE__);
			gl_delete_program(_Program);
		}
		return ( _Program );
	}

	inline GLuint gl_relink_program(GLuint& _Program, GLuint* _New_shaders, GLuint _Size) {
		auto _Old_shaders = gl_program_attached_shaders(_Program);
		gl_delete_program(_Program);

		auto _Shaders = std::vector<GLuint>(_Old_shaders.size() + _Size);
		auto _ResIt   = std::set_union(
			_Old_shaders.begin(), _Old_shaders.end(), _New_shaders, _New_shaders + _Size,
			_Shaders.begin());
		_Shaders.erase(_ResIt, _Shaders.end());
		return ( gl_create_program(_Shaders.data(), _Shaders.size()) );
	}

	//inline bool relinkShaders(GLuint& shaderProgram, glsl_shader_info& shader)
	//{
	//	// glGetProgramiv(shaderProgram, GL_ATTACHED_SHADERS, &shaderCount);	// also select
	//	// ...

	//	// 1. get invariant shaders
	//	const GLint maxCount = 6;
	//	GLint count = 0;
	//	GLuint shaders[maxCount];
	//	glGetAttachedShaders(shaderProgram, maxCount, &count, shaders);

	//	std::vector<GLuint> invariantShaders(count - 1);
	//	for (size_t i = 0, ct = 0; i != count; ++i)
	//	{
	//		if (shaders[i] != shader.identity)
	//		{
	//			invariantShaders[ct++] = shaders[i];
	//		}
	//	}

	//	// 2. reload shader
	//	shdread(shader);

	//	// 3. recreate program
	//	glDeleteProgram(shaderProgram);
	//	shaderProgram = glCreateProgram();

	//	// 4. reattach shader to program
	//	for (size_t i = 0; i != invariantShaders.size(); ++i)
	//	{
	//		glAttachShader(shaderProgram, invariantShaders[i]);
	//	}
	//	glAttachShader(shaderProgram, shader.identity);


	//	// 5. relink program
	//	glLinkProgram(shaderProgram);

	//	_Shader_linker _Linker(std::vector<GLuint>(), shaderProgram);
	//	if (_Linker.check())
	//	{
	//		return (true);
	//	}
	//	else
	//	{
	//		std::cout << _Linker.get_error() << std::endl;
	//		glDeleteProgram(_Linker._Myprog);
	//		return (false);
	//	}
	//}



	///*
	//@gpu_id: @glCreateShader()
	//*/
	//struct shader_info
	//{
	//	size_t _Mytype;// shader type
	//	fileproxy _Mysource;// shader file source
	//	size_t _Myid;// graphics interface identifier

	//	static shader_info from_file(size_t Type, std::string _Filename) {
	//		return (shader_info{ Type, _Filename });
	//	}

	//	static shader_info from_str(size_t Type, std::string _Source) {
	//		return (shader_info{ Type, "",  _Source });
	//	}
	//};

//#if defined(_CLMAGIC_CORE_H_)
//#define PLATFORM_SHADER virtual
//#else
//#define PLATFORM_SHADER
//#endif
//
//	struct _Shader_properties_glsl
//	{
//		int num_texture;
//		int num_lightsource;
//	};
//
//	class glsl_shader
//	{
//	public:
//		glsl_shader();
//		~glsl_shader();
//
//		template<typename _Function>
//		void _Gpu_operation(_Function _Func);
//
//		PLATFORM_SHADER bool operator< (const glsl_shader& _Right) const;
//		PLATFORM_SHADER bool operator> (const glsl_shader& _Right) const;
//		PLATFORM_SHADER bool operator== (const glsl_shader& _Right) const;
//		PLATFORM_SHADER bool operator!= (const glsl_shader& _Right) const;
//
//		PLATFORM_SHADER void init(std::initializer_list<shader_info> _Shaderinfos);
//		PLATFORM_SHADER void link();
//		PLATFORM_SHADER void use();
//		PLATFORM_SHADER void close();
//		PLATFORM_SHADER void set_shader(shader_info _Shaderinfo, bool _Islink = false);
//		PLATFORM_SHADER void set_shader(size_t _Type, const std::string& _Filename, bool _Islink = false);
//
//		PLATFORM_SHADER void uniform_set(const std::string& _Uname, int _Data);
//		PLATFORM_SHADER void uniform_set(const std::string& _Uname, float _Data);
//		PLATFORM_SHADER void uniform_set(const std::string& _Uname, const Vec2& _Data);
//		PLATFORM_SHADER void uniform_set(const std::string& _Uname, const Vec3& _Data);
//		PLATFORM_SHADER void uniform_set(const std::string& _Uname, const Vec4& _Data);
//		PLATFORM_SHADER void uniform_set(const std::string& _Uname, const Mat2& _Data);
//		PLATFORM_SHADER void uniform_set(const std::string& _Uname, const Mat4& _Data);
//		PLATFORM_SHADER void uniform_set(const std::string& _Uname, const Mat3x4& _Data);
//
//		PLATFORM_SHADER shader_info& find_shader(GLenum _Type);
//		PLATFORM_SHADER shader_info& vertex_shader();
//		PLATFORM_SHADER shader_info& pixel_shader();
//		PLATFORM_SHADER shader_info& tesc_shader();
//		PLATFORM_SHADER shader_info& tese_shader();
//		PLATFORM_SHADER shader_info& geometry_shader();
//		PLATFORM_SHADER shader_info& compute_shader();
//
//		PLATFORM_SHADER int num_lightsource() const;
//		PLATFORM_SHADER int num_texture() const;
//
//		PLATFORM_SHADER size_t id() const;
//
//	protected:
//		size_t _Myprog;
//		std::map<size_t, shader_info> _Myglsls;
//
//		_Shader_properties_glsl _Myprop;
//		bool _My_is_using;
//	};


}// namespace clmagic

#endif
