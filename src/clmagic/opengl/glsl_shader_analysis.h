#pragma once
#ifndef _CLMAGIC_CORE_SHADER_INTERPRETER_H_
#define _CLMAGIC_CORE_SHADER_INTERPRETER_H_
#include <map>
#include <vector>
#include <fstream>
#include <sstream>

#include "plugin.h"


namespace clmagic
{
	struct code_var 
	{
		/*
		uniform float          t  =  1.0;
		| desc  type  nmspace name  value|
		*/
		std::vector<std::string> desc;
		std::string type;
		std::string nmspace;
		std::string name;
		std::string value;
	};

	struct code_func : public code_var
	{
		/*
		        void           intersect(vec3 v0, ..., vec3 direction, out vec3 point)
		| desc  type  nmspace    name    args, value = ""|
		*/
		std::vector<code_var> args;
	};


	struct shader_property 
	{
		std::string _Myversion;
		std::map<std::string, code_var> _Myvar;
		std::map<std::string, code_func> _Myfunc;
	};


	void shader_analysis(const std::string& _Filename, shader_property& _Dst);

	/*
	@_example:
		clmagic::glslshader_interpreter shader;
		shader.push_const("const int MAX_TEXTURE_COUNT = 2;");
		shader.push_uniform("uniform materialProperties matERIAL[MAX_TEXTURE_COUNT];");
		shader.push_uniform("uniform mat4 PROJECTION_matRIX;");
		shader.push_uniform("uniform mat4 VIEW_matRIX;");
		shader.push_uniform("uniform mat4 WORLD_matRIX;");
		shader.push_inblock("in vec4 in_Position;");
		shader.push_inblock("in vec3 in_Normal;");
		shader.push_inblock("in vec2 in_Texcoord;");
		shader.push_struct("struct materialProperties\n"
							"{\n"
							"int type;\n"
							"vec3 \t\t\tambient;\n"
							"vec3 diffuse;\n"
							"vec3 specular;\n"
							"vec3 emission;\n"
							"float strength;\n"
							"};\n");
		shader.push_function("vec3 PropertiesColor()\n"
							"{\n"
							"return (matERIAL.ambient + matERIAL.diffuse) * 0.5;\n"
							"}\n");


		clmagic::glslshader_interpreter _TestTarget;
		clmagic::readShaderOnlyUniform(_TestTarget, "deferredshader.frag" );
		_TestTarget.check_struct();
		_TestTarget.check_uniform();
		auto _Uniformlist = _TestTarget.get_uniform_list();
	*/
	class glslshader_interpreter
	{
	public:
		using variant = std::pair<std::string, std::string>;

		struct Function
		{
			std::string m_Return;
			std::string m_Name;
			std::vector<variant> m_Arguments;
			std::string m_Body;
		};

		constexpr static const char* Keyword_In = "in";
		constexpr static const char* Keyword_Out = "out";
		constexpr static const char* Keyword_Const = "const";
		constexpr static const char* Keyword_Struct = "struct";
		constexpr static const char* Keyword_Uniform = "uniform";
		constexpr static const char* Keyword_Main = "main";
		

		constexpr static const char* Keyword_Int = "int";
		constexpr static const char* Keyword_Uint = "uint";
		constexpr static const char* Keyword_Float = "float";
		constexpr static const char* Keyword_Double = "double";
		constexpr static const char* Keyword_Void = "void";
		constexpr static const char* Keyword_Vec2 = "vec2";
		constexpr static const char* Keyword_Vec3 = "vec3";
		constexpr static const char* Keyword_Vec4 = "vec4";
		constexpr static const char* Keyword_mat2 = "mat2";
		constexpr static const char* Keyword_mat3 = "mat3";
		constexpr static const char* Keyword_mat4 = "mat4";
		constexpr static const char* Keyword_mat3x4 = "mat3x4";
		


		variant variant_cast(const std::string& _Chunk);

		inline std::string variant_cast(const variant& _Chunk)
			{
			return (_Chunk.first + ' ' + _Chunk.second + ';');
			}

		/*
		@_Chunk: uniform vec3 LightPosition;
				 uniform int materialType[10];
		*/
		void push_uniform(const std::string& _Chunk);
		void check_uniform();

		/*
		@_Chunk: in vec3 in_Position;
				 in int in_Type;
		*/
		void push_inblock(const std::string& _Chunk);

		/*
		@_Chunk: out vec3 out_Color;
				 out int out_Result;
		*/
		void push_outblock(const std::string& _Chunk);

		/*
		@_Chunk: struct materialProperties{ int type; vec3 color; }
				 struct lightproperties{ int type; vec3 ambient; }
		*/
		void push_struct(const std::string& _Chunk);
		void check_struct();

		/*
		@_Chunk: const int MAX_TEXTURE_COUNT = 10;
				 const int WHITE_COLOR = vec3(1.0);
		*/
		void push_const(const std::string& _Chunk);
		void check_const();

		/*
		@_Chunk: type funName(type argName){ body }
				 type funName(type argName, type argName1, type argName2){ body }
		*/
		void push_function(const std::string& _Chunk);
		void check_function();

		std::map<variant, int> get_uniform_list();

		inline void clear() 
			{
			m_MainFunction = Function();
			m_Functions.clear();
			m_OutBlock.clear();
			m_InBlock.clear();
			m_Uniforms.clear();
			m_Structs.clear();
			m_Consts.clear();
			}

	private:
		std::map<std::string, variable32bit2> m_Consts;
		std::map<std::string, std::vector<variant>> m_Structs;
		std::vector<variant> m_Uniforms;
		std::vector<variant> m_InBlock;
		std::vector<variant> m_OutBlock;

		std::vector<Function> m_Functions;
		Function m_MainFunction;
	};


	void
	read(
		const std::string& In_Filename,
		std::string&	   Out_Dst
		);

	void
	read(
		const std::string&		In_Shaderfile,
		glslshader_interpreter& Out_Dst
		);

	void
	read_uniform_only(
		const std::string&		In_Shaderfile,
		glslshader_interpreter& Out_Dst
		);

}// namespace clmagic


#endif
