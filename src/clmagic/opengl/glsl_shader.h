#pragma once
#ifndef _CLMAGIC_OPENGL_SHADER_H_
#define _CLMAGIC_OPENGL_SHADER_H_
#include <set>
#include <map>
#include <fstream>

#include "glsl_shader_read.h"
#include "glsl_shader_analysis.h"


namespace clmagic 
{
	///*
	//@: glslshader
	//*/
	//class glslshader
	//{
	//public:
	//	glslshader();
	//	~glslshader();

	//	//------------------------------- parent function -----------------------------------
	//	bool operator<(const glslshader& _Right) const;
	//	bool operator>(const glslshader& _Right) const;
	//	bool operator==(const glslshader& _Right) const;
	//	bool operator!=(const glslshader& _Right) const;

	//	void init(const std::vector<shd_info>& _Shaderinfos);
	//	void use();
	//	void close() { glUseProgram(0); }

	//	void set_uniform_data(const std::string& _Uname, void* _Data, size_t _Size);
	//	void set_uniform_data(const std::string& _Uname, const vec3& _Vec3);
	//	void set_uniform_data(const std::string& _Uname, const vec4& _Vec4);
	//	void set_uniform_data(const std::string& _Uname, const mat2& _mat2);
	//	void set_uniform_data(const std::string& _Uname, const mat4& _mat4);
	//	void set_uniform_data(const std::string& _Uname, const mat_<3, 4, float>& _mat3x4);
	//	void set_uniform_data(const uniform& _Uniform);
	//	void set_uniform_data(const visualshader& _Sender);

	//	//virtual void fill_renderchain(render_chain& _Chain) override;

	//	void setMaxLightCount(int _Newval) { _Set_max_light_count(_Newval, m_MaxTextureCount, _Myprog, _Myfsh); }
	//	void setMaxTextureCount(int _Newval) { _Set_max_texture_count(_Newval, m_MaxTextureCount, _Myprog, _Myfsh); }

	//	int getMaxLightCount() const { return (0); }
	//	int getMaxTextureCount() const { return (m_MaxTextureCount); }

	//	unsigned int _Get_program() const { return (_Myprog); }

	//protected:
	//	void _Load_content_from_file(
	//		std::vector<std::string>& _Destination,
	//		const std::string& _Filename
	//		);

	//	void _Frush_uniform_locations(
	//		const std::vector<std::string>& _Shaderfiles
	//		);

	//	void _Set_shader_file_variable(
	//		const std::string& _NewVar,
	//		const std::string& _FilePath,
	//		const std::vector<std::string>& _Condition,
	//		bool _IsEach = true
	//		);

	//	void _Set_max_light_count(
	//		int _Newval,
	//		int& _Currentval,
	//		unsigned int& _Prog,
	//		shaderinfo&   _Fragshader
	//		);

	//	void _Set_max_texture_count(
	//		int _Newval,
	//		int& _Currentval,
	//		unsigned int& _Prog,
	//		shaderinfo& _Fragshader
	//		);

	//	void _Correction_sampler();

	//	//-----------------------------------------------------------------------------------

	//	unsigned int _Myprog;
	//	int m_TextureCount;
	//	int m_MaxTextureCount;
	//};



	
	
}// namespace clmagic 

#endif
