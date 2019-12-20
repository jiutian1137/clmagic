#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH_RAYTRACING___SHAPE___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH_RAYTRACING___SHAPE___H__
#include "../raytracing.h"

namespace clmagic 
{
	template<typename T> inline
		Hit_<T> Hit_<T>::no_hit() 
		{
		return ( Hit_<T>{HitType::NOHIT, T(100000000.0)} );
		}

}// namespace clmagic

#endif