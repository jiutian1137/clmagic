//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
// Apache Licene 2.0
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_math_lapack_GENERAL_inl_h_
#define clmagic_math_lapack_GENERAL_inl_h_
#include "../lapack.h"

namespace clmagic {
	// < mul matrix >
	template<typename _Ty> inline
	void matrix_mul( /*in*/const _Ty* _SrcMxN, /*in*/const _Ty* _SrcNxP, size_t M, size_t N, size_t P, /*out*/_Ty* _DstMxP ) {
#if defined(clmagic_using_MKL)
		cblas_sgemm(
			CBLAS_LAYOUT::CblasRowMajor,
			CBLAS_TRANSPOSE::CblasNoTrans,
			CBLAS_TRANSPOSE::CblasNoTrans,
			int(_M), int(_N), int(_P),
			1.f, _Plhs, int(_N),
			_Prhs, int(_N), 1.f,
			_Pdst, int(_P)
		);
#else
		auto A = [_SrcMxN, N](size_t _Row, size_t _Col) ->const _Ty& { return (_SrcMxN[_Row * N + _Col]); };// matMxN
		auto B = [_SrcNxP, P](size_t _Row, size_t _Col) ->const _Ty& { return (_SrcNxP[_Row * P + _Col]); };// matNxP
		auto C = [_DstMxP, P](size_t _Row, size_t _Col) ->      _Ty& { return (_DstMxP[_Row * P + _Col]); };// matMxP

		/* ∑(k=0,N)(Aik * Bkj), 
		i in [0, M)
		j in [0, P)
		*/
		for (size_t i = 0; i < M; ++i) {
			for (size_t j = 0; j < P; ++j) {
				C(i, j) = A(i, 0) * B(0, j);
				for (size_t k = 1; k < N; ++k) {
					C(i, j) += A(i, k) * B(k, j);
				}
			}
		}
#endif
	}
	// </ mul matrix>

	template<typename _Ty>
	size_t min_major(const matrix_slice<_Ty>& A, size_t k) {
		assert(k < A.cols());
		for (size_t i = k + 1; i < A.rows(); ++i) {
			if ( abs(abs(A.at(i, k)) - _Ty(1)) < abs(abs(A.at(k, k)) - _Ty(1)) ) {
				k = i;
			}
		}
		return ( k );
	}

	/* Please clear to ZERO before */
	template<typename _Ty>
	size_t find_row_major(matrix_slice<_Ty> A, size_t k) {
		assert(k < A.rows() && k < A.cols());
		for (size_t i = k; i < A.rows(); ++i) {
			if ( A[i][k/*solid*/] != _Ty(0) ) {
				return ( i );
			}
		}
		return ( std::string::npos );
	}

	template<typename _Ty>
	size_t find_col_major(matrix_slice<_Ty> A, size_t k) {
		assert(k < A.rows() && k < A.cols());
		for (size_t j = k; j < A.cols(); ++j) {
			if ( A[k/*solid*/][j] != _Ty(0) ) {
				return ( j );
			}
		}
		return ( std::string::npos );
	}

	template<typename _Ty>
	void swap_row(matrix_slice<_Ty> A, size_t i, size_t r) {
		assert(i < A.rows() && r < A.rows());
		for (size_t j = 0; j != A.cols(); ++j) {
			std::swap(A.at(i, j), A.at(r, j));
		}
	}

	template<typename _Ty>
	void swap_col(matrix_slice<_Ty> A, size_t j, size_t r) {
		assert(j < A.cols() && r < A.cols());
		for (size_t i = 0; i != A.rows(); ++i) {
			std::swap(A.at(i, j), A.at(i, r));
		}
	}

	template<typename _Ty>
	void scale_matrix(matrix_slice<_Ty> A, _Ty u) {
		for (size_t i = 0; i != A.rows(); ++i) {
			for (size_t j = 0; j != A.cols(); ++j) {
				A[i][j] *= u;
			}
		}
	}

	template<typename _Ty>
	void scale_row(matrix_slice<_Ty> A, size_t i, _Ty alpha, size_t j = 0) {
		assert(i < A.rows() && j < A.cols());
		for (; j < A.cols(); ++j) {
			A.at(i, j) *= alpha;
		}
	}

	template<typename _Ty>
	void scale_col(matrix_slice<_Ty> A, size_t j, _Ty alpha, size_t i = 0) {
		assert(i < A.rows() && j < A.cols());
		for (; i < A.rows(); ++i) {
			A.at(i, j) *= alpha;
		}
	}

	template<typename _Ty>
	void elimination(matrix_slice<_Ty> A, size_t i, size_t k, _Ty alpha, size_t j = 0) {
		assert(k < A.rows() && k < A.cols());
		for (; j < A.cols(); ++j) {
			A.at(i, j) += alpha * A.at(k, j);
		}
	}

	template<typename _Ty> inline
	bool Lu(matrix_slice<_Ty> A) {
		_Ty t = _Ty(R(1));
		size_t N = minval(A.rows(), A.cols());
		
		/* c * !N */
		for (size_t k = 0; k != N; ++k) {
			size_t mj = find_row_major(A, k);
			if (mj != std::string::npos) {
				if (mj != k) {
					swap_row(A, k, mj);
					t = -t;
				}
			} else {
				mj = find_col_major(A, k);
				if (mj != std::string::npos) {
					swap_col(A, k, mj);
					t = -t;
				} else {
					continue;
				}
			}

			if ( A[k][k] != _Ty(1)) { /* 1 = x*Aij, x = 1/Aij */
				scale_row(A, k, _Ty(1) / A[k][k], k);
			}

			for (size_t i = k + 1; i != A.rows(); ++i) {
				elimination(A, i, k, -A[i][k], k);
			}
		}

		/* c * !N */
		for (size_t k = N - 1; k != -1; --k) {
			for (size_t i = k - 1; i != -1; --i) {
				elimination(A, i, k, -A[i][k], k);
			}
		}

		/*
		①:
		[ 1 0 0 ] x0
		[ 0 1 0 ] x1
		[ 0 0 1 ] x2
		good solve

		②:
		[ 1 0 0  0 ]
		[ 0 1 0  12]
		[ 0 0 1 -29]
		[ 0 0 0  1 final]
		[ 0 0 0  0 ] error row
		if is not Homogeneous matrix, no solution

		[ 1 0 0 ] [ 1 0 0 ]
		[ 0 0 0 ] [ 0 1 0 ]
		[ 0 0 1 ] [ 0 0 0 ]
		no solution
		*/
		if ( A.rows() < A.cols() ) {
			return (false);
		} else {
			for (size_t i = 0; i != N; ++i) {
				if (A[i][i] == _Ty(0)) {
					return (false);
				}
			}
		}
		return ( true );
	}

	template<typename _Ty> inline
	bool Lu(/*inout*/_Ty* pA, size_t M, size_t N) {
		return Lu(matrix_slice<_Ty>(pA, M, N));
	}

	template<typename _Ty> inline
	bool Lu(matrix_slice<_Ty> A, matrix_slice<_Ty> Ainv) {
		_Ty t = _Ty(R(1));
		size_t N = minval(A.rows(), A.cols());

		/* c * !N */
		for (size_t k = 0; k != N; ++k) {
			size_t mj = find_row_major(A, k);
			if (mj != std::string::npos) {
				if (mj != k) {
					swap_row(A, k, mj);
					swap_row(Ainv, k, mj);
					t = -t;
				}
			} else {
				mj = find_col_major(A, k);
				if (mj != std::string::npos) {
					swap_col(A, k, mj);
					swap_col(Ainv, k, mj);
					t = -t;
				} else {
					continue;
				}
			}

			if (A[k][k] != _Ty(1)) { /* 1 = x*Aij, x = 1/Aij */
				_Ty alpha = _Ty(R(1)) / A[k][k];
				scale_row(A, k, alpha, k);
				scale_row(Ainv, k, alpha, k);
			}

			for (size_t i = k + 1; i != A.rows(); ++i) {
				_Ty alpha = -A[i][k];
				elimination(A, i, k, alpha, k);
				elimination(Ainv, i, k, alpha, k);
			}
		}

		/* c * !N */
		for (size_t k = N - 1; k != -1; --k) {
			for (size_t i = k - 1; i != -1; --i) {
				_Ty alpha = -A[i][k];
				elimination( A  , i, k, alpha, k);
				elimination(Ainv, i, k, alpha, 0/*inv matrix index*/);
			}
		}

		if (A.rows() < A.cols()) {
			return (false);
		} else {
			for (size_t i = 0; i != N; ++i) {
				if (A[i][i] == _Ty(0)) {
					return (false);
				}
			}
		}
		return (true);
	}

	template<typename _Ty> inline
	bool Lu(/*inout*/_Ty* pA, size_t M, size_t N, _Ty* pAinv) {
		/* 
		@_Result:
		       A       Ainv
		    [ a b c | 1 0 0 ]
			[ d e f | 0 1 0 ]
			[ g h i | 0 0 1 ]
			       ↓
			[ 1 0 0 | a b c ]
			[ 0 1 0 | d e f ]
			[ 0 0 1 | g h i ]
			   A       Ainv
		@_Describ:
			two matrix combine to one matrix, and solve
		*/
		return Lu(matrix_slice<_Ty>(pA, M, N), matrix_slice<_Ty>(pAinv, M, N));
	}

}// namespace clmagic


#endif