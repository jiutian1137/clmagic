#include "opengl_util.h"

namespace clmagic 
{
	void gl_draw(GLuint _Vao, GLuint _Vbo, GLuint _Ebo, GLenum _Primtive, GLsizei _Off, GLsizei _Count)
		{
		// 2. bind
		glBindVertexArray(_Vao);
		glBindBuffer(GL_ARRAY_BUFFER, _Vbo);

		// 3. draw
		if (_Ebo != OPENGL_INVALIDE_INT)
			{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _Ebo);
			glDrawElements( _Primtive, _Count - _Off, GL_UNSIGNED_INT, nullptr);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
		else
			{
			glDrawArrays(_Primtive, _Off, _Count );
			}// -3.

		// -2. unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		}

	// <primitive>
	GLenum gl_convert_primitive(PRIMITIVE _Prim)
		{
		switch (_Prim)
		{
		case PRIMITIVE::POINTS:		 return (GL_POINTS);
		case PRIMITIVE::LINES:		 return (GL_LINES);
		case PRIMITIVE::LINE_LOOP:	 return (GL_LINE_LOOP);
		case PRIMITIVE::LINE_STRIP:	 return (GL_LINE_STRIP);
		case PRIMITIVE::TRIANGLES:		return (GL_TRIANGLES);
		case PRIMITIVE::TRIANGLE_STRIP: return (GL_TRIANGLE_STRIP);
		case PRIMITIVE::TRIANGLE_FAN:   return (GL_TRIANGLE_FAN);
		case PRIMITIVE::QUADS:			return (GL_QUADS);
		case PRIMITIVE::QUAD_STRIP:	    return (GL_QUAD_STRIP);
		case PRIMITIVE::POLYGON:	    return (GL_POLYGON);
		default: return (GL_POINTS);
		}
	}

	PRIMITIVE gl_convert_primitive(GLenum _Prim)
		{
		switch (_Prim)
			{
			case GL_POINTS:			return (PRIMITIVE::POINTS);
			case GL_LINES:			return (PRIMITIVE::LINES);
			case GL_LINE_LOOP:		return (PRIMITIVE::LINE_LOOP);
			case GL_LINE_STRIP:		return (PRIMITIVE::LINE_STRIP);
			case GL_TRIANGLES:		return (PRIMITIVE::TRIANGLES);
			case GL_TRIANGLE_STRIP: return (PRIMITIVE::TRIANGLE_STRIP);
			case GL_TRIANGLE_FAN:	return (PRIMITIVE::TRIANGLE_FAN);
			case GL_QUADS:			return (PRIMITIVE::QUADS);
			case GL_QUAD_STRIP:		return (PRIMITIVE::QUAD_STRIP);
			case GL_POLYGON:		return (PRIMITIVE::POLYGON);
			default: return (PRIMITIVE::POINTS);
			}
		}
	// </primitive>

	// <uniform>
	UNIFORM_TYPE gl_convert_uniform_type(const std::string& _Type)
		{
		if (_Type == "vec2")
			return (UNIFORM_TYPE::UF_VEC2F);
		else if (_Type == "vec3")
			return (UNIFORM_TYPE::UF_VEC3F);
		else if (_Type == "vec4")
			return (UNIFORM_TYPE::UF_VEC4F);
		else if (_Type == "mat2")
			return (UNIFORM_TYPE::UF_MAT2);
		else if (_Type == "mat3")
			return (UNIFORM_TYPE::UF_MAT3);
		else if (_Type == "mat4")
			return (UNIFORM_TYPE::UF_MAT4);
		else if (_Type == "mat3x4")
			return (UNIFORM_TYPE::UF_MAT3X4);
		else if (_Type == "bool")
			return (UNIFORM_TYPE::UF_BOOL);
		else if (_Type == "int")
			return (UNIFORM_TYPE::UF_INT32);
		else if (_Type == "uint")
			return (UNIFORM_TYPE::UF_UINT32);
		else if (_Type == "float")
			return (UNIFORM_TYPE::UF_FLOAT);
		else if (_Type == "double")
			return (UNIFORM_TYPE::UF_DOUBLE);
		else if (_Type == "sampler2D")
			return (UNIFORM_TYPE::UF_SAMPLER2D);
		else
			return (UNIFORM_TYPE::UF_UNKNOWN);
		}

	std::string gl_convert_uniform_type(UNIFORM_TYPE _Type)
		{
		switch (_Type)
			{
			case clmagic::UNIFORM_TYPE::UF_BOOL:   return std::string("bool");
			case clmagic::UNIFORM_TYPE::UF_INT32:  return std::string("int");
			case clmagic::UNIFORM_TYPE::UF_UINT32: return std::string("uint");
			case clmagic::UNIFORM_TYPE::UF_FLOAT:  return std::string("float");
			case clmagic::UNIFORM_TYPE::UF_DOUBLE: return std::string("double");
			case clmagic::UNIFORM_TYPE::UF_VEC2F:  return std::string("vec2");
			case clmagic::UNIFORM_TYPE::UF_VEC2I:  return std::string("vec2i");
			case clmagic::UNIFORM_TYPE::UF_VEC3F:  return std::string("vec3");
			case clmagic::UNIFORM_TYPE::UF_VEC3I:  return std::string("vec3i");
			case clmagic::UNIFORM_TYPE::UF_VEC4F:  return std::string("vec4");
			case clmagic::UNIFORM_TYPE::UF_VEC4I:  return std::string("vec4i");
			case clmagic::UNIFORM_TYPE::UF_MAT2:   return std::string("mat2");
			case clmagic::UNIFORM_TYPE::UF_MAT2X3: return std::string("mat2x3");
			case clmagic::UNIFORM_TYPE::UF_MAT2X4: return std::string("mat2x4");
			case clmagic::UNIFORM_TYPE::UF_MAT3:   return std::string("mat3");
			case clmagic::UNIFORM_TYPE::UF_MAT3X2: return std::string("mat3x2");
			case clmagic::UNIFORM_TYPE::UF_MAT3X4: return std::string("mat3x4");
			case clmagic::UNIFORM_TYPE::UF_MAT4:   return std::string("mat4");
			case clmagic::UNIFORM_TYPE::UF_MAT4X2: return std::string("mat4x2");
			case clmagic::UNIFORM_TYPE::UF_MAT4X3: return std::string("mat4x3");
			default: return (std::string());
			}
		}

	void gl_set_uniform(GLint _Location, UNIFORM_TYPE _Type, void* _Data)
		{
		switch (_Type)
			{
			case UNIFORM_TYPE::UF_BOOL:   glUniform1i(_Location, *(static_cast<bool*>(_Data)));
				break;
			case UNIFORM_TYPE::UF_INT32:  glUniform1i(_Location, *(static_cast<int*>(_Data)));
				break;
			case UNIFORM_TYPE::UF_UINT32: glUniform1ui(_Location, *(static_cast<unsigned int*>(_Data)));
				break;
			case UNIFORM_TYPE::UF_FLOAT:  glUniform1f(_Location, *(static_cast<float*>(_Data)));
				break;
			case UNIFORM_TYPE::UF_DOUBLE: glUniform1d(_Location, *(static_cast<double*>(_Data)));
				break;
			case UNIFORM_TYPE::UF_VEC2F:  glUniform2fv(_Location, 1, static_cast<const GLfloat*>(_Data));
				break;
			case UNIFORM_TYPE::UF_VEC2I:  glUniform2iv(_Location, 1, static_cast<const GLint*>(_Data));
				break;
			case UNIFORM_TYPE::UF_VEC3F:  glUniform3fv(_Location, 1, static_cast<const GLfloat*>(_Data));
				break;
			case UNIFORM_TYPE::UF_VEC3I:  glUniform3iv(_Location, 1, static_cast<const GLint*>(_Data));
				break;
			case UNIFORM_TYPE::UF_VEC4F:  glUniform4fv(_Location, 1, static_cast<const GLfloat*>(_Data));
				break;
			case UNIFORM_TYPE::UF_VEC4I:  glUniform4iv(_Location, 1, static_cast<const GLint*>(_Data));
				break;
			case UNIFORM_TYPE::UF_MAT2:   glUniformMatrix2fv(_Location, 1, GL_FALSE, static_cast<const GLfloat*>(_Data));
				break;
			case UNIFORM_TYPE::UF_MAT2X3: glUniformMatrix2x3fv(_Location, 1, GL_FALSE, static_cast<const GLfloat*>(_Data));
				break;
			case UNIFORM_TYPE::UF_MAT2X4: glUniformMatrix2x4fv(_Location, 1, GL_FALSE, static_cast<const GLfloat*>(_Data));
				break;
			case UNIFORM_TYPE::UF_MAT3:   glUniformMatrix3fv(_Location, 1, GL_FALSE, static_cast<const GLfloat*>(_Data));
				break;
			case UNIFORM_TYPE::UF_MAT3X2: glUniformMatrix3x2fv(_Location, 1, GL_FALSE, static_cast<const GLfloat*>(_Data));
				break;
			case UNIFORM_TYPE::UF_MAT3X4: glUniformMatrix3x4fv(_Location, 1, GL_FALSE, static_cast<const GLfloat*>(_Data));
				break;
			case UNIFORM_TYPE::UF_MAT4:   glUniformMatrix4fv(_Location, 1, GL_FALSE, static_cast<const GLfloat*>(_Data));
				break;
			case UNIFORM_TYPE::UF_MAT4X2: glUniformMatrix4x2fv(_Location, 1, GL_FALSE, static_cast<const GLfloat*>(_Data));
				break;
			case UNIFORM_TYPE::UF_MAT4X3: glUniformMatrix4x3fv(_Location, 1, GL_FALSE, static_cast<const GLfloat*>(_Data));
				break;
			default:
				logcout << "Warning[ UNIFORM_TYPE unknown ] In[ opengl_api::_Set_uniform(GLint, UNIFORM_TYPE, void*) ]";
				break;
			}
		}
	
	void gl_set_uniform(GLint _Location, int _Val)
		{
		glUniform1i(_Location, _Val);
		}

	void gl_set_uniform(GLint _Location, unsigned int _Val)
		{
		glUniform1ui(_Location, _Val);
		}

	void gl_set_uniform(GLint _Location, float _Val)
		{
		glUniform1f(_Location, _Val);
		}

	void gl_set_uniform(GLint _Location, double _Val)
		{
		glUniform1d(_Location, _Val);
		}

	void gl_set_uniform(GLint _Location, const Vec2& _Data)
		{
		glUniform2fv(_Location, 1, static_cast<const float*>(_Data));
		}

	void gl_set_uniform(GLint _Location, const Vec3& _Data)
		{
		glUniform3fv(_Location, 1, static_cast<const float*>(_Data));
		}

	void gl_set_uniform(GLint _Location, const Vec4& _Data)
		{
		glUniform4fv(_Location, 1, static_cast<const float*>(_Data));
		}

	void gl_set_uniform(GLint _Location, const Vec2i& _Data)
		{
		glUniform2iv(_Location, 1, static_cast<const int*>(_Data));
		}

	void gl_set_uniform(GLint _Location, const Vec3i& _Data)
		{
		glUniform3iv(_Location, 1, static_cast<const int*>(_Data));
		}

	void gl_set_uniform(GLint _Location, const Vec4i& _Data)
		{
		glUniform4iv(_Location, 1, static_cast<const int*>(_Data));
		}

	void gl_set_uniform(GLint _Location, const Mat2& _Data)
		{
		glUniformMatrix2fv(_Location, 1, GL_FALSE, static_cast<const float*>(_Data));
		}

	void gl_set_uniform(GLint _Location, const Mat3& _Data)
		{
		glUniformMatrix3fv(_Location, 1, GL_FALSE, static_cast<const float*>(_Data));
		}

	void gl_set_uniform(GLint _Location, const Mat4& _Data)
		{
		glUniformMatrix4fv(_Location, 1, GL_FALSE, static_cast<const float*>(_Data));
		}

	GLint gl_find_uniform_location(GLuint _Program, const char* _Uname)
		{
		return (glGetUniformLocation(_Program, _Uname));
		}
	// </uniform>

	// <gpubuf>
	GLenum gl_convert_gpubuf_type(GPUBUF_TYPE _Type)
		{
		switch (_Type)
			{
			case GPUBUF_TYPE::VBO: return (GL_ARRAY_BUFFER);
			case GPUBUF_TYPE::EBO: return (GL_ELEMENT_ARRAY_BUFFER);
			case GPUBUF_TYPE::TBO: return (GL_TEXTURE_BUFFER);
			case GPUBUF_TYPE::FBO: return (GL_FRAMEBUFFER);
			default: return (GL_ARRAY_BUFFER);
			}
		}

	GPUBUF_TYPE gl_convert_gpubuf_type(GLenum _Type)
		{
		switch (_Type)
			{
			case GL_ARRAY_BUFFER:		  return (GPUBUF_TYPE::VBO);
			case GL_ELEMENT_ARRAY_BUFFER: return (GPUBUF_TYPE::EBO);
			case GL_TEXTURE_BUFFER:		  return (GPUBUF_TYPE::TBO);
			case GL_FRAMEBUFFER:		  return (GPUBUF_TYPE::FBO);
			default: return (GPUBUF_TYPE::VBO);
			}
		}

	GLenum gl_convert_gpubuf_usage(GPUBUF_USAGE _Usage)
		{
		switch (_Usage)
			{
			case GPUBUF_USAGE::STATIC_DRAW:  return (GL_STATIC_DRAW);
			case GPUBUF_USAGE::DYNAMIC_DRAW: return (GL_DYNAMIC_DRAW);
			default: return (GL_STATIC_DRAW);
			}
		}

	GPUBUF_USAGE gl_convert_gpubuf_usage(GLenum _Usage)
		{
		switch (_Usage)
			{
			case GL_STATIC_DRAW:  return (GPUBUF_USAGE::STATIC_DRAW);
			case GL_DYNAMIC_DRAW: return (GPUBUF_USAGE::DYNAMIC_DRAW);
			default: return (GPUBUF_USAGE::STATIC_DRAW);
			}
		}

	void gl_create_gpubuf(GLuint& _Dst, GLenum _Target, GLenum _Usage, void* _Data, GLsizeiptr _Size, GLsizeiptr _Capcity)
		{
		if (_Capcity < _Size)
			{	// check if capacity is error
			logcout << std_mes("warn", "capacity < size", "gl_create_gpubuf(...)", __FILE__, __LINE__);
			}

		glGenBuffers(1, &_Dst);

		glBindBuffer(_Target, _Dst);
		glBufferData(_Target, maxval(_Size, _Capcity), nullptr, _Usage);
		if (_Data != nullptr) glBufferSubData(_Target, 0, _Size, _Data);
		glBindBuffer(_Target, 0);
		}

	void gl_create_renderbuffer(GLuint& _Dst, size_t _Width, size_t _Height)
		{
		gl_delete_renderbuffer(_Dst);

		glGenRenderbuffers(1, &_Dst);
		glBindRenderbuffer(GL_RENDERBUFFER, _Dst);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, _Width, _Height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
	
	void gl_create_framebuffer(GLuint& _Dst, size_t _Width, size_t _Height, const std::vector<GLuint>& _Colortex)
		{
		gl_delete_framebuffer(_Dst);

		glGenFramebuffers(1, &_Dst);
		
		glBindFramebuffer(GL_FRAMEBUFFER, _Dst);

		for (size_t i = 0; i != _Colortex.size(); ++i)
			{
			glFramebufferTexture2D(GL_FRAMEBUFFER, static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i), GL_TEXTURE_2D, _Colortex[i], 0);
			}
		GLuint _Rbo = OPENGL_INVALIDE_UINT;
		gl_create_renderbuffer(_Rbo, _Width, _Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _Rbo);
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	
	void gl_memcpy_gpubuf(GLuint _Dst, GLuint _Src, GLsizeiptr _Size)
		{
		glBindBuffer(GL_COPY_READ_BUFFER, _Src);
		glBindBuffer(GL_COPY_WRITE_BUFFER, _Dst);

		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, _Size);

		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		glBindBuffer(GL_COPY_READ_BUFFER, 0);
		}

	void gl_delete_gpubuf(GLuint& _Buffer)
		{
		if (glIsBuffer(_Buffer))
			glDeleteBuffers(1, &_Buffer);
		_Buffer = -1;
		}

	void gl_delete_renderbuffer(GLuint& _Rbo)
		{
		if (glIsRenderbuffer(_Rbo))
			{
			glDeleteRenderbuffers(1, &_Rbo);
			}
		_Rbo = OPENGL_INVALIDE_UINT;
		}

	void gl_delete_framebuffer(GLuint& _Fbo)
		{
		if (glIsFramebuffer(_Fbo))
			{
			glDeleteFramebuffers(1, &_Fbo);
			}
		_Fbo = OPENGL_INVALIDE_UINT;
		}
	// </gpubuf>

	// <vertex attrib>
	GLenum gl_convert_base_type(BASE_TYPE _Type)
		{
		switch (_Type)
			{
			case BASE_TYPE::BS_BOOL:   return (GL_BOOL);
			case BASE_TYPE::BS_BYTE:   return (GL_BYTE);
			case BASE_TYPE::BS_UBYTE:  return (GL_UNSIGNED_BYTE);
			case BASE_TYPE::BS_INT32:  return (GL_INT);
			case BASE_TYPE::BS_UINT32: return (GL_UNSIGNED_INT);
			case BASE_TYPE::BS_FLOAT:  return (GL_FLOAT);
			case BASE_TYPE::BS_DOUBLE: return (GL_DOUBLE);
			default: return (GL_NONE);
			}
		}

	BASE_TYPE gl_convert_base_type(GLenum _Type)
		{
		switch (_Type)
			{
			case GL_BOOL:		   return (BASE_TYPE::BS_BOOL);
			case GL_BYTE:		   return (BASE_TYPE::BS_BYTE);
			case GL_UNSIGNED_BYTE: return (BASE_TYPE::BS_UBYTE);
			case GL_INT:		   return (BASE_TYPE::BS_INT32);
			case GL_UNSIGNED_INT:  return (BASE_TYPE::BS_UINT32);
			case GL_FLOAT:		   return (BASE_TYPE::BS_FLOAT);
			case GL_DOUBLE:		   return (BASE_TYPE::BS_DOUBLE);
			default: return (BASE_TYPE::BS_UNKNOWN);
			}
		}

	void gl_set_current_vertex_object(GLuint _Index, GLint _Size, GLenum _Type, GLboolean _Isnomal, GLsizei _Stride, GLvoid* _Off)
		{
		if (_Type == GL_INT || _Type == GL_UNSIGNED_INT)
			{
			glVertexAttribIPointer(_Index, _Size, _Type, _Stride, _Off);
			}
		else
			{
			glVertexAttribPointer(_Index, _Size, _Type, _Isnomal, _Stride, _Off);
			}

		glEnableVertexAttribArray(_Index);
		}
	
	void gl_delete_vertex_array_object(GLuint& _Array)
		{
		if (glIsVertexArray(_Array))
			glDeleteVertexArrays(1, &_Array);
		_Array = OPENGL_INVALIDE_UINT;
		}
	
	void  gl_create_vertex_array_object(GLuint& _Dst, const vertex_descriptor& _Vdesc, GLuint _Vbo)
		{
		// delete VAO if exists _Dst
		gl_delete_vertex_array_object(_Dst);

		// 1. reference VBO
		glBindBuffer(GL_ARRAY_BUFFER, _Vbo);

		// 2. generate VAO
		glGenVertexArrays(1, &_Dst);
		// 3. for_each
		glBindVertexArray(_Dst);
		for (decltype(_Vdesc.size()) _Index = 0; _Index != _Vdesc.size(); ++_Index)
			{
			const attribute_descriptor& _Rattrib = _Vdesc[_Index];

			gl_set_current_vertex_object(static_cast<GLuint>(_Index), static_cast<GLint>(_Rattrib.size), gl_convert_base_type(_Rattrib.type),
				static_cast<GLboolean>(_Rattrib.normalize), static_cast<GLsizei>(_Rattrib.stride), (GLvoid*)(_Rattrib.offset)
				);
			}
		// -2.
		glBindVertexArray(0);

		// -1. reference VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	// </vertex attrib>

	// <blend>
	GLenum gl_convert_blend_operator(blender::_Operator _Op)
		{
		if (_Op == blend_operator_none)
			return (GL_NONE);
		else if (_Op == blend_operator_add)
			return (GL_FUNC_ADD);
		else if (_Op == blend_operator_sub)
			return (GL_FUNC_SUBTRACT);
		else if (_Op == blend_operator_sub_rv)
			return (GL_FUNC_REVERSE_SUBTRACT);
		else if (_Op == blend_operator_min)
			return (GL_MIN);
		else if (_Op == blend_operator_max)
			return (GL_MAX);

		logcout << std_mes("warn", "invalid blend operator", "gl_convert_blend_operator(blender::_Operator)", __FILE__, __LINE__);
		return (GL_NONE);
		}

	GLenum gl_convert_blend_factorfunc(blender::_Factorfunc _Func)
		{
		if (_Func == blend_factor_one)
			return (GL_ONE);
		else if (_Func == blend_factor_zero)
			return (GL_ZERO);
		else if (_Func == blend_factor_src_alpha)
			return (GL_SRC_ALPHA);
		else if (_Func == blend_factor_dst_alpha)
			return (GL_DST_ALPHA);
		else if (_Func == blend_factor_src_color)
			return (GL_SRC_COLOR);
		else if (_Func == blend_factor_dst_color)
			return (GL_DST_COLOR);
		else if (_Func == blend_factor_one_minus_src_alpha)
			return (GL_ONE_MINUS_SRC_ALPHA);
		else if (_Func == blend_factor_one_minus_dst_alpha)
			return (GL_ONE_MINUS_DST_ALPHA);
		else if (_Func == blend_factor_one_minus_src_color)
			return (GL_ONE_MINUS_SRC_COLOR);
		else if (_Func == blend_factor_one_minus_dst_color)
			return (GL_ONE_MINUS_DST_COLOR);

		logcout << std_mes("warn", "invalid blend factorfunc", "gl_convert_blend_factorfunc(blender::_Factorfunc)", __FILE__, __LINE__);
		return (GL_NONE);
		}

	blender::_Operator gl_convert_blend_operator(GLenum _Op)
		{
		switch (_Op)
			{
			case GL_FUNC_ADD: return (blend_operator_add);
			case GL_FUNC_SUBTRACT: return (blend_operator_sub);
			case GL_FUNC_REVERSE_SUBTRACT: return (blend_operator_sub_rv);
			case GL_MIN:	  return (blend_operator_min);
			case GL_MAX:	  return (blend_operator_max);
			default: return (blend_operator_none);
			}
		}

	blender::_Factorfunc gl_convert_blend_factorfunc(GLenum _Func)
		{
		switch (_Func)
			{
			case GL_ONE: return (blend_factor_one);
			case GL_ZERO: return (blend_factor_zero);
			case GL_SRC_ALPHA: return (blend_factor_src_alpha);
			case GL_DST_ALPHA: return (blend_factor_dst_alpha);
			case GL_SRC_COLOR: return (blend_factor_src_color);
			case GL_DST_COLOR: return (blend_factor_dst_color);
			case GL_ONE_MINUS_SRC_ALPHA: return (blend_factor_one_minus_src_alpha);
			case GL_ONE_MINUS_DST_ALPHA: return (blend_factor_one_minus_dst_alpha);
			case GL_ONE_MINUS_SRC_COLOR: return (blend_factor_one_minus_src_color);
			case GL_ONE_MINUS_DST_COLOR: return (blend_factor_one_minus_dst_color);
			default:
				logcout << std_mes("warn", "invalid blend GLenum", "gl_convert_blend_factorfunc(GLenum)", __FILE__, __LINE__);
				return (blend_factor_src_alpha);
			}
		}
	
	bool gl_exist_blend_operator(GLenum _Op)
		{
		if (_Op == GL_NONE)
			return (false);

		if (_Op == GL_FUNC_ADD || _Op == GL_FUNC_SUBTRACT || _Op == GL_FUNC_REVERSE_SUBTRACT
			|| _Op == GL_MIN || _Op == GL_MAX)
			{
			return (true);
			}
		return (false);
		}

	bool gl_exist_blend_factorfunc(GLenum _Func)
		{
		if (_Func == GL_NONE)
			return (false);

		if (_Func == GL_SRC_ALPHA || _Func == GL_DST_ALPHA
			|| _Func == GL_SRC_COLOR || _Func == GL_DST_COLOR
			|| _Func == GL_ONE_MINUS_SRC_ALPHA || _Func == GL_ONE_MINUS_DST_ALPHA
			|| _Func == GL_ONE_MINUS_SRC_COLOR || _Func == GL_ONE_MINUS_DST_COLOR)
			{
			return (true);
			}
		return (false);
		}
	
	blend_param
		gl_convert_blender(const blender& _Blender)
	{
		blend_param _Fblend;
		_Fblend.op = static_cast<size_t>(gl_convert_blend_operator(_Blender._Myop));
		_Fblend.src = static_cast<size_t>(gl_convert_blend_factorfunc(_Blender._Mysrc));
		_Fblend.dst = static_cast<size_t>(gl_convert_blend_factorfunc(_Blender._Mydst));
		return (_Fblend);
	}

	blender
		gl_convert_blender(const blend_param& _Fblend)
	{
		blender _Blender;
		_Blender._Myop = gl_convert_blend_operator(static_cast<GLenum>(_Fblend.op));
		_Blender._Mysrc = gl_convert_blend_operator(static_cast<GLenum>(_Fblend.src));
		_Blender._Mydst = gl_convert_blend_operator(static_cast<GLenum>(_Fblend.dst));
		return (_Blender);
	}

	void
		gl_set_blend(const blender& _Blender)
	{
		glBlendEquation(gl_convert_blend_operator(_Blender._Myop));
		glBlendFunc(gl_convert_blend_factorfunc(_Blender._Mysrc), gl_convert_blend_factorfunc(_Blender._Mydst));
	}

	void
		gl_set_blend(const blend_param& _Fblend)
	{
		glBlendEquation(static_cast<GLenum>(_Fblend.op));
		glBlendFunc(static_cast<GLenum>(_Fblend.src), static_cast<GLenum>(_Fblend.dst));
	}

	void gl_enable_blend(const blender& _Blender)
	{
		glEnable(GL_BLEND);
		gl_set_blend(_Blender);
	}

	void gl_enable_blend(const blend_param& _Fblend)
	{
		glEnable(GL_BLEND);
		gl_set_blend(_Fblend);
	}

	void gl_disable_blend()
	{
		glDisable(GL_BLEND);
	}
	// </blend>

	// <texture>
	GLenum gl_convert_texture_wrap(TEXTURE_WRAP _Wrap)
		{
		switch (_Wrap)
			{
			case clmagic::TEXTURE_WRAP::REPEAT:			 return (GL_REPEAT);
			case clmagic::TEXTURE_WRAP::MIRRORED_REPEAT: return (GL_MIRRORED_REPEAT);
			case clmagic::TEXTURE_WRAP::CLAMP_TO_EDGE:   return (GL_CLAMP_TO_EDGE);
			case clmagic::TEXTURE_WRAP::CLAMP_TO_BORDER: return (GL_CLAMP_TO_BORDER);
			default:
				logcout << std_mes("warn", "not exist texture wrap", "gl_convert_texture_wrap(TEXTURE_WRAP)", __FILE__, __LINE__);
				return (GL_REPEAT);
			}
		}

	GLenum gl_convert_texture_filter(TEXTURE_FILTER _Filter)
		{
		switch (_Filter)
			{
			case TEXTURE_FILTER::LINEAR: return (GL_LINEAR);
			case TEXTURE_FILTER::NEAREST: return (GL_NEAREST);
			default:
				logcout << std_mes("warn", "not exist texture filter", "gl_convert_texture_filter(TEXTURE_FILTER)", __FILE__, __LINE__);
				return (GL_LINEAR);
			}
		}

	void gl_create_texture(GLuint& _Dst, void* _Data, size_t _Width, size_t _Height,
		GLenum _Internalformat, GLenum _Format,
		GLenum _Xwrap, GLenum _Ywrap, GLenum _Fltmag, GLenum _Fltmin, std::initializer_list<GLint> _Swizzles)
		{
		gl_delete_texture(_Dst);

		glGenTextures(1, &_Dst);
		glBindTexture(GL_TEXTURE_2D, _Dst);

		glTexImage2D(GL_TEXTURE_2D, 0, _Internalformat, _Width, _Height, 0, _Format, GL_UNSIGNED_BYTE, _Data);


		if (_Swizzles.size() == 3 || _Swizzles.size() == 4)
			{
			glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, _Swizzles.begin());
			}
		glTexParameteri(_Dst, GL_TEXTURE_MIN_FILTER, _Fltmin);
		glTexParameteri(_Dst, GL_TEXTURE_MAG_FILTER, _Fltmag);
		glTexParameteri(_Dst, GL_TEXTURE_WRAP_S, _Xwrap);
		glTexParameteri(_Dst, GL_TEXTURE_WRAP_T, _Ywrap);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		}

	


	class _My_freetype_
	{
	public:
		constexpr static FT_UInt DEFAULT_FTSIZE = 32;
		constexpr static const char* DEFAULT_FTFAMILY = "C:/Windows/Fonts/simsun.ttc";

		static _My_freetype_& instance() 
			{
			static _My_freetype_ _Impl;
			return (_Impl);
			}

	private:
		void _Init_library_unchecked()
		{
			FT_Init_FreeType(&_Mylib);
		}

		void _Create_face_unchecked()
		{
			FT_New_Face(_Mylib, DEFAULT_FTFAMILY, 0, &_Myface);
			FT_Set_Pixel_Sizes(_Myface, 0, DEFAULT_FTSIZE);
		}

		void _Destroy()
		{
			if (_Myface != nullptr)
			{
				FT_Done_Face(_Myface);
				_Myface = nullptr;
			}

			if (_Mylib != nullptr)
			{
				FT_Done_FreeType(_Mylib);
				_Mylib = nullptr;
			}
		}

	public:
		_My_freetype_()
			{
			_Init_library_unchecked();
			_Create_face_unchecked();
			}

		~_My_freetype_()
			{
			_Destroy();
			}

		void init()
			{
			_Init_library_unchecked();
			_Create_face_unchecked();
			}

		void destroy() 
			{
			_Destroy();
			}

		FT_Library _Mylib = nullptr;
		FT_Face _Myface = nullptr;
	};

	void gl_create_ft_texture(ft_texture& _Dst, int _Charactor, const std::string& _Ftfamily, size_t _Ftsize, bool _Isdeostrylib)
		{
		FT_Face _Face = _My_freetype_::instance()._Myface;
		FT_Load_Char(_Face, _Charactor, FT_LOAD_RENDER);

		FT_GlyphSlot _Glyph = _Face->glyph;
		FT_Bitmap& _Rbitmap = _Glyph->bitmap;

		// 6. create ft_texture gpu memory
		GLuint _Texture;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		gl_create_texture(_Texture, _Rbitmap.buffer, _Rbitmap.width, _Rbitmap.rows, GL_RED, GL_RED, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		_Dst.left = static_cast<float>(_Glyph->bitmap_left);
		_Dst.top = static_cast<float>(_Glyph->bitmap_top);
		_Dst.width = static_cast<float>(_Glyph->bitmap.width);
		_Dst.height = static_cast<float>(_Glyph->bitmap.rows);
		_Dst.advance[0] = _Glyph->advance.x;
		_Dst.advance[1] = _Glyph->advance.y;
		_Dst.id = static_cast<size_t>(_Texture);

		if (_Isdeostrylib)
			{
			_My_freetype_::instance().destroy();
			}
		}
	
	

	void gl_create_texture(GLuint& _Dst, void* _Data, size_t _Width, size_t _Height, size_t _Channels,
			GLenum _Xwrap, GLenum _Ywrap, GLenum _Fltmag, GLenum _Fltmin, std::initializer_list<GLint> _Swizzles)
		{
		if (_Channels == 4)
			{
			gl_create_texture(_Dst, _Data, _Width, _Height, GL_RGBA, GL_RGBA, _Xwrap, _Ywrap, _Fltmag, _Fltmin, _Swizzles);
			}
		else
			{
			gl_create_texture(_Dst, _Data, _Width, _Height, GL_RGB, GL_RGB, _Xwrap, _Ywrap, _Fltmag, _Fltmin, _Swizzles);
			}
		}

	void gl_create_texture(GLuint& _Dst, const cv::Mat& _Src,
			GLenum _Xwrap, GLenum _Ywrap, GLenum _Fltmag, GLenum _Fltmin)
		{
		gl_create_texture(_Dst, _Src.data, _Src.cols, _Src.rows, _Src.channels(), _Xwrap, _Ywrap, _Fltmag, _Fltmin, { GL_BLUE, GL_GREEN, GL_RED, GL_ALPHA });
		}

	void gl_delete_texture(GLuint& _Dst)
		{
		if (glIsTexture(_Dst))
			{
			glDeleteTextures(1, &_Dst);
			}
		_Dst = -1;
		}
	// </texture>

	void gl_read_frame(cv::OutputArray _Dst, Vec2i _Winsize)
	{
		_Dst.getMatRef() = cv::Mat(_Winsize[1], _Winsize[0], CV_8UC4);
		glReadPixels(0, 0, _Winsize[0], _Winsize[1], GL_RGBA, GL_UNSIGNED_BYTE, _Dst.getMatRef().data);
	}



	constexpr int MAX_TEXTURE_MIPS = 14;

	struct vglImageMipData
	{
		GLsizei width;                              // Width of this mipmap level
		GLsizei height;                             // Height of this mipmap level
		GLsizei depth;                              // Depth pof mipmap level
		GLsizeiptr mipStride;                       // Distance in bytes between mip levels in memory
		GLvoid* data;                               // Pointer to data
	};

	struct vglImageData
	{
		GLenum target;                              // Texture target (1D, 2D, cubemap, array, etc.)
		GLenum internalFormat;                      // Recommended internal format (GL_RGBA32F, etc).
		GLenum format;                              // Format in memory
		GLenum type;                                // Type in memory (GL_RED, GL_RGB, etc.)
		GLenum swizzle[4];                          // Swizzle for RGBA
		GLsizei mipLevels;                          // Number of present mipmap levels
		GLsizei slices;                             // Number of slices (for arrays)
		GLsizeiptr sliceStride;                     // Distance in bytes between slices of an array texture
		GLsizeiptr totalDataSize;                   // Complete amount of data allocated for texture
		vglImageMipData mip[MAX_TEXTURE_MIPS];      // Actual mipmap data
	};

	class _DDS_reader 
	{
	public:
		enum DDS_FORMAT
		{
			DDS_FORMAT_UNKNOWN = 0,
			DDS_FORMAT_R32G32B32A32_TYPELESS = 1,
			DDS_FORMAT_R32G32B32A32_FLOAT = 2,
			DDS_FORMAT_R32G32B32A32_UINT = 3,
			DDS_FORMAT_R32G32B32A32_SINT = 4,
			DDS_FORMAT_R32G32B32_TYPELESS = 5,
			DDS_FORMAT_R32G32B32_FLOAT = 6,
			DDS_FORMAT_R32G32B32_UINT = 7,
			DDS_FORMAT_R32G32B32_SINT = 8,
			DDS_FORMAT_R16G16B16A16_TYPELESS = 9,
			DDS_FORMAT_R16G16B16A16_FLOAT = 10,
			DDS_FORMAT_R16G16B16A16_UNORM = 11,
			DDS_FORMAT_R16G16B16A16_UINT = 12,
			DDS_FORMAT_R16G16B16A16_SNORM = 13,
			DDS_FORMAT_R16G16B16A16_SINT = 14,
			DDS_FORMAT_R32G32_TYPELESS = 15,
			DDS_FORMAT_R32G32_FLOAT = 16,
			DDS_FORMAT_R32G32_UINT = 17,
			DDS_FORMAT_R32G32_SINT = 18,
			DDS_FORMAT_R32G8X24_TYPELESS = 19,
			DDS_FORMAT_D32_FLOAT_S8X24_UINT = 20,
			DDS_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
			DDS_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
			DDS_FORMAT_R10G10B10A2_TYPELESS = 23,
			DDS_FORMAT_R10G10B10A2_UNORM = 24,
			DDS_FORMAT_R10G10B10A2_UINT = 25,
			DDS_FORMAT_R11G11B10_FLOAT = 26,
			DDS_FORMAT_R8G8B8A8_TYPELESS = 27,
			DDS_FORMAT_R8G8B8A8_UNORM = 28,
			DDS_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
			DDS_FORMAT_R8G8B8A8_UINT = 30,
			DDS_FORMAT_R8G8B8A8_SNORM = 31,
			DDS_FORMAT_R8G8B8A8_SINT = 32,
			DDS_FORMAT_R16G16_TYPELESS = 33,
			DDS_FORMAT_R16G16_FLOAT = 34,
			DDS_FORMAT_R16G16_UNORM = 35,
			DDS_FORMAT_R16G16_UINT = 36,
			DDS_FORMAT_R16G16_SNORM = 37,
			DDS_FORMAT_R16G16_SINT = 38,
			DDS_FORMAT_R32_TYPELESS = 39,
			DDS_FORMAT_D32_FLOAT = 40,
			DDS_FORMAT_R32_FLOAT = 41,
			DDS_FORMAT_R32_UINT = 42,
			DDS_FORMAT_R32_SINT = 43,
			DDS_FORMAT_R24G8_TYPELESS = 44,
			DDS_FORMAT_D24_UNORM_S8_UINT = 45,
			DDS_FORMAT_R24_UNORM_X8_TYPELESS = 46,
			DDS_FORMAT_X24_TYPELESS_G8_UINT = 47,
			DDS_FORMAT_R8G8_TYPELESS = 48,
			DDS_FORMAT_R8G8_UNORM = 49,
			DDS_FORMAT_R8G8_UINT = 50,
			DDS_FORMAT_R8G8_SNORM = 51,
			DDS_FORMAT_R8G8_SINT = 52,
			DDS_FORMAT_R16_TYPELESS = 53,
			DDS_FORMAT_R16_FLOAT = 54,
			DDS_FORMAT_D16_UNORM = 55,
			DDS_FORMAT_R16_UNORM = 56,
			DDS_FORMAT_R16_UINT = 57,
			DDS_FORMAT_R16_SNORM = 58,
			DDS_FORMAT_R16_SINT = 59,
			DDS_FORMAT_R8_TYPELESS = 60,
			DDS_FORMAT_R8_UNORM = 61,
			DDS_FORMAT_R8_UINT = 62,
			DDS_FORMAT_R8_SNORM = 63,
			DDS_FORMAT_R8_SINT = 64,
			DDS_FORMAT_A8_UNORM = 65,
			DDS_FORMAT_R1_UNORM = 66,
			DDS_FORMAT_R9G9B9E5_SHAREDEXP = 67,
			DDS_FORMAT_R8G8_B8G8_UNORM = 68,
			DDS_FORMAT_G8R8_G8B8_UNORM = 69,
			DDS_FORMAT_BC1_TYPELESS = 70,
			DDS_FORMAT_BC1_UNORM = 71,
			DDS_FORMAT_BC1_UNORM_SRGB = 72,
			DDS_FORMAT_BC2_TYPELESS = 73,
			DDS_FORMAT_BC2_UNORM = 74,
			DDS_FORMAT_BC2_UNORM_SRGB = 75,
			DDS_FORMAT_BC3_TYPELESS = 76,
			DDS_FORMAT_BC3_UNORM = 77,
			DDS_FORMAT_BC3_UNORM_SRGB = 78,
			DDS_FORMAT_BC4_TYPELESS = 79,
			DDS_FORMAT_BC4_UNORM = 80,
			DDS_FORMAT_BC4_SNORM = 81,
			DDS_FORMAT_BC5_TYPELESS = 82,
			DDS_FORMAT_BC5_UNORM = 83,
			DDS_FORMAT_BC5_SNORM = 84,
			DDS_FORMAT_B5G6R5_UNORM = 85,
			DDS_FORMAT_B5G5R5A1_UNORM = 86,
			DDS_FORMAT_B8G8R8A8_UNORM = 87,
			DDS_FORMAT_B8G8R8X8_UNORM = 88,
			DDS_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
			DDS_FORMAT_B8G8R8A8_TYPELESS = 90,
			DDS_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
			DDS_FORMAT_B8G8R8X8_TYPELESS = 92,
			DDS_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
			DDS_FORMAT_BC6H_TYPELESS = 94,
			DDS_FORMAT_BC6H_UF16 = 95,
			DDS_FORMAT_BC6H_SF16 = 96,
			DDS_FORMAT_BC7_TYPELESS = 97,
			DDS_FORMAT_BC7_UNORM = 98,
			DDS_FORMAT_BC7_UNORM_SRGB = 99,
			DDS_FORMAT_AYUV = 100,
			DDS_FORMAT_Y410 = 101,
			DDS_FORMAT_Y416 = 102,
			DDS_FORMAT_NV12 = 103,
			DDS_FORMAT_P010 = 104,
			DDS_FORMAT_P016 = 105,
			DDS_FORMAT_420_OPAQUE = 106,
			DDS_FORMAT_YUY2 = 107,
			DDS_FORMAT_Y210 = 108,
			DDS_FORMAT_Y216 = 109,
			DDS_FORMAT_NV11 = 110,
			DDS_FORMAT_AI44 = 111,
			DDS_FORMAT_IA44 = 112,
			DDS_FORMAT_P8 = 113,
			DDS_FORMAT_A8P8 = 114,
			DDS_FORMAT_B4G4R4A4_UNORM = 115
		};

		enum
		{
			DDS_MAGIC = 0x20534444,

			DDSCAPS_COMPLEX = 0x00000008,
			DDSCAPS_MIPMAP = 0x00400000,
			DDSCAPS_TEXTURE = 0x00001000,

			DDSCAPS2_CUBEMAP = 0x00000200,
			DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400,
			DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800,
			DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000,
			DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000,
			DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000,
			DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000,
			DDSCAPS2_VOLUME = 0x00200000,

			DDS_CUBEMAP_ALLFACES = (DDSCAPS2_CUBEMAP_POSITIVEX |
			DDSCAPS2_CUBEMAP_NEGATIVEX |
				DDSCAPS2_CUBEMAP_POSITIVEY |
				DDSCAPS2_CUBEMAP_NEGATIVEY |
				DDSCAPS2_CUBEMAP_POSITIVEZ |
				DDSCAPS2_CUBEMAP_NEGATIVEZ),

			DDS_RESOURCE_DIMENSION_UNKNOWN = 0,
			DDS_RESOURCE_DIMENSION_BUFFER = 1,
			DDS_RESOURCE_DIMENSION_TEXTURE1D = 2,
			DDS_RESOURCE_DIMENSION_TEXTURE2D = 3,
			DDS_RESOURCE_DIMENSION_TEXTURE3D = 4,

			DDS_RESOURCE_MISC_TEXTURECUBE = 0x00000004,

			DDS_FOURCC_DX10 = 0x30315844,
			DDS_FOURCC_DXT1 = 0x31545844,
			DDS_FOURCC_DXT2 = 0x32545844,
			DDS_FOURCC_DXT3 = 0x33545844,
			DDS_FOURCC_DXT4 = 0x34545844,
			DDS_FOURCC_DXT5 = 0x35545844,

			DDS_DDPF_ALPHAPIXELS = 0x00000001,
			DDS_DDPF_ALPHA = 0x00000002,
			DDS_DDPF_FOURCC = 0x00000004,
			DDS_DDPF_RGB = 0x00000040,
			DDS_DDPF_YUV = 0x00000200,
			DDS_DDPF_LUMINANCE = 0x00020000,
		};

		struct DDS_PIXELFORMAT
		{
			uint32_t                dwSize;
			uint32_t                dwFlags;
			uint32_t                dwFourCC;
			uint32_t                dwRGBBitCount;
			uint32_t                dwRBitMask;
			uint32_t                dwGBitMask;
			uint32_t                dwBBitMask;
			uint32_t                dwABitMask;
		};

		struct DDS_HEADER
		{
			uint32_t                size;
			uint32_t                flags;
			uint32_t                height;
			uint32_t                width;
			uint32_t                pitch_or_linear_size;
			uint32_t                depth;
			uint32_t                mip_levels;
			uint32_t                reserved[11];
			DDS_PIXELFORMAT         ddspf;
			uint32_t                caps1;
			uint32_t                caps2;
			uint32_t                caps3;
			uint32_t                caps4;
			uint32_t                reserved2;
		};

		struct DDS_HEADER_DXT10
		{
			uint32_t                format;
			uint32_t                dimension;
			uint32_t                misc_flag;
			uint32_t                array_size;
			uint32_t                reserved;
		};

		struct DDS_FILE_HEADER
		{
			uint32_t                magic;
			DDS_HEADER              std_header;
			DDS_HEADER_DXT10        dxt10_header;
		};

		struct DDS_FORMAT_GL_INFO
		{
			GLenum                  format;
			GLenum                  type;
			GLenum                  internalFormat;
			GLenum                  swizzle_r;
			GLenum                  swizzle_g;
			GLenum                  swizzle_b;
			GLenum                  swizzle_a;
			GLsizei                 bits_per_texel;
		};

		static constexpr DDS_FORMAT_GL_INFO gl_info_table[] =
		{
			// format,              type,               internalFormat,     swizzle_r,      swizzle_g,      swizzle_b,      swizzle_a
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    0 },        // DDS_FORMAT_UNKNOWN
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    0 },        // DDS_FORMAT_R32G32B32A32_TYPELESS
			{ GL_RGBA,              GL_FLOAT,           GL_RGBA32F,         GL_RED,         GL_GREEN,       GL_BLUE,        GL_ALPHA,   128 },      // DDS_FORMAT_R32G32B32A32_FLOAT
			{ GL_RGBA_INTEGER,      GL_UNSIGNED_INT,    GL_RGBA32UI,        GL_RED,         GL_GREEN,       GL_BLUE,        GL_ALPHA,   128 },      // DDS_FORMAT_R32G32B32A32_UINT
			{ GL_RGBA_INTEGER,      GL_INT,             GL_RGBA32I,         GL_RED,         GL_GREEN,       GL_BLUE,        GL_ALPHA,   128 },      // DDS_FORMAT_R32G32B32A32_SINT
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    96 },       // DDS_FORMAT_R32G32B32_TYPELESS
			{ GL_RGB,               GL_FLOAT,           GL_RGB32F,          GL_RED,         GL_GREEN,       GL_BLUE,        GL_ONE,     96 },       // DDS_FORMAT_R32G32B32_FLOAT
			{ GL_RGB_INTEGER,       GL_UNSIGNED_INT,    GL_RGB32UI,         GL_RED,         GL_GREEN,       GL_BLUE,        GL_ONE,     96 },       // DDS_FORMAT_R32G32B32_UINT
			{ GL_RGB_INTEGER,       GL_INT,             GL_RGB32I,          GL_RED,         GL_GREEN,       GL_BLUE,        GL_ONE,     96 },       // DDS_FORMAT_R32G32B32_SINT
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    64 },       // DDS_FORMAT_R16G16B16A16_TYPELESS
			{ GL_RGBA,              GL_HALF_FLOAT,      GL_RGBA16F,         GL_RED,         GL_GREEN,       GL_BLUE,        GL_ALPHA,   64 },       // DDS_FORMAT_R16G16B16A16_FLOAT
			{ GL_RGBA,              GL_UNSIGNED_SHORT,  GL_RGBA16,          GL_RED,         GL_GREEN,       GL_BLUE,        GL_ALPHA,   64 },       // DDS_FORMAT_R16G16B16A16_UNORM
			{ GL_RGBA_INTEGER,      GL_UNSIGNED_SHORT,  GL_RGBA16UI,        GL_RED,         GL_GREEN,       GL_BLUE,        GL_ALPHA,   64 },       // DDS_FORMAT_R16G16B16A16_UINT
			{ GL_RGBA,              GL_SHORT,           GL_RGBA16_SNORM,    GL_RED,         GL_GREEN,       GL_BLUE,        GL_ALPHA,   64 },       // DDS_FORMAT_R16G16B16A16_SNORM
			{ GL_RGBA_INTEGER,      GL_SHORT,           GL_RGBA16I,         GL_RED,         GL_GREEN,       GL_BLUE,        GL_ALPHA,   64 },       // DDS_FORMAT_R16G16B16A16_SINT
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    64 },       // DDS_FORMAT_R32G32_TYPELESS
			{ GL_RG,                GL_FLOAT,           GL_RG32F,           GL_RED,         GL_GREEN,       GL_ZERO,        GL_ONE,     64 },       // DDS_FORMAT_R32G32_FLOAT
			{ GL_RG_INTEGER,        GL_UNSIGNED_INT,    GL_RG32UI,          GL_RED,         GL_GREEN,       GL_ZERO,        GL_ONE,     64 },       // DDS_FORMAT_R32G32_UINT
			{ GL_RG_INTEGER,        GL_INT,             GL_RG32I,           GL_RED,         GL_GREEN,       GL_ZERO,        GL_ONE,     64 },       // DDS_FORMAT_R32G32_SINT
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    64 },       // DDS_FORMAT_R32G8X24_TYPELESS
			{ GL_DEPTH_STENCIL,     GL_FLOAT_32_UNSIGNED_INT_24_8_REV,  GL_DEPTH32F_STENCIL8, GL_NONE, GL_NONE, GL_NONE,    GL_NONE,    64 },       // DDS_FORMAT_D32_FLOAT_S8X24_UINT (THIS MAY NOT BE RIGHT)
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    64 },       // DDS_FORMAT_R32_FLOAT_X8X24_TYPELESS
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    64 },      // DDS_FORMAT_X32_TYPELESS_G8X24_UINT
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    32 },      // DDS_FORMAT_R10G10B10A2_TYPELESS
			{ GL_RGBA,              GL_UNSIGNED_INT,    GL_RGB10_A2,        GL_RED,         GL_GREEN,       GL_BLUE,        GL_ALPHA,   32 },      // DDS_FORMAT_R10G10B10A2_UNORM
			{ GL_RGBA_INTEGER,      GL_UNSIGNED_INT,    GL_RGB10_A2UI,      GL_RED,         GL_GREEN,       GL_BLUE,        GL_ALPHA,   32 },      // DDS_FORMAT_R10G10B10A2_UINT
			{ GL_RGB,               GL_UNSIGNED_INT,    GL_R11F_G11F_B10F,  GL_RED,         GL_GREEN,       GL_BLUE,        GL_ONE,     32 },      // DDS_FORMAT_R11G11B10_FLOAT
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    32 },      // DDS_FORMAT_R8G8B8A8_TYPELESS
			{ GL_RGBA,              GL_UNSIGNED_BYTE,   GL_RGBA8,           GL_RED,         GL_GREEN,       GL_BLUE,        GL_ALPHA,   32 },      // DDS_FORMAT_R8G8B8A8_UNORM
			{ GL_RGBA,              GL_UNSIGNED_BYTE,   GL_SRGB8_ALPHA8,    GL_RED,         GL_GREEN,       GL_BLUE,        GL_ALPHA,   32 },      // DDS_FORMAT_R8G8B8A8_UNORM_SRGB
			{ GL_RGBA_INTEGER,      GL_UNSIGNED_BYTE,   GL_RGBA8UI,         GL_RED,         GL_GREEN,       GL_BLUE,        GL_ALPHA,   32 },      // DDS_FORMAT_R8G8B8A8_UINT
			{ GL_RGBA,              GL_BYTE,            GL_RGBA8_SNORM,     GL_RED,         GL_GREEN,       GL_BLUE,        GL_ALPHA,   32 },      // DDS_FORMAT_R8G8B8A8_SNORM
			{ GL_RGBA_INTEGER,      GL_BYTE,            GL_RGBA8I,          GL_RED,         GL_GREEN,       GL_BLUE,        GL_ALPHA,   32 },      // DDS_FORMAT_R8G8B8A8_SINT
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    32 },      // DDS_FORMAT_R16G16_TYPELESS
			{ GL_RG,                GL_HALF_FLOAT,      GL_RG16F,           GL_RED,         GL_GREEN,       GL_ZERO,        GL_ONE,     32 },      // DDS_FORMAT_R16G16_FLOAT
			{ GL_RG,                GL_UNSIGNED_SHORT,  GL_RG16,            GL_RED,         GL_GREEN,       GL_ZERO,        GL_ONE,     32 },      // DDS_FORMAT_R16G16_UNORM
			{ GL_RG_INTEGER,        GL_UNSIGNED_SHORT,  GL_RG16UI,          GL_RED,         GL_GREEN,       GL_ZERO,        GL_ONE,     32 },      // DDS_FORMAT_R16G16_UINT
			{ GL_RG,                GL_SHORT,           GL_RG16_SNORM,      GL_RED,         GL_GREEN,       GL_ZERO,        GL_ONE,     32 },      // DDS_FORMAT_R16G16_SNORM
			{ GL_RG_INTEGER,        GL_SHORT,           GL_RG16I,           GL_RED,         GL_GREEN,       GL_ZERO,        GL_ONE,     32 },      // DDS_FORMAT_R16G16_SINT
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    32 },      // DDS_FORMAT_R32_TYPELESS
			{ GL_DEPTH_COMPONENT,   GL_FLOAT,           GL_DEPTH_COMPONENT32F,  GL_RED,     GL_ZERO,        GL_ZERO,        GL_ZERO,    32 },      // DDS_FORMAT_D32_FLOAT
			{ GL_RED,               GL_FLOAT,           GL_R32F,            GL_RED,         GL_ZERO,        GL_ZERO,        GL_ONE,     32 },      // DDS_FORMAT_R32_FLOAT
			{ GL_RED_INTEGER,       GL_UNSIGNED_INT,    GL_R32UI,           GL_RED,         GL_ZERO,        GL_ZERO,        GL_ONE,     32 },      // DDS_FORMAT_R32_UINT
			{ GL_RED_INTEGER,       GL_INT,             GL_R32I,            GL_RED,         GL_ZERO,        GL_ZERO,        GL_ONE,     32 },      // DDS_FORMAT_R32_SINT
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    32 },      // DDS_FORMAT_R24G8_TYPELESS
			{ GL_DEPTH_STENCIL,     GL_UNSIGNED_INT,    GL_DEPTH24_STENCIL8, GL_RED,        GL_GREEN,       GL_ZERO,        GL_ZERO,    32 },      // DDS_FORMAT_D24_UNORM_S8_UINT (MAY NOT BE CORRECT)
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    32 },      // DDS_FORMAT_R24_UNORM_X8_TYPELESS
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    32 },      // DDS_FORMAT_X24_TYPELESS_G8_UINT
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    16 },      // DDS_FORMAT_R8G8_TYPELESS
			{ GL_RG,                GL_UNSIGNED_BYTE,   GL_RG8,             GL_RED,         GL_GREEN,       GL_ZERO,        GL_ONE,     16 },      // DDS_FORMAT_R8G8_UNORM
			{ GL_RG_INTEGER,        GL_UNSIGNED_BYTE,   GL_RG8UI,           GL_RED,         GL_GREEN,       GL_ZERO,        GL_ONE,     16 },      // DDS_FORMAT_R8G8_UINT
			{ GL_RG,                GL_BYTE,            GL_RG8_SNORM,       GL_RED,         GL_GREEN,       GL_ZERO,        GL_ONE,     16 },      // DDS_FORMAT_R8G8_SNORM
			{ GL_RG_INTEGER,        GL_BYTE,            GL_RG8I,            GL_RED,         GL_GREEN,       GL_ZERO,        GL_ONE,     16 },      // DDS_FORMAT_R8G8_SINT
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    16 },      // DDS_FORMAT_R16_TYPELESS
			{ GL_RED,               GL_HALF_FLOAT,      GL_R16F,            GL_RED,         GL_ZERO,        GL_ZERO,        GL_ONE,     16 },      // DDS_FORMAT_R16_FLOAT
			{ GL_DEPTH_COMPONENT,   GL_HALF_FLOAT,      GL_DEPTH_COMPONENT16, GL_RED,       GL_ZERO,        GL_ZERO,        GL_ZERO,    16 },      // DDS_FORMAT_D16_UNORM
			{ GL_RED,               GL_UNSIGNED_SHORT,  GL_R16,             GL_RED,         GL_ZERO,        GL_ZERO,        GL_ONE,     16 },      // DDS_FORMAT_R16_UNORM
			{ GL_RED_INTEGER,       GL_UNSIGNED_SHORT,  GL_R16UI,           GL_RED,         GL_ZERO,        GL_ZERO,        GL_ONE,     16 },      // DDS_FORMAT_R16_UINT
			{ GL_RED,               GL_SHORT,           GL_R16_SNORM,       GL_RED,         GL_ZERO,        GL_ZERO,        GL_ONE,     16 },      // DDS_FORMAT_R16_SNORM
			{ GL_RED_INTEGER,       GL_SHORT,           GL_R16I,            GL_RED,         GL_ZERO,        GL_ZERO,        GL_ONE,     16 },      // DDS_FORMAT_R16_SINT
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    8 },       // DDS_FORMAT_R8_TYPELESS
			{ GL_RED,               GL_UNSIGNED_BYTE,   GL_R8,              GL_RED,         GL_ZERO,        GL_ZERO,        GL_ONE,     8 },       // DDS_FORMAT_R8_UNORM
			{ GL_RED_INTEGER,       GL_UNSIGNED_BYTE,   GL_R8UI,            GL_RED,         GL_ZERO,        GL_ZERO,        GL_ONE,     8 },       // DDS_FORMAT_R8_UINT
			{ GL_RED,               GL_BYTE,            GL_R8_SNORM,        GL_RED,         GL_ZERO,        GL_ZERO,        GL_ONE,     8 },       // DDS_FORMAT_R8_SNORM
			{ GL_RED_INTEGER,       GL_BYTE,            GL_R8I,             GL_RED,         GL_ZERO,        GL_ZERO,        GL_ONE,     8 },       // DDS_FORMAT_R8_SINT
			{ GL_RED,               GL_BYTE,            GL_R8,              GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_RED,     8 },       // DDS_FORMAT_A8_UNORM
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    0 },       // DDS_FORMAT_R1_UNORM
			{ GL_RGB,               GL_UNSIGNED_SHORT,  GL_RGB9_E5,         GL_RED,         GL_GREEN,       GL_BLUE,        GL_ONE,     16 },      // DDS_FORMAT_R9G9B9E5_SHAREDEXP
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    16 },      // DDS_FORMAT_R8G8_B8G8_UNORM
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO,    16 },      // DDS_FORMAT_G8R8_G8B8_UNORM
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC1_TYPELESS
		#if defined GL_EXT_texture_compression_s3tc
			{ GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_NONE, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_RED, GL_GREEN,  GL_BLUE,        GL_ONE              },      // DDS_FORMAT_BC1_UNORM
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC1_UNORM_SRGB
		#else
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC1_UNORM
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC1_UNORM_SRGB
		#endif
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC2_TYPELESS
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC2_UNORM
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC2_UNORM_SRGB
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC3_TYPELESS
		#if defined GL_EXT_texture_compression_s3tc
		//    { GL_COMPRESSED_RGB_S3TC_DXT3_EXT,  GL_NONE, GL_COMPRESSED_RGB_S3TC_DXT3_EXT, GL_ZERO, GL_ZERO, GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC3_UNORM
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC3_UNORM
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC3_UNORM_SRGB
		#else
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC3_UNORM
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC3_UNORM_SRGB
		#endif
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC4_TYPELESS
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC4_UNORM
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC4_SNORM
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC5_TYPELESS
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC5_UNORM
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC5_SNORM
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_B5G6R5_UNORM
			{ GL_RGBA,              GL_UNSIGNED_SHORT,  GL_RGB5_A1,         GL_RED,         GL_GREEN,       GL_BLUE,        GL_ALPHA            },      // DDS_FORMAT_B5G5R5A1_UNORM
			{ GL_RGBA,              GL_UNSIGNED_BYTE,   GL_RGBA8,           GL_BLUE,        GL_GREEN,       GL_RED,         GL_ALPHA            },      // DDS_FORMAT_B8G8R8A8_UNORM
			{ GL_RGBA,              GL_UNSIGNED_BYTE,   GL_RGBA8,           GL_RED,         GL_GREEN,       GL_BLUE,        GL_ONE              },      // DDS_FORMAT_B8G8R8X8_UNORM
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_R10G10B10_XR_BIAS_A2_UNORM
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_B8G8R8A8_TYPELESS
			{ GL_RGBA,              GL_UNSIGNED_BYTE,   GL_SRGB8_ALPHA8,    GL_BLUE,        GL_GREEN,       GL_RED,         GL_ALPHA            },      // DDS_FORMAT_B8G8R8A8_UNORM_SRGB
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_B8G8R8X8_TYPELESS
			{ GL_RGBA,              GL_UNSIGNED_BYTE,   GL_SRGB8_ALPHA8,    GL_BLUE,        GL_GREEN,       GL_RED,         GL_ONE              },      // DDS_FORMAT_B8G8R8X8_UNORM_SRGB
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC6H_TYPELESS
			{ GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB, GL_NONE, GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB, GL_RED, GL_GREEN, GL_BLUE,     GL_ONE          },      // DDS_FORMAT_BC6H_UF16
			{ GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB, GL_NONE, GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB, GL_RED, GL_GREEN, GL_BLUE, GL_ONE         },   // DDS_FORMAT_BC6H_SF16
			{ GL_NONE,              GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_BC7_TYPELESS
			{ GL_COMPRESSED_RGBA_BPTC_UNORM_ARB, GL_NONE, GL_COMPRESSED_RGBA_BPTC_UNORM_ARB, GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA            },      // DDS_FORMAT_BC7_UNORM
			{ GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB, GL_NONE, GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB, GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA         }, // DDS_FORMAT_BC7_UNORM_SRGB
			{ GL_NONE,          GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_AYUV
			{ GL_NONE,          GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_Y410
			{ GL_NONE,          GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_Y416
			{ GL_NONE,          GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_NV12
			{ GL_NONE,          GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_P010
			{ GL_NONE,          GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_P016
			{ GL_NONE,          GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_420_OPAQUE
			{ GL_NONE,          GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_YUY2
			{ GL_NONE,          GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_Y210
			{ GL_NONE,          GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_Y216
			{ GL_NONE,          GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_NV11
			{ GL_NONE,          GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_AI44
			{ GL_NONE,          GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_IA44
			{ GL_NONE,          GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_P8
			{ GL_NONE,          GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_A8P8
			{ GL_NONE,          GL_NONE,            GL_NONE,            GL_ZERO,        GL_ZERO,        GL_ZERO,        GL_ZERO             },      // DDS_FORMAT_B4G4R4A4_UNORM
		};

		static constexpr int NUM_DDS_FORMATS = (sizeof(gl_info_table) / sizeof(gl_info_table[0]));

		bool 
			vgl_DDSHeaderToImageDataHeader(const DDS_FILE_HEADER& header, vglImageData& image)
			{
			if (header.std_header.ddspf.dwFlags == DDS_DDPF_FOURCC &&
				header.std_header.ddspf.dwFourCC == DDS_FOURCC_DX10)
				{
				if (header.dxt10_header.format < NUM_DDS_FORMATS)
					{
					const DDS_FORMAT_GL_INFO& format = gl_info_table[header.dxt10_header.format];
					image.format = format.format;
					image.type = format.type;
					image.internalFormat = format.internalFormat;
					image.swizzle[0] = format.swizzle_r;
					image.swizzle[1] = format.swizzle_g;
					image.swizzle[2] = format.swizzle_b;
					image.swizzle[3] = format.swizzle_a;
					image.mipLevels = header.std_header.mip_levels;
					return true;
					}
				}
			else if (header.std_header.ddspf.dwFlags == DDS_DDPF_FOURCC)
				{
				image.swizzle[0] = GL_RED;
				image.swizzle[1] = GL_GREEN;
				image.swizzle[2] = GL_BLUE;
				image.swizzle[3] = GL_ALPHA;
				image.mipLevels = header.std_header.mip_levels;

				switch (header.std_header.ddspf.dwFourCC)
					{
					case 116:
						image.format = GL_RGBA;
						image.type = GL_FLOAT;
						image.internalFormat = GL_RGBA32F;
						/*
						image->swizzle[0] = GL_ALPHA;
						image->swizzle[1] = GL_BLUE;
						image->swizzle[2] = GL_GREEN;
						image->swizzle[3] = GL_RED;
						*/
						return true;
					default:
						break;
					}
				}
			else
				{
				image.swizzle[0] = GL_RED;
				image.swizzle[1] = GL_GREEN;
				image.swizzle[2] = GL_BLUE;
				image.swizzle[3] = GL_ALPHA;
				image.mipLevels = header.std_header.mip_levels;

				switch (header.std_header.ddspf.dwFlags)
					{
					case DDS_DDPF_RGB:
						image.format = GL_BGR;
						image.type = GL_UNSIGNED_BYTE;
						image.internalFormat = GL_RGB8;
						image.swizzle[3] = GL_ONE;
						return true;
					case (DDS_DDPF_RGB | DDS_DDPF_ALPHA):
					case (DDS_DDPF_RGB | DDS_DDPF_ALPHAPIXELS):
						image.format = GL_BGRA;
						image.type = GL_UNSIGNED_BYTE;
						image.internalFormat = GL_RGBA8;
						return true;
					case DDS_DDPF_ALPHA:
						image.format = GL_RED;
						image.type = GL_UNSIGNED_BYTE;
						image.internalFormat = GL_R8;
						image.swizzle[0] = image.swizzle[1] = image.swizzle[2] = GL_ZERO;
						image.swizzle[3] = GL_RED;
						return true;
					case DDS_DDPF_LUMINANCE:
						image.format = GL_RED;
						image.type = GL_UNSIGNED_BYTE;
						image.internalFormat = GL_R8;
						image.swizzle[0] = image.swizzle[1] = image.swizzle[2] = GL_RED;
						image.swizzle[3] = GL_ONE;
						return true;
					case (DDS_DDPF_LUMINANCE | DDS_DDPF_ALPHA):
						image.format = GL_RG;
						image.type = GL_UNSIGNED_BYTE;
						image.internalFormat = GL_RG8;
						image.swizzle[0] = image.swizzle[1] = image.swizzle[2] = GL_RED;
						image.swizzle[3] = GL_GREEN;
						return true;
					default:
						break;
					}
				}

			image.format = image.type = image.internalFormat = GL_NONE;
			image.swizzle[0] = image.swizzle[1] = image.swizzle[2] = image.swizzle[3] = GL_ZERO;

			return false;
			}

		GLsizei
			vgl_GetDDSStride(const DDS_FILE_HEADER& header, GLsizei width)
			{
			if (header.std_header.ddspf.dwFlags == DDS_DDPF_FOURCC &&
				header.std_header.ddspf.dwFourCC == DDS_FOURCC_DX10)
				{
				if (header.dxt10_header.format < NUM_DDS_FORMATS)
					{
					const DDS_FORMAT_GL_INFO& format = gl_info_table[header.dxt10_header.format];
					return (format.bits_per_texel * width + 7) / 8;
					}
				}
			else
				{
				switch (header.std_header.ddspf.dwFlags)
					{
					case DDS_DDPF_RGB:
						return width * 3;
					case (DDS_DDPF_RGB | DDS_DDPF_ALPHA):
					case (DDS_DDPF_RGB | DDS_DDPF_ALPHAPIXELS):
						return width * 4;
					case DDS_DDPF_ALPHA:
						return width;
					default:
						break;
					}
				}

			return 0;
			}

		GLenum 
			vgl_GetTargetFromDDSHeader(const DDS_FILE_HEADER& header)
			{
			// If the DX10 header is present it's format should be non-zero (unless it's unknown)
			if (header.dxt10_header.format != 0)
				{
				// Check the dimension...
				switch (header.dxt10_header.dimension)
					{
						// Could be a 1D or 1D array texture
					case DDS_RESOURCE_DIMENSION_TEXTURE1D:
						if (header.dxt10_header.array_size > 1)
						{
							return GL_TEXTURE_1D_ARRAY;
						}
						return GL_TEXTURE_1D;
						// 2D means 2D, 2D array, cubemap or cubemap array
					case DDS_RESOURCE_DIMENSION_TEXTURE2D:
						if (header.dxt10_header.misc_flag & DDS_RESOURCE_MISC_TEXTURECUBE)
						{
							if (header.dxt10_header.array_size > 1)
								return GL_TEXTURE_CUBE_MAP_ARRAY;
							return GL_TEXTURE_CUBE_MAP;
						}
						if (header.dxt10_header.array_size > 1)
							return GL_TEXTURE_2D_ARRAY;
						return GL_TEXTURE_2D;
						// 3D should always be a volume texture
					case DDS_RESOURCE_DIMENSION_TEXTURE3D:
						return GL_TEXTURE_3D;
					}
				return GL_NONE;
				}

			// No DX10 header. Check volume texture flag
			if (header.std_header.caps2 & DDSCAPS2_VOLUME)
				return GL_TEXTURE_3D;

			// Could be a cubemap
			if (header.std_header.caps2 & DDSCAPS2_CUBEMAP)
				{
				// This shouldn't happen if the DX10 header is present, but what the hey
				if (header.dxt10_header.array_size > 1)
					return GL_TEXTURE_CUBE_MAP_ARRAY;
				else
					return GL_TEXTURE_CUBE_MAP;
				}

			// Alright, if there's no height, guess 1D
			if (header.std_header.height <= 1)
				return GL_TEXTURE_1D;

			// Last ditch, probably 2D
			return GL_TEXTURE_2D;
			}

		void 
			vglLoadDDS(const char* filename, vglImageData& image)
			{
			FILE* f;

			fopen_s(&f, filename, "rb");

			if (f == NULL) 
				{	// open file faild
				return;
				}
				
			DDS_FILE_HEADER file_header = { 0, };

			fread(&file_header, sizeof(file_header.magic) + sizeof(file_header.std_header), 1, f);

			if (file_header.magic != DDS_MAGIC)
				{
				fclose(f);
				return;
				}

			if (file_header.std_header.ddspf.dwFourCC == DDS_FOURCC_DX10)
				{
				fread(&file_header.dxt10_header, sizeof(file_header.dxt10_header), 1, f);
				}

			if (!vgl_DDSHeaderToImageDataHeader(file_header, image)) 
				{
				fclose(f);
				return;
				}

			image.target = vgl_GetTargetFromDDSHeader(file_header);

			if (image.target == GL_NONE) 
				{
				fclose(f);
				return;
				}

			size_t current_pos = ftell(f);
			size_t file_size;
			fseek(f, 0, SEEK_END);
			file_size = ftell(f);
			fseek(f, (long)current_pos, SEEK_SET);

			image.totalDataSize = file_size - current_pos;
			image.mip[0].data = new uint8_t[image.totalDataSize];

			fread(image.mip[0].data, file_size - current_pos, 1, f);

			int level;
			GLubyte * ptr = reinterpret_cast<GLubyte*>(image.mip[0].data);

			int width = file_header.std_header.width;
			int height = file_header.std_header.height;
			int depth = file_header.std_header.depth;

			image.sliceStride = 0;

			if (image.mipLevels == 0)
				{
				image.mipLevels = 1;
				}

			for (level = 0; level < image.mipLevels; ++level)
				{
				image.mip[level].data = ptr;
				image.mip[level].width = width;
				image.mip[level].height = height;
				image.mip[level].depth = depth;
				image.mip[level].mipStride = vgl_GetDDSStride(file_header, width) * height;
				image.sliceStride += image.mip[level].mipStride;
				ptr += image.mip[level].mipStride;
				width >>= 1;
				height >>= 1;
				depth >>= 1;
				}

			fclose(f);
			}

		void 
			vglUnloadImage(vglImageData& image)
			{
			delete[] reinterpret_cast<uint8_t *>(image.mip[0].data);
			}

		void set_source(const std::string& _Filename) 
			{
			_Myfilename = _Filename;
			}

		void operator() (GLuint& _Dst) 
			{
			GLuint _Texture;
			vglImageData image;
			int level;


			vglLoadDDS(_Myfilename.c_str(), image);

			glGenTextures(1, &_Texture);

			glBindTexture(image.target, _Texture);

			GLubyte * ptr = (GLubyte *)image.mip[0].data;

			switch (image.target)
				{
				case GL_TEXTURE_1D:
					glTexStorage1D(image.target,
						image.mipLevels,
						image.internalFormat,
						image.mip[0].width);
					for (level = 0; level < image.mipLevels; ++level)
					{
						glTexSubImage1D(GL_TEXTURE_1D,
							level,
							0,
							image.mip[level].width,
							image.format, image.type,
							image.mip[level].data);
					}
					break;
				case GL_TEXTURE_1D_ARRAY:
					glTexStorage2D(image.target,
						image.mipLevels,
						image.internalFormat,
						image.mip[0].width,
						image.slices);
					for (level = 0; level < image.mipLevels; ++level)
					{
						glTexSubImage2D(GL_TEXTURE_1D,
							level,
							0, 0,
							image.mip[level].width, image.slices,
							image.format, image.type,
							image.mip[level].data);
					}
					break;
				case GL_TEXTURE_2D:
					glTexStorage2D(image.target,
						image.mipLevels,
						image.internalFormat,
						image.mip[0].width,
						image.mip[0].height);
					for (level = 0; level < image.mipLevels; ++level)
					{
						glTexSubImage2D(GL_TEXTURE_2D,
							level,
							0, 0,
							image.mip[level].width, image.mip[level].height,
							image.format, image.type,
							image.mip[level].data);
					}
					break;
				case GL_TEXTURE_CUBE_MAP:
					for (level = 0; level < image.mipLevels; ++level)
					{
						ptr = (GLubyte *)image.mip[level].data;
						for (int face = 0; face < 6; face++)
						{
							glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
								level,
								image.internalFormat,
								image.mip[level].width, image.mip[level].height,
								0,
								image.format, image.type,
								ptr + image.sliceStride * face);
						}
					}
					break;
				case GL_TEXTURE_2D_ARRAY:
					glTexStorage3D(image.target,
						image.mipLevels,
						image.internalFormat,
						image.mip[0].width,
						image.mip[0].height,
						image.slices);
					for (level = 0; level < image.mipLevels; ++level)
					{
						glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
							level,
							0, 0, 0,
							image.mip[level].width, image.mip[level].height, image.slices,
							image.format, image.type,
							image.mip[level].data);
					}
					break;
				case GL_TEXTURE_CUBE_MAP_ARRAY:
					glTexStorage3D(image.target,
						image.mipLevels,
						image.internalFormat,
						image.mip[0].width,
						image.mip[0].height,
						image.slices);
					break;
				case GL_TEXTURE_3D:
					glTexStorage3D(image.target,
						image.mipLevels,
						image.internalFormat,
						image.mip[0].width,
						image.mip[0].height,
						image.mip[0].depth);
					for (level = 0; level < image.mipLevels; ++level)
					{
						glTexSubImage3D(GL_TEXTURE_3D,
							level,
							0, 0, 0,
							image.mip[level].width, image.mip[level].height, image.mip[level].depth,
							image.format, image.type,
							image.mip[level].data);
					}
					break;
				default:
					break;
				}

			glTexParameteriv(image.target, GL_TEXTURE_SWIZZLE_RGBA, reinterpret_cast<const GLint *>(image.swizzle));

			vglUnloadImage(image);

			_Dst = _Texture;
			}

		std::string _Myfilename;
	};

	void gl_create_texture_dds(GLuint& _Dst, const std::string& _Filename,
		GLenum _Xwrap, GLenum _Ywrap, GLenum _Fltmag, GLenum _Fltmin)
		{
		_DDS_reader _Reader;
		_Reader.set_source(_Filename);
		_Reader(_Dst);

		//const int D3DFMT_DXT1 = (('D' << 0) | ('X' << 8) | ('T' << 16) | ('1' << 24));
		//const int D3DFMT_DXT2 = (('D' << 0) | ('X' << 8) | ('T' << 16) | ('2' << 24));
		//const int D3DFMT_DXT3 = (('D' << 0) | ('X' << 8) | ('T' << 16) | ('3' << 24));
		//const int D3DFMT_DXT4 = (('D' << 0) | ('X' << 8) | ('T' << 16) | ('4' << 24));
		//const int D3DFMT_DXT5 = (('D' << 0) | ('X' << 8) | ('T' << 16) | ('5' << 24));

		//gl_delete_texture(_Dst);

		//glGenTextures(1, &_Dst);
		//glBindTexture(GL_TEXTURE_2D, _Dst);

		//if (In_Compressformat == D3DFMT_DXT1)
		//{
		//	size_t _Bufsize = maxval(size_t(1), (_Width + 3) >> 2)
		//		* maxval(size_t(1), (_Height + 3) >> 2) * 8;
		//	glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
		//		static_cast<GLsizei>(_Width), static_cast<GLsizei>(_Height), 0,
		//		static_cast<GLsizei>(_Bufsize), _Data);
		//}
		//else if (In_Compressformat == D3DFMT_DXT3)
		//{
		//	size_t _Bufsize = maxval(size_t(1), (_Width + 3) >> 2)
		//		* maxval(size_t(1), (_Height + 3) >> 2) * 16;
		//	glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
		//		static_cast<GLsizei>(_Width), static_cast<GLsizei>(_Height), 0,
		//		static_cast<GLsizei>(_Bufsize), _Data);
		//}
		//else if (In_Compressformat == D3DFMT_DXT5)
		//{
		//	size_t _Bufsize = maxval(size_t(1), (_Width + 3) >> 2)
		//		* maxval(size_t(1), (_Height + 3) >> 2) * 16;
		//	glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
		//		static_cast<GLsizei>(_Width), static_cast<GLsizei>(_Height), 0,
		//		static_cast<GLsizei>(_Bufsize), _Data);
		//}
		//else if (In_Compressformat == D3DFMT_DXT2 || In_Compressformat == D3DFMT_DXT4)
		//{	// opengl not suppert
		//	logcout << "Warning[ not suppert dds texture compress ] In[ gl_create_texture_dds(...) ]";
		//	_Dst = -1;
		//	return;
		//}
		//else
		//{
		//	logcout << "Warning[ not suppert dds texture compress ] In[ gl_create_texture_dds(...) ]";
		//	_Dst = -1;
		//	return;
		//}

		//glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _Xwrap);
		//glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _Ywrap);
		//glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _Fltmin);
		//glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _Fltmag);
		//glGenerateMipmap(GL_TEXTURE_2D);

		//glBindTexture(GL_TEXTURE_2D, 0);
		}

}// namespace clmagic