#ifndef clmagic_matH_h_
#define clmagic_matH_h_

// debug mode if debug compile
#ifdef _DEBUG

#ifndef _DEBUG
#define _DEBUG 1
#endif

#endif

#include "math/general/clmagic.h"
#include "math/general/general.h"
#include "math/general/unit.h"
#include "math/lapack/vector.h"
#include "math/lapack/matrix.h"
//#include "math/lapack/diag_matrix.h"
//#include "math/real.h"
//#include "math/simd.h"
//#include "math/bitop.h"
//#include "math/general.h"
//#include "math/lapack.h"
#include "math/complex.h"
#include "math/calculus.h"
#include "math/illumunation.h"
#include "math/raytracing.h"
#include "math/fraction.h"
#include "math/physics.h"
#include "math/other.h"
#include "math/mstring.h"


//#include "math/lapack/general.inl.h"
//#include "math/lapack/vector.inl.h"
//#include "math/lapack/matrix.inl.h"
//#include "math/lapack/diag_matrix.inl.h"
//#include "math/lapack/world_matrix.inl.h"
//#include "math/lapack/transform.inl.h"
//#include "math/lapack/intersect.inl.h"

//#include "math/complex/quaternion.inl.h"

#include "math/illumunation/illumunation.inl.h"

#include "math/raytracing/shape.inl.h"
#include "math/raytracing/intersect.inl.h"

#include "math/fraction/Perlin.inl.h"
//#include "math/fraction/Perlin_simplex.inl.h"
//#include "math/fraction/Worley.inl.h"
//#include "math/fraction/Worley_cellular.inl.h"
//#include "math/fraction/gradient.inl.h"
//#include "math/fraction/value_noise.inl.h"

#include "math/other/rect2d.inl.h"
#include "math/other/color_space.inl.h"

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