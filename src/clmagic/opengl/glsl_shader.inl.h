#pragma once
#ifndef _CLMAGIC_OPENGL_SHADER_INLINE_H_
#define _CLMAGIC_OPENGL_SHADER_INLINE_H_
#include "glsl_shader.h"

namespace clmagic 
{
	inline glsl_shader::glsl_shader()
		: _Myprog(-1), _Myglsls(),
		_Myprop(), _My_is_using(false)
		{
		}

	inline glsl_shader::~glsl_shader()
		{
		for (auto _Where = _Myglsls.begin(); _Where != _Myglsls.end(); ++_Where) 
			{	// delete shader
			glDeleteShader(_Where->second.gpu_id);
			}
		glDeleteProgram(_Myprog); // delete program
		}

	template<typename _Function> inline 
		void
		glsl_shader::_Gpu_operation(_Function _Func)
		{
		if (_My_is_using)
			{
			_Func();
			}
		else
			{
			use();
			_Func();
			close();
			}
		}

	inline bool 
		glsl_shader::operator<(const glsl_shader& _Right) const 
		{
		return (_Myprog < _Right._Myprog); 
		}

	inline bool
		glsl_shader::operator>(const glsl_shader& _Right) const 
		{
		return (_Myprog > _Right._Myprog); 
		}

	inline bool
		glsl_shader::operator==(const glsl_shader& _Right) const 
		{
		return (_Myprog == _Right._Myprog);
		}

	inline bool
		glsl_shader::operator!=(const glsl_shader& _Right) const 
		{
		return (_Myprog != _Right._Myprog); 
		}

	inline void 
		glsl_shader::init(std::initializer_list<shader_info> _Shaderinfos)
		{
		std::for_each(_Shaderinfos.begin(), _Shaderinfos.end(), 
			[&](const shader_info& _Sinfo) { this->set_shader(_Sinfo); }
			);
		this->link();
		}

	inline void 
		glsl_shader::link()
		{
		std::vector<GLuint> _Shaders;

		for (auto _Where = _Myglsls.begin(); _Where != _Myglsls.end(); ++_Where)
			{
			_Shaders.push_back( _Where->second.gpu_id );
			}

		_Myprog = shdlink(_Shaders);
		}

	inline void 
		glsl_shader::use()
		{
		glUseProgram(_Myprog);
		_My_is_using = true;
		}

	inline void 
		glsl_shader::close()
		{
		glUseProgram(0);
		_My_is_using = false;
		}

	inline void
		glsl_shader::set_shader(shader_info _Shaderinfo, bool _Islink)
		{
		// 1. correct shader source
		if (_Shaderinfo.source.empty()) 
			{	
			_Shaderinfo.source = text_from_file(_Shaderinfo.filename.c_str());
			}

		// 2. read shader
		auto _Where = _Myglsls.find(_Shaderinfo.type);
		_Shaderinfo.gpu_id = shdread(
			glsl_shader_info{ GLenum(_Shaderinfo.type),
							  _Where != _Myglsls.end() ? GLuint(_Where->second.gpu_id) : GLuint(-1),
							 _Shaderinfo.source }
			);

		// 3. store to memory
		_Myglsls[_Shaderinfo.type] = _Shaderinfo;

		// 4. is link
		if (_Islink)
			{
			this->link();
			}
		}

	inline void
		glsl_shader::set_shader(size_t _Type, const std::string& _Filename, bool _Islink)
		{
		auto _Shaderinfo = shader_info::from_file(_Type, _Filename);
		set_shader(_Shaderinfo, _Islink);
		}

	inline void
		glsl_shader::uniform_set(const std::string& _Uname, float _Data)
		{
		_Gpu_operation([&]() { glUniform1f(glGetUniformLocation(_Myprog, _Uname.c_str()), _Data); });
		}

	inline void
		glsl_shader::uniform_set(const std::string& _Uname, int _Data)
		{
		_Gpu_operation([&]() { glUniform1i(glGetUniformLocation(_Myprog, _Uname.c_str()), _Data); });
		}

	inline void
		glsl_shader::uniform_set(const std::string& _Uname, const Vec2& _Data)
		{
		_Gpu_operation([&]() { glUniform2fv(glGetUniformLocation(_Myprog, _Uname.c_str()), 1, static_cast<const float*>(_Data)); });
		}

	inline void 
		glsl_shader::uniform_set(const std::string& _Uname, const Vec3& _Data)
		{
		_Gpu_operation([&]() { glUniform3fv(glGetUniformLocation(_Myprog, _Uname.c_str()), 1, static_cast<const float*>(_Data)); });
		}

	inline void
		glsl_shader::uniform_set(const std::string& _Uname, const Vec4& _Data)
		{
		_Gpu_operation([&]() { glUniform4fv(glGetUniformLocation(_Myprog, _Uname.c_str()), 1, static_cast<const float*>(_Data)); });
		}

	inline void
		glsl_shader::uniform_set(const std::string& _Uname, const mat2& _Data)
		{
		_Gpu_operation([&]() { glUniformmatrix2fv(glGetUniformLocation(_Myprog, _Uname.c_str()), 1, GL_FALSE, static_cast<const float*>(_Data)); });
		}

	inline void
		glsl_shader::uniform_set(const std::string& _Uname, const mat4& _Data)
		{
		_Gpu_operation([&]() { glUniformmatrix4fv(glGetUniformLocation(_Myprog, _Uname.c_str()), 1, GL_FALSE, static_cast<const float*>(_Data)); });
		}

	inline void 
		glsl_shader::uniform_set(const std::string& _Uname, const mat3x4& _Data)
		{
		_Gpu_operation([&]() { glUniformmatrix3x4fv(glGetUniformLocation(_Myprog, _Uname.c_str()), 1, GL_FALSE, static_cast<const float*>(_Data)); });
		}

	inline shader_info&
		glsl_shader::find_shader(GLenum _Type)
		{ return ( _Myglsls.find(_Type)->second ); }

	inline shader_info&
		glsl_shader::vertex_shader() 
		{ return (find_shader(GL_VERTEX_SHADER)); }

	inline shader_info&
		glsl_shader::pixel_shader() 
		{ return (find_shader(GL_FRAGMENT_SHADER)); }
	
	inline shader_info&
		glsl_shader::tesc_shader() 
		{ return (find_shader(GL_TESS_CONTROL_SHADER)); }

	inline shader_info&
		glsl_shader::tese_shader() 
		{ return (find_shader(GL_TESS_EVALUATION_SHADER)); }

	inline shader_info&
		glsl_shader::geometry_shader() 
		{ return (find_shader(GL_GEOMETRY_SHADER)); }

	inline shader_info&
		glsl_shader::compute_shader() 
		{ return (find_shader(GL_COMPUTE_SHADER)); }

	inline int
		glsl_shader::num_lightsource() const
		{ return (_Myprop.num_lightsource); }

	inline int 
		glsl_shader::num_texture() const 
		{ return (_Myprop.num_texture); }

	inline GLuint
		glsl_shader::id() const 
		{ return (_Myprog); }
}// namespace clmagic 

#endif