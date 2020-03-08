#pragma once
#ifndef clmagic_math_lapack_ROTATION_MATRIX_inl_h_
#define clmagic_math_lapack_ROTATION_MATRIX_inl_h_
#include "../lapack.h"

namespace clmagic {
	template<size_t M, size_t N, typename T, bool J, size_t V>
	rotation_matrix<M, N, T, J, V>::rotation_matrix() : _Mybase(static_cast<T>(1)) { }

	template<size_t M, size_t N, typename T, bool J, size_t V>
	rotation_matrix<M, N, T, J, V>::rotation_matrix(radian<T> roll, radian<T> yaw, radian<T> pitch)
		: rotation_matrix() /* diag(1) */ {

		const auto yaw_cos   = cos((T)yaw);
		const auto yaw_sin   = sin((T)yaw);
		const auto roll_cos  = cos((T)roll);
		const auto roll_sin  = sin((T)roll);
		const auto pitch_cos = cos((T)pitch);
		const auto pitch_sin = sin((T)pitch);

		(*this).row(0) = { yaw_cos * roll_cos, yaw_cos * roll_sin, yaw_sin };
		(*this).row(1) = { pitch_sin * -yaw_sin * roll_cos - pitch_cos * roll_sin, pitch_sin * -yaw_sin * roll_sin + pitch_cos * roll_cos, pitch_sin * yaw_cos };
		(*this).row(2) = { pitch_cos * -yaw_sin * roll_cos + pitch_sin * roll_sin, pitch_cos * -yaw_sin * roll_sin - pitch_sin * roll_cos, pitch_cos * yaw_cos };
		
		if constexpr(_Mybase::col_major()) {
			this->transpose();
		}
	}

	template<size_t M, size_t N, typename T, bool J, size_t V>
	rotation_matrix<M, N, T, J, V>::rotation_matrix(const unit_vector3<T, V>& Axis, radian<T> theta) {
		
	}

}// namespace clmagic

#endif