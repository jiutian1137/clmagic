#pragma once
#ifndef __GLUT___GLBLEND___H__
#define __GLUT___GLBLEND___H__

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "GL/glew.h"

enum class GLBlendOp {
	eAdd    = GL_FUNC_ADD,
	eSub    = GL_FUNC_SUBTRACT,
	eSubRev = GL_FUNC_REVERSE_SUBTRACT,
	eMin    = GL_MIN,
	eMax    = GL_MAX,
};

enum class GLBlendFactor {
	eOne = GL_ONE,
	eZero = GL_ZERO,
	eSrcAlpha = GL_SRC_ALPHA,
	eDstAlpha = GL_DST_ALPHA,
	eSrcColor = GL_SRC_COLOR,
	eDstColor = GL_DST_COLOR,
	eOneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
	eOneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
	eOneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
	eOneMinusDstColor = GL_ONE_MINUS_DST_COLOR
};

struct GLBlendDesc {
	GLBlendFactor _Src  = GLBlendFactor::eSrcAlpha;
	GLBlendFactor _Dst  = GLBlendFactor::eOneMinusSrcAlpha;
	GLBlendOp _Operator = GLBlendOp::eAdd;
};

inline void gl_blend_close() {
	glDisable(GL_BLEND);
}

inline void gl_blend_open(const GLBlendDesc& _Desc) {
	glEnable(GL_BLEND);
	glBlendEquation(GLenum(_Desc._Operator));
	glBlendFunc(GLenum(_Desc._Src), GLenum(_Desc._Dst));
}

inline void gl_blend_reset(const GLBlendDesc& _Desc) {
	glBlendEquation(GLenum(_Desc._Operator));
	glBlendFunc(GLenum(_Desc._Src), GLenum(_Desc._Dst));
}

#endif