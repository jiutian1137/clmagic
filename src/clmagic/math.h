#ifndef clmagic_matH_h_
#define clmagic_matH_h_

// debug mode if debug compile
#ifdef _DEBUG

#ifndef _DEBUG
#define _DEBUG 1
#endif

#endif

#include "calculation/general/clmagic.h"
#include "calculation/general/general.h"
#include "calculation/real/bitset.h"
#include "calculation/real/rational.h"
#include "calculation/real/unit.h"
#include "calculation/real/real.h"
#include "calculation/real/Stirling.h"

#include "calculation/lapack/vector.h"
#include "calculation/lapack/matrix.h"
#include "calculation/lapack/Euler.h"
#include "calculation/lapack/Rodrigues.h"
#include "calculation/lapack/geometry.h"

#include "calculation/complex/WilliamRowanHamilton.h"

#include "calculation/calculus/derivative.h"

#include "calculation/physic/atmosphere_scattering.h"


//#include "math/lapack/general.inl.h"
//#include "math/lapack/vector.inl.h"
//#include "math/lapack/matrix.inl.h"
//#include "math/lapack/diag_matrix.inl.h"
//#include "math/lapack/world_matrix.inl.h"
//#include "math/lapack/transform.inl.h"
//#include "math/lapack/intersect.inl.h"

//#include "math/complex/quaternion.inl.h"

//#include "math/illumunation/illumunation.inl.h"
//
//#include "math/raytracing/shape.inl.h"
//#include "math/raytracing/intersect.inl.h"
//
//#include "math/fraction/Perlin.inl.h"
////#include "math/fraction/Perlin_simplex.inl.h"
////#include "math/fraction/Worley.inl.h"
////#include "math/fraction/Worley_cellular.inl.h"
////#include "math/fraction/gradient.inl.h"
////#include "math/fraction/value_noise.inl.h"
//
//#include "math/other/rect2d.inl.h"
//#include "math/other/color_space.inl.h"

/*
	  +-----------+---------------+------------+
	  |			  |				  |
	simd.h     real.h
	  |			  |				  |
	  +-----------+--------+------+------------+
						   |
	  +--------------------+-----------------+
	  |
   general.h
	  |
	  +--------------------+-----------------+
						   |
						   |
	  +--------------------+-----------------+
	  |
lapack.h
	  |
	  +--------------------+-----------------+
						   |
	  +----------------+---+-----------------+
	  |				   |
complex.h  raytracing.h  fraction.h  physics.h  other.h
	  |
	  +--------------------+-----------------+
*/

#endif