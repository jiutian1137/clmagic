#pragma once
#ifndef clmagic_math_lapack_ORTHO_MATRIX_inl_h_
#define clmagic_math_lapack_ORTHO_MATRIX_inl_h_
#include "../lapack.h"

namespace clmagic {
	template<size_t N, typename T, bool _Major, size_t V> inline
	void ortho_matrix<N, T, _Major, V>::assign(std::initializer_list<vector<N, T, V>> _IVlist) {
		assert(_IVlist.size() == N);
		size_t _Counter = 0;
		for (auto _First = _IVlist.begin(); _First != _IVlist.end(); ++_First, ++_Counter) {
			this->row(_Counter) = normalize(*_First);
		}
	}

	template<size_t N, typename T, bool _Major, size_t V> inline
	ortho_matrix<N, T, _Major, V>::ortho_matrix(std::initializer_list<vector<N, T, V>> _IVlist) {
		this->assign(_IVlist);
	}

	template<size_t N, typename T, bool _Major, size_t V> inline
	void ortho_matrix<N, T, _Major, V>::inv() {
		for (size_t i = 0; i != this->rows(); ++i)
			for (size_t j = i; j != this->cols(); ++j)
				std::swap(this->at(i, j), this->at(j, i));
	}

	template<size_t N, typename T, bool _Major, size_t V> inline
	ortho_matrixN inverse(const ortho_matrixN& _Matrix) {
		return transpose(_Matrix);
	}

}// namespace clmagic


#endif