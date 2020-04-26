#pragma once
#ifndef __GLUT___GLSHADER___H__
#define __GLUT___GLSHADER___H__
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "GL/glew.h"
#include "noncopyable.h"
#include "gltexture.h"
#include "glbuffer.h"
#include "glvarray.h"

namespace gl {
	enum class eShaderTarget {
		Undefined            = GL_NONE,
		VertexShader	     = GL_VERTEX_SHADER,
		TessControlShader    = GL_TESS_CONTROL_SHADER,
		TessEvaluationShader = GL_TESS_EVALUATION_SHADER,
		GeometryShader		 = GL_GEOMETRY_SHADER,
		FragmentShader       = GL_FRAGMENT_SHADER,
		ComputeShader        = GL_COMPUTE_SHADER
	};

	enum class eShaderStatus {
		Compile  = GL_COMPILE_STATUS,
		Delete   = GL_DELETE_STATUS,
		VALIDATE = GL_VALIDATE_STATUS
	};




	inline std::string read_file(const std::string& _Filename) {
		auto _Fin = std::fstream();
		_Fin.open(_Filename, std::ios::in);

		_Fin.seekg(0, std::ios::end);
		auto _Result = std::string(size_t(_Fin.tellg()), ' ');
		_Fin.seekg(std::ios::beg);
		_Fin.read(&_Result[0], _Result.size());

		_Fin.close();
		return (_Result);
	};

	inline void shader_delete(GLuint& _Shader) {
		if (glIsShader(_Shader)) {
			glDeleteShader(_Shader);
		}
		_Shader = -1;
	}

	inline std::string shader_infolog(GLuint _Shader) {
		auto _Messize = GLsizei(0);
		glGetShaderiv(_Shader, GL_INFO_LOG_LENGTH, &_Messize);
		auto _Message = std::string(static_cast<size_t>(_Messize), ' ');
		glGetShaderInfoLog(_Shader, _Messize, &_Messize, &_Message[0]);
		return (_Message);
	}

	inline GLuint compile_shader(GLenum _Target, const GLchar* const* _Source) {
		auto _Shader = glCreateShader(_Target);
		glShaderSource( _Shader, 1, _Source, nullptr);
		glCompileShader(_Shader);
	
		GLint _Status;
		glGetShaderiv(_Shader, GL_COMPILE_STATUS, &_Status);
		if (_Status == GL_FALSE) {
			std::string _Message = shader_infolog(_Shader);
			glDeleteShader(_Shader);
			throw std::exception(_Message.c_str());
		}

		return (_Shader);
	}

	inline void program_delete(GLuint& _Shaderprogram) {
		if (glIsProgram(_Shaderprogram)) {
			glDeleteProgram(_Shaderprogram);
		}
		_Shaderprogram = -1;
	}

	inline bool program_link_status(GLuint _Shaderprogram) {
		GLint _Result = GL_FALSE;
		glGetProgramiv(_Shaderprogram, GL_LINK_STATUS, &_Result);
		return (_Result == GL_FALSE ? false : true);
	}

	inline std::string program_infolog(GLuint _Program) {
		auto _Messize = GLint( 0 );
		glGetProgramiv( _Program, GL_INFO_LOG_LENGTH, &_Messize );
		auto _Message = std::string( static_cast<size_t>(_Messize), ' ' );
		glGetProgramInfoLog( _Program, _Messize, &_Messize, (GLchar*)(_Message.data()) );
		return ( _Message );
	}

	inline std::vector<GLuint> program_attached_shaders(GLuint _Shaderprogram) {
		const GLint _Maxcount = 10;
		GLuint _Shaders[_Maxcount];
		GLsizei _Size = 0;
		glGetAttachedShaders(_Shaderprogram, _Maxcount, &_Size, _Shaders);
		return ( std::vector<GLuint>(_Shaders, _Shaders + _Size) );
	}

	inline GLuint program_create(const GLuint* _Shaders, size_t _Size) {
		GLuint _Program = glCreateProgram();
		while (_Size--) {
			glAttachShader(_Program, _Shaders[_Size]);
		}
		glLinkProgram(_Program);

		if (program_link_status(_Program) == GL_FALSE) {
			std::string _Message = program_infolog(_Program);
			glDeleteProgram(_Program);
			throw std::exception( _Message.c_str() );
		}

		return ( _Program );
	}

	inline GLuint program_create(const std::vector<GLuint>& _Shaders) {
		return ( program_create(&_Shaders[0], _Shaders.size()) );
	}

	inline GLuint program_relink(GLuint& _Program, GLuint* _New_shaders, GLuint _Size) {
		auto _Old_shaders = program_attached_shaders(_Program);
		program_delete(_Program);

		auto _Shaders = std::vector<GLuint>(_Old_shaders.size() + _Size);
		auto _ResIt   = std::set_union(
			_Old_shaders.begin(), _Old_shaders.end(), _New_shaders, _New_shaders + _Size,
			_Shaders.begin());
		_Shaders.erase(_ResIt, _Shaders.end());
		return ( program_create(_Shaders.data(), _Shaders.size()) );
	}

	inline GLuint program_create(const std::string& _Vert, const std::string& _Frag) {
		std::vector<GLuint> _Shaders;
		auto* _Ptr1 = _Vert.c_str();
		auto* _Ptr2 = _Frag.c_str();
		_Shaders.push_back( compile_shader(GL_VERTEX_SHADER, &_Ptr1) );
		_Shaders.push_back( compile_shader(GL_FRAGMENT_SHADER, &_Ptr2) );
		GLuint _Prog = program_create(_Shaders);
		glDeleteShader(_Shaders[0]);
		glDeleteShader(_Shaders[1]);
		return (_Prog);
	}

	enum class eUniformBlockStatus {
		Binding = GL_UNIFORM_BLOCK_BINDING,
		DataSize = GL_UNIFORM_BLOCK_DATA_SIZE,
		NameLength = GL_UNIFORM_BLOCK_NAME_LENGTH,
		ActiveUniforms = GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,
		ActiveUniformIndices = GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,
		MaxUniformBlockSize = GL_MAX_UNIFORM_BLOCK_SIZE
	};

	inline GLuint uniform_block_status(GLuint _Program, GLuint _Blockindex, eUniformBlockStatus _Enum) {
		GLint _Status;
		glGetActiveUniformBlockiv(_Program, _Blockindex, (GLenum)_Enum, &_Status);
		return (_Status);
	}



	namespace glsl {

		

		using shader_source = std::string;

		/*
		@_Interface: glsl shader
		*/
		struct shader {
			virtual ~shader() { /* empty */ }
			virtual GLuint       id() const = 0;
			virtual eShaderTarget target() const = 0;
			virtual std::string  source() const = 0;
			virtual const char*  source_ptr() const = 0;
		};


		/* 
		@_basic_shader: ...
		@_Describ: 
		@_Noncopyable: When the copied shader is destroyed, the shader will be inexplicably useless, which is unreasonable
					   Support move constructor, and compiler can do statements 
					   like following:
						   vertex_shader _Vert  = "#version 330 core\n void main(){  }";
						   vertex_shader _Vert2 = std::string("#version 330 core\n");
		*/
		template<eShaderTarget _Target>
		struct basic_shader : public shader, public _Noncopyable {
			basic_shader() : _Myid(0), _Mysource() {  }
		
			basic_shader(const char* _Source) : basic_shader() {
				_Myid     = compile_shader((GLenum)_Target, &_Source);
				_Mysource = _Source;
			}

			basic_shader(const shader_source& _Source)
				: basic_shader(_Source.c_str()) {
				/* empty */
			}

			basic_shader(basic_shader&& _Right) noexcept
				: _Myid(_Right._Myid), _Mysource(_Right._Mysource) {
				_Right._Myid = 0;
			}

			~basic_shader() {
				if (glIsShader(_Myid)) {
					glDeleteShader(_Myid);
				}
			}
			
			basic_shader& operator=(basic_shader&& _Right) {
				this->destroy();
				this->_Myid     = _Right._Myid;
				this->_Mysource = _Right._Mysource;
				_Right._Myid    = 0;
				return (*this);
			}

			basic_shader& operator=(const char* _Source) {
				(*this) = basic_shader<_Target>(_Source);
				return ( *this );
			}

			void destroy() {
				if (glIsShader(_Myid)) {
					glDeleteShader(_Myid);
				}
			}

			GLuint       id() const override { return (_Myid);  }
			eShaderTarget target() const override { return (_Target); }
			std::string  source()    const override { return (_Mysource); }
			const char*  source_ptr()   const override { return (&_Mysource[0]); }

			GLuint        _Myid;
			shader_source _Mysource;
		};
		using vertex_shader          = basic_shader<eShaderTarget::VertexShader>;
		using geometry_shader        = basic_shader<eShaderTarget::GeometryShader>;
		using tess_evaluation_shader = basic_shader<eShaderTarget::TessEvaluationShader>;
		using tess_control_shader    = basic_shader<eShaderTarget::TessControlShader>;
		using fragment_shader        = basic_shader<eShaderTarget::FragmentShader>;
		using compute_shader         = basic_shader<eShaderTarget::ComputeShader>;



		/*
		                              readonly  read_write     readonly       readonly
		@_Interface: Shader-Resource: sampler*,   image*,   uniform varient, uniform_block
		@_Describ: This series of classes don't manage real memory, for example: @Free, @Alloc
		*/
		struct resource {
			virtual void ready() const = 0;
			virtual void done() const = 0;
		};


		/*
		@_inputlayout:
		*/
		struct inputlayout
			: public resource {
			inputlayout() = default;

			inputlayout(GLuint _Buffer, VertexPointer* _Vin, size_t _Size)
				: _Mydesc{ _Vin, _Size, _Buffer, 0 } {
				inputlayout_create(_Mydesc);
				/* empty */
			}

			~inputlayout() {
				inputlayout_delete(_Mydesc._Inputlayout);
			}

			virtual void ready() const override {
				glBindVertexArray(_Mydesc._Inputlayout);
			}

			virtual void done() const override {
				glBindVertexArray(0);
			}

			VertexArrayDesc _Mydesc;
		};


		/*
		@_Noncopyable: this struct NON COPY
		@_Binding_index is opengl buffer common index
		@_Binding_point is private index with @program 
		*/
		template<typename _Ty>
		struct uniform_block : public _Ty, public resource {
			GLuint _Binding_index  = 0;
			GLuint _Binding_point  = 0;
			uniform_buffer<_Ty> _Binding_buffer;
			//GLuint _Binding_buffer = 0;// !!!Error
			GLuint _Program_id     = 0;

			uniform_block() = default;
			
			uniform_block(uniform_block&& _Right) : _Ty(_Right),
				_Binding_index(_Right._Binding_index),
				_Binding_point(_Right._Binding_point),
				_Binding_buffer(std::move(_Right._Binding_buffer)),
				_Program_id(_Right._Program_id){
				_Right._Binding_index = 0;
				_Right._Binding_point = 0;
				_Right._Program_id = 0;
			}

			uniform_block& operator=(uniform_block&& _Right) {
				this->_Binding_index      = _Right._Binding_index;
				this->_Binding_point      = _Right._Binding_point;
				this->_Binding_buffer     = std::move(_Right._Binding_buffer);
				this->_Program_id         = _Right._Program_id;
				dynamic_cast<_Ty&>(*this) = _Right;
				_Right._Binding_index = 0;
				_Right._Binding_point = 0;
				_Right._Program_id    = 0;
				return (*this);
			}

			void   update() { 
				buffer_copy<eBufferTarget::UniformBuffer, _Ty>(_Binding_buffer.id(), *this); 
			}

			GLuint infolog(eUniformBlockStatus _Enum) const {
				return (uniform_block_status(_Program_id, _Binding_index, _Enum)); 
			}

			virtual void ready() const override {
				glBindBufferBase(GL_UNIFORM_BUFFER, _Binding_index, _Binding_buffer.id()); 
			}

			virtual void done() const override { 
				glBindBufferBase(GL_UNIFORM_BUFFER, _Binding_index, 0);
			}
		};
		

		/*
		@_Sampler2D:
			Only used for the corresponding texture
			Readonly data in shader, 
			Sample pass linear vector, for example vec2(0.3, 0.7)
		*/
		template<eTextureInternalFormat _Fmt>
		struct sampler2D : public resource {
			using texture_type = typename texture2D<_Fmt>;

			virtual void ready() const override { 
				if ( not _Texture.expired() ) {
					glActiveTexture(GL_TEXTURE0 + _Texture_unit);
					glBindTexture(GL_TEXTURE_2D, _Texture.lock()->id());
				}		
			}

			virtual void done() const override { 
				glActiveTexture(GL_TEXTURE0 + _Texture_unit);
				glBindTexture(GL_TEXTURE_2D, 0/*unbind*/);
			}
			
			GLuint _Texture_unit = 0;
			std::weak_ptr<texture_type> _Texture;
		};


		/*
		@_Sampler3D: 
			readonly data in shader,
			sample pass linear vector, for example vec3(0.5, 0.5, 0.5)
		*/
		template<eTextureInternalFormat _Fmt>
		struct sampler3D : public resource {
			using texture_type = typename texture3D<_Fmt>;

			virtual void ready() const override {
				if (not _Texture.expired()) {
					texture_bind(GL_TEXTURE0 + _Texture_unit, GL_TEXTURE_3D, _Texture.lock()->id());
				}
			}

			virtual void done() const override {
				texture_bind(GL_TEXTURE0 + _Texture_unit, GL_TEXTURE_3D, 0);
			}

			GLuint _Texture_unit = 0;
			std::weak_ptr<texture_type> _Texture;
		};


		/*
		@_sampler2D_array:
		*/
		template<eTextureInternalFormat _Fmt>
		struct sampler2D_array : public resource {
			using texture_type = typename texture2D_array<_Fmt>;

			virtual void ready() const override {
				if (not _Texture.expired()) {
					texture_bind(GL_TEXTURE0 + _Texture_unit, GL_TEXTURE_2D_ARRAY, _Texture.lock()->id());
				}
			}

			virtual void done() const override {
				texture_bind(GL_TEXTURE0 + _Texture_unit, GL_TEXTURE_2D_ARRAY, 0);
			}

			GLuint _Texture_unit = 0;
			std::weak_ptr<texture_type> _Texture;
		};


		template<eTextureInternalFormat _Fmt>
		struct sampler2D_array_shadow : public sampler2D_array<_Fmt>{
			using texture_type = typename texture2D_array<_Fmt>;

		};




		/*
		@_Relation: ->gl::glsl::program, ->gl::glsl::program::make_image2D(...)
		@_Image2D: 
			read write data in shader,
			read pass imageLoad(image2D, ivec2)
			write pass imageStore(image2D, ivec2, vec4())
		@_Other:
			vec4 imageExchange(image2D, ivec2, vec4())
		@_Note:
			unsupport rgb*
		*/
		template<eTextureInternalFormat _Fmt>
		struct image2D : public resource {
			static_assert(_Fmt != eTextureInternalFormat::RGB16f && _Fmt != eTextureInternalFormat::RGB16i && _Fmt != eTextureInternalFormat::RGB16ui,
				"OpenGL shader not have rgb* image*");
			using texture_type = texture2D<_Fmt>;

			virtual void ready() const override {
				if (not _Texture.expired()) {
					glBindImageTexture(_Image_unit, _Texture.lock()->id(), 0, false, _Image_slice, _Image_access, (GLenum)_Fmt);
				}
			}

			virtual void done() const override {
				glBindImageTexture(_Image_unit, 0/*unbind*/, 0, false, _Image_slice, _Image_access, (GLenum)_Fmt);
			}

			GLuint _Image_unit   = 0;
			GLint  _Image_slice  = 0;
			GLenum _Image_access = GL_READ_WRITE;
			std::weak_ptr<texture_type> _Texture;
		};


		/*
		@_program
		*/
		struct program : public _Noncopyable {
			struct _Program_data {
				std::vector<GLuint> block_indices;
			};

			program() : _Myid(0), _Myvert(), _Mygeom(), _Mytese(), _Mytesc(), _Myfrag(), _Mycomp() { }
		
			program(vertex_shader&& _Vert, fragment_shader&& _Frag) : program() {
				_Myvert.reset( new vertex_shader(std::move(_Vert)) );
				_Myfrag.reset( new fragment_shader(std::move(_Frag)) );
				this->link();
			}

			program(compute_shader&& _Comp) : program() {
				_Mycomp.reset( new compute_shader(std::move(_Comp)) );
				this->link();
			}

			program(program&& _Right) noexcept
				: _Myid(_Right._Myid), 
				_Myvert(std::move(_Right._Myvert)),
				_Mygeom(std::move(_Right._Mygeom)),
				_Mytese(std::move(_Right._Mytese)),
				_Mytesc(std::move(_Right._Mytesc)),
				_Myfrag(std::move(_Right._Myfrag)),
				_Mycomp(std::move(_Right._Mycomp)) {
				_Right._Myid = 0;
			}

			program& operator=(program&& _Right) noexcept {
				if (/*_Right != *this*/true) {
					this->destroy();
					_Myid = _Right._Myid; _Right._Myid = 0;
					_Myvert = std::move(_Right._Myvert);
					_Mygeom = std::move(_Right._Mygeom);
					_Mytese = std::move(_Right._Mytese);
					_Mytesc = std::move(_Right._Mytesc);
					_Myfrag = std::move(_Right._Myfrag);
					_Mycomp = std::move(_Right._Mycomp);
				}
				return (*this);
			}

			~program() {
				this->destroy();
			}

			void destroy() {
				if (glIsProgram(_Myid)) {
					glDeleteProgram(_Myid);
					_Mydata.block_indices.clear();
				}
			}

			void link() {
				// 1.
				this->destroy();

				// 2.
				_Myid = glCreateProgram();
				if (_Myvert != nullptr) { glAttachShader(_Myid, _Myvert->id()); }
				if (_Mygeom != nullptr) { glAttachShader(_Myid, _Mygeom->id()); }
				if (_Mytese != nullptr) { glAttachShader(_Myid, _Mytese->id()); }
				if (_Mytesc != nullptr) { glAttachShader(_Myid, _Mytesc->id()); }
				if (_Myfrag != nullptr) { glAttachShader(_Myid, _Myfrag->id()); }
				if (_Mycomp != nullptr) { glAttachShader(_Myid, _Mycomp->id()); }

				// 3.
				glLinkProgram(_Myid);

				// 4.
				if (!program_link_status(_Myid)) {
					auto _Message = program_infolog(_Myid);
					glDeleteProgram(_Myid);
					throw std::exception(_Message.c_str());
				}
				/*
				1. destroy old program(without shaders)
				2. attached shaders
				3. link
				4. check error
				*/
			}

			std::string infolog() const {
				return (program_infolog(_Myid));
			}

			template<typename _Ty>
			uniform_block<_Ty> make_uniform_block(const std::string& _Name) {
				uniform_block<_Ty> _Block;

				/* 1. */
				buffer_descriptor _Desc;
				_Desc._Usage   = eBufferUsage::DynamicDraw;
				_Desc._Target  = eBufferTarget::UniformBuffer;
				_Desc._Capcity = sizeof(_Ty);
				_Block._Binding_buffer = uniform_buffer<_Ty>(_Desc);
				//glGenBuffers(1, &_Block._Binding_buffer);
				//glBindBuffer(GL_UNIFORM_BUFFER, _Block._Binding_buffer);
				//glBufferData(GL_UNIFORM_BUFFER, sizeof(_Ty), (const void*)&_Block/*default data*/, GL_DYNAMIC_DRAW);// Note: default value
				//glBindBufferBase(GL_UNIFORM_BUFFER, 0, _Block._Binding_buffer);

				/* 2. */
				_Block._Binding_index = glGetUniformBlockIndex(_Myid, _Name.c_str());
				_Block._Binding_point = static_cast<GLuint>(_Mydata.block_indices.size());// initial value is 0
				glUniformBlockBinding(_Myid, _Block._Binding_index, _Block._Binding_point);
			
				/* 3. */
				glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
				glBindBuffer(GL_UNIFORM_BUFFER, 0);
				_Block._Program_id    = _Myid;

				/* 4. */
				GLenum _Ec;
				while ( (_Ec = glGetError()) != GL_NO_ERROR ) {
					_Block._Binding_buffer.destroy();
					char _Error[512]; snprintf(_Error, 511, "[ Error %i] [ ... glsl::program::make_uniform_block(...) ]", _Ec);
					throw std::exception(_Error);
				}
				
				_Mydata.block_indices.push_back(_Block._Binding_point);
				return (_Block);
				/*
				1. create buffer
				2. get uniform block index, and binding point
				3. binding block and program
				4. check
				*/
			}

			template<eTextureInternalFormat _Fmt>
			sampler2D<_Fmt> make_sampler2D(size_t _Unit, std::shared_ptr<texture2D<_Fmt>> _Texture) {
				sampler2D<_Fmt> _Sampler;
				_Sampler._Texture_unit = static_cast<GLuint>(_Unit);
				_Sampler._Texture      = _Texture;
				return ( _Sampler );
			}

			template<eTextureInternalFormat _Fmt>
			sampler3D<_Fmt> make_sampler3D(size_t _Unit, std::shared_ptr<texture3D<_Fmt>> _Texture) {
				sampler3D<_Fmt> _Sampler;
				_Sampler._Texture_unit = static_cast<GLuint>(_Unit);
				_Sampler._Texture      = _Texture;
				return (_Sampler);
			}

			template<eTextureInternalFormat _Fmt>
			sampler2D_array<_Fmt> make_sampler2D_array(size_t _Unit, std::shared_ptr<texture2D_array<_Fmt>> _Texture) {
				sampler2D_array<_Fmt> _Sampler;
				_Sampler._Texture_unit = static_cast<GLuint>(_Unit);
				_Sampler._Texture      = _Texture;
				return (_Sampler);
			}

			template<eTextureInternalFormat _Fmt>
			sampler2D_array_shadow<_Fmt> make_sampler2D_array_shadow(size_t _Unit, std::shared_ptr<texture2D_array<_Fmt>> _Texture) {
				sampler2D_array_shadow<_Fmt> _Sampler;
				_Sampler._Texture_unit = static_cast<GLuint>(_Unit);
				_Sampler._Texture      = _Texture;
				return (_Sampler);
			}

			template<eTextureInternalFormat _Fmt>
			image2D<_Fmt> make_image2D(size_t _Unit, std::shared_ptr<texture2D<_Fmt>> _Texture = nullptr) {
				image2D<_Fmt> _Image;
				_Image._Image_unit = static_cast<GLuint>(_Unit);
				_Image._Texture    = _Texture;
				return (_Image);
			}

			template<typename _Tex>
			image2D<slice<_Tex>::internal_format> make_image2D(size_t _Unit, std::shared_ptr<slice<_Tex>> _Texture) {
				image2D<slice<_Tex>::internal_format> _Image;
				_Image._Image_unit  = static_cast<GLuint>(_Unit);
				_Image._Texture     = _Texture;
				_Image._Image_slice = _Texture->_Myindex;
				return (_Image);
			}

			void ready() const {
				glUseProgram(_Myid);
			}

		private:
			void _Recursion_ready() const {
				/* final */
			}
		
			template<typename ..._Tys>
			void _Recursion_ready(const resource& _Resource, const _Tys&... _Resources) const {
				_Resource.ready();
				_Recursion_ready(_Resources...);
			}

			template<typename _Fty, typename ..._Tys>
			void _Recursion_ready(const std::function<_Fty>& _Func, typename std::function<_Fty>::argument_type _Args,  const _Tys&... _Resources) const {
				_Func(_Args);
				_Recursion_ready(_Resources...);
			}

		public:
			template<typename ..._Tys>
			void ready(const _Tys&... _Resources) const {
				glUseProgram(_Myid);/* opengl can don't to do this first, just in case  */
				_Recursion_ready(_Resources...);
			}

			void done() const {
				glUseProgram(0);
			}

			template<typename ..._Tys>
			void done(const resource& _Enditem, const _Tys&... _Enditems) const {
				_Enditem.done();
				done(_Enditems...);
			}

			GLuint id() const { return (_Myid); }

			GLuint _Myid;
			_Program_data _Mydata;
			std::shared_ptr<shader> _Myvert;
			std::shared_ptr<shader> _Mygeom;
			std::shared_ptr<shader> _Mytese;
			std::shared_ptr<shader> _Mytesc;
			std::shared_ptr<shader> _Myfrag;
			std::shared_ptr<shader> _Mycomp;
		};

	}// namespace glsl

}// namespace gl

#endif
