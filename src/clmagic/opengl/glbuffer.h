#pragma once
#ifndef __GLUT___GLBUFFER___H__
#define __GLUT___GLBUFFER___H__
#include <iostream>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "GL/glew.h"
#include "noncopyable.h"

namespace gl {

	enum class eBufferTarget {
		ArrayBuffer        = GL_ARRAY_BUFFER,
		TextureBuffer      = GL_TEXTURE_BUFFER,
		UniformBuffer      = GL_UNIFORM_BUFFER,
		PixelUnpackBuffer  = GL_PIXEL_UNPACK_BUFFER,
		ElementArrayBuffer = GL_ELEMENT_ARRAY_BUFFER,
		AtomicCounterBuffer= GL_ATOMIC_COUNTER_BUFFER
	};

	constexpr auto eVBO = eBufferTarget::ArrayBuffer;
	constexpr auto eEBO = eBufferTarget::ElementArrayBuffer;
	constexpr auto eUBO = eBufferTarget::UniformBuffer;

	enum class eBufferUsage {
		StaticDraw = GL_STATIC_DRAW,
		StaticRead = GL_STATIC_READ,
		StaticCopy = GL_STATIC_COPY,
		DynamicDraw = GL_DYNAMIC_DRAW,
		DynamicRead = GL_DYNAMIC_READ,
		DynamicCopy = GL_DYNAMIC_COPY
	};

	struct buffer_descriptor {
		eBufferTarget _Target = eBufferTarget::ArrayBuffer;
		eBufferUsage  _Usage  = eBufferUsage::StaticDraw;
		GLsizeiptr    _Capcity = 0;
		const void*   _Data   = nullptr;
		GLsizeiptr    _Size   = 0;
		GLuint        _Buffer = 0;
	};


	inline void buffer_delete(GLuint& _Gpubuf) {
		if (glIsBuffer(_Gpubuf)) {
			glDeleteBuffers(1, &_Gpubuf);
		}
		_Gpubuf = -1;
	}

	inline GLuint buffer_create(buffer_descriptor& _Desc) {
		auto _Max = [](size_t a, size_t b) { return (a > b ? a : b); };

		if (_Desc._Capcity < _Desc._Size) {// check if capacity is error
			_Desc._Capcity = _Desc._Size;
		}

		GLuint _Newbuf = -1;
		glGenBuffers(1, &_Newbuf);
		glBindBuffer((GLenum)_Desc._Target, _Newbuf);
		glBufferData((GLenum)_Desc._Target, _Max(_Desc._Size, _Desc._Capcity), nullptr, (GLenum)_Desc._Usage);
		if (_Desc._Data != nullptr) {// initial data
			glBufferSubData((GLenum)_Desc._Target, 0, _Desc._Size, _Desc._Data);
		}
		glBindBuffer((GLenum)_Desc._Target, 0);

		if ( glIsBuffer(_Newbuf) ) {// OK
			glDeleteBuffers(1, &_Desc._Buffer);
			_Desc._Buffer = _Newbuf;
			return ( _Newbuf );
		} else {
			glDeleteBuffers(1, &_Newbuf);
			throw std::exception("[ERROR] [... buffer_create(...)]");
		}
	}

	/* this function from AGRE */
	inline void buffer_copy(GLuint _Dst, GLuint _Src, size_t _Size) {
		glBindBuffer(GL_COPY_READ_BUFFER, _Src);
		glBindBuffer(GL_COPY_WRITE_BUFFER, _Dst);

		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, _Size);

		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		glBindBuffer(GL_COPY_READ_BUFFER, 0);
	}

	template<eBufferTarget _Target, typename _Ty> inline
	void buffer_set(GLuint _Gpubuf, size_t _Pos, const _Ty& _Data) {
		glBindBuffer((GLenum)_Target, _Gpubuf);
		auto* _Ptr = (_Ty*)glMapBuffer((GLenum)_Target, GL_WRITE_ONLY);
		_Ptr[_Pos] = _Data;
		glUnmapBuffer((GLenum)_Target);
		glBindBuffer((GLenum)_Target, 0/*unbind*/);
	}

	template<eBufferTarget _Target, typename _Iter> inline
	void buffer_set(GLuint _Gpubuf, size_t _Pos, _Iter _First, _Iter _Last) {
		glBindBuffer((GLenum)_Target, _Gpubuf);
		auto* _Ptr = (std::remove_reference_t<std::remove_const_t<decltype(*_First)>>*)glMapBuffer((GLenum)_Target, GL_WRITE_ONLY);
		std::copy(_First, _Last, _Ptr + _Pos);
		glUnmapBuffer((GLenum)_Target);
		glBindBuffer((GLenum)_Target, 0/*unbind*/);
	}

	template<eBufferTarget _Target, typename _Ty> inline
	void buffer_copy(GLuint _Gpubuf, const _Ty& _Data) {
		glBindBuffer((GLenum)_Target, _Gpubuf);
		auto* _Ptr = (_Ty*)glMapBuffer((GLenum)_Target, GL_WRITE_ONLY);
		*_Ptr = _Data;
		glUnmapBuffer((GLenum)_Target);
		glBindBuffer((GLenum)_Target, 0/*unbind*/);
	}

	template<eBufferTarget _Target, typename _Fn, typename ..._Tys>
	void buffer_mapping(GLuint _Gpubuf, _Fn _Func, _Tys... _Args) {
		glBindBuffer((GLenum)_Target, _Gpubuf);
		void* _Ptr = glMapBuffer((GLenum)_Target, GL_READ_WRITE);
		_Func(_Ptr, _Args...);
		glUnmapBuffer((GLenum)_Target);
		glBindBuffer((GLenum)_Target, 0/*unbind*/);
	}



	/* 
	@_buffer<target, T>: opengl buffer
	*/
	template<eBufferTarget _Target, typename _Ty>
	struct buffer : public _Noncopyable {
		constexpr static auto target = _Target;

		using resource_type = typename std::shared_ptr<buffer<target, _Ty>>;

		buffer() : _Mydesc() { /* empty */ }
		
		buffer(const _Ty* _Data, size_t _Size) : _Mydesc() {
			buffer_descriptor _Desc;
			_Desc._Data   = _Data;
			_Desc._Size   = sizeof(_Ty) * _Size;
			_Desc._Target = this->target;
			this->initializer(_Desc);
		}

		buffer(buffer&& _Right) : _Mydesc() {
			this->assign(std::move(_Right));
		}

		buffer(buffer_descriptor& _Desc) {
			this->initializer(_Desc);
		}

		~buffer() {
			buffer_delete(_Mydesc._Buffer);
		}

		buffer& operator=(buffer&& _Right) {
			this->assign(std::move(_Right));
			return ( _Right );
		}

		void initializer(buffer_descriptor& _Desc) {
			if (_Desc._Target == this->target) {
				buffer_create(_Desc);
				_Mydesc = _Desc;
				_Desc._Buffer = 0;// erase _Desc.buffer
				return;
			}
			throw std::exception("[ERROR] [buffer target miss]");
		}

		void assign(buffer&& _Right) {
			this->destroy();
			this->_Mydesc  = _Right._Mydesc;
			_Right._Mydesc = buffer_descriptor();
		}

		void destroy() {
			buffer_delete(_Mydesc._Buffer);
			_Mydesc = buffer_descriptor();
		}

		GLuint id() const { return (_Mydesc._Buffer); }

		size_t size() const { return (_Mydesc._Size / sizeof(_Ty)); }

		buffer_descriptor _Mydesc;
	};

	template<typename _Ty> 
	using array_buffer = buffer<eBufferTarget::ArrayBuffer, _Ty>;
	
	template<typename _Ty>
	using texture_buffer = buffer<eBufferTarget::TextureBuffer, _Ty>;

	template<typename _Ty>
	using uniform_buffer = buffer<eBufferTarget::UniformBuffer, _Ty>;

	template<typename _Ty> 
	using pixel_unpack_buffer = buffer<eBufferTarget::PixelUnpackBuffer, _Ty>;
	
	template<typename _Ty> 
	using element_array_buffer = buffer<eBufferTarget::ElementArrayBuffer, _Ty>;
	
	template<typename _Ty> 
	using atomic_counter_buffer = buffer<eBufferTarget::AtomicCounterBuffer, _Ty>;
	
}// namespace gl

#endif