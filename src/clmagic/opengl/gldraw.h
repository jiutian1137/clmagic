#pragma once
#ifndef __GLUT___GLDRAW___H__
#define __GLUT___GLDRAW___H__

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "GL/glew.h"

enum class GLPrimive {
	ePoints    = GL_POINTS,
	eLines     = GL_LINES,
	eLineLoop  = GL_LINE_LOOP,
	eLineStrip = GL_LINE_STRIP,
	eTriangles = GL_TRIANGLES,
	eTriangleStrip = GL_TRIANGLE_STRIP,
	eTriangleFan = GL_TRIANGLE_FAN,
	eQuads     = GL_QUADS,
	eQuadStrip = GL_QUAD_STRIP,
	ePolygon   = GL_POLYGON,
	ePatchs    = GL_PATCHES
};


/*
@_Effect: used for no data rendering
*/
inline void gl_draw(GLPrimive _Primtive, GLsizei _Off, GLsizei _Count) {
	glDrawArrays(GLenum(_Primtive), _Off, _Count);
}

/*
@_Note: _Vao _Vbo must be effective
*/
inline void gl_draw(GLuint _Vao, GLuint _Vbo, GLuint _Ebo, GLPrimive _Primtive, GLsizei _Off, GLsizei _Count) {
	glBindVertexArray(_Vao);
	glBindBuffer(GL_ARRAY_BUFFER, _Vbo);

	if (glIsBuffer(_Ebo)) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _Ebo);
		glDrawElements(GLenum(_Primtive), _Count - _Off, GL_UNSIGNED_INT, nullptr);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	} else {
		glDrawArrays(GLenum(_Primtive), _Off, _Count);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}



#endif