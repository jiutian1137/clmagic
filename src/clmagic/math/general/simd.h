//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_math_SIMD_h_
#define clmagic_math_SIMD_h_

#include "clmagic.h"

#include <assert.h>
#include <iostream>
#include <functional>

#include <ivec.h>
#include <fvec.h>
#include <dvec.h>


#define _mm_psfunc_epi32_1st(_mmXXX_, NAME, X) \
_mmXXX_##cvtps_epi32( _mmXXX_##NAME##_ps(_mmXXX_##cvtepi32_ps(X)) )

#define _mm_psfunc_epi32_2nd(_mmXXX_, NAME, X, Y) \
_mmXXX_##cvtps_epi32( _mmXXX_##NAME##_ps(_mmXXX_##cvtepi32_ps(X), _mmXXX_##cvtepi32_ps(Y)) )


// error C2679: binary '>>': no operator found which takes a right-hand operand of type '__m128'
	// @_Solve: using namespace::clmagic;
inline __m128 operator-(__m128 A) {
	return _mm_mul_ps(A, _mm_set1_ps(-1.0f));
}
inline __m128 operator+(__m128 A, __m128 B) {
	return _mm_add_ps(A, B);
}
inline __m128 operator-(__m128 A, __m128 B) {
	return _mm_sub_ps(A, B);
}
inline __m128 operator*(__m128 A, __m128 B) {
	return _mm_mul_ps(A, B);
}
inline __m128 operator/(__m128 A, __m128 B) {
	return _mm_div_ps(A, B);
}
inline __m128 operator%(__m128 A, __m128 B) {
	return _mm_fmod_ps(A, B);
}

inline __m128d operator-(__m128d A) {
	return _mm_mul_pd(A, _mm_set1_pd(-1.0));
}
inline __m128d operator+(__m128d A, __m128d B) {
	return _mm_add_pd(A, B);
}
inline __m128d operator-(__m128d A, __m128d B) {
	return _mm_sub_pd(A, B);
}
inline __m128d operator*(__m128d A, __m128d B) {
	return _mm_mul_pd(A, B);
}
inline __m128d operator/(__m128d A, __m128d B) {
	return _mm_div_pd(A, B);
}
inline __m128d operator%(__m128d A, __m128d B) {
	return _mm_fmod_pd(A, B);
}

inline __m128i operator-(__m128i A) {
	return _mm_mul_epi32(A, _mm_set1_epi32(-1));
}
inline __m128i operator+(__m128i A, __m128i B) {
	return _mm_add_epi32(A, B);
}
inline __m128i operator-(__m128i A, __m128i B) {
	return _mm_sub_epi32(A, B);
}
inline __m128i operator*(__m128i A, __m128i B) {
	return _mm_mul_epi32(A, B);
}
inline __m128i operator/(__m128i A, __m128i B) {
	return _mm_div_epi32(A, B);
}
inline __m128i operator%(__m128i A, __m128i B) {
	return _mm_psfunc_epi32_2nd(_mm_, fmod, A, B);
}

inline __m256 operator-(__m256 A) {
	return _mm256_mul_ps(A, _mm256_set1_ps(-1.0f));
}
inline __m256 operator+(__m256 A, __m256 B) {
	return _mm256_add_ps(A, B);
}
inline __m256 operator-(__m256 A, __m256 B) {
	return _mm256_sub_ps(A, B);
}
inline __m256 operator*(__m256 A, __m256 B) {
	return _mm256_mul_ps(A, B);
}
inline __m256 operator/(__m256 A, __m256 B) {
	return _mm256_div_ps(A, B);
}
inline __m256 operator%(__m256 A, __m256 B) {
	return _mm256_fmod_ps(A, B);
}

inline __m256d operator-(__m256d A) {
	return _mm256_mul_pd(A, _mm256_set1_pd(-1.0));
}
inline __m256d operator+(__m256d A, __m256d B) {
	return _mm256_add_pd(A, B);
}
inline __m256d operator-(__m256d A, __m256d B) {
	return _mm256_sub_pd(A, B);
}
inline __m256d operator*(__m256d A, __m256d B) {
	return _mm256_mul_pd(A, B);
}
inline __m256d operator/(__m256d A, __m256d B) {
	return _mm256_div_pd(A, B);
}
inline __m256d operator%(__m256d A, __m256d B) {
	return _mm256_fmod_pd(A, B);
}

inline __m256i operator-(__m256i A) {
	return _mm256_mul_epi32(A, _mm256_set1_epi32(-1));
}
inline __m256i operator+(__m256i A, __m256i B) {
	return _mm256_add_epi32(A, B);
}
inline __m256i operator-(__m256i A, __m256i B) {
	return _mm256_sub_epi32(A, B);
}
inline __m256i operator*(__m256i A, __m256i B) {
	return _mm256_mul_epi32(A, B);
}
inline __m256i operator/(__m256i A, __m256i B) {
	return _mm256_div_epi32(A, B);
}
inline __m256i operator%(__m256i A, __m256i B) {
	return _mm_psfunc_epi32_2nd(_mm256_, fmod, A, B);
}

inline __m512 operator-(__m512 A) {
	return _mm512_mul_ps(A, _mm512_set1_ps(-1.0f));
}
inline __m512 operator+(__m512 A, __m512 B) {
	return _mm512_add_ps(A, B);
}
inline __m512 operator-(__m512 A, __m512 B) {
	return _mm512_sub_ps(A, B);
}
inline __m512 operator*(__m512 A, __m512 B) {
	return _mm512_mul_ps(A, B);
}
inline __m512 operator/(__m512 A, __m512 B) {
	return _mm512_div_ps(A, B);
}
inline __m512 operator%(__m512 A, __m512 B) {
	return _mm512_fmod_ps(A, B);
}

inline __m512d operator-(__m512d A) {
	return _mm512_mul_pd(A, _mm512_set1_pd(-1.0));
}
inline __m512d operator+(__m512d A, __m512d B) {
	return _mm512_add_pd(A, B);
}
inline __m512d operator-(__m512d A, __m512d B) {
	return _mm512_sub_pd(A, B);
}
inline __m512d operator*(__m512d A, __m512d B) {
	return _mm512_mul_pd(A, B);
}
inline __m512d operator/(__m512d A, __m512d B) {
	return _mm512_div_pd(A, B);
}
inline __m512d operator%(__m512d A, __m512d B) {
	return _mm512_fmod_pd(A, B);
}
// SIMD has function more too

namespace clmagic {
	    
	template<typename _Ty>
		constexpr bool is_simd_v = std::_Is_any_of_v< 
			std::remove_cv_t<_Ty>, __m128, __m128d, __m128i, __m256, __m256d, __m512, __m512d, __m512i>;

	template<typename _Ty>
	struct block_traits {
		using block_type  = _Ty;
		using scalar_type = _Ty;

		static constexpr size_t size() {
			return 1;
		}

		static block_type set1(const scalar_type& _Val) {
			return block_type(_Val);
		}
	};

	template<>
	struct block_traits<__m128> {
		using block_type  = __m128;
		using scalar_type = float;

		static constexpr size_t size() {
			return 4;
		}

		static block_type set1(scalar_type _Val) { 
			return _mm_set1_ps(_Val);
		}
	};

	template<>
	struct block_traits<__m128d> {
		using block_type  = __m128d;
		using scalar_type = double;

		static constexpr size_t size() {
			return 2;
		}

		static block_type set1(scalar_type _Val) {
			return _mm_set1_pd(_Val);
		}
	};

	template<>
	struct block_traits<__m128i> {
		using block_type  = __m128i;
		using scalar_type = int32_t;

		static constexpr size_t size() {
			return 4;
		}

		static block_type set1(scalar_type _Val) {
			return _mm_set1_epi32(_Val);
		}
	};
	
	template<>
	struct block_traits<__m256> {
		using block_type  = __m256;
		using scalar_type = float;

		static constexpr size_t size() {
			return 8;
		}

		static block_type set1(scalar_type _Val) {
			return _mm256_set1_ps(_Val);
		}
	};

	template<>
	struct block_traits<__m256d> {
		using block_type  = __m256d;
		using scalar_type = double;

		static constexpr size_t size() {
			return 4;
		}

		static block_type set1(scalar_type _All) {
			return _mm256_set1_pd(_All);
		}
	};

	template<>
	struct block_traits<__m256i> {
		using block_type  = __m256i;
		using scalar_type = int32_t;

		static constexpr size_t size() {
			return 8;
		}

		static block_type set1(scalar_type _All) {
			return _mm256_set1_epi32(_All);
		}
	};

	template<>
	struct block_traits<__m512> {
		using block_type = __m512;
		using scalar_type = float;

		static constexpr size_t size() {
			return 16;
		}

		static block_type set1(scalar_type _All) {
			return _mm512_set1_ps(_All);
		}
	};

	template<>
	struct block_traits<__m512d> {
		using block_type = __m512d;
		using scalar_type = double;

		static constexpr size_t size() {
			return 8;
		}

		static block_type set1(scalar_type _All) {
			return _mm512_set1_pd(_All);
		}
	};

	template<>
	struct block_traits<__m512i> {
		using block_type = __m512i;
		using scalar_type = int32_t;

		static constexpr size_t size() {
			return 16;
		}

		static block_type set1(scalar_type _All) {
			return _mm512_set1_epi32(_All);
		}
	};


	template<typename _Block>
	typename block_traits<_Block>::scalar_type _Block_sum(const _Block& _Data) {
		using _Ty = typename block_traits<_Block>::scalar_type;

		const auto* _First = reinterpret_cast<const _Ty*>(&_Data);
		const auto* _Last  = _First + block_traits<_Block>::size();
		auto        _Sum   = *_First++;
		for (; _First != _Last; ++_First) {
			_Sum += *_First;
		}
		return _Sum;
	}

	template<typename _Block>
	typename block_traits<_Block>::scalar_type _Block_product(const _Block& _Data) {
		using _Ty = typename block_traits<_Block>::scalar_type;

		auto*       _First   = reinterpret_cast<const _Ty*>(&_Data);
		const auto* _Last    = _First + block_traits<_Block>::size();
		auto        _Product = *_First++;
		for (; _First != _Last; ++_First) {
			_Product *= *_First;
		}
		return _Product;
	}


	const union {
		int i[8];
		__m256 m;
	} __f32vec8_abs_mask_cheat = { 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff };
	const union {
		int i[8];
		__m256d m;
	} __f64vec4_abs_mask_cheat = { -1, 0x7fffffff, -1, 0x7fffffff, -1, 0x7fffffff, -1, 0x7fffffff };
	

	inline __m128  mod(__m128  _X, __m128  _Y) { return _mm_fmod_ps(_X, _Y); }
	inline __m128d mod(__m128d _X, __m128d _Y) { return _mm_fmod_pd(_X, _Y); }
	inline __m128i mod(__m128i _X, __m128i _Y) { return _mm_psfunc_epi32_2nd(_mm_, fmod, _X, _Y); }
	inline __m256  mod(__m256  _X, __m256  _Y) { return _mm256_fmod_ps(_X, _Y); }
	inline __m256d mod(__m256d _X, __m256d _Y) { return _mm256_fmod_pd(_X, _Y); }
	inline __m256i mod(__m256i _X, __m256i _Y) { return _mm_psfunc_epi32_2nd(_mm256_, fmod, _X, _Y); }
	inline __m512  mod(__m512  _X, __m512  _Y) { return _mm512_fmod_ps(_X, _Y); }
	inline __m512d mod(__m512d _X, __m512d _Y) { return _mm512_fmod_pd(_X, _Y); }
	inline __m512i mod(__m512i _X, __m512i _Y) { return _mm_psfunc_epi32_2nd(_mm512_, fmod, _X, _Y); }

	inline __m128  pow(__m128  _X, __m128  _Y) { return _mm_pow_ps(_X, _Y); }
	inline __m128d pow(__m128d _X, __m128d _Y) { return _mm_pow_pd(_X, _Y); }
	inline __m128i pow(__m128i _X, __m128i _Y) { return _mm_psfunc_epi32_2nd(_mm_, pow, _X, _Y); }
	inline __m256  pow(__m256  _X, __m256  _Y) { return _mm256_pow_ps(_X, _Y); }
	inline __m256d pow(__m256d _X, __m256d _Y) { return _mm256_pow_pd(_X, _Y); }
	inline __m256i pow(__m256i _X, __m256i _Y) { return _mm_psfunc_epi32_2nd(_mm256_, pow, _X, _Y); }
	inline __m512  pow(__m512  _X, __m512  _Y) { return _mm512_pow_ps(_X, _Y); }
	inline __m512d pow(__m512d _X, __m512d _Y) { return _mm512_pow_pd(_X, _Y); }
	inline __m512i pow(__m512i _X, __m512i _Y) { return _mm_psfunc_epi32_2nd(_mm512_, pow, _X, _Y); }
	
	inline __m128  abs(__m128  _X) { return _mm_and_ps(_X, _f32vec4_abs_mask); }
	inline __m128d abs(__m128d _X) { return _mm_and_pd(_X, _f64vec2_abs_mask); }
	inline __m128i abs(__m128i _X) { return _mm_abs_epi32(_X); }
	inline __m256  abs(__m256  _X) { return _mm256_and_ps(_X, __f32vec8_abs_mask_cheat.m); }
	inline __m256d abs(__m256d _X) { return _mm256_and_pd(_X, __f64vec4_abs_mask_cheat.m); }
	inline __m256i abs(__m256i _X) { return _mm256_abs_epi32(_X); }
	inline __m512  abs(__m512  _X) { return _mm512_abs_ps(_X); }
	inline __m512d abs(__m512d _X) { return _mm512_abs_pd(_X); }
	inline __m512i abs(__m512i _X) { return _mm512_abs_epi32(_X); }

	inline __m128  trunc(__m128  _X) { return _mm_trunc_ps(_X); }
	inline __m128d trunc(__m128d _X) { return _mm_trunc_pd(_X); }
	inline __m128i trunc(__m128i _X) { return _mm_set1_epi32(0); }
	inline __m256  trunc(__m256  _X) { return _mm256_trunc_ps(_X); }
	inline __m256d trunc(__m256d _X) { return _mm256_trunc_pd(_X); }
	inline __m256i trunc(__m256i _X) { return _mm256_set1_epi32(0); }
	inline __m512  trunc(__m512  _X) { return _mm512_trunc_ps(_X); }
	inline __m512d trunc(__m512d _X) { return _mm512_trunc_pd(_X); }
	inline __m512i trunc(__m512i _X) { return _mm512_set1_epi32(0); }

	inline __m128  floor(__m128  _X) { return _mm_floor_ps(_X); }
	inline __m128d floor(__m128d _X) { return _mm_floor_pd(_X); }
	inline __m128i floor(__m128i _X) { return _X; }
	inline __m256  floor(__m256  _X) { return _mm256_floor_ps(_X); }
	inline __m256d floor(__m256d _X) { return _mm256_floor_pd(_X); }
	inline __m256i floor(__m256i _X) { return _X; }
	inline __m512  floor(__m512  _X) { return _mm512_floor_ps(_X); }
	inline __m512d floor(__m512d _X) { return _mm512_floor_pd(_X); }
	inline __m512i floor(__m512i _X) { return _X; }

	inline __m128  ceil(__m128  _X) { return _mm_ceil_ps(_X); }
	inline __m128d ceil(__m128d _X) { return _mm_ceil_pd(_X); }
	inline __m128i ceil(__m128i _X) { return _X; }
	inline __m256  ceil(__m256  _X) { return _mm256_ceil_ps(_X); }
	inline __m256d ceil(__m256d _X) { return _mm256_ceil_pd(_X); }
	inline __m256i ceil(__m256i _X) { return _X; }
	inline __m512  ceil(__m512  _X) { return _mm512_ceil_ps(_X); }
	inline __m512d ceil(__m512d _X) { return _mm512_ceil_pd(_X); }
	inline __m512i ceil(__m512i _X) { return _X; }

	inline __m128  round(__m128  _X) { return _mm_round_ps(_X, _MM_ROUND_MODE_NEAREST); }
	inline __m128d round(__m128d _X) { return _mm_round_pd(_X, _MM_ROUND_MODE_NEAREST); }
	inline __m128i round(__m128i _X) { return _X; }
	inline __m256  round(__m256  _X) { return _mm256_round_ps(_X, _MM_ROUND_MODE_NEAREST); }
	inline __m256d round(__m256d _X) { return _mm256_round_pd(_X, _MM_ROUND_MODE_NEAREST); }
	inline __m256i round(__m256i _X) { return _X; }
	inline __m512  round(__m512  _X) { return _mm512_roundscale_ps(_X, _MM_ROUND_MODE_NEAREST); }
	inline __m512d round(__m512d _X) { return _mm512_roundscale_pd(_X, _MM_ROUND_MODE_NEAREST); }
	inline __m512i round(__m512i _X) { return _X; }

	inline __m128  sqrt(__m128  _X) { return _mm_sqrt_ps(_X); }
	inline __m128d sqrt(__m128d _X) { return _mm_sqrt_pd(_X); }
	inline __m128i sqrt(__m128i _X) { return _mm_psfunc_epi32_1st(_mm_, sqrt, _X); }
	inline __m256  sqrt(__m256  _X) { return _mm256_sqrt_ps(_X); }
	inline __m256d sqrt(__m256d _X) { return _mm256_sqrt_pd(_X); }
	inline __m256i sqrt(__m256i _X) { return _mm_psfunc_epi32_1st(_mm256_, sqrt, _X); }
	inline __m512  sqrt(__m512  _X) { return _mm512_sqrt_ps(_X); }
	inline __m512d sqrt(__m512d _X) { return _mm512_sqrt_pd(_X); }
	inline __m512i sqrt(__m512i _X) { return _mm_psfunc_epi32_1st(_mm512_, sqrt, _X); }

	inline __m128  cbrt(__m128  _X) { return _mm_cbrt_ps(_X); }
	inline __m128d cbrt(__m128d _X) { return _mm_cbrt_pd(_X); }
	inline __m128i cbrt(__m128i _X) { return _mm_psfunc_epi32_1st(_mm_, cbrt, _X); }
	inline __m256  cbrt(__m256  _X) { return _mm256_cbrt_ps(_X); }
	inline __m256d cbrt(__m256d _X) { return _mm256_cbrt_pd(_X); }
	inline __m256i cbrt(__m256i _X) { return _mm_psfunc_epi32_1st(_mm256_, cbrt, _X); }
	inline __m512  cbrt(__m512  _X) { return _mm512_cbrt_ps(_X); }
	inline __m512d cbrt(__m512d _X) { return _mm512_cbrt_pd(_X); }
	inline __m512i cbrt(__m512i _X) { return _mm_psfunc_epi32_1st(_mm512_, cbrt, _X); }

	inline __m128  invsqrt(__m128  _X) { return _mm_invsqrt_ps(_X); }
	inline __m128d invsqrt(__m128d _X) { return _mm_invsqrt_pd(_X); }
	inline __m128i invsqrt(__m128i _X) { return _mm_psfunc_epi32_1st(_mm_, invsqrt, _X); }
	inline __m256  invsqrt(__m256  _X) { return _mm256_invsqrt_ps(_X); }
	inline __m256d invsqrt(__m256d _X) { return _mm256_invsqrt_pd(_X); }
	inline __m256i invsqrt(__m256i _X) { return _mm_psfunc_epi32_1st(_mm256_, invsqrt, _X); }
	inline __m512  invsqrt(__m512  _X) { return _mm512_invsqrt_ps(_X); }
	inline __m512d invsqrt(__m512d _X) { return _mm512_invsqrt_pd(_X); }
	inline __m512i invsqrt(__m512i _X) { return _mm_psfunc_epi32_1st(_mm512_, invsqrt, _X); }

	inline __m128  invcbrt(__m128  _X) { return _mm_invcbrt_ps(_X); }
	inline __m128d invcbrt(__m128d _X) { return _mm_invcbrt_pd(_X); }
	inline __m128i invcbrt(__m128i _X) { return _mm_psfunc_epi32_1st(_mm_, invcbrt, _X); }
	inline __m256  invcbrt(__m256  _X) { return _mm256_invcbrt_ps(_X); }
	inline __m256d invcbrt(__m256d _X) { return _mm256_invcbrt_pd(_X); }
	inline __m256i invcbrt(__m256i _X) { return _mm_psfunc_epi32_1st(_mm256_, invcbrt, _X); }
	inline __m512  invcbrt(__m512  _X) { return _mm512_invcbrt_ps(_X); }
	inline __m512d invcbrt(__m512d _X) { return _mm512_invcbrt_pd(_X); }
	inline __m512i invcbrt(__m512i _X) { return _mm_psfunc_epi32_1st(_mm512_, invcbrt, _X); }

	// log(e)(x)
	inline __m128  log(__m128  _X) { return _mm_log_ps(_X); }
	inline __m128d log(__m128d _X) { return _mm_log_pd(_X); }
	inline __m128i log(__m128i _X) { return _mm_psfunc_epi32_1st(_mm_, log, _X); }
	inline __m256  log(__m256  _X) { return _mm256_log_ps(_X); }
	inline __m256d log(__m256d _X) { return _mm256_log_pd(_X); }
	inline __m256i log(__m256i _X) { return _mm_psfunc_epi32_1st(_mm256_, log, _X); }
	inline __m512  log(__m512  _X) { return _mm512_log_ps(_X); }
	inline __m512d log(__m512d _X) { return _mm512_log_pd(_X); }
	inline __m512i log(__m512i _X) { return _mm_psfunc_epi32_1st(_mm512_, log, _X); }

	inline __m128  log2(__m128  _X) { return _mm_log2_ps(_X); }
	inline __m128d log2(__m128d _X) { return _mm_log2_pd(_X); }
	inline __m128i log2(__m128i _X) { return _mm_psfunc_epi32_1st(_mm_, log2, _X); }
	inline __m256  log2(__m256  _X) { return _mm256_log2_ps(_X); }
	inline __m256d log2(__m256d _X) { return _mm256_log2_pd(_X); }
	inline __m256i log2(__m256i _X) { return _mm_psfunc_epi32_1st(_mm256_, log2, _X); }
	inline __m512  log2(__m512  _X) { return _mm512_log2_ps(_X); }
	inline __m512d log2(__m512d _X) { return _mm512_log2_pd(_X); }
	inline __m512i log2(__m512i _X) { return _mm_psfunc_epi32_1st(_mm512_, log2, _X); }

	inline __m128  log10(__m128  _X) { return _mm_log10_ps(_X); }
	inline __m128d log10(__m128d _X) { return _mm_log10_pd(_X); }
	inline __m128i log10(__m128i _X) { return _mm_psfunc_epi32_1st(_mm_, log10, _X); }
	inline __m256  log10(__m256  _X) { return _mm256_log10_ps(_X); }
	inline __m256d log10(__m256d _X) { return _mm256_log10_pd(_X); }
	inline __m256i log10(__m256i _X) { return _mm_psfunc_epi32_1st(_mm256_, log10, _X); }
	inline __m512  log10(__m512  _X) { return _mm512_log10_ps(_X); }
	inline __m512d log10(__m512d _X) { return _mm512_log10_pd(_X); }
	inline __m512i log10(__m512i _X) { return _mm_psfunc_epi32_1st(_mm512_, log10, _X); }

	// pow(e,x)
	inline __m128  exp(__m128  _X) { return _mm_exp_ps(_X); }
	inline __m128d exp(__m128d _X) { return _mm_exp_pd(_X); }
	inline __m128i exp(__m128i _X) { return _mm_psfunc_epi32_1st(_mm_, exp, _X); }
	inline __m256  exp(__m256  _X) { return _mm256_exp_ps(_X); }
	inline __m256d exp(__m256d _X) { return _mm256_exp_pd(_X); }
	inline __m256i exp(__m256i _X) { return _mm_psfunc_epi32_1st(_mm256_, exp, _X); }
	inline __m512  exp(__m512  _X) { return _mm512_exp_ps(_X); }
	inline __m512d exp(__m512d _X) { return _mm512_exp_pd(_X); }
	inline __m512i exp(__m512i _X) { return _mm_psfunc_epi32_1st(_mm512_, exp, _X); }

	// pow(2, _X)
	inline __m128  exp2(__m128  _X) { return _mm_exp2_ps(_X); }
	inline __m128d exp2(__m128d _X) { return _mm_exp2_pd(_X); }
	inline __m128i exp2(__m128i _X) { return _mm_psfunc_epi32_1st(_mm_, exp2, _X); }
	inline __m256  exp2(__m256  _X) { return _mm256_exp2_ps(_X); }
	inline __m256d exp2(__m256d _X) { return _mm256_exp2_pd(_X); }
	inline __m256i exp2(__m256i _X) { return _mm_psfunc_epi32_1st(_mm256_, exp2, _X); }
	inline __m512  exp2(__m512  _X) { return _mm512_exp2_ps(_X); }
	inline __m512d exp2(__m512d _X) { return _mm512_exp2_pd(_X); }
	inline __m512i exp2(__m512i _X) { return _mm_psfunc_epi32_1st(_mm512_, exp2, _X); }

	inline __m128  exp10(__m128  _X) { return _mm_exp10_ps(_X); }
	inline __m128d exp10(__m128d _X) { return _mm_exp10_pd(_X); }
	inline __m128i exp10(__m128i _X) { return _mm_psfunc_epi32_1st(_mm_, exp10, _X); }
	inline __m256  exp10(__m256  _X) { return _mm256_exp10_ps(_X); }
	inline __m256d exp10(__m256d _X) { return _mm256_exp10_pd(_X); }
	inline __m256i exp10(__m256i _X) { return _mm_psfunc_epi32_1st(_mm256_, exp10, _X); }
	inline __m512  exp10(__m512  _X) { return _mm512_exp10_ps(_X); }
	inline __m512d exp10(__m512d _X) { return _mm512_exp10_pd(_X); }
	inline __m512i exp10(__m512i _X) { return _mm_psfunc_epi32_1st(_mm512_, exp10, _X); }

	// log(e)(_X+1)
	inline __m128  log1p(__m128  _X) { return _mm_log1p_ps(_X); }
	inline __m128d log1p(__m128d _X) { return _mm_log1p_pd(_X); }
	inline __m128i log1p(__m128i _X) { return _mm_psfunc_epi32_1st(_mm_, log1p, _X); }
	inline __m256  log1p(__m256  _X) { return _mm256_log1p_ps(_X); }
	inline __m256d log1p(__m256d _X) { return _mm256_log1p_pd(_X); }
	inline __m256i log1p(__m256i _X) { return _mm_psfunc_epi32_1st(_mm256_, log1p, _X); }
	inline __m512  log1p(__m512  _X) { return _mm512_log1p_ps(_X); }
	inline __m512d log1p(__m512d _X) { return _mm512_log1p_pd(_X); }
	inline __m512i log1p(__m512i _X) { return _mm_psfunc_epi32_1st(_mm512_, log1p, _X); }

	// exp(x)-1
	inline __m128  expm1(__m128  _X) { return _mm_expm1_ps(_X); }
	inline __m128d expm1(__m128d _X) { return _mm_expm1_pd(_X); }
	inline __m128i expm1(__m128i _X) { return _mm_psfunc_epi32_1st(_mm_, expm1, _X); }
	inline __m256  expm1(__m256  _X) { return _mm256_expm1_ps(_X); }
	inline __m256d expm1(__m256d _X) { return _mm256_expm1_pd(_X); }
	inline __m256i expm1(__m256i _X) { return _mm_psfunc_epi32_1st(_mm256_, expm1, _X); }
	inline __m512  expm1(__m512  _X) { return _mm512_expm1_ps(_X); }
	inline __m512d expm1(__m512d _X) { return _mm512_expm1_pd(_X); }
	inline __m512i expm1(__m512i _X) { return _mm_psfunc_epi32_1st(_mm512_, expm1, _X); }

	inline __m128  erf(__m128  _X) { return _mm_erf_ps(_X); }
	inline __m128d erf(__m128d _X) { return _mm_erf_pd(_X); }
	inline __m256  erf(__m256  _X) { return _mm256_erf_ps(_X); }
	inline __m256d erf(__m256d _X) { return _mm256_erf_pd(_X); }
	inline __m512  erf(__m512  _X) { return _mm512_erf_ps(_X); }
	inline __m512d erf(__m512d _X) { return _mm512_erf_pd(_X); }

	inline __m128  erfc(__m128  _X) { return _mm_erfc_ps(_X); }
	inline __m128d erfc(__m128d _X) { return _mm_erfc_pd(_X); }
	inline __m256  erfc(__m256  _X) { return _mm256_erfc_ps(_X); }
	inline __m256d erfc(__m256d _X) { return _mm256_erfc_pd(_X); }
	inline __m512  erfc(__m512  _X) { return _mm512_erfc_ps(_X); }
	inline __m512d erfc(__m512d _X) { return _mm512_erfc_pd(_X); }

	// sqrt(a² + b²)
	inline __m128  hypot(__m128  _X, __m128  _Y) { return _mm_hypot_ps(_X, _Y); }
	inline __m128d hypot(__m128d _X, __m128d _Y) { return _mm_hypot_pd(_X, _Y); }
	inline __m128i hypot(__m128i _X, __m128i _Y) { return _mm_psfunc_epi32_2nd(_mm_, hypot, _X, _Y); }
	inline __m256  hypot(__m256  _X, __m256  _Y) { return _mm256_hypot_ps(_X, _Y); }
	inline __m256d hypot(__m256d _X, __m256d _Y) { return _mm256_hypot_pd(_X, _Y); }
	inline __m256i hypot(__m256i _X, __m256i _Y) { return _mm_psfunc_epi32_2nd(_mm256_, hypot, _X, _Y); }
	inline __m512  hypot(__m512  _X, __m512  _Y) { return _mm512_hypot_ps(_X, _Y); }
	inline __m512d hypot(__m512d _X, __m512d _Y) { return _mm512_hypot_pd(_X, _Y); }
	inline __m512i hypot(__m512i _X, __m512i _Y) { return _mm_psfunc_epi32_2nd(_mm512_, hypot, _X, _Y); }

	inline __m128  sin(__m128  _X) { return _mm_sin_ps(_X); }
	inline __m128d sin(__m128d _X) { return _mm_sin_pd(_X); }
	inline __m256  sin(__m256  _X) { return _mm256_sin_ps(_X); }
	inline __m256d sin(__m256d _X) { return _mm256_sin_pd(_X); }
	inline __m512  sin(__m512  _X) { return _mm512_sin_ps(_X); }
	inline __m512d sin(__m512d _X) { return _mm512_sin_pd(_X); }

	inline __m128  cos(__m128  _X) { return _mm_cos_ps(_X); }
	inline __m128d cos(__m128d _X) { return _mm_cos_pd(_X); }
	inline __m256  cos(__m256  _X) { return _mm256_cos_ps(_X); }
	inline __m256d cos(__m256d _X) { return _mm256_cos_pd(_X); }
	inline __m512  cos(__m512  _X) { return _mm512_cos_ps(_X); }
	inline __m512d cos(__m512d _X) { return _mm512_cos_pd(_X); }

	inline __m128  tan(__m128  _X) { return _mm_tan_ps(_X); }
	inline __m128d tan(__m128d _X) { return _mm_tan_pd(_X); }
	inline __m256  tan(__m256  _X) { return _mm256_tan_ps(_X); }
	inline __m256d tan(__m256d _X) { return _mm256_tan_pd(_X); }
	inline __m512  tan(__m512  _X) { return _mm512_tan_ps(_X); }
	inline __m512d tan(__m512d _X) { return _mm512_tan_pd(_X); }

	inline __m128  asin(__m128  _X) { return _mm_asin_ps(_X); }
	inline __m128d asin(__m128d _X) { return _mm_asin_pd(_X); }
	inline __m256  asin(__m256  _X) { return _mm256_asin_ps(_X); }
	inline __m256d asin(__m256d _X) { return _mm256_asin_pd(_X); }
	inline __m512  asin(__m512  _X) { return _mm512_asin_ps(_X); }
	inline __m512d asin(__m512d _X) { return _mm512_asin_pd(_X); }

	inline __m128  acos(__m128  _X) { return _mm_acos_ps(_X); }
	inline __m128d acos(__m128d _X) { return _mm_acos_pd(_X); }
	inline __m256  acos(__m256  _X) { return _mm256_acos_ps(_X); }
	inline __m256d acos(__m256d _X) { return _mm256_acos_pd(_X); }
	inline __m512  acos(__m512  _X) { return _mm512_acos_ps(_X); }
	inline __m512d acos(__m512d _X) { return _mm512_acos_pd(_X); }

	inline __m128  atan(__m128  _X) { return _mm_atan_ps(_X); }
	inline __m128d atan(__m128d _X) { return _mm_atan_pd(_X); }
	inline __m256  atan(__m256  _X) { return _mm256_atan_ps(_X); }
	inline __m256d atan(__m256d _X) { return _mm256_atan_pd(_X); }
	inline __m512  atan(__m512  _X) { return _mm512_atan_ps(_X); }
	inline __m512d atan(__m512d _X) { return _mm512_atan_pd(_X); }

	inline __m128  atan2(__m128  _Y, __m128  _X) { return _mm_atan2_ps(_X, _Y); }
	inline __m128d atan2(__m128d _Y, __m128d _X) { return _mm_atan2_pd(_X, _Y); }
	inline __m256  atan2(__m256  _Y, __m256  _X) { return _mm256_atan2_ps(_X, _Y); }
	inline __m256d atan2(__m256d _Y, __m256d _X) { return _mm256_atan2_pd(_X, _Y); }
	inline __m512  atan2(__m512  _Y, __m512  _X) { return _mm512_atan2_ps(_X, _Y); }
	inline __m512d atan2(__m512d _Y, __m512d _X) { return _mm512_atan2_pd(_X, _Y); }

	inline __m128  sinh(__m128  _X) { return _mm_sinh_ps(_X); }
	inline __m128d sinh(__m128d _X) { return _mm_sinh_pd(_X); }
	inline __m256  sinh(__m256  _X) { return _mm256_sinh_ps(_X); }
	inline __m256d sinh(__m256d _X) { return _mm256_sinh_pd(_X); }
	inline __m512  sinh(__m512  _X) { return _mm512_sinh_ps(_X); }
	inline __m512d sinh(__m512d _X) { return _mm512_sinh_pd(_X); }

	inline __m128  cosh(__m128  _X) { return _mm_cosh_ps(_X); }
	inline __m128d cosh(__m128d _X) { return _mm_cosh_pd(_X); }
	inline __m256  cosh(__m256  _X) { return _mm256_cosh_ps(_X); }
	inline __m256d cosh(__m256d _X) { return _mm256_cosh_pd(_X); }
	inline __m512  cosh(__m512  _X) { return _mm512_cosh_ps(_X); }
	inline __m512d cosh(__m512d _X) { return _mm512_cosh_pd(_X); }

	inline __m128  tanh(__m128  _X) { return _mm_tanh_ps(_X); }
	inline __m128d tanh(__m128d _X) { return _mm_tanh_pd(_X); }
	inline __m256  tanh(__m256  _X) { return _mm256_tanh_ps(_X); }
	inline __m256d tanh(__m256d _X) { return _mm256_tanh_pd(_X); }
	inline __m512  tanh(__m512  _X) { return _mm512_tanh_ps(_X); }
	inline __m512d tanh(__m512d _X) { return _mm512_tanh_pd(_X); }

	inline __m128  asinh(__m128  _X) { return _mm_asinh_ps(_X); }
	inline __m128d asinh(__m128d _X) { return _mm_asinh_pd(_X); }
	inline __m256  asinh(__m256  _X) { return _mm256_asinh_ps(_X); }
	inline __m256d asinh(__m256d _X) { return _mm256_asinh_pd(_X); }
	inline __m512  asinh(__m512  _X) { return _mm512_asinh_ps(_X); }
	inline __m512d asinh(__m512d _X) { return _mm512_asinh_pd(_X); }

	inline __m128  acosh(__m128  _X) { return _mm_acosh_ps(_X); }
	inline __m128d acosh(__m128d _X) { return _mm_acosh_pd(_X); }
	inline __m256  acosh(__m256  _X) { return _mm256_acosh_ps(_X); }
	inline __m256d acosh(__m256d _X) { return _mm256_acosh_pd(_X); }
	inline __m512  acosh(__m512  _X) { return _mm512_acosh_ps(_X); }
	inline __m512d acosh(__m512d _X) { return _mm512_acosh_pd(_X); }

	inline __m128  atanh(__m128  _X) { return _mm_atanh_ps(_X); }
	inline __m128d atanh(__m128d _X) { return _mm_atanh_pd(_X); }
	inline __m256  atanh(__m256  _X) { return _mm256_atanh_ps(_X); }
	inline __m256d atanh(__m256d _X) { return _mm256_atanh_pd(_X); }
	inline __m512  atanh(__m512  _X) { return _mm512_atanh_ps(_X); }
	inline __m512d atanh(__m512d _X) { return _mm512_atanh_pd(_X); }

}// namespace clmagic

#endif