#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY___NAMESPACE___H__
#define __CLMAGIC_CORE_GEOMETRY___NAMESPACE___H__

namespace clmagic
{
	/* @_clmagic: graphics library */
}

/* reference */
//#define CLMAGIC_USING_MKL 1
#define CLMAGIC_USING_DXMATH 1

/* Intel math kernel library */
#if defined(CLMAGIC_USING_MKL)
#include <mkl.h>
#endif

/* MicrosoftDirectX math library */
#if defined(CLMAGIC_USING_DXMATH)
#include <DirectXMath.h>
#include <DirectXCollision.h>
#endif

/* GLM math library */
#if defined(CLMAGIC_USING_GLM)
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif


/* please enter type */
#ifndef _in
#define _in(...) const __VA_ARGS__&
#endif
#ifndef _inout
#define _inout(...) __VA_ARGS__&
#endif
#ifndef _out
#define _out(...) __VA_ARGS__&
#endif

#endif