#pragma once
#ifndef __GLUT___GLVERTEXARRAY___H__
#define __GLUT___GLVERTEXARRAY___H__

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "GL/glew.h"

namespace gl {
	/*
	@_Example:
		{4, GL_FLOAT, GL_FALSE, 16, 0}
	*/
	struct VertexPointer {
		GLint	  _Size;
		GLenum    _Type = GL_FLOAT;
		GLboolean _Normalized = GL_FALSE;
		GLsizei   _Stride;
		GLint     _Offset;
	};

	struct VertexArrayDesc {
		VertexPointer* _Vertex_points = nullptr;
		size_t _Size = 0;
		GLuint _Buffer      = 0;
		GLuint _Inputlayout = 0;
	};

	inline void inputlayout_delete(GLuint& _Array) {
		if (glIsVertexArray(_Array)) {
			glDeleteVertexArrays(1, &_Array);
		}
		_Array = 0;
	}

	inline GLuint inputlayout_create(VertexArrayDesc& _Desc) {
		if (glIsBuffer(_Desc._Buffer) && _Desc._Size > 0) {
			GLuint _Result = 0;
			glGenVertexArrays(1, &_Result);

			glBindBuffer(GL_ARRAY_BUFFER, _Desc._Buffer);
			glBindVertexArray(_Result);
			for (GLuint i = 0; i != _Desc._Size; ++i) {
				if ( _Desc._Vertex_points[i]._Type == GL_UNSIGNALED || _Desc._Vertex_points[i]._Type == GL_INT ) {
					glVertexAttribIPointer(  i,
						_Desc._Vertex_points[i]._Size, 
						_Desc._Vertex_points[i]._Type, 
						_Desc._Vertex_points[i]._Stride, 
						(void*)(_Desc._Vertex_points[i]._Offset));
				} else {
					glVertexAttribPointer(   i,
						_Desc._Vertex_points[i]._Size,
						_Desc._Vertex_points[i]._Type,
						_Desc._Vertex_points[i]._Normalized,
						_Desc._Vertex_points[i]._Stride,
						(void*)(_Desc._Vertex_points[i]._Offset));
				}
				glEnableVertexAttribArray(i);
			}

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			inputlayout_delete(_Desc._Inputlayout);
			_Desc._Inputlayout = _Result;
			return (_Desc._Inputlayout);
		} else {
			return (0);
		}
	}


	
}


#endif