#pragma once
#ifndef __GLUT___GLTEXTURE___H__
#define __GLUT___GLTEXTURE___H__

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "GL/glew.h"

namespace gl {
	enum class eTextureTarget {
		Texture1D      = GL_TEXTURE_1D,
		Texture2D      = GL_TEXTURE_2D,
		Texture3D      = GL_TEXTURE_3D,
		Texture2DArray = GL_TEXTURE_2D_ARRAY
	};

	/*
		RGBA, RGB, R, RG

		16, 32 include all types
		8      no float type
	*/
	enum class eTextureInternalFormat {
		RGBA8i   = GL_RGBA8I,
		RGBA8ui  = GL_RGBA8UI,
		RGBA16f  = GL_RGBA16F,
		RGBA16i  = GL_RGBA16I,
		RGBA16ui = GL_RGBA16UI,
		RGBA32f  = GL_RGBA32F,
		RGBA32i  = GL_RGBA32I,
		RGBA32ui = GL_RGBA32UI,

		RGB8i    = GL_RGB8I,
		RGB8ui   = GL_RGB8UI,
		RGB16f   = GL_RGB16F,
		RGB16i   = GL_RGB16I,
		RGB16ui  = GL_RGB16UI,
		RGB32f   = GL_RGB32F,
		RGB32i   = GL_RGB32I,
		RGB32ui  = GL_RGB32UI,
	
		R8i      = GL_R8I,
		R8ui     = GL_R8UI,
		R16f     = GL_R16F,
		R16i     = GL_R16I,
		R16ui    = GL_R16UI,
		R32f     = GL_R32F,
		R32i     = GL_R32I,
		R32ui    = GL_R32UI,
		
		RG8i     = GL_RG8I,
		RG8ui    = GL_RG8UI,
		RG16f    = GL_RG16F,
		RG16i    = GL_RG16I,
		RG16ui   = GL_RG16UI,
		RG32f    = GL_RG32F,
		RG32i    = GL_RG32I,
		RG32ui   = GL_RG32UI,

		Depth32f = GL_DEPTH_COMPONENT32F
	};

	enum class eTextureDataFormat {
		Red    = GL_RED,   /* float */
		RG     = GL_RG,
		RGB    = GL_RGB,
		RGBA   = GL_RGBA,
		Depth  = GL_DEPTH_COMPONENT, /* used to shadow, depth compare, @texture2D */

		RedI   = GL_RED_INTEGER, /* integer */
		RGBI   = GL_RGB_INTEGER,
		RGBAI  = GL_RGBA_INTEGER,
		BGRI   = GL_BGR_INTEGER, /* used to OpenCV */
		BGRAI  = GL_BGRA_INTEGER
	};

	inline eTextureDataFormat texture_dataformat(size_t _Channels) {
		return (_Channels == 4 ? eTextureDataFormat::RGBA
				: _Channels == 3 ? eTextureDataFormat::RGB
				: _Channels == 2 ? eTextureDataFormat::RG
				: _Channels == 1 ? eTextureDataFormat::Red : throw std::exception("...") );
	}

	inline size_t dataformat_channels(eTextureDataFormat _Format) {
		return (_Format == eTextureDataFormat::Red ? 1
				: _Format == eTextureDataFormat::RG ? 2
				: _Format == eTextureDataFormat::RGB ? 3
				: _Format == eTextureDataFormat::RGBA ? 4
				: _Format == eTextureDataFormat::Depth ? 1
				: _Format == eTextureDataFormat::RedI ? 1
				: _Format == eTextureDataFormat::RGBI ? 3
				: _Format == eTextureDataFormat::RGBAI ? 4
				: _Format == eTextureDataFormat::BGRI ? 3
				: _Format == eTextureDataFormat::BGRAI ? 4
				: 0);
	}

	enum class eTextureDataType {
		Undefined = GL_NONE,
		Byte      = GL_BYTE,
		Ubyte     = GL_UNSIGNED_BYTE,
		Int16     = GL_SHORT,
		Uint16    = GL_UNSIGNED_SHORT,
		Int32     = GL_INT,
		Uint32    = GL_UNSIGNED_INT,
		Float     = GL_FLOAT
	};

	template<typename _Ty>
	struct texture_data_type {
		constexpr static auto value = eTextureDataType::Undefined;
	};

	template<>
	struct texture_data_type<char> {
		constexpr static auto value = eTextureDataType::Byte;
	};

	template<>
	struct texture_data_type<unsigned char> {
		constexpr static auto value = eTextureDataType::Ubyte;
	};

	template<>
	struct texture_data_type<short> {
		constexpr static auto value = eTextureDataType::Int16;
	};
	
	template<>
	struct texture_data_type<unsigned short> {
		constexpr static auto value = eTextureDataType::Uint16;
	};

	template<>
	struct texture_data_type<int> {
		constexpr static auto value = eTextureDataType::Int32;
	};

	template<>
	struct texture_data_type<unsigned int> {
		constexpr static auto value = eTextureDataType::Uint32;
	};

	template<>
	struct texture_data_type<float> {
		constexpr static auto value = eTextureDataType::Float;
	};

	template<typename _Ty> 
	constexpr eTextureDataType texture_data_type_v = texture_data_type<std::remove_cv_t<_Ty>>::value;

	enum class eTextureWrap {
		ClampToEdge   = GL_CLAMP_TO_EDGE,
		ClampToBorder = GL_CLAMP_TO_BORDER,
		Repeat        = GL_REPEAT,
		MirrorRepeat  = GL_MIRRORED_REPEAT
	};

	enum class eTextureFilter {
		Linear  = GL_LINEAR,
		Nearest = GL_NEAREST
	};

	/*
	@_Example:
		first : texture_descriptor _Desc; 
				_Desc.width  = 100; 
				_Desc.height = 100; 
				_Desc._Data  = _Img.data;
		second: texture_descriptor _Desc; 
				_Desc.width   = 100; 
				_Desc.height  = 100; 
				_Desc._Data   = _Img.data;
				_Desc._Internalformat = eTextureInternalFormat::eRGBA32f;
				_Desc._Format = eTextureDataFormat::eRGBA;
				_Desc._Type   = eTextureDataType::Float;
	*/
	struct texture_descriptor {
		eTextureDataFormat _Format = eTextureDataFormat::RGB;
		eTextureDataType   _Type   = eTextureDataType::Ubyte;
		const void*        _Data   = nullptr;
		GLint              _Level  = 0;
		GLsizei            _Width  = 0;/* must */
		GLsizei            _Height = 0;/* must */
		GLsizei            _Depth  = 0;
		eTextureTarget     _Target = eTextureTarget::Texture2D;
		eTextureInternalFormat _Internalformat = eTextureInternalFormat::RGB32f;
	
		eTextureWrap       _Wrapx   = eTextureWrap::Repeat;
		eTextureWrap       _Wrapy   = eTextureWrap::Repeat;
		eTextureWrap       _Wrapz   = eTextureWrap::Repeat;
		eTextureFilter     _Magfilter = eTextureFilter::Linear;
		eTextureFilter     _Minfilter = eTextureFilter::Linear;
		GLuint             _Texture = 0;
	};

	inline void texture_delete(GLuint& _Texture) {
		if (glIsTexture(_Texture)) {
			glDeleteTextures(1, &_Texture);
		}
		_Texture = -1;
	}

	inline void texture_bind(GLenum _Unit, GLenum _Target, GLuint _Texture) {
		glActiveTexture(_Unit);
		glBindTexture(_Target, _Texture);
	}

	/*
	_Width ~= 2063 is error
	*/
	inline GLuint texture_create(texture_descriptor& _Desc){
		/* 1. */
		GLuint _Texture = 0;
		glGenTextures(1, &_Texture);
		/* 2. */
		if ( dataformat_channels(_Desc._Format) == 1 ) { glPixelStorei(GL_UNPACK_ALIGNMENT, 1); }
		glBindTexture(GLenum(_Desc._Target), _Texture);

		/* 3. */
		switch (_Desc._Target) {
			case eTextureTarget::Texture2D:
				/*
				@_Example:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, _DATA);
					_Width = _Height = 1024;
					_Internalformat = GL_RGBA;
					_Format		    = GL_RGBA;
					_Type	        = GL_UNSIGNED_BYTE
				*/
				glTexImage2D(GLenum(_Desc._Target), _Desc._Level, GLint(_Desc._Internalformat), _Desc._Width, _Desc._Height, 0,
					GLenum(_Desc._Format), GLenum(_Desc._Type), _Desc._Data);
				break;
			case eTextureTarget::Texture2DArray:
			case eTextureTarget::Texture3D:
				/*
				@_Example:
					glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA8, 128, 128, 128);
					glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, 128, 128, 128, GL_RGBA, GL_FLOAT, _Data);
					_Width = _Height = _Depth = 128;
					_Internalformat = GL_RGBA8;
					_Format			= GL_RGBA;
					_Type		    = GL_FLOAT
				*/
				glTexStorage3D(GLenum(_Desc._Target), 1, GLenum(_Desc._Internalformat), _Desc._Width, _Desc._Height, _Desc._Depth);
				if (_Desc._Data != nullptr) {
					glTexSubImage3D(GLenum(_Desc._Target), 0, 0, 0, 0, _Desc._Width, _Desc._Height, _Desc._Depth, GLenum(_Desc._Format), GLenum(_Desc._Type), _Desc._Data);
				}
				break;
			default:
				break;
		}

		/* 4. */
		glTexParameteri(GLenum(_Desc._Target), GL_TEXTURE_WRAP_R, GLint(_Desc._Wrapx));
		glTexParameteri(GLenum(_Desc._Target), GL_TEXTURE_WRAP_S, GLint(_Desc._Wrapy));
		glTexParameteri(GLenum(_Desc._Target), GL_TEXTURE_WRAP_T, GLint(_Desc._Wrapz));
		glTexParameteri(GLenum(_Desc._Target), GL_TEXTURE_MAG_FILTER, GLint(_Desc._Magfilter));
		glTexParameteri(GLenum(_Desc._Target), GL_TEXTURE_MIN_FILTER, GLint(_Desc._Minfilter));

		/* 5. */
		glBindTexture(GLenum(_Desc._Target), 0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		/* 6. */
		if ( glIsTexture(_Texture) ) {
			texture_delete(_Desc._Texture);
			_Desc._Texture = _Texture;
			return (_Texture);
		} else {
			glDeleteTextures(1, &_Texture);
			throw std::exception("[Error] [... gl::texture_create(texture_descriptor& ) ]");
		}
		/*
		1. create
		2. bind
		3. setup texture
		4. set misc properties
		5. unbind
		6. check...return
		*/
	}

	inline GLuint texture_create_texturebuffer(GLuint _Buf, eTextureInternalFormat _Internalformat) {
		GLuint _Texture = -1;
		glGenTextures(1, &_Texture);
		glBindTexture(GL_TEXTURE_BUFFER, _Texture);
		glTextureBuffer(GL_TEXTURE_BUFFER, (GLenum)_Internalformat, _Buf);
		glBindTexture(GL_TEXTURE_BUFFER, 0);
		return (_Texture);
	}



	struct texture : public _Noncopyable {
		texture() = default;
		
		texture(texture&& _Right) {
			this->assign(std::move(_Right), false);
		}

		virtual ~texture() { 
			texture_delete(_Mydesc._Texture);
		}

		virtual void destroy() {
			texture_delete(_Mydesc._Texture);
		}

		texture& operator=(texture&& _Right) {
			this->assign(std::move(_Right));
			return (*this);
		}

		virtual void assign(texture&& _Right, bool _Constructed = true) {
			if (_Constructed) {
				this->destroy();
			}
			this->_Mydesc  = _Right._Mydesc;
			_Right._Mydesc = texture_descriptor();
		}

		virtual void initializer(const texture_descriptor& _Desc, bool _Constructed = true) {
			if (_Constructed) {
				this->destroy();
			}
			_Mydesc = _Desc;
			texture_create(_Mydesc);
		}

		GLuint id() const { return (_Mydesc._Texture); }

		bool valid() const { return (glIsTexture(_Mydesc._Texture)); }

		size_t width() const { return (static_cast<size_t>(_Mydesc._Width)); }

		size_t height() const { return (static_cast<size_t>(_Mydesc._Height)); }

		size_t depth() const { return (static_cast<size_t>(_Mydesc._Depth)); }

		texture_descriptor _Mydesc;
	};


	/*
	@_Texture2D: width * height
	*/
	template<eTextureInternalFormat _Fmt>
	struct texture2D : public texture {
		constexpr static auto target          = eTextureTarget::Texture2D;
		constexpr static auto internal_format = _Fmt;

		using texture_type  = typename texture2D<_Fmt>;
		using resource_type = typename std::shared_ptr<texture_type>;

		texture2D() = default;

		template<typename _Ty>
		texture2D(eTextureDataFormat _Format, const _Ty* _Data, size_t _Width, size_t _Height, 
			eTextureWrap   _WrapR = eTextureWrap::Repeat, eTextureWrap   _WrapS = eTextureWrap::Repeat,
			eTextureFilter _Magflt = eTextureFilter::Linear, eTextureFilter _Minflt = eTextureFilter::Linear) {
			texture_descriptor _Desc;
			_Desc._Format    = _Format;
			_Desc._Type      = texture_data_type_v<_Ty>;
			_Desc._Data      = _Data;
			if (_Desc._Data != nullptr && _Desc._Type == eTextureDataType::Undefined) {
				throw std::exception("[unsupport data type] [ texture2D::texture2D(...) ]");
			}
			_Desc._Width     = static_cast<GLsizei>(_Width);
			_Desc._Height    = static_cast<GLsizei>(_Height);
			_Desc._Target    = target;
			_Desc._Internalformat = internal_format;
			_Desc._Wrapx     = _WrapR;
			_Desc._Wrapy     = _WrapS;
			_Desc._Magfilter = _Magflt;
			_Desc._Minfilter = _Minflt;
			this->initializer(_Desc, false);
		}

		template<typename _Ty>
		texture2D(const _Ty* _Data, size_t _Channels, size_t _Width, size_t _Height,
			eTextureWrap   _WrapR = eTextureWrap::Repeat, eTextureWrap   _WrapS = eTextureWrap::Repeat,
			eTextureFilter _Magflt = eTextureFilter::Linear, eTextureFilter _Minflt = eTextureFilter::Linear)
			: texture2D(texture_dataformat(_Channels), _Data, _Width, _Height, _WrapR, _WrapS, _Magflt, _Minflt) {
			/* empty */
		}
	};
	using texture2D_rgba32f = texture2D<eTextureInternalFormat::RGBA32f>;
	using texture2D_rgba16f = texture2D<eTextureInternalFormat::RGBA16f>;
	using texture2D_rgb32f  = texture2D<eTextureInternalFormat::RGB32f>;
	using texture2D_rgb16f  = texture2D<eTextureInternalFormat::RGB16f>;
	using texture2D_rg32f   = texture2D<eTextureInternalFormat::RG32f>;
	using texture2D_rg16f   = texture2D<eTextureInternalFormat::RG16f>;
	using texture2D_r32f    = texture2D<eTextureInternalFormat::R32f>;
	using texture2D_r16f    = texture2D<eTextureInternalFormat::R16f>;
	using texture2D_r16ui   = texture2D<eTextureInternalFormat::R16ui>;



	//template<eTextureInternalFormat _Fmt>
	//	struct texture3D; /* forward definition*/

	/*
	@_Texture3D_slice:
	*/
	template<typename _Tex>
	struct slice : public texture2D<_Tex::internal_format>{
		constexpr static auto internal_format = _Tex::internal_format;

		using _Mybase       = texture2D<_Tex::internal_format>;
		using slice_type    = typename slice<_Tex>;
		using texture_type  = _Tex;
		using resource_type = std::shared_ptr<texture_type>;

		slice(texture_type& _Texture, size_t _Pos = 0)
			: _Mybase(), _Myindex(_Pos) {
			_Mybase::_Mydesc = _Texture._Mydesc;
		}

		virtual ~slice() {
			_Mybase::_Mydesc._Texture = -1;
		}

		slice_type& operator=(const texture2D<_Tex::internal_format> & _Right) {
			auto _Minval = [](size_t a, size_t b) { return (a < b ? a : b); };

			if ( this->valid() && _Right.valid() ) {
				const auto _Width  = _Minval(this->width(), _Right.width());
				const auto _Height = _Minval(this->height(), _Right.height());
				glCopyImageSubData(_Right.id(), (GLenum)_Right.target, 0, 0, 0, 0,
					this->id(), (GLenum)this->target, 0, 0, 0, _Myindex,
					_Width, _Height, 1);
				return (*this);
			}
			throw std::exception("[invalid slice] [ slice::operator=(const texture2D&) ]");
		}

		template<typename _Tex2>
		slice_type& operator=(const typename slice<_Tex2>& _Right) {
			if ( _Right.internal_format == this->internal_format && _Right.valid() ) {
				return (this->id() = _Right.id());
			}
			throw std::exception("[invalid slice] [ slice::operator=(const slice&) ]");
		}

		slice_type& operator++() {
			if ( this->valid() ) {
				if (++_Myindex <= this->depth()) {
					return ( *this );
				} else {
					/* index out of after bound */
				}
			}
			return (*this);
		}

		friend bool operator==(const slice& _A, const slice& _B) {
			if ( _A.valid() && _B.valid() ) {// both not expired
				return (_A.id() == _B.id());
			}
			return ( false );
		}

		friend bool operator!=(const slice& _A, const slice& _B) {
			return (!(_A == _B));
		}

		size_t              _Myindex = 0;
	};


	/*
	@_Texture3D: width * height * depth
	*/
	template<eTextureInternalFormat _Fmt>
	struct texture3D : public texture {
		constexpr static auto target          = eTextureTarget::Texture3D;
		constexpr static auto internal_format = _Fmt;

		using texture_type  = typename texture3D<_Fmt>;
		using resource_type = typename std::shared_ptr<texture_type>;
		using slice_type    = typename slice<texture_type>;

		texture3D() = default;

		texture3D(const texture_descriptor& _Desc) {
			if (_Desc._Target != target) {
				throw std::exception("[Bad descriptor] [ texture3D(const texture_descriptor&) ]");
			}
			this->initializer(_Desc, false);
		}

		template<typename _Ty>
		texture3D(eTextureDataFormat _Format, const _Ty* _Data, size_t _Width, size_t _Height, size_t _Depth,
			eTextureWrap _WrapR = eTextureWrap::Repeat, eTextureWrap _WrapS = eTextureWrap::Repeat, eTextureWrap _WrapT = eTextureWrap::Repeat,
			eTextureFilter _Magflt = eTextureFilter::Linear, eTextureFilter _Minflt = eTextureFilter::Linear) { /* constructor */
			texture_descriptor _Desc;
			_Desc._Format    = _Format;
			_Desc._Type      = texture_data_type_v<_Ty>;
			_Desc._Data      = _Data;
			if (_Desc._Type == eTextureDataType::Undefined) {
				throw std::exception("[unsupport data type] [ texture3D::texture3D(...) ]");
			}

			_Desc._Width     = static_cast<GLsizei>(_Width);
			_Desc._Height    = static_cast<GLsizei>(_Height);
			_Desc._Depth     = static_cast<GLsizei>(_Depth);
			_Desc._Target    = target;
			_Desc._Internalformat = internal_format;
			_Desc._Wrapx     = _WrapR;
			_Desc._Wrapy     = _WrapS;
			_Desc._Wrapz     = _WrapT;
			_Desc._Magfilter = _Magflt;
			_Desc._Minfilter = _Minflt;
			this->initializer(_Desc);
		}

		template<typename _Ty>
		texture3D(const _Ty* _Data, size_t _Channels, size_t _Width, size_t _Height, size_t _Depth,
			eTextureWrap _WrapR = eTextureWrap::Repeat, eTextureWrap _WrapS = eTextureWrap::Repeat, eTextureWrap _WrapT = eTextureWrap::Repeat,
			eTextureFilter _Magflt = eTextureFilter::Linear, eTextureFilter _Minflt = eTextureFilter::Linear)
			: texture3D(texture_dataformat(_Channels) , _Data, _Width, _Height, _Depth, _WrapR, _WrapS, _WrapT, _Magflt, _Minflt) {
			/* empty */
		}

		std::shared_ptr<slice_type> operator[](size_t _Pos) {
			if (this->valid()) {
				auto _Slice = std::make_shared<slice_type>(*this, _Pos);
				return (_Slice);
			} else {
				throw std::exception("[to use slice, must dynamic memory, Example:std::shared_ptr<...>] [ texture2D_array::operator[](size_t) ]");
			}
		}

		std::shared_ptr<slice_type> begin() {
			return ((*this)[0]);
		}

		std::shared_ptr<slice_type> end() {
			return ((*this)[this->depth()]);
		}

		template<typename _Ty>
		void reset(size_t i, const _Ty* _Data, size_t _Channels, size_t _Width, size_t _Height, size_t _Depth = 1) {
			auto _Format = texture_dataformat(_Channels);
			auto _Type = texture_data_type_v<_Ty>;

			glBindTexture((GLenum)target, this->id());
			glTexSubImage3D(GLenum(target), 0, 0, 0, i, _Width, _Height, _Depth, (GLenum)_Format, (GLenum)_Type, _Data);
			glBindTexture((GLenum)target, 0);
		}
	};
	using texture3D_rgba32f = texture3D<eTextureInternalFormat::RGBA32f>;
	using texture3D_rgba16f = texture3D<eTextureInternalFormat::RGBA16f>;
	using texture3D_rgb32f  = texture3D<eTextureInternalFormat::RGB32f>;
	using texture3D_rgb16f  = texture3D<eTextureInternalFormat::RGB16f>;


	template<eTextureInternalFormat _Fmt>
	struct texture2D_array : public texture {
		constexpr static auto target          = eTextureTarget::Texture2DArray;
		constexpr static auto internal_format = _Fmt;

		using texture_type  = typename texture2D_array<_Fmt>;
		using resource_type = typename std::shared_ptr<texture_type>;
		using slice_type    = typename slice<texture_type>;

		texture2D_array() = default;

		texture2D_array(const texture_descriptor& _Desc) {
			if (_Desc._Target != target) {
				throw std::exception("[Bad descriptor] [ texture2D_array(const texture_descriptor&) ]");
			}
			this->initializer(_Desc, false);
		}

		template<typename _Ty>
		texture2D_array(eTextureDataFormat _Format, const _Ty* _Data, size_t _Width, size_t _Height, size_t _Depth,
			eTextureWrap   _WrapR = eTextureWrap::Repeat, eTextureWrap _WrapS = eTextureWrap::Repeat, eTextureWrap _WrapT = eTextureWrap::Repeat,
			eTextureFilter _Magflt = eTextureFilter::Linear, eTextureFilter _Minflt = eTextureFilter::Linear) { /* constructor */
			texture_descriptor _Desc;
			_Desc._Format = _Format;
			_Desc._Type   = texture_data_type_v<_Ty>;
			_Desc._Data   = _Data;
			if (_Desc._Type == eTextureDataType::Undefined) {
				throw std::exception("[unsupport data type] [ texture2D::texture2D(...) ]");
			}
			_Desc._Width     = static_cast<GLsizei>(_Width);
			_Desc._Height    = static_cast<GLsizei>(_Height);
			_Desc._Depth     = static_cast<GLsizei>(_Depth);
			_Desc._Target    = target;
			_Desc._Internalformat = internal_format;
			_Desc._Wrapx     = _WrapR;
			_Desc._Wrapy     = _WrapS;
			_Desc._Wrapz     = _WrapT;
			_Desc._Magfilter = _Magflt;
			_Desc._Minfilter = _Minflt;
			this->initializer(_Desc, false);
		}

		template<typename _Ty>
		texture2D_array(const _Ty* _Data, size_t _Channels, size_t _Width, size_t _Height, size_t _Depth,
			eTextureWrap   _WrapR = eTextureWrap::Repeat, eTextureWrap _WrapS = eTextureWrap::Repeat, eTextureWrap _WrapT = eTextureWrap::Repeat,
			eTextureFilter _Magflt = eTextureFilter::Linear, eTextureFilter _Minflt = eTextureFilter::Linear)
			: texture2D_array(texture_dataformat(_Channels), _Data, _Width, _Height, _Depth, _WrapR, _WrapS, _WrapT, _Magflt, _Minflt) {
			/* empty */
		}

		std::shared_ptr<slice_type> operator[](size_t _Pos) {
			if ( this->valid() ) {
				auto _Slice = std::make_shared<slice_type>(*this, _Pos);
				return (_Slice);
			} else {
				throw std::exception("[to use slice, must dynamic memory, Example:std::shared_ptr<...>] [ texture2D_array::operator[](size_t) ]");
			}
		}

		std::shared_ptr<slice_type> begin() {
			return ((*this)[0]);
		}

		std::shared_ptr<slice_type> end() {
			return ((*this)[this->depth()]);
		}

		template<typename _Ty>
		void reset(size_t i, const _Ty* _Data, size_t _Channels, size_t _Width, size_t _Height, size_t _Depth = 1) {
			auto _Format = texture_dataformat(_Channels);
			auto _Type   = texture_data_type_v<_Ty>;

			glBindTexture((GLenum)target, this->id());
			glTexSubImage3D(GLenum(target), 0, 0, 0, i, _Width, _Height, _Depth, (GLenum)_Format, (GLenum)_Type, _Data);
			glBindTexture((GLenum)target, 0);
		}
	};
	using texture2D_array_rgba32f = texture2D_array<eTextureInternalFormat::RGBA32f>;
	using texture2D_array_rgba16f = texture2D_array<eTextureInternalFormat::RGBA16f>;
	using texture2D_array_rgb32f  = texture2D_array<eTextureInternalFormat::RGB32f>;
	using texture2D_array_rgb16f  = texture2D_array<eTextureInternalFormat::RGB16f>;
	using texture2D_array_r32f    = texture2D_array<eTextureInternalFormat::R32f>;
	using texture2D_array_r16f    = texture2D_array<eTextureInternalFormat::R32f>;
	using texture2D_array_d32f    = texture2D_array<eTextureInternalFormat::Depth32f>;
}// namespace gl

#endif