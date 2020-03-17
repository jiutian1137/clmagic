//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_math_lapack_MATRIX_h_
#define clmagic_math_lapack_MATRIX_h_
#include "vector.h"

namespace clmagic {

	template<typename _Ty> inline
	void transpose(matrix_slice<const _Ty> _Left, matrix_slice<_Ty> _Right) {
		assert(_Left.rows() == _Right.cols());
		assert(_Left.cols() == _Right.rows());
		for (size_t i = 0; i != _Left.rows(); ++i) {
			for (size_t j = 0; j != _Left.cols(); ++j) {
				_Right->at(j, i) = _Left->at(i, j);
			}
		}
	}

	template<typename _Ty> inline
	void transpose(const _Ty* _InPtr, size_t _InRow, size_t _InCol, _Ty* _OutPtr, size_t _OutRow, size_t _OutCol) {
		transpose(matrix_slice<const _Ty>(_InPtr, _InRow, _InCol), 
			matrix_slice<_Ty>(_OutPtr, _OutRow, _OutCol) );
	}


	/*- - - - - - - - - - - - - - - transposed_reference - - - - - - - - - - - - - - - - -*/
	template<typename _Matx>
	struct transposed_reference {
		using type        = transposed_reference<_Matx>;
		using transpose_type = _Matx;
		using scalar_type = typename _Matx::scalar_type;
		using vector_type = typename _Matx::vector_type;

		using row_type       = typename transpose_type::col_type;
		using row_const_type = typename transpose_type::col_const_type;
		using col_type       = typename transpose_type::row_type;
		using col_const_type = typename transpose_type::row_const_type;

		/*using iterator       = typename transpose_type::iterator;
		using const_iterator = typename transpose_type::const_iterator;*/

		static constexpr size_t vect() {// vector type
			return transpose_type::vect(); }
		static constexpr size_t rows() {// Number of matrix rows
			return transpose_type::cols(); }
		static constexpr size_t cols() {// Number of matrix columes
			return transpose_type::rows(); }
		static constexpr size_t diags() {// Number of diagonal elements
			return (rows() < cols() ? rows() : cols()); }
		static constexpr size_t size() {// Number of scalar
			return transpose_type::size(); }
		static constexpr bool col_major() {// Is colume major order 
			return transpose_type::col_major(); }
		static constexpr bool row_major() {// Is row major order
			return (!col_major()); }

		transposed_reference(_Matx& _Matrix) : _Myref(_Matrix) { }

		_Matx& _Myref;

		decltype(_Myref.at(0, 0)) at(size_t i, size_t j) const { return _Myref.at(j, i); }

		decltype(_Myref.col(0)) row(size_t i) const { return (_Myref.col(i)); }

		decltype(_Myref.row(0)) col(size_t j) const { return (_Myref.row(j)); }

		_Matx& transpose() const {
			return _Myref;
		}

		friend std::string to_string(const transposed_reference<_Matx>& _Matrix) {
			std::string _Str = to_string(_Matrix.row(0)) + "\n";
			for (size_t i = 1; i != _Matrix.rows(); ++i) {
				_Str += to_string(_Matrix.row(i)) + "\n";
			}
			_Str.pop_back();

			return (_Str);
		}

		friend std::ostream& operator<<(std::ostream& _Ostr, const transposed_reference<_Matx>& _Matrix) {
			return (_Ostr << to_string(_Matrix));
		}
	};

	template<typename _TyMat>
		using transposed_const_reference = transposed_reference<const _TyMat>;


	// Used to <_Mtag>
	struct normal_matrix_tag {};
	struct diagonal_matrix_tag {};

	/*- - - - - - - - - - - - - - - _Common_matrix - - - - - - - - - - - - - - - - -*/
	template<typename _Ty, size_t _Rows, size_t _Cols, typename _Block = _Ty>
	class _Common_matrix {
		class _Colume_reference : public vector<_Ty, _Rows, _Block> {
			using _Mybase = vector<_Ty, _Rows, _Block>;

		public:
			using scalar_type = _Ty;
			using vector_type = vector<_Ty, _Rows, _Block>;

			_Colume_reference(_Common_matrix& _Matrix, size_t _Jx)
				: _Myref(_Matrix), _Colume(_Jx) {
				this->_Pull();
			}

			template<typename ..._Tys>
			void assign(const _Tys&... _Args) {
				_Mybase::assign(_Args...);
				this->_Push();
			}

			_Colume_reference& operator=(const vector_type& _Right) {
				static_cast<_Mybase&>(*this) = _Right;
				this->_Push();
				return (*this);
			}

			_Colume_reference& operator=(const _Colume_reference& _Right) {
				return ( (*this) = static_cast<const vector_type&>(_Right) );
			}

			_Colume_reference& operator+=(const vector_type& _Right) {
				static_cast<_Mybase&>(*this) += _Right;
				this->_Push();
				return (*this);
			}

			_Colume_reference& operator-=(const vector_type& _Right) {
				static_cast<_Mybase&>(*this) -= _Right;
				this->_Push();
				return (*this);
			}

			_Colume_reference& operator*=(const vector_type& _Right) {
				static_cast<_Mybase&>(*this) *= _Right;
				this->_Push();
				return (*this);
			}

			_Colume_reference& operator/=(const vector_type& _Right) {
				static_cast<_Mybase&>(*this) /= _Right;
				this->_Push();
				return (*this);
			}

			_Colume_reference& operator+=(const scalar_type& _Scalar) {
				static_cast<_Mybase&>(*this) += _Scalar;
				this->_Push();
				return (*this);
			}

			_Colume_reference& operator-=(const scalar_type& _Scalar) {
				static_cast<_Mybase&>(*this) -= _Scalar;
				this->_Push();
				return (*this);
			}

			_Colume_reference& operator*=(const scalar_type& _Scalar) {
				static_cast<_Mybase&>(*this) *= _Scalar;
				this->_Push();
				return (*this);
			}

			_Colume_reference& operator/=(const scalar_type& _Scalar) {
				static_cast<_Mybase&>(*this) /= _Scalar;
				this->_Push();
				return (*this);
			}

			scalar_type& operator[](size_t _Pos) {
				return _Myref.at(_Pos, _Colume);
			}

		private:
			void _Pull() {
				for (size_t i = 0; i != _Myref.rows(); ++i) {
					static_cast<_Mybase&>(*this)[i] = _Myref.at(i, _Colume);
				}
			}

			void _Push() {
				for (size_t i = 0; i != _Myref.rows(); ++i) {
					_Myref.at(i, _Colume) = static_cast<_Mybase&>(*this)[i];
				}
			}

			_Common_matrix& _Myref;
			size_t _Colume;
		};
		
	public:
		constexpr static size_t _Diags = constexpr_minval(_Rows, _Cols);

		using scalar_type          = _Ty;
		using vector_type          = vector<_Ty, _Cols, _Block>;
		using scalar_pointer       = scalar_type*;
		using scalar_const_pointer = scalar_type const*;
		using row_type             = vector_type;
		using col_type             = _Colume_reference;
		using col_const_type       = vector<_Ty, _Rows, _Block>;
		using diag_type            = vector<_Ty, _Diags, _Block>;

		using scalar_iterator       = std::_Array_iterator<_Ty, _Rows * _Cols>;
		using scalar_const_iterator = std::_Array_const_iterator<_Ty, _Rows * _Cols>;
		using row_iterator          = std::_Array_iterator<vector_type, _Rows>;
		using row_const_iterator    = std::_Array_const_iterator<vector_type, _Rows>;

		using _This_type      = _Common_matrix<_Ty, _Rows, _Cols, _Block>;
		using _Transpose_type = _Common_matrix<_Ty, _Cols, _Rows, _Block>;
		using _Minor_type     = _Common_matrix<_Ty, _Rows-1, _Cols-1, _Block>;

		_Common_matrix() = default;

		static constexpr size_t rows() {// Number of matrix rows
			return _Rows; }
		static constexpr size_t cols() {// Number of matrix columes
			return _Cols; }
		static constexpr size_t diags() {// Number of diagonal elements
			return _Diags; }
		static constexpr size_t size() {// Number of scalar
			return (sizeof(_Common_matrix) / sizeof(_Ty)); }
		static constexpr size_t vector_tag() {// vector tag
			return _Block(); }

		vector_type& operator[](size_t _Pos) {
			return _Mydata[_Pos]; }
		const vector_type& operator[](size_t _Pos) const {
			return _Mydata[_Pos]; }

		scalar_pointer ptr(size_t _Pos = 0) {
			return _Mydata[0].ptr(_Pos); }
		scalar_const_pointer ptr(size_t _Pos = 0) const {
			return _Mydata[0].ptr(_Pos); }
		scalar_type& at(size_t _Row, size_t _Col) {
			return _Mydata[_Row][_Col]; }
		const scalar_type& at(size_t _Row, size_t _Col) const {
			return _Mydata[_Row][_Col]; }
		scalar_type& at(point2_size_t _Pos) {
			return _Mydata[_Pos[0]][_Pos[1]]; }
		const scalar_type& at(point2_size_t _Pos) const {
			return _Mydata[_Pos[0]][_Pos[1]]; }

		scalar_iterator begin() {
			return scalar_iterator(this->ptr(), 0); }
		scalar_iterator end() {
			return scalar_iterator(this->ptr(), this->size()); }
		scalar_const_iterator begin() const {
			return scalar_const_iterator(this->ptr(), 0); }
		scalar_const_iterator end() const {
			return scalar_const_iterator(this->ptr(), this->size()); }

		row_iterator row_begin() {
			return row_iterator(_Mydata, 0); }
		row_iterator row_end() {
			return row_iterator(_Mydata, this->rows()); }
		row_const_iterator row_begin() const {
			return row_const_iterator(_Mydata, 0); }
		row_const_iterator row_end() const {
			return row_const_iterator(_Mydata, this->rows()); }

		row_type& row(size_t i) {
			return _Mydata[i]; }
		const row_type& row(size_t i) const {
			return _Mydata[i]; }
		col_type col(size_t j) {
			return col_type(*this, j); }
		col_const_type col(size_t j) const {
			auto _First = _Slice_const_iterator<_Ty, size()>(this->ptr(), j, sizeof(row_type) / sizeof(scalar_type));
			auto _Last  = _Slice_const_iterator<_Ty, size()>(this->ptr(), size() + j, sizeof(row_type) / sizeof(scalar_type));
			return col_const_type(_First, _Last); }
		
		template<size_t _Size = _Diags> 
		vector<_Ty, _Size, _Block> diag() const {
			auto _Diag_vector = vector<_Ty, _Size, _Block>();
			for (size_t k = 0; k != _STD min(_Size, this->diags()); ++k) {
				_Diag_vector[k] = this->at(k, k);
			}
			return _Diag_vector;
		}

		scalar_type trace() const;
		size_t rank() const;

		void assign(const scalar_type& _Val);
		void assign(scalar_const_pointer _Array);
		void assign(std::initializer_list<scalar_type> _Ilist);
		void assign(std::initializer_list<vector_type> _IVlist);
		template<typename _Fn>
		void assign(const _Common_matrix& _Left, _Fn _Func);
		template<typename _Fn>
		void assign(const _Common_matrix& _Left, const _Common_matrix& _Right, _Fn _Func);
		template<typename _Fn>
		void assign(const _Common_matrix& _Left, const scalar_type& _Right, _Fn _Func);
		template<typename _Fn>
		void assign(const scalar_type& _Left, const _Common_matrix& _Right, _Fn _Func);

		_Transpose_type transpose_copy() const;
		_Minor_type minor(size_t i, size_t j) const;

		friend std::string to_string(const _Common_matrix& _Left) {
			using std::to_string;
			std::string _Str = "[";
			for (size_t i = 0; i != _Left.rows(); ++i) {
				_Str += to_string(_Left.row(i)) + "\n";
			}
			_Str.back() = ']';
			return _Str;
		}

		friend std::ostream& operator<<(std::ostream& _Ostr, const _Common_matrix& _Left) {
			return (_Ostr << to_string(_Left));
		}

	protected:
		vector_type _Mydata[_Rows];
	};

	template<typename _Ty, size_t _Rows, size_t _Cols, typename _Block> inline
	typename _Common_matrix<_Ty, _Rows, _Cols, _Block>::scalar_type 
		_Common_matrix<_Ty, _Rows, _Cols, _Block>::trace() const {// sum of diagonal-vector
		scalar_type _Trace = this->at(0, 0);
		for (size_t k = 1; k != this->diags(); ++k) {
			_Trace += this->at(k, k);
		}
		return _Trace;
	}

	template<typename _Ty, size_t _Rows, size_t _Cols, typename _Block> inline
	size_t _Common_matrix<_Ty, _Rows, _Cols, _Block>::rank() const {
		const auto epsilon = static_cast<row_type>(0.000'0002);
		size_t     _Rank   = 0;
		for (size_t i = 0; i != this->rows(); ++i) {
			if( any_of(abs(this->row(i)) > epsilon) ) {
				++_Rank;
			}
		}
		return _Rank;
	}

	template<typename _Ty, size_t _Rows, size_t _Cols, typename _Block> inline
	void _Common_matrix<_Ty, _Rows, _Cols, _Block>::assign(const scalar_type& _Val) {
		std::fill(this->begin(), this->end(), static_cast<scalar_type>(0));
		for (size_t k = 0; k != this->diags(); ++k) {
			this->at(k, k) = _Val;
		}
	}

	template<typename _Ty, size_t _Rows, size_t _Cols, typename _Block> inline
	void _Common_matrix<_Ty, _Rows, _Cols, _Block>::assign(scalar_const_pointer _Array) {
		std::copy(_Array, _Array + this->size(), this->begin());
	}

	template<typename _Ty, size_t _Rows, size_t _Cols, typename _Block> inline
	void _Common_matrix<_Ty, _Rows, _Cols, _Block>::assign(_STD initializer_list<scalar_type> _Ilist) {
		assert(_Ilist.size() <= this->size());
		auto _Dest = std::copy(_Ilist.begin(), _Ilist.end(), this->begin());
		std::fill(_Dest, this->end(), static_cast<scalar_type>(0));
	}

	template<typename _Ty, size_t _Rows, size_t _Cols, typename _Block> inline
	void _Common_matrix<_Ty, _Rows, _Cols, _Block>::assign(_STD initializer_list<vector_type> _IVlist) {
		assert(_IVlist.size() <= this->rows());
		auto _Dest = std::copy(_IVlist.begin(), _IVlist.end(), this->row_begin());
		for (; _Dest != this->row_begin(); ++_Dest) {
			_Dest->assign(static_cast<scalar_type>(0));
		}
	}

	template<typename _Ty, size_t _Rows, size_t _Cols, typename _Block> template<typename _Fn> inline
	void _Common_matrix<_Ty, _Rows, _Cols, _Block>::assign(const _Common_matrix& _Left, _Fn _Func) {
		_STD transform(_Left.row_begin(), _Left.row_end(), this->row_begin(), _Func);
	}

	template<typename _Ty, size_t _Rows, size_t _Cols, typename _Block> template<typename _Fn> inline
	void _Common_matrix<_Ty, _Rows, _Cols, _Block>::assign(const _Common_matrix& _Left, const _Common_matrix& _Right, _Fn _Func) {
		_STD transform(_Left.row_begin(), _Left.row_end(), _Right.row_begin(), this->row_begin(), _Func);
	}

	template<typename _Ty, size_t _Rows, size_t _Cols, typename _Block> template<typename _Fn> inline
	void _Common_matrix<_Ty, _Rows, _Cols, _Block>::assign(const _Common_matrix& _Left, const scalar_type& _Right, _Fn _Func) {
		auto _First = _Left.row_begin();
		auto _Last  = _Left.row_end();
		auto _Dest  = this->row_begin();
		for (; _First != _Last; ++_First, ++_Dest) {
			*_Dest = _Func(*_First, _Right);
		}
	}

	template<typename _Ty, size_t _Rows, size_t _Cols, typename _Block> template<typename _Fn> inline
	void _Common_matrix<_Ty, _Rows, _Cols, _Block>::assign(const scalar_type& _Left, const _Common_matrix& _Right, _Fn _Func) {
		auto _First = _Right.row_begin();
		auto _Last  = _Right.row_end();
		auto _Dest  = this->row_begin();
		for (; _First != _Last; ++_First, ++_Dest) {
			*_Dest = _Func(_Left, *_First);
		}
	}

	template<typename _Ty, size_t _Rows, size_t _Cols, typename _Block> inline
	typename _Common_matrix<_Ty, _Rows, _Cols, _Block>::_Transpose_type 
		_Common_matrix<_Ty, _Rows, _Cols, _Block>::transpose_copy() const {
		_Transpose_type _Tr;
		for (size_t i = 0; i != this->rows(); ++i) {
			for (size_t j = 0; j != this->cols(); ++j) {
				_Tr.at(j, i) = this->at(i, j);
			}
		}
		return _Tr;
	}

	template<typename _Ty, size_t _Rows, size_t _Cols, typename _Block> inline
	typename _Common_matrix<_Ty, _Rows, _Cols, _Block>::_Minor_type 
		_Common_matrix<_Ty, _Rows, _Cols, _Block>::minor(size_t i, size_t j) const {
		_Minor_type _Mir;
		size_t _Rseek = 0, _Cseek;
		for (size_t _Rfirst = 0; _Rfirst < this->rows(); ++_Rfirst) {
			if (_Rfirst != i) {
				_Cseek = 0;
				for (size_t _Cfirst = 0; _Cfirst < this->cols(); ++_Cfirst) {
					if (_Cfirst != j) {
						_Mir.at(_Rseek, _Cseek) = this->at(_Rfirst, _Cfirst);
						++_Cseek;
					}
				}
				++_Rseek;
			}
		}
		return _Mir;
	}

	//template<typename _Ty, size_t _Rows, size_t _Cols, typename _Vtag>
	//bool _Upper_triangular_matrix(_Common_matrix<_Ty, _Rows, _Cols, _Vtag>& _Matrix) {
	//	size_t W = _Matrix.diags();

	//	/* c * !N */
	//	for (size_t k = 0; k != W - 1; ++k) {
	//		size_t _Major = _Matrix.find_row_major(k);
	//		if (_Major != std::string::npos) {
	//			if (_Major != k) {
	//				_Matrix.row_swap(k, _Major);
	//			}
	//		} else {
	//			_Major = _Matrix.find_col_major(k);
	//			if (_Major != std::string::npos) {
	//				_Matrix.col_swap(k, _Major);
	//			} else {
	//				continue;
	//			}
	//		}

	//		//if (_Matrix.at(k, k) != static_cast<_Ty>(1)) { /* 1 = x*Aij, x = 1/Aij */
	//		//	_Matrix.row_scale(k, static_cast<_Ty>(1) / _Matrix.at(k, k), k);
	//		//}

	//		for (size_t i = k + 1; i != _Matrix.rows(); ++i) {// Ai. += -Aik * Ak.
	//			_Matrix.row_eliminate(i, -_Matrix.at(i, k) / _Matrix.at(k, k), k, k);
	//		}
	//	}

	//	///* c * !N */
	//	//for (size_t k = W - 1; k != -1; --k) {
	//	//	for (size_t i = k - 1; i != -1; --i) {// Ai. += -Aik * Ak.
	//	//		_Matrix.row_eliminate(i, -(_Matrix.at(i, k) / _Matrix.at(k, k)), k, k);
	//	//	}
	//	//}

	//	if (_Matrix.rows() < _Matrix.cols()) {
	//		return false;
	//	} else {
	//		for (size_t i = 0; i != W; ++i) {
	//			if (_Matrix.at(i, i) == static_cast<_Ty>(0)) {
	//				return false;
	//			}
	//		}
	//	}
	//	return true;
	//}

	//template<typename _Ty, size_t _Rows, size_t _Cols, typename _Vtag>
	//bool _Lu(_Common_matrix<_Ty, _Rows, _Cols, _Vtag>& A, _Common_matrix<_Ty, _Rows, _Cols, _Vtag>& Ainv) {
	//	size_t W = std::min(A.rows(), A.cols());

	//	/* c * !N */
	//	for (size_t k = 0; k != W; ++k) {
	//		size_t mj = A.find_row_major(k);
	//		if (mj != std::string::npos) {
	//			if (mj != k) {
	//				A.row_swap(k, mj);
	//				Ainv.row_swap(k, mj);
	//			}
	//		} else {
	//			mj = A.find_col_major(k);
	//			if (mj != std::string::npos) {
	//				A.col_swap(k, mj);
	//				Ainv.col_swap(k, mj);
	//			} else {
	//				continue;
	//			}
	//		}

	//		if (A.at(k, k) != static_cast<_Ty>(1)) { /* 1 = x*Aij, x = 1/Aij */
	//			const auto alpha = static_cast<_Ty>(1) / A.at(k, k);
	//			A.row_scale(k, alpha, k);
	//			Ainv.row_scale(k, alpha, 0);
	//		}

	//		for (size_t i = k + 1; i != A.rows(); ++i) {
	//			auto const alpha = -A.at(i, k);
	//			A.row_eliminate(i, alpha, k, k);
	//			Ainv.row_eliminate(i, alpha, k, 0);
	//		}
	//	}

	//	/* c * !N */
	//	for (size_t k = W - 1; k != -1; --k) {
	//		for (size_t i = k - 1; i != -1; --i) {
	//			const auto alpha = -A.at(i, k);
	//			//A.row_eliminate(i, alpha, k, k);
	//			A.at(i, k) += alpha * A.at(k, k);
	//			Ainv.row_eliminate(i, alpha, k, 0/*inv matrix index*/);
	//		}
	//	}

	//	if (A.rows() < A.cols()) {
	//		return (false);
	//	} else {
	//		for (size_t i = 0; i != W; ++i) {
	//			if ( A[i][i] == static_cast<_Ty>(0) ) {
	//				return (false);
	//			}
	//		}
	//	}
	//	return true;
	//}


	/*- - - - - - - - - - - - - - - - - - - - - _Augmented_matrix - - - - - - - - - - - - - - - - - - - - - - - - -*/
	template<typename _Ty, size_t _Rows, size_t _Cols1, size_t _Cols2, typename _Vtag1, typename _Vtag2>
	struct _Augmented_matrix {
		using first_common_matrix_type  = _Common_matrix<_Ty, _Rows, _Cols1, _Vtag1>;
		using second_common_matrix_type = _Common_matrix<_Ty, _Rows, _Cols2, _Vtag2>;
		using scalar_type               = _Ty;

		constexpr static size_t _Common_cols = _Cols1 + _Cols2;
		constexpr static size_t _Cols_middle = _Cols1;

		static constexpr size_t rows() {
			return _Rows;
		}

		static constexpr size_t cols() {
			return _Common_cols;
		}

		_Augmented_matrix(first_common_matrix_type& _Arg1, second_common_matrix_type& _Arg2)
			: first(_Arg1), second(_Arg2) {}

		scalar_type& at(size_t i, size_t j) {
			return (j < _Cols_middle ?
				first.at(i, j) :
				second.at(i, j - _Cols_middle) );
		}

		scalar_type& at(point2_size_t _Pos) {
			return (_Pos[1] < _Cols_middle ?
				first.at(_Pos) :
				second.at(_Pos[0], _Pos[1] - _Cols_middle) );
		}

		const scalar_type& at(size_t i, size_t j) const {
			return (j < _Cols_middle ?
				first.at(i, j) :
				second.at(i, j - _Cols_middle));
		}

		const scalar_type& at(point2_size_t _Pos) const {
			return (_Pos[1] < _Cols_middle ?
				first.at(_Pos) :
				second.at(_Pos[0], _Pos[1] - _Cols_middle));
		}

		size_t rank() const {
			return std::max(first.rank(), second.rank());
		}

		friend std::string to_string(const _Augmented_matrix& _Matrix) {
			std::string _Str = "_Augmented_matrix" + std::to_string(_Matrix.rows()) + "x" + std::to_string(_Matrix.rows()) + "[\n";
			for (size_t i = 0; i != _Matrix.rows(); ++i) {
				_Str += to_string(_Matrix.first.row(i)) + " ";
				_Str += to_string(_Matrix.second.row(i));
				_Str += "\n";
			}
			_Str.back() = ']';
			return _Str;
		}

		friend std::ostream& operator<<(std::ostream& _Ostr, const _Augmented_matrix& _Matrix) {
			return (_Ostr << to_string(_Matrix));
		}

		first_common_matrix_type&  first;
		second_common_matrix_type& second;
	};


	/*- - - - - - - - - - - - - - - - - - - - - major_iterator - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	/*
	@Name:  major_iterator
	@Apply: find major position
	@Note:  can swap matrix-row when next row is Zero  
	*/
	template<typename _CommonMat>
	struct major_iterator { };

	template<typename _Ty, size_t _Rows, size_t _Cols, typename _Block>
	struct major_iterator< const _Common_matrix<_Ty, _Rows, _Cols, _Block> > { 
		using common_matrix_type = std::add_const_t<_Common_matrix<_Ty, _Rows, _Cols, _Block>>;
		using scalar_type        = std::add_const_t<typename common_matrix_type::scalar_type>;

		static point2_size_t _First_major(const common_matrix_type& _Matrix, const scalar_type& _Threshold) {
			auto _First = point2_size_t{ 0, 0 };
			for ( ; _First[1] != _Matrix.cols(); ++_First[1], _First[0] = 0) {
				for ( ; _First[0] != _Matrix.rows(); ++_First[0]) {
					if ( abs(_Matrix.at(_First)) > _Threshold ) {
						return _First;
					}
				}
			}
			return point2_size_t{ _Matrix.rows(), _Matrix.cols() };
		}

		static point2_size_t _Last_major(const common_matrix_type& _Matrix, const scalar_type& _Threshold) {
			auto _First = point2_size_t{ _Matrix.rows() - 1, 0 };
			for ( ; _First[0] != -1; --_First[0], _First[1] = 0) {// [rows-1, 0]
				for ( ; _First[1] != _Matrix.cols(); ++_First[1]) {// [0, cols-1]
					if ( abs(_Matrix.at(_First)) > _Threshold ) {
						return _First;
					}
				}
			}
			return point2_size_t{ std::string::npos , std::string::npos };
		}

		static point2_size_t _Next_major(const common_matrix_type& _Matrix, point2_size_t& _Pos, const scalar_type& _Threshold) {// _Next major-pos, _Pos must be valid major-pos
			auto _First = _Pos + point2_size_t{ 1, 1 };
			for (; _First[1] != _Matrix.cols(); ++_First[1], _First[0] = _Pos[0] + 1) {// [ _Pos[0]+1, cols )
				for (; _First[0] != _Matrix.rows(); ++_First[0]) {// [ _Pos[1]+1, rows )
					if ( abs(_Matrix.at(_First)) > _Threshold ) {
						return _First;
					}
				}
			}
			return point2_size_t{ _Matrix.rows(), _Matrix.cols() };
		}

		static point2_size_t _Prev_major(const common_matrix_type& _Matrix, point2_size_t& _Pos, const scalar_type& _Threshold) {// _Pos must be valid major-pos
			auto _First = _Pos - point2_size_t{ 1, 1 };
			if (_Pos[0] == _Pos[1]) {// Rank(_Matrix) == _Matrix.diags()
				return _First;
			} else {
				_First[1] = 0;
				for ( ; _First[0] != -1; --_First[0], _First[1] = 0) {// (_Pos[0], 0]
					for ( ; _First[1] != _Pos[1]; ++_First[1]) {// [0, _Pos[1])
						if ( abs(_Matrix.at(_First)) > _Threshold ) {
							return _First;
						}
					}
				}
				return point2_size_t{ std::string::npos, std::string::npos };
			}
		}

		major_iterator(common_matrix_type& _Ref) : _Matrix(_Ref), 
			_Pos(_First_major(_Matrix, static_cast<scalar_type>(0.000'0002))) {
			if (_Pos[0] != 0) {
				throw std::exception("no-swapable major_iterator");
			}
		}

		major_iterator(common_matrix_type& _Ref, point2_size_t _Mjpos) : _Matrix(_Ref), _Pos(_Mjpos) {}

		scalar_type& operator*() {
			return _Matrix.at(_Pos);
		}

		major_iterator& operator++() {
			auto _Next_pos = _Pos + point2_size_t{ 1, 1 };
			_Pos = _Next_major(_Matrix, _Pos, static_cast<scalar_type>(0.000'0002));
			if (_Pos[0] != _Next_pos[0]) {
				throw std::exception("no-swapable major_iterator");
			}
			return (*this);
		}

		major_iterator& operator--() {
			_Pos = _Prev_major(_Matrix, _Pos, static_cast<scalar_type>(0.000'0002));
			return (*this);
		}

		major_iterator operator--(int) {
			auto _Copy = *this;
			--(*this);
			return _Copy;
		}

		major_iterator operator++(int) {
			auto _Copy = *this;
			++(*this);
			return _Copy;
		}

		bool operator==(const major_iterator& _Right) const {
			return ((&_Matrix) == (&_Right._Matrix) && all_of(_Pos == _Right._Pos));
		}

		bool operator!=(const major_iterator& _Right) const {
			return !(*this == _Right);
		}

		bool operator<(const major_iterator& _Right) const {
			return ((&_Matrix) == (&_Right._Matrix) 
				&& (_Pos[0] < _Right._Pos[0] ? true : _Pos[1] < _Right._Pos[1]) );
		}

		bool operator>(const major_iterator& _Right) const {
			return (_Right < *this);
		}

		bool operator<=(const major_iterator& _Right) const {
			return !(*this > _Right);
		}
		
		bool operator>=(const major_iterator& _Right) const {
			return !(*this < _Right);
		}

		void seek_to_first() {
			this->_Pos = _First_major(_Matrix, static_cast<scalar_type>(0.000'0002));
		}

		void seek_to_last() {
			this->_Pos = _Last_major(_Matrix, static_cast<scalar_type>(0.000'0002));
		}

		operator size_t() const {
			return _Pos[0];
		}

		operator point2_size_t() const {
			return _Pos;
		}

		common_matrix_type& _Matrix;
		point2_size_t       _Pos;
	};

	template<typename _Ty, size_t _Rows, size_t _Cols, typename _Block>
	struct major_iterator< _Common_matrix<_Ty, _Rows, _Cols, _Block> > {
		using common_matrix_type = _Common_matrix<_Ty, _Rows, _Cols, _Block>;
		using scalar_type        = typename common_matrix_type::scalar_type;
		using _Myhelper          = major_iterator<const common_matrix_type>;

		major_iterator(common_matrix_type& _Ref) : _Matrix(_Ref),
			_Pos(_Myhelper::_First_major(_Matrix, static_cast<scalar_type>(0.000'0002))) { 
			if (_Pos[0] != 0) {
				std::swap(_Matrix.row(_Pos[0]), _Matrix.row(0));
				_Pos[0] = 0;
			}
		}

		major_iterator(common_matrix_type& _Ref, point2_size_t _Mjpos) : _Matrix(_Ref), _Pos(_Mjpos) {}

		scalar_type& operator*() {
			return _Matrix.at(_Pos);
		}

		major_iterator& operator++() {
			const auto _Next_pos = _Pos + point2_size_t{ 1, 1 };
			_Pos = _Myhelper::_Next_major(_Matrix, _Pos, static_cast<scalar_type>(0.000'0002));
			if (_Pos[0] != _Next_pos[0] && /*safe_check*/_Pos[0] < _Matrix.rows() && _Next_pos[0] < _Matrix.rows()) {
				std::swap(_Matrix.row(_Pos[0]), _Matrix.row(_Next_pos[0]));
				_Pos[0] = _Next_pos[0];
			}
			return (*this);
		}

		major_iterator& operator--() {
			_Pos = _Myhelper::_Prev_major(_Matrix, _Pos, static_cast<scalar_type>(0.000'0002));
			return (*this);
		}

		major_iterator operator--(int) {
			auto _Copy = *this;
			--(*this);
			return _Copy;
		}

		major_iterator operator++(int) {
			auto _Copy = *this;
			++(*this);
			return _Copy;
		}

		bool operator==(const major_iterator& _Right) const {
			return ((&_Matrix) == (&_Right._Matrix) && all_of(_Pos == _Right._Pos));
		}

		bool operator!=(const major_iterator& _Right) const {
			return !(*this == _Right);
		}

		bool operator<(const major_iterator& _Right) const {
			return ((&_Matrix) == (&_Right._Matrix)
				&& (_Pos[0] < _Right._Pos[0] ? true : _Pos[1] < _Right._Pos[1]));
		}

		bool operator>(const major_iterator& _Right) const {
			return (_Right < *this);
		}

		bool operator<=(const major_iterator& _Right) const {
			return !(*this > _Right);
		}

		bool operator>=(const major_iterator& _Right) const {
			return !(*this < _Right);
		}

		void seek_to_first() {
			this->_Pos = _Myhelper::_First_major(_Matrix, static_cast<scalar_type>(0.000'0002));
		}

		void seek_to_last() {
			this->_Pos = _Myhelper::_Last_major(_Matrix, static_cast<scalar_type>(0.000'0002));
		}

		operator size_t() const {
			return _Pos[0];
		}

		operator point2_size_t() const {
			return _Pos;
		}

		common_matrix_type& _Matrix;
		point2_size_t       _Pos;
	};

	template<typename _Ty, size_t _Rows, size_t _Cols1, size_t _Cols2, typename _Vtag1, typename _Vtag2>
	struct major_iterator< const _Augmented_matrix<_Ty, _Rows, _Cols1, _Cols2, _Vtag1, _Vtag2> > {
		using common_matrix_type = std::add_const_t<_Augmented_matrix<_Ty, _Rows, _Cols1, _Cols2, _Vtag1, _Vtag2>>;
		using scalar_type        = std::add_const_t<typename common_matrix_type::scalar_type>;

		static point2_size_t _First_major(const common_matrix_type& _Matrix, const scalar_type& _Threshold) {
			auto _First = point2_size_t{ 0, 0 };
			for ( ; _First[1] != _Matrix.cols(); ++_First[1], _First[0] = 0) {// [0, _Matrix.cols()]
				for ( ; _First[0] != _Matrix.rows(); ++_First[0]) {// [0, _Matrix.rows())
					if ( abs(_Matrix.at(_First)) > _Threshold ) {
						return _First;
					}
				}
			}
			return point2_size_t{ _Matrix.rows(), _Matrix.cols() };
		}

		static point2_size_t _Last_major(const common_matrix_type& _Matrix, const scalar_type& _Threshold) {
			auto _First = point2_size_t{ _Matrix.rows() - 1, 0 };
			for ( ; _First[0] != -1; --_First[0], _First[1] = 0) {// [rows-1, 0]
				for ( ; _First[1] != _Matrix.cols(); ++_First[1]) {// [0, cols-1]
					if ( abs(_Matrix.at(_First)) > _Threshold ) {
						return _First;
					}
				}
			}
			return point2_size_t{ std::string::npos , std::string::npos };
		}

		static point2_size_t _Next_major(const common_matrix_type& _Matrix, point2_size_t& _Pos, const scalar_type& _Threshold) {// _Next major-pos, _Pos must be valid major-pos
			auto _First = _Pos + point2_size_t{ 1, 1 };
			for ( ; _First[1] != _Matrix.cols(); ++_First[1], _First[0] = _Pos[0] + 1) {// [ _Pos[0]+1, cols )
				for ( ; _First[0] != _Matrix.rows(); ++_First[0]) {// [ _Pos[1]+1, rows )
					if (abs(_Matrix.at(_First)) > _Threshold) {
						return _First;
					}
				}
			}
			return point2_size_t{ _Matrix.rows(), _Matrix.cols() };
		}

		static point2_size_t _Prev_major(const common_matrix_type& _Matrix, point2_size_t& _Pos, const scalar_type& _Threshold) {// _Pos must be valid major-pos
			auto _First = _Pos - point2_size_t{ 1, 1 };
			if (_Pos[0] == _Pos[1]) {// Rank(_Matrix) == _Matrix.diags()
				return _First;
			} else {
				_First[1] = 0;
				for ( ; _First[0] != -1; --_First[0], _First[1] = 0) {// (_Pos[0], 0]
					for ( ; _First[1] != _Pos[1]; ++_First[1]) {// [0, _Pos[1])
						if ( abs(_Matrix.at(_First)) > _Threshold ) {
							return _First;
						}
					}
				}
				return point2_size_t{ std::string::npos, std::string::npos };
			}
		}
	
		major_iterator(common_matrix_type& _Ref) : _Matrix(_Ref),
			_Pos(_First_major(_Matrix, static_cast<scalar_type>(0.000'0002))) { 
			if (_Pos[0] != 0) {
				throw std::exception("no-swapable major_iterator");
			}
		}

		major_iterator(common_matrix_type& _Ref, point2_size_t _Mjpos) : _Matrix(_Ref), _Pos(_Mjpos) {}

		scalar_type& operator*() {
			return _Matrix.at(_Pos);
		}

		major_iterator& operator++() {
			auto _Next_pos = _Pos + point2_size_t{ 1, 1 };
			_Pos = _Next_major(_Matrix, _Pos, static_cast<scalar_type>(0.000'0002));
			if (_Pos[0] != _Next_pos[0]) {
				throw std::exception("no-swapable major_iterator");
			}
			return (*this);
		}

		major_iterator& operator--() {
			_Pos = _Prev_major(_Matrix, _Pos, static_cast<scalar_type>(0.000'0002));
			return (*this);
		}

		major_iterator operator--(int) {
			auto _Copy = *this;
			--(*this);
			return _Copy;
		}

		major_iterator operator++(int) {
			auto _Copy = *this;
			++(*this);
			return _Copy;
		}

		bool operator==(const major_iterator& _Right) const {
			return ((&_Matrix) == (&_Right._Matrix) && all_of(_Pos == _Right._Pos));
		}

		bool operator!=(const major_iterator& _Right) const {
			return !(*this == _Right);
		}

		bool operator<(const major_iterator& _Right) const {
			return ((&_Matrix) == (&_Right._Matrix)
				&& (_Pos[0] < _Right._Pos[0] ? true : _Pos[1] < _Right._Pos[1]));
		}

		bool operator>(const major_iterator& _Right) const {
			return (_Right < *this);
		}

		bool operator<=(const major_iterator& _Right) const {
			return !(*this > _Right);
		}

		bool operator>=(const major_iterator& _Right) const {
			return !(*this < _Right);
		}

		void seek_to_first() {
			this->_Pos = _First_major(_Matrix, static_cast<scalar_type>(0.000'0002));
		}

		void seek_to_last() {
			this->_Pos = _Last_major(_Matrix, static_cast<scalar_type>(0.000'0002));
		}

		operator size_t() const {
			return _Pos[0];
		}

		operator point2_size_t() const {
			return _Pos;
		}

		common_matrix_type& _Matrix;
		point2_size_t       _Pos;
	};

	template<typename _Ty, size_t _Rows, size_t _Cols1, size_t _Cols2, typename _Vtag1, typename _Vtag2>
	struct major_iterator< _Augmented_matrix<_Ty, _Rows, _Cols1, _Cols2, _Vtag1, _Vtag2> > {
		using common_matrix_type = _Augmented_matrix<_Ty, _Rows, _Cols1, _Cols2, _Vtag1, _Vtag2>;
		using scalar_type	     = typename common_matrix_type::scalar_type;
		using _Myhelper          = major_iterator<const common_matrix_type>;

		major_iterator(common_matrix_type& _Ref) : _Matrix(_Ref),
			_Pos(_Myhelper::_First_major(_Matrix, static_cast<scalar_type>(0.000'0002))) {
			if (_Pos[0] != 0) {
				std::swap(_Matrix.first.row(_Pos[0]), _Matrix.first.row( 0 ));
				std::swap(_Matrix.second.row(_Pos[0]), _Matrix.second.row( 0 ));
				_Pos[0] = 0;
			}
		}

		major_iterator(common_matrix_type& _Ref, point2_size_t _Mjpos) : _Matrix(_Ref), _Pos(_Mjpos) {}

		scalar_type& operator*() {
			return _Matrix.at(_Pos);
		}

		major_iterator& operator++() {
			const auto _Next_pos = _Pos + point2_size_t{ 1, 1 };
			_Pos = _Myhelper::_Next_major(_Matrix, _Pos, static_cast<scalar_type>(0.000'0002));
			if (_Pos[0] != _Next_pos[0] && /*safe_check*/_Pos[0] < _Matrix.rows() && _Next_pos[0] < _Matrix.rows()) {
				std::swap(_Matrix.first.row(_Pos[0]), _Matrix.first.row(_Next_pos[0]));
				std::swap(_Matrix.second.row(_Pos[0]), _Matrix.second.row(_Next_pos[0]));
				_Pos[0] = _Next_pos[0];
			}
			return (*this);
		}

		major_iterator& operator--() {
			_Pos = _Myhelper::_Prev_major(_Matrix, _Pos, static_cast<scalar_type>(0.000'0002));
			return (*this);
		}

		major_iterator operator--(int) {
			auto _Copy = *this;
			--(*this);
			return _Copy;
		}

		major_iterator operator++(int) {
			auto _Copy = *this;
			++(*this);
			return _Copy;
		}

		bool operator==(const major_iterator& _Right) const {
			return ((&_Matrix) == (&_Right._Matrix) && all_of(_Pos == _Right._Pos));
		}

		bool operator!=(const major_iterator& _Right) const {
			return !(*this == _Right);
		}

		bool operator<(const major_iterator& _Right) const {
			return ((&_Matrix) == (&_Right._Matrix)
				&& (_Pos[0] < _Right._Pos[0] ? true : _Pos[1] < _Right._Pos[1]));
		}

		bool operator>(const major_iterator& _Right) const {
			return (_Right < *this);
		}

		bool operator<=(const major_iterator& _Right) const {
			return !(*this > _Right);
		}

		bool operator>=(const major_iterator& _Right) const {
			return !(*this < _Right);
		}

		void seek_to_first() {
			this->_Pos = _Myhelper::_First_major(_Matrix, static_cast<scalar_type>(0.000'0002));
		}

		void seek_to_last() {
			this->_Pos = _Myhelper::_Last_major(_Matrix, static_cast<scalar_type>(0.000'0002));
		}

		operator size_t() const {
			return _Pos[0];
		}

		operator point2_size_t() const {
			return _Pos;
		}

		common_matrix_type& _Matrix;
		point2_size_t       _Pos;
	};

	/*
	@Name: const_major_iterator
	@Apply: find major position
	@Note: not swap matrix-row, but throw std::exception
	*/
	template<typename _CommonMat>
		using const_major_iterator = major_iterator<std::add_const_t<_CommonMat>>;


	/*- - - - - - - - - - - - - - - - - - - matrix_row_transform - - - - - - - - - - - - - - - - - - - - - - - -*/
	/*
	@Name: matrix_row_transform
	@Apply: solve matrix
	@Function: #solve_down, #solve_up
	*/
	template<typename _TyMat, typename _TyMat2 = void>
	struct matrix_row_transform { };

	template<typename _Ty, size_t _Rows, size_t _Cols, typename _Block>
	struct matrix_row_transform< _Common_matrix<_Ty, _Rows, _Cols, _Block>, void > {
		static_assert(_Rows != 1 && _Cols != 1, "[->] matrix_row_transform<>");

		using common_matrix_type = _Common_matrix<_Ty, _Rows, _Cols, _Block>;
		using scalar_type        = typename common_matrix_type::scalar_type;
		using vector_type        = typename common_matrix_type::vector_type;
		using row_type           = typename common_matrix_type::row_type;

		using iterator       = major_iterator<common_matrix_type>;
		using const_iterator = major_iterator<const common_matrix_type>;

		static void _Scale(row_type& Arow, const scalar_type& alpha, size_t j) {// Arow *= alpha
			if (j < row_type::block_traits::size()) {
				Arow *= alpha;
			} else {
				for (; j < Arow.size(); ++j) {
					Arow[j] *= alpha;
				}
			}
		}

		static void _Eliminate(row_type& Ai, const row_type& Aj, const scalar_type& alpha, size_t j) {// Ai += Aj*alpha
			if (j < row_type::block_traits::size()) {
				Ai += Aj * alpha;
			} else {
				for (; j < Ai.size(); ++j) {
					Ai[j] += Aj[j] * alpha;
				}
			}
		}

		template<bool _Scalable>
		static void _Eliminate_down_uncheck(common_matrix_type& _Matrix, point2_size_t _Pos) {
			const auto _Threshold = static_cast<scalar_type>(0.000'0002);

			if _CONSTEXPR_IF(_Scalable) {
				auto&      _Right = _Matrix.row(_Pos[0]);
				const auto _Alpha = _Matrix.at(_Pos);
				if ( abs(_Alpha - static_cast<scalar_type>(1)) > _Threshold ) {// 1 = Aij*x, x=1/Aij
					const auto _Beta = static_cast<scalar_type>(1) / _Alpha;
					_Scale(_Right, _Beta, _Pos[1]/*offset*/);
				}

				for (size_t i = _Pos[0] + 1; i != _Matrix.rows(); ++i) {// A:j + x*Aij = 0, x = -A:j/Aij
					const auto _Beta = _Matrix.at(i, _Pos[1]);
					if (abs(_Beta) > _Threshold) {// is not a Zero
						_Eliminate(_Matrix.row(i), _Right, -_Beta, _Pos[1]/*offset*/);
					}
				}
			} else {
				const auto& _Right = _Matrix.row(_Pos[0]);
				const auto  _Alpha = _Matrix.at(_Pos);
				for (size_t i = _Pos[0] + 1; i != _Matrix.rows(); ++i) {// A:j + x*Aij = 0, x = -A:j/Aij
					const auto _Beta = _Matrix.at(i, _Pos[1]);
					if (abs(_Beta) > _Threshold) {// beta is not Zero
						_Eliminate(_Matrix.row(i), _Right, -_Beta / _Alpha, _Pos[1]/*offset*/);
					}
				}
			}
		}

		template<bool _Scalable = false, typename _Iter = iterator>
		static bool solve_down(common_matrix_type& _Matrix, _Iter& _Where) {// [0, rows-1)
			if (static_cast<size_t>(_Where) + 1 < _Matrix.rows()) {
				_Eliminate_down_uncheck<_Scalable>(_Matrix, static_cast<point2_size_t>(_Where));
				++_Where;
				return true;
			} else {// invalide major pos
				return false;
			}
		}

		template<bool _Scalable>
		static void _Eliminate_up_uncheck(common_matrix_type& _Matrix, point2_size_t _Pos) {
			const auto _Threshold = static_cast<scalar_type>(0.000'0002);
			
			if _CONSTEXPR_IF(_Scalable) {
				auto&      _Right = _Matrix.row(_Pos[0]);
				const auto _Alpha = _Matrix.at(_Pos);
				if ( abs(_Alpha - static_cast<scalar_type>(1)) > _Threshold ) {// 1 = Aij*x, x=1/Aij
					const auto _Beta = static_cast<scalar_type>(1) / _Alpha;
					_Scale(_Right, _Beta, _Pos[1]/*offset*/);
				}

				for (size_t i = _Pos[0] - 1; i != -1; --i) {// A:j + x*Aij = 0, x = -A:j/Aij
					const auto _Beta = _Matrix.at(i, _Pos[1]);
					if ( abs(_Beta) > _Threshold ) {// beta is not Zero
						_Eliminate(_Matrix.row(i), _Right, -_Beta, _Pos[1]/*offset*/);
					}
				}
			} else {
				const auto& _Right = _Matrix.row(_Pos[0]);
				const auto  _Alpha = _Matrix.at(_Pos);
				for (size_t i = _Pos[0] - 1; i != -1; --i) {// A:j + x*Aij = 0, x = -A:j/Aij
					const auto _Beta = _Matrix.at(i, _Pos[1]);
					if ( abs(_Beta) > _Threshold ) {// beta is not Zero
						_Eliminate(_Matrix.row(i), _Right, -_Beta / _Alpha, _Pos[1]/*offset*/);
					}
				}
			}
		}

		template<bool _Scalable = false, typename _Iter = iterator>
		static bool solve_up(common_matrix_type& _Matrix, _Iter& _Where) {// [rows-1, 0)
			if (/*static_cast<size_t>(_Where) - 1 != -1 && */static_cast<size_t>(_Where) < _Matrix.rows()) {
				_Eliminate_up_uncheck<_Scalable>(_Matrix, static_cast<point2_size_t>(_Where));
				--_Where;
				return true;
			} else {
				return false;
			}
		}
	
		template<bool _Scalable = false>
		static point2_size_t solve_to_upper_triangular(common_matrix_type& _Matrix) {
			auto _Major_pos = iterator(_Matrix);
			while (solve_down<_Scalable>(_Matrix, _Major_pos)) {
				// do nothing
			}

			if _CONSTEXPR_IF(_Scalable) {
				_Eliminate_down_uncheck<true>(_Matrix, static_cast<point2_size_t>(_Major_pos));
			}

			return static_cast<point2_size_t>(_Major_pos);
		}

		static size_t solve_until(common_matrix_type& _Matrix) {
			auto _Major_pos = iterator(_Matrix);
			while (solve_down<true>(_Matrix, _Major_pos)) {
				// do nothing
			}
			if (static_cast<size_t>(_Major_pos) == _Matrix.rows()) {
				_Major_pos.seek_to_last();
			}
			while (solve_up<true>(_Matrix, _Major_pos)) {
				// do nothing
			}
			return _Matrix.rank();
		}
	};

	template<typename _Ty, size_t _Rows, size_t _Cols1, typename _Vtag1, size_t _Cols2, typename _Vtag2>
	struct matrix_row_transform< _Augmented_matrix<_Ty, _Rows, _Cols1, _Cols2, _Vtag1, _Vtag2>, void > {
		using common_matrix_type        = _Augmented_matrix<_Ty, _Rows, _Cols1, _Cols2, _Vtag1, _Vtag2>;
		using first_common_matrix_type  = _Common_matrix<_Ty, _Rows, _Cols1, _Vtag1>;
		using second_common_matrix_type = _Common_matrix<_Ty, _Rows, _Cols2, _Vtag2>;
		using scalar_type               = _Ty;

		using iterator       = major_iterator<common_matrix_type>;
		using const_iterator = major_iterator<const common_matrix_type>;

		using first_row_transform  = matrix_row_transform< first_common_matrix_type >;
		using second_row_transform = matrix_row_transform< second_common_matrix_type >;

		template<bool _Scalable>
		static void _Eliminate_down_uncheck(common_matrix_type& _Matrix, point2_size_t _Pos) {
			const auto _Threshold          = static_cast<scalar_type>(0.000'0002);
			const auto _Cols_middle        = common_matrix_type::_Cols_middle;
			const bool _Is_eliminate_first = _Pos[1] < _Cols_middle;
			const auto _Second_cols_offset = _Is_eliminate_first ? 0 : _Pos[1] - _Cols_middle;

			if _CONSTEXPR_IF(_Scalable) {
				auto&      _Right1 = _Matrix.first.row(_Pos[0]);
				auto&      _Right2 = _Matrix.second.row(_Pos[0]);
				const auto _Alpha  = _Matrix.at(_Pos);
				if (abs(_Alpha - static_cast<scalar_type>(1)) > _Threshold) {// is not a One
					const auto _Beta = static_cast<scalar_type>(1) / _Alpha;
					if (_Is_eliminate_first) {
						first_row_transform::_Scale(_Right1, _Beta, _Pos[1]/*offset*/);
					}
					second_row_transform::_Scale(_Right2, _Beta, _Second_cols_offset);
				}

				for (size_t i = _Pos[0] + 1; i != _Matrix.rows(); ++i) {// A:j + x*Aij = 0, x = -A:j/Aij
					const auto _Beta = _Matrix.at(i, _Pos[1]);
					if (abs(_Beta) > _Threshold) {// is not Zero
						if (_Is_eliminate_first) {
							first_row_transform::_Eliminate(_Matrix.first.row(i), _Right1, -_Beta, _Pos[1]/*offset*/);
						}
						second_row_transform::_Eliminate(_Matrix.second.row(i), _Right2, -_Beta, _Second_cols_offset);
					}
				}
			} else {
				const auto& _Right1 = _Matrix.first.row(_Pos[0]);
				const auto& _Right2 = _Matrix.second.row(_Pos[0]);
				const auto  _Alpha  = _Matrix.at(_Pos);
				for (size_t i = _Pos[0] + 1; i != _Matrix.rows(); ++i) {// A:j + x*Aij = 0, x = -A:j/Aij
					auto _Beta = _Matrix.at(i, _Pos[1]);
					if (abs(_Beta) > _Threshold) {// is not a Zero
						_Beta  = -_Beta / _Alpha;
						if (_Is_eliminate_first) {
							first_row_transform::_Eliminate(_Matrix.first.row(i), _Right1, _Beta, _Pos[1]/*offset*/);
						}
						second_row_transform::_Eliminate(_Matrix.second.row(i), _Right2, _Beta, _Second_cols_offset);
					}
				}
			}
		}

		template<bool _Scalable = false, typename _Iter = iterator>
		static bool solve_down(common_matrix_type& _Matrix, _Iter& _Where) {// [0, rows-1)
			if (static_cast<size_t>(_Where) + 1 < _Matrix.rows()) {
				_Eliminate_down_uncheck<_Scalable>(_Matrix, static_cast<point2_size_t>(_Where));
				++_Where;
				return true;
			} else {// { _Matrix.rows(), _Matrix.cols() }
				return false;
			}
		}

		template<bool _Scalable>
		static void _Eliminate_up_uncheck(common_matrix_type& _Matrix, point2_size_t _Pos) {
			const auto _Threshold          = static_cast<scalar_type>(0.000'0002);
			const auto _Cols_middle        = common_matrix_type::_Cols_middle;
			const bool _Is_eliminate_first = _Pos[1] < _Cols_middle;
			const auto _Second_cols_offset = _Is_eliminate_first ? 0 : _Pos[1] - _Cols_middle;

			if _CONSTEXPR_IF(_Scalable) {
				auto&      _Right1 = _Matrix.first.row(_Pos[0]);
				auto&      _Right2 = _Matrix.second.row(_Pos[0]);
				const auto _Alpha  = _Matrix.at(_Pos);
				if (abs(_Alpha - static_cast<scalar_type>(1)) > _Threshold) {// is not a One
					const auto _Beta = static_cast<scalar_type>(1) / _Alpha;
					if (_Is_eliminate_first) {
						first_row_transform::_Scale(_Right1, _Beta, _Pos[1]/*offset*/);
					}
					second_row_transform::_Scale(_Right2, _Beta, _Second_cols_offset);
				}

				for (size_t i = _Pos[0] - 1; i != -1; --i) {
					const auto _Beta = _Matrix.at(i, _Pos[1]);
					if (abs(_Beta) > _Threshold) {// is not Zero
						if (_Is_eliminate_first) {
							first_row_transform::_Eliminate(_Matrix.first.row(i), _Right1, -_Beta, _Pos[1]/*offset*/);
						}
						second_row_transform::_Eliminate(_Matrix.second.row(i), _Right2, -_Beta, _Second_cols_offset);
					}
				}
			} else {
				const auto& _Right1 = _Matrix.first.row(_Pos[0]);
				const auto& _Right2 = _Matrix.second.row(_Pos[0]);
				const auto  _Alpha  = _Matrix.at(_Pos);
				for (size_t i = _Pos[0] - 1; i != -1; --i) {// A:j + x*Aij = 0, x = -A:j/Aij
					auto _Beta = _Matrix.at(i, _Pos[1]);
					if (abs(_Beta) > _Threshold) {// is not a Zero
						_Beta = -_Beta / _Alpha;
						if (_Is_eliminate_first) {
							first_row_transform::_Eliminate(_Matrix.first.row(i), _Right1, _Beta, _Pos[1]/*offset*/);
						}
						second_row_transform::_Eliminate(_Matrix.second.row(i), _Right2, _Beta, _Second_cols_offset);
					}
				}
			}
		}

		template<bool _Scalable = false, typename _Iter = iterator>
		static bool solve_up(common_matrix_type& _Matrix, _Iter& _Where) {// [0, rows-1)
			if (static_cast<size_t>(_Where) - 1 != -1) {
				_Eliminate_up_uncheck<_Scalable>(_Matrix, static_cast<point2_size_t>(_Where));
				--_Where;
				return true;
			} else {// invalide major pos
				return false;
			}
		}
	
		template<bool _Scalable = false>
		static point2_size_t solve_to_upper_triangular(common_matrix_type& _Matrix) {
			auto _Major_pos = iterator(_Matrix);
			while (solve_down<_Scalable>(_Matrix, _Major_pos)) {
				// do nothing
			}

			if _CONSTEXPR_IF(_Scalable) {
				_Eliminate_down_uncheck<true>(_Matrix, static_cast<point2_size_t>(_Major_pos));
			}

			return static_cast<point2_size_t>(_Major_pos);
		}
	
		static size_t solve_until(common_matrix_type& _Matrix) {
			auto _Major_pos = iterator(_Matrix);
			while (solve_down<true>(_Matrix, _Major_pos)) {
				// do nothing
			}
			if (static_cast<size_t>(_Major_pos) == _Matrix.rows()) {
				_Major_pos.seek_to_last();
			}
			while (solve_up<true>(_Matrix, _Major_pos)) {
				// do nothing
			}
			return _Matrix.rank();
		}
	
		template<bool _Scalable = false>
		static point2_size_t solve_to_upper_triangular(first_common_matrix_type& _Matrix1, second_common_matrix_type& _Matrix2) {
			auto _Matrix = common_matrix_type(_Matrix1, _Matrix2);
			return solve_to_upper_triangular<_Scalable>(_Matrix);
		}

		static size_t solve_until(first_common_matrix_type& _Matrix1, second_common_matrix_type& _Matrix2) {
			auto _Matrix = common_matrix_type(_Matrix1, _Matrix2);
			return solve_until(_Matrix);
		}
	};

	
	template<typename _Ty, size_t _Rows, size_t _Cols1, typename _Vtag1, size_t _Cols2, typename _Vtag2>
		using augumented_matrix_row_transform = matrix_row_transform<_Augmented_matrix<_Ty, _Rows, _Cols1, _Cols2, _Vtag1, _Vtag2>>;

	
	/*- - - - - - - - - - - - - - - - - - - _Matrix_inverse - - - - - - - - - - - - - - - - - - - - - -*/
	/*
	①: inv(A*B) = inv(B) * inv(A)
	②: inv(T(A)) = T(inv(A))
	*/
	template<typename _CommonMat>
	struct _Matrix_inverse { };

	template<typename _Ty, size_t _Rows, typename _Block>
	struct _Matrix_inverse< _Common_matrix<_Ty, _Rows, _Rows, _Block> > {
		using common_matrix_type    = _Common_matrix<_Ty, _Rows, _Rows, _Block>;
		using augmented_matrix_type = _Augmented_matrix<_Ty, _Rows, _Rows, _Rows, _Block, _Block>;
		using row_transform_type    = matrix_row_transform<augmented_matrix_type>;

		static bool process(common_matrix_type& A, common_matrix_type& Ainv, bool _Until = false) {
			Ainv.assign(static_cast<_Ty>(1));// identity-matrix
			auto _Matrix   = augmented_matrix_type(A, Ainv);
			auto _Iterator = const_major_iterator<augmented_matrix_type>(_Matrix);
			
			try {
				auto _Pos = row_transform_type::solve_to_upper_triangular<true>(_Matrix);

				if (_Pos[0] == _Pos[1] && _Pos[0] == _Matrix.rows() - 1) {// A is identity-matrix
					if (_Until) {
						while (row_transform_type::solve_up<true>(_Matrix, _Iterator)) { 
							// do nothing
						}
					} else {
						for (size_t k = _Rows - 1; k != -1; --k) {
							//Ainv.row(k) /= A.at(k, k);// scalar to 1
							for (size_t i = k - 1; i != -1; --i) {
								Ainv.row(i) += -A.at(i, k) * Ainv.row(k);
							}
						}
					}
					return true;
				}
				return false;
			} catch (const std::exception&) {
				return false;
			}
		}
	};


	/*- - - - - - - - - - - - - - - - - - - LU - - - - - - - - - - - - - - - - - - - - - -*/
	/* A = L * U

	Ax = b
	L(Ux) = b
	*/
	template<typename _Ty, size_t _Rows, typename _Block>
	struct LU {
		using common_matrix_type    = _Common_matrix<_Ty, _Rows, _Rows, _Block>;
		using augmented_matrix_type = _Augmented_matrix<_Ty, _Rows, _Rows, _Rows, _Block, _Block>;
		using row_transform_type    = matrix_row_transform<augmented_matrix_type>;

		static bool decompose(const common_matrix_type& A, common_matrix_type& L, common_matrix_type& U) {
			if (&U != &A) {
				U = A;
			}

			for (size_t k = 0; k != _Rows - 1; ++k) {
				if (abs(U.at(k, k)) < static_cast<_Ty>(0.000'0002)) {
					return false;
				}
				for (size_t i = k + 1; i != _Rows; ++i) {
					L.at(i, k) = U.at(i, k) / U.at(k, k);
					U.row(i) += -L.at(i, k) * U.row(k);
				}
			}
			return true;
		}

		template<size_t _Cols>
		static _Common_matrix<_Ty, _Rows, _Cols, _Block> process(const common_matrix_type& L, const common_matrix_type& U,
			const _Common_matrix<_Ty, _Rows, _Cols, _Block>& b) {
			/*
			   [1 0 0][x]   [a b c][x]
			Lb:[a 1 0][y] U:[0 d e][y]
			   [b c 1][z]   [0 0 f][z]
			
			1.
			[1 0 0][x]
			[a 1 0][y]
			[b c 1][z]

			2.
			[1 0 0][x]
			[0 1 0][y-ax]
			[0 c 1][z-bx]

			3.
			[1 0 0][x]
			[0 1 0][y-ax]
			[0 0 1][z-bx-cy]

			21.
			[a b c][x]
			[0 d e][y]
			[0 0 f][z]
			
			22.
			[a b c][x]
			[0 d e][y]
			[0 0 1][z/f]

			23.
			[a b 0][x+(-c)(z/f)]
			[0 d 0][y+(-e)(z/f)]
			[0 0 1][z/f]
			*/
			auto x = b;
			
			// solve [L,b] to [E,y]
			for (size_t k = 0; k != _Rows - 1; ++k) {
				for (size_t i = k + 1; i != _Rows; ++i) {
					x.row(i) += -L.at(i, k) * x.row(k);
				}
			}

			// solve [U,y] to [E,x]
			for (size_t k = _Rows - 1; k != -1; --k) {
				x.row(k) /= U.at(k, k);// scalar to 1
				for (size_t i = k - 1; i != -1; --i) {
					x.row(i) += -U.at(i, k) * x.row(k);
				}
			}

			return x;
		}
	};


	/*- - - - - - - - - - - - - - - - - - - matrix - - - - - - - - - - - - - - - - - - - - -*/
	/*
	@_matrix_<M,N,T>:
	@_M: Number of row
	@_N: Number of colume
	@_T: Scalar type
	@_J: Matrix major order, Effect mul of matrix and vector
				  |       MATRIX             |          VECTOR
		----------+--------------------------+---------------------------
		row_major | [ m00 m01 m02 ... m0N ]  |  [ x0, x1, x2, ... xN ]
				  |                          |
				  | [ m10 m11 m12 ... m1N ]  |
				  |                          |
				  | [...                     |
				  |                          |
				  | [ mM0 mM1 mM2 ... mMN ]  |
		----------+--------------------------+--------------------------
		col_major |  [m00] [m10]  ... [mN0]  |        [x0]
				  |  [m01] [m11]      [mN1]  |        [x1]
				  |  [m02] [m12]      [mN2]  |        [x2]
				  |  [...                    |        ...
				  |  [m0M] [m1M]      [mNM]  |        [xM]
	@_V: Vector type { "0":"Normal", "1":"SIMD" ... }
	@_Note: [rows() * cols()] possible not equal size(), when V != 0
	*/
	template<typename _Ty, size_t _Rows, size_t _Cols, bool _Major = true, 
		typename _Block = _Ty, typename _Mtag = normal_matrix_tag>
	class matrix : public _Common_matrix<_Ty, _Rows, _Cols, _Block> {
		using _Mybase = _Common_matrix<_Ty, _Rows, _Cols, _Block>;

	public:
		using common_matrix_type   = _Mybase;
		using scalar_type          = typename common_matrix_type::scalar_type;
		using vector_type          = typename common_matrix_type::vector_type;
		using scalar_pointer       = typename common_matrix_type::scalar_pointer;
		using scalar_const_pointer = typename common_matrix_type::scalar_const_pointer;
		using row_type             = typename common_matrix_type::row_type;
		using col_type             = typename common_matrix_type::col_type;
		using col_const_type       = typename common_matrix_type::col_const_type;
		using diag_type            = typename common_matrix_type::diag_type;

		using matrix_type		    = matrix<_Ty, _Rows, _Cols, _Major, _Block, _Mtag>;
		using minor_matrix_type     = matrix<_Ty, _Rows-1, _Cols-1, _Major, _Block, _Mtag>;
		using transpose_matrix_type = matrix<_Ty, _Cols, _Rows, _Major, _Block, _Mtag>;
		using invmajor_matrix_type  = matrix<_Ty, _Cols, _Rows, !_Major, _Block, _Mtag>;

		using transposed_reference       = transposed_reference< matrix_type >;
		using transposed_const_reference = transposed_const_reference< matrix_type >;

		using row_transform = matrix_row_transform<common_matrix_type>;

		/*
		_Mybase:
		static constexpr size_t rows() 
		static constexpr size_t cols() 
		static constexpr size_t diags()
		static constexpr size_t size() 
		static constexpr size_t vector_tag()
		*/
		static constexpr bool col_major() {// Is colume major order 
			return _Major; }
		static constexpr bool row_major() {// Is row major order
			return !col_major(); }

		matrix() = default;
		explicit matrix(const _Mybase& _Right) : _Mybase(_Right) {}
		explicit matrix(const scalar_type& _Val) {// diagonal-matrix
			_Mybase::assign(_Val); }
		explicit matrix(scalar_const_pointer _Array) { 
			_Mybase::assign(_Array); }
		matrix(std::initializer_list<scalar_type> _Ilist) { 
			_Mybase::assign(_Ilist); }
		matrix(std::initializer_list<vector_type> _IVlist) {
			_Mybase::assign(_IVlist); }
		matrix(const invmajor_matrix_type& _Right) {
			for (size_t i = 0; i != _Right.rows(); ++i) {
				for (size_t j = 0; j != _Right.cols(); ++j) {
					this->at(j, i) = _Right.at(i, j); } } }
		template<typename _Fn> matrix(const matrix& _Left, _Fn _Func) {
			_Mybase::assign(_Left, _Func); }
		template<typename _Fn> matrix(const matrix& _Left, const matrix& _Right, _Fn _Func) { 
			_Mybase::assign(_Left, _Right, _Func); }
		template<typename _Fn> matrix(const matrix& _Left, const scalar_type& _Scalar, _Fn _Func) {
			_Mybase::assign(_Left, _Scalar, _Func); }
		template<typename _Fn> matrix(const scalar_type& _Scalar, const matrix& _Right, _Fn _Func) { 
			_Mybase::assign(_Scalar, _Right, _Func); }

		transposed_reference transpose() { 
			return transposed_reference(*this); }
		transposed_const_reference transpose() const { 
			return transposed_const_reference(*this); }
		transpose_matrix_type transpose_copy() const {
			return transpose_matrix_type(_Mybase::transpose_copy()); }
		minor_matrix_type minor(size_t i, size_t j) const {
			return minor_matrix_type(_Mybase::minor(i, j)); }
		/* </memory> */

		matrix operator-() const {
			return matrix(*this, _STD negate<row_type>()); }
		matrix operator+(const matrix& _Right) const {
			return matrix(*this, _Right, _STD plus<row_type>()); }
		matrix operator-(const matrix& _Right) const {
			return matrix(*this, _Right, _STD minus<row_type>()); }
		matrix operator*(const scalar_type& _Scalar) const {
			return matrix(*this, _Scalar, _STD multiplies<>()); }
		matrix operator/(const scalar_type& _Scalar) const { 
			return matrix(*this, _Scalar, _STD divides<>()); }
		matrix& operator+=(const matrix& _Right) { 
			this->assign(*this, _Right, _STD plus<row_type>()); return (*this); }
		matrix& operator-=(const matrix& _Right) { 
			this->assign(*this, _Right, _STD minus<row_type>()); return (*this); }
		matrix& operator*=(const scalar_type& _Scalar) { 
			this->assign(*this, _Scalar, _STD multiplies<>()); return (*this); }
		matrix& operator/=(const scalar_type& _Scalar) { 
			this->assign(*this, _Scalar, _STD divides<>()); return (*this); }
		friend matrix operator*(const scalar_type& _Scalar, const matrix& _Right) {
			return matrix(_Scalar, _Right, std::multiplies<>()); }
		friend matrix operator/(const scalar_type& _Scalar, const matrix& _Right) {
			return matrix(_Scalar, _Right, std::divides<>()); }
	};

	/* square-matrix template-implement */
	template<typename _Ty, size_t _Rows, bool _Major, typename _Block>
	class matrix<_Ty, _Rows, _Rows, _Major, _Block, normal_matrix_tag> : public _Common_matrix<_Ty, _Rows, _Rows, _Block> {
		using _Mybase = _Common_matrix<_Ty, _Rows, _Rows, _Block>;
	
	public:
		using common_matrix_type   = _Mybase;
		using scalar_type          = typename common_matrix_type::scalar_type;
		using vector_type          = typename common_matrix_type::vector_type;
		using scalar_pointer       = typename common_matrix_type::scalar_pointer;
		using scalar_const_pointer = typename common_matrix_type::scalar_const_pointer;
		using row_type             = typename common_matrix_type::row_type;
		using col_type             = typename common_matrix_type::col_type;
		using col_const_type       = typename common_matrix_type::col_const_type;
		using diag_type            = typename common_matrix_type::diag_type;

		using matrix_type           = matrix<_Ty, _Rows, _Rows, _Major, _Block, normal_matrix_tag>;
		using minor_matrix_type     = matrix<_Ty, _Rows-1, _Rows-1, _Major, _Block, normal_matrix_tag>;
		using transpose_matrix_type = matrix_type;
		using invmajor_matrix_type  = matrix<_Ty, _Rows, _Rows, !_Major, _Block, normal_matrix_tag>;

		using transposed_reference       = matrix_type&;
		using transposed_const_reference = matrix_type const&;

		using row_transform = matrix_row_transform<common_matrix_type>;

		/*
		_Mybase:
		static constexpr size_t rows()
		static constexpr size_t cols()
		static constexpr size_t diags()
		static constexpr size_t size()
		static constexpr size_t vector_tag()
		*/
		static constexpr bool col_major() {// Is colume major order 
			return _Major; }
		static constexpr bool row_major() {// Is row major order
			return !col_major(); }

		matrix() = default;
		explicit matrix(const _Mybase& _Right) : _Mybase(_Right) { }
		explicit matrix(const scalar_type & _Val) {// diagonal-matrix
			_Mybase::assign(_Val); }
		explicit matrix(scalar_const_pointer _Array) {
			_Mybase::assign(_Array); }
		matrix(_STD initializer_list<scalar_type> _Ilist) {
			_Mybase::assign(_Ilist); }
		matrix(_STD initializer_list<vector_type> _IVlist) {
			_Mybase::assign(_IVlist); }
		matrix(const invmajor_matrix_type & _Right) {
			for (size_t i = 0; i != _Right.rows(); ++i) {
				for (size_t j = 0; j != _Right.cols(); ++j) {
					this->at(j, i) = _Right.at(i, j); } } }

		template<typename _Fn> matrix(const matrix & _Left, _Fn _Func) {
			_Mybase::assign(_Left, _Func); }
		template<typename _Fn> matrix(const matrix & _Left, const matrix & _Right, _Fn _Func) {
			_Mybase::assign(_Left, _Right, _Func); }
		template<typename _Fn> matrix(const matrix & _Left, const scalar_type & _Scalar, _Fn _Func) {
			_Mybase::assign(_Left, _Scalar, _Func); }
		template<typename _Fn> matrix(const scalar_type & _Scalar, const matrix & _Right, _Fn _Func) {
			_Mybase::assign(_Scalar, _Right, _Func); }

		matrix& transpose() {
			return (*this); }
		const matrix& transpose() const {
			return (*this); }
		matrix transpose_copy() const {
			return (*this); }
		minor_matrix_type minor(size_t i, size_t j) const {
			return minor_matrix_type(_Mybase::minor(i, j)); }
		
		matrix operator-() const {
			return matrix(*this, std::negate<>()); }
		matrix operator+(const matrix& _Right) const {
			return matrix(*this, _Right, std::plus<>()); }
		matrix operator-(const matrix& _Right) const {
			return matrix(*this, _Right, std::minus<>()); }
		matrix operator*(const scalar_type& _Scalar) const {
			return matrix(*this, _Scalar, std::multiplies<>()); }
		matrix operator/(const scalar_type& _Scalar) const {
			return matrix(*this, _Scalar, std::divides<>()); }
		matrix& operator+=(const matrix& _Right) {
			this->assign(*this, _Right, std::plus<>()); return (*this); }
		matrix& operator-=(const matrix& _Right) {
			this->assign(*this, _Right, std::minus<>()); return (*this); }
		matrix& operator*=(const scalar_type& _Scalar) {
			this->assign(*this, _Scalar, std::multiplies<>()); return (*this); }
		matrix& operator/=(const scalar_type& _Scalar) {
			this->assign(*this, _Scalar, std::divides<>()); return (*this); }
		matrix& operator*=(const matrix& _Right) {
			row_type _Tmp;
			for (size_t i = 0; i != this->rows(); ++i) {
				for (size_t j = 0; j != this->cols(); ++j) {
					_Tmp[j] = dot(this->row(i), _Right.col(j));
				}
				this->row(i) = _Tmp;
			}
			return (*this);
		}

		friend matrix operator*(const scalar_type& _Scalar, const matrix& _Right) {
			return matrix(_Scalar, _Right, std::multiplies<>()); }
		friend matrix operator/(const scalar_type& _Scalar, const matrix& _Right) {
			return matrix(_Scalar, _Right, std::divides<>()); }

		bool inv() {
			matrix _Copied = *this;
			return _Matrix_inverse<common_matrix_type>::process(_Copied, *this);
		}
	
		scalar_type det() const {
			auto _Copied     = *this;
			auto _Last_major = row_transform::solve_to_upper_triangular(_Copied);
			if (_Last_major[0] == this->rows() - 1 && _Last_major[1] == this->cols() - 1) {
				return _Copied.diag().product();
			} else {
				return static_cast<scalar_type>(0);
			}
		}
	};

	/* diagonal-matrix template-implement */
	template<typename _Ty, size_t _Rows, size_t _Cols, bool _Major, typename _Block>
	class matrix<_Ty, _Rows, _Cols, _Major, _Block, diagonal_matrix_tag> : public matrix<_Ty, _Rows, _Cols, _Major, _Block, normal_matrix_tag> {
		using _Mybase = matrix<_Ty, _Rows, _Cols, _Major, _Block, normal_matrix_tag>;

	public:
		using common_matrix_type   = _Mybase;
		using scalar_type          = typename common_matrix_type::scalar_type;
		using vector_type          = typename common_matrix_type::vector_type;
		using scalar_pointer       = typename common_matrix_type::scalar_pointer;
		using scalar_const_pointer = typename common_matrix_type::scalar_const_pointer;
		using row_type             = typename common_matrix_type::row_type;
		using col_type             = typename common_matrix_type::col_type;
		using col_const_type       = typename common_matrix_type::col_const_type;
		using diag_type            = typename common_matrix_type::diag_type;

		using matrix_type           = matrix<_Ty, _Rows, _Cols, _Major, _Block, diagonal_matrix_tag>;
		using minor_matrix_type     = matrix<_Ty, _Rows-1, _Cols-1, _Major, _Block, diagonal_matrix_tag>;
		using transpose_matrix_type = matrix<_Ty, _Cols, _Rows, _Major, _Block, diagonal_matrix_tag>;
		using invmajor_matrix_type  = matrix<_Ty, _Cols, _Rows, !_Major, _Block, diagonal_matrix_tag>;

		using transposed_reference       = transposed_reference< matrix_type >;
		using transposed_const_reference = transposed_const_reference< matrix_type >;

		using row_transform = matrix_row_transform<common_matrix_type>;

		/*
		_Mybase:
		static constexpr size_t rows()
		static constexpr size_t cols()
		static constexpr size_t diags()
		static constexpr size_t size()
		static constexpr size_t vector_tag()
		*/
		static constexpr bool col_major() {// Is colume major order 
			return _Major; }
		static constexpr bool row_major() {// Is row major order
			return !col_major(); }

		matrix() = default;
		explicit matrix(const scalar_type& _Val) {
			_Mybase::assign(_Val); }
		explicit matrix(const diag_type& _Diag_vector) {
			for (size_t k = 0; k != this->diags(); ++k) _Mybase::at(k, k) = _Diag_vector[k]; }
		explicit matrix(const _Mybase& _Right) : matrix(_Right.diag()) {}

		void assign(const scalar_type& _Val) {
			_Mybase::assign(_Val); }
		void assign(const diag_type& _Diag_vector) {
			for (size_t k = 0; k != this->diags(); ++k) _Mybase::at(k, k) = _Diag_vector[k]; }

		scalar_type& at(size_t i, size_t j) {
			assert(i == j); return _Mybase::at(i, j); }
		const scalar_type& at(size_t i, size_t j) const {
			return _Mybase::at(i, j); }

		transposed_reference transpose() {
			return transposed_reference(*this); }
		transposed_const_reference transpose() const {
			return transposed_const_reference(*this); }
		transpose_matrix_type transpose_copy() const {
			return transpose_matrix_type(_Mybase::transpose_copy()); }
		minor_matrix_type minor(size_t i, size_t j) const {
			return minor_matrix_type(_Mybase::minor(i, j)); }

		matrix operator-() const {
			return matrix(std::negate<>(_Mybase::diag())); }
		matrix operator+(const matrix& _Right) const {
			return matrix(_Mybase::diag() + _Right.diag()); }
		matrix operator-(const matrix& _Right) const {
			return matrix(_Mybase::diag() - _Right.diag()); }
		matrix operator*(const scalar_type& _Scalar) const {
			return matrix(_Mybase::diag() * _Scalar); }
		matrix operator/(const scalar_type& _Scalar) const {
			return matrix(_Mybase::diag() / _Scalar); }
		matrix& operator+=(const matrix& _Right) {
			this->assign(_Mybase::diag() + _Right.diag()); return (*this); }
		matrix& operator-=(const matrix& _Right) {
			this->assign(_Mybase::diag() - _Right.diag()); return (*this); }
		matrix& operator*=(const scalar_type& _Scalar) {
			this->assign(_Mybase::diag() * _Scalar); return (*this); }
		matrix& operator/=(const scalar_type& _Scalar) {
			this->assign(_Mybase::diag() / _Scalar); return (*this); }
		friend matrix operator*(const scalar_type& _Scalar, const matrix& _Right) {
			return matrix(_Scalar * _Right.diag()); }
		friend matrix operator/(const scalar_type& _Scalar, const matrix& _Right) {
			return matrix(_Scalar / _Right.diag()); }
	};

	// diagonal-square-matrix template-implement
	template<typename _Ty, size_t _Rows, bool _Major, typename _Block>
	class matrix<_Ty, _Rows, _Rows, _Major, _Block, diagonal_matrix_tag> : public matrix<_Ty, _Rows, _Rows, _Major, _Block, normal_matrix_tag> {
		using _Mybase = matrix<_Ty, _Rows, _Rows, _Major, _Block, normal_matrix_tag>;

	public:
		using common_matrix_type   = typename _Mybase::common_matrix_type;
		using scalar_type          = typename common_matrix_type::scalar_type;
		using vector_type          = typename common_matrix_type::vector_type;
		using scalar_pointer       = typename common_matrix_type::scalar_pointer;
		using scalar_const_pointer = typename common_matrix_type::scalar_const_pointer;
		using row_type             = typename common_matrix_type::row_type;
		using col_type             = typename common_matrix_type::col_type;
		using col_const_type       = typename common_matrix_type::col_const_type;
		using diag_type            = typename common_matrix_type::diag_type;

		using matrix_type           = matrix<_Ty, _Rows, _Rows, _Major, _Block, diagonal_matrix_tag>;
		using minor_matrix_type     = matrix<_Ty, _Rows-1, _Rows-1, _Major, _Block, diagonal_matrix_tag>;
		using transpose_matrix_type = matrix_type;
		using invmajor_matrix_type  = matrix<_Ty, _Rows, _Rows, !_Major, _Block, diagonal_matrix_tag>;

		using transposed_reference       = matrix_type&;
		using transposed_const_reference = matrix_type const&;

		using row_transform = matrix_row_transform<common_matrix_type>;

		matrix() = default;
		explicit matrix(const scalar_type& _Val) {
			_Mybase::assign(_Val); }
		explicit matrix(const diag_type& _Diag_vector) {
			for (size_t k = 0; k != this->diags(); ++k) _Mybase::at(k, k) = _Diag_vector[k]; }
		explicit matrix(const _Mybase& _Right) : matrix(_Right.diag()){ }

		void assign(const scalar_type& _Val) {
			_Mybase::assign(_Val); }
		void assign(const diag_type& _Diag_vector) {
			for (size_t k = 0; k != this->diags(); ++k) _Mybase::at(k, k) = _Diag_vector[k]; }

		scalar_type& at(size_t i, size_t j) {
			assert(i == j); return _Mybase::at(i, j); }
		const scalar_type& at(size_t i, size_t j) const {
			return _Mybase::at(i, j); }

		matrix& transpose() {
			return (*this); }
		const matrix& transpose() const {
			return (*this); }
		matrix transpose_copy() const {
			return (*this); }
		minor_matrix_type minor(size_t i, size_t j) const {
			return minor_matrix_type(_Mybase::minor(i, j)); }

		matrix operator-() const {
			return matrix(std::negate<>(_Mybase::diag())); }
		matrix operator+(const matrix& _Right) const {
			return matrix(_Mybase::diag() + _Right.diag()); }
		matrix operator-(const matrix& _Right) const {
			return matrix(_Mybase::diag() - _Right.diag()); }
		matrix operator*(const scalar_type& _Scalar) const {
			return matrix(_Mybase::diag() * _Scalar); }
		matrix operator/(const scalar_type& _Scalar) const {
			return matrix(_Mybase::diag() / _Scalar); }
		matrix& operator+=(const matrix& _Right) {
			this->assign(_Mybase::diag() + _Right.diag()); return (*this); }
		matrix& operator-=(const matrix& _Right) {
			this->assign(_Mybase::diag() - _Right.diag()); return (*this); }
		matrix& operator*=(const scalar_type& _Scalar) {
			this->assign(_Mybase::diag() * _Scalar); return (*this); }
		matrix& operator/=(const scalar_type& _Scalar) {
			this->assign(_Mybase::diag() / _Scalar); return (*this); }
		friend matrix operator*(const scalar_type& _Scalar, const matrix& _Right) {
			return matrix(_Scalar * _Right.diag()); }
		friend matrix operator/(const scalar_type& _Scalar, const matrix& _Right) {
			return matrix(_Scalar / _Right.diag()); }

		bool inv() {
			this->assign(static_cast<scalar_type>(1) / _Mybase::diag()); return true; }
		scalar_type det() const {// ∏this[k][k], k in {0, 1, ..., this->diags()-1}
			return this->diag().product(); }
	};

	template<size_t _Rows, size_t _Cols, bool _Major = true, typename _Block = float, typename _Mtag = normal_matrix_tag>
		using fmatrix = matrix<float, _Rows, _Cols, _Major, _Block, _Mtag>;
	template<size_t _Rows, size_t _Cols, bool _Major = true, typename _Block = double, typename _Mtag = normal_matrix_tag>
		using dmatrix = matrix<double, _Rows, _Cols, _Major, _Block, _Mtag>;
	template<size_t _Rows, size_t _Cols, bool _Major = true, typename _Block = int32_t, typename _Mtag = normal_matrix_tag>
		using imatrix = matrix<int32_t, _Rows, _Cols, _Major, _Block, _Mtag>;

	template<typename _Ty, size_t _Rows, bool _Major = true, typename _Block = _Ty, typename _Mtag = normal_matrix_tag>
		using square_matrix = matrix<_Ty, _Rows, _Rows, _Major, _Block, _Mtag>;
	template<typename _Ty, bool _Major = true, typename _Block = _Ty, typename _Mtag = normal_matrix_tag>
		using matrix2x2 = square_matrix<_Ty, 2, _Major, _Block, _Mtag>;
	template<typename _Ty, bool _Major = true, typename _Block = _Ty, typename _Mtag = normal_matrix_tag>
		using matrix3x3 = square_matrix<_Ty, 3, _Major, _Block, _Mtag>;
	template<typename _Ty, bool _Major = true, typename _Block = _Ty, typename _Mtag = normal_matrix_tag>
		using matrix4x4 = square_matrix<_Ty, 4, _Major, _Block, _Mtag>;

	template<typename _Ty, size_t _Rows, size_t _Cols, bool _Major = true, typename _Block = _Ty>
		using diagonal_matrix = matrix<_Ty, _Rows, _Cols, _Major, _Block, diagonal_matrix_tag>;


	/*- - - - - - - - - - - - - - - - - _Matrix_cast - - - - - - - - - - - - - - - - - - - -*/
	template<typename _OutMatrix, typename _InMatrix>
	struct _Matrix_cast {
		using source_matrix_type = _InMatrix;
		using dest_matrix_type   = _OutMatrix;

		static _OutMatrix process(const _InMatrix& _Src) {
			abort();
		}
	};

	template<typename _OutTy, size_t _OutRows, size_t _OutCols, bool _OutMajor, typename _OutVtag, typename _OutMtag,
		typename _InTy, size_t _InRows, size_t _InCols, bool _InMajor, typename _InVtag, typename _InMtag>
	struct _Matrix_cast< matrix<_OutTy, _OutRows, _OutCols, _OutMajor, _OutVtag, _OutMtag>,
		                 matrix<_InTy, _InRows, _InCols, _InMajor, _InVtag, _InMtag> > {
		using dest_matrix_type   = matrix<_OutTy, _OutRows, _OutCols, _OutMajor, _OutVtag, _OutMtag>;
		using source_matrix_type = matrix<_InTy, _InRows, _InCols, _InMajor, _InVtag, _InMtag>;

		static dest_matrix_type process(const source_matrix_type& _Src) {
			if _CONSTEXPR_IF(_OutMajor == _InMajor) {
				dest_matrix_type _Dest;
				constexpr size_t _Common_rows = _OutRows < _InRows ? _OutRows : _InRows;
				constexpr size_t _Common_cols = _OutCols < _InCols ? _OutCols : _InCols;
				for (size_t i = 0; i != _Common_rows; ++i) {
					for (size_t j = 0; j != _Common_cols; ++j) {
						_Dest.at(i, j) = static_cast<_OutTy>(_Src.at(i, j));
					}
				}
				return _Dest;
			} else {// transpose(_Src)
				dest_matrix_type _Dest;
				constexpr size_t _Common_rows = _OutRows < _InCols ? _OutRows : _InCols;
				constexpr size_t _Common_cols = _OutCols < _InRows ? _OutCols : _InRows;
				for (size_t i = 0; i != _Common_rows; ++i) {
					for (size_t j = 0; j != _Common_cols; ++j) {
						_Dest.at(i, j) = static_cast<_OutTy>(_Src.at(j, i));
					}
				}
				return _Dest;
			}
		}
	};

	template<typename _OutTy, size_t _OutRows, size_t _OutCols, bool _OutMajor, typename _OutVtag/*diagonal-matrix*/,
		typename _InTy, size_t _InRows, size_t _InCols, bool _InMajor, typename _InVtag, typename _InMtag>
	struct _Matrix_cast< matrix<_OutTy, _OutRows, _OutCols, _OutMajor, _OutVtag, diagonal_matrix_tag>,
		                 matrix<_InTy, _InRows, _InCols, _InMajor, _InVtag, _InMtag> > {
		using dest_matrix_type   = diagonal_matrix<_OutTy, _OutRows, _OutCols, _OutMajor, _OutVtag>;
		using source_matrix_type = matrix<_InTy, _InRows, _InCols, _InMajor, _InVtag, _InMtag>;

		static dest_matrix_type process(const source_matrix_type& _Src) {
			dest_matrix_type _Dest;
			const size_t _Common_diags = std::min(_Dest.diags(), _Src.diags());
			for (size_t k = 0; k != _Common_diags; ++k) {
				_Dest.at(k, k) = _Src.at(k, k);
			}
			return _Dest;
		}
	};

	template<typename _OutMatrix, typename _InMatrix>
	_OutMatrix matrix_cast(const _InMatrix& _Src) {
		return _Matrix_cast<_OutMatrix, _InMatrix>::process(_Src);
	}


	/*- - - - - - - - - - - - - - - - - _Matrix_multiples - - - - - - - - - - - - - - - - - - -*/
	template<typename _Ty, size_t _Mx, size_t _Nx, size_t _Px, bool _Major, 
		typename _Block, typename _Mtag1, typename _Mtag2>
	struct _Matrix_multiples {// default
		using dest_type  = matrix<_Ty, _Mx, _Px, _Major, _Block, _Mtag1>;
		using left_type  = matrix<_Ty, _Mx, _Nx, _Major, _Block, _Mtag1>;
		using right_type = matrix<_Ty, _Nx, _Px, _Major, _Block, _Mtag2>;

		static dest_type process(const left_type& _Left, const right_type& _Right) {
			dest_type _Dest;
			for (size_t i = 0; i != _Dest.rows(); ++i) {
				for (size_t j = 0; j != _Dest.cols(); ++j) {
					_Dest.at(i, j) = dot(_Left.row(i), _Right.col(j));
				}
			}
			return _Dest;
		}
	};

	// diagonal-matrix * diagonal-matrix
	template<typename _Ty, size_t _Mx, size_t _Nx, size_t _Px, bool _Major,
		typename _Block>
	struct _Matrix_multiples<_Ty, _Mx, _Nx, _Px, _Major, _Block, diagonal_matrix_tag, diagonal_matrix_tag> {
		using dest_type  = diagonal_matrix<_Ty, _Mx, _Px, _Major, _Block>;
		using left_type  = diagonal_matrix<_Ty, _Mx, _Nx, _Major, _Block>;
		using right_type = diagonal_matrix<_Ty, _Nx, _Px, _Major, _Block>;

		static dest_type process(const left_type& _Left, const right_type& _Right) {
			const auto _Left_diag  = _Left.diag<dest_type::diags()>();
			const auto _Right_diag = _Right.diag<dest_type::diags()>();
			return dest_type(_Left_diag * _Right_diag);
		}
	};

	// matrix * diagonal-matrix
	template<typename _Ty, size_t _Mx, size_t _Nx, size_t _Px, bool _Major,
		typename _Block, typename _Mtag1>
	struct _Matrix_multiples<_Ty, _Mx, _Nx, _Px, _Major, _Block, _Mtag1, diagonal_matrix_tag> {
		using dest_type  = matrix<_Ty, _Mx, _Px, _Major, _Block, _Mtag1>;
		using left_type  = matrix<_Ty, _Mx, _Nx, _Major, _Block, _Mtag1>;
		using right_type = diagonal_matrix<_Ty, _Nx, _Px, _Major, _Block>;

		static dest_type process(const left_type& _Left, const right_type& _Right) {
			dest_type _Dest;
			const auto _Diag_vector = _Right.diag<_Left.cols()>();// constexpr
			for (size_t i = 0; i != _Dest.rows(); ++i) {
				const auto _Tmp = _Dest.row(i) * _Diag_vector;
				_Dest.row(i).assign( _Tmp.begin(), _Tmp.end() );
			}
			return _Dest;
			/*
			[a b c]   [x 0 0]   [a*x b*y c*z]
			[d e f] * [  y  ] = [d*x e*y f*z]
			[g h i]   [    z]   [g*x h*y i*z]

			[a b c]   [x 0 0 0]   [a*x b*y c*z 0]
			[d e f] * [  y   0] = [d*x e*y f*z 0]
			[g h i]   [    z 0]   [g*x h*y i*z 0]

			[a b c]   [x 0]   [a*x b*y]
			[d e f] * [  y] = [d*x e*y]
			[g h i]   [   ]   [g*x h*y]
			*/
		}
	};

	// diagonal-matrix * matrix
	template<typename _Ty, size_t _Mx, size_t _Nx, size_t _Px, bool _Major,
		typename _Block, typename _Mtag2>
	struct _Matrix_multiples<_Ty, _Mx, _Nx, _Px, _Major, _Block, diagonal_matrix_tag, _Mtag2> {
		using dest_type  = matrix<_Ty, _Mx, _Px, _Major, _Block, _Mtag2>;
		using left_type  = diagonal_matrix<_Ty, _Nx, _Px, _Major, _Block>;
		using right_type = matrix<_Ty, _Mx, _Nx, _Major, _Block, _Mtag2>;

		static dest_type process(const left_type& _Left, const right_type& _Right) {
			auto _Dest = dest_type();
			for (size_t k = 0; k != _Dest.diags(); ++k) {
				_Dest.row(k) = _Left.at(k, k) * _Right.row(k);
			}

			return _Dest;
			/*
			① Aii * Bi., i∈{0,...M}
			  [ x 0 0 ]     [ a b c ]   [x*a x*b x*c]   [x * [a b c]]
			A:[ 0 y 0 ] * B:[ d e f ] = [y*d y*e y*f] = [y * [d e f]]
			  [ 0 0 z ]     [ g h i ]   [z*g z*h z*i]   [z * [g h i]]

			②Same of above
			  [x 0 0 0]     [a b c d]
			A:[0 y 0 0] * B:[e f g h]
			  [0 0 z 0]     [i j k l]
							[m n r s]

			[x*a x*b x*c x*d] = x * B0.
			[y*e y*f y*g y*h] = y * B1.
			[z*i z*j z*k z*l] = z * B2.

			③ 1.construct Zero matrix 2.compute K=min(M,N) matrix-mul
			  [x 0 0]     [a b c]   [x * [a b c]]
			A:[0 y 0] * B:[d e f] = [y * [d e f]]
			  [0 0 z]     [g h i]   [z * [g h i]]
			  [0 0 0]               [     0     ]
			*/
		}
	};

	template<typename _Ty, size_t _Mx, size_t _Nx, size_t _Px, bool _Major,
		typename _Block, typename _Mtag1, typename _Mtag2> inline
	auto operator*( const matrix<_Ty, _Mx, _Nx, _Major, _Block, _Mtag1>& _Left, 
		            const matrix<_Ty, _Nx, _Px, _Major, _Block, _Mtag2>& _Right )
		->decltype(_Matrix_multiples<_Ty, _Mx, _Nx, _Px, _Major, _Block, _Mtag1, _Mtag2>::process(_Left, _Right)) {
		return _Matrix_multiples<_Ty, _Mx, _Nx, _Px, _Major, _Block, _Mtag1, _Mtag2>::process(_Left, _Right);
	}


#define matrixMxN matrix<_Ty, _Rows, _Cols, _Major, _Block, _Mtag>
#define matrixNxM matrix<_Ty, _Cols, _Rows, _Major, _Block, _Mtag>
#define matrixSqu matrix<_Ty, _Rows, _Rows, _Major, _Block, _Mtag>

	template<typename _Ty, size_t _Rows, size_t _Cols, bool _Major, typename _Block, typename _Mtag> inline
	matrixNxM transpose(const matrixMxN& _Source) {
		using invmajor_type  = matrix<_Ty, _Rows, _Cols, !_Major, _Block, _Mtag>;
		using transpose_type = matrix<_Ty, _Cols, _Rows, _Major, _Block, _Mtag>;
		return matrix_cast<transpose_type>(*reinterpret_cast<const invmajor_type*>(&_Source));
	}

	template<typename _Ty, size_t _Rows, size_t _Cols, bool _Major, typename _Block, typename _Mtag> inline
	matrixMxN inverse(const matrixMxN& _Source) {
		auto A    = _Source;
		auto Ainv = matrixMxN(static_cast<_Ty>(1));
		//_Lu(A, Ainv);
		return Ainv;
	}

	template<typename _Ty, size_t _Rows, bool _Major, typename _Block, typename _Mtag> inline
	matrixSqu pow(const matrixSqu& _Matrix, size_t _Power) {
		if (_Power == 0) {// A⁰ = 1
			return matrixSqu(static_cast<_Ty>(1));
		} else if (_Power == 1) {
			return _Matrix;
		} else {
			auto _Matrix_pow = _Matrix;
			for (size_t i = 1; i != _Power; ++i) {
				_Matrix_pow *= _Matrix;
			}
			return _Matrix_pow;
		}
	}


	template<typename _Iter>
	size_t count_invseq(_Iter _First, _Iter _Last) {// get sum of inverse sequence 
		auto _Count = size_t(0);
		auto _Where = std::is_sorted_until(_First, _Last);

		while (_Where != _Last) {
			_Count += std::count_if(_First, _Where, [_Where](size_t _Val) { return (_Val > (*_Where)); });
			++_Where;
		}
		return (_Count);
	}

	inline void _Seque(const std::vector<size_t>& _Numbers, std::vector<size_t>& _Tmp, std::vector<std::vector<size_t>>& _Result, size_t n) {
		if (n == 1) {// push last value
			_Tmp.push_back(_Numbers.front());
			_Result.push_back(_Tmp);
			_Tmp.pop_back();
			return;
		} else {// push _Numbers[any], and not have it at _Next cascade 
			auto _Next = std::vector<size_t>(n - 1);
			for (size_t i = 0; i != n; ++i) {
				_Tmp.push_back(_Numbers[i]);
				std::copy_if(_Numbers.begin(), _Numbers.end(), _Next.begin(), [&_Numbers, i](size_t a) { return (a != _Numbers[i]); });
				_Seque(_Next, _Tmp, _Result, n - 1);
				_Tmp.pop_back();
			}
		}
	}

	inline std::vector<std::vector<size_t>> seque(const std::vector<size_t>& _Numbers) {
		auto _Sequences = std::vector<std::vector<size_t>>();
		auto _Tmp = std::vector<size_t>();
		_Seque(_Numbers, _Tmp, _Sequences, _Numbers.size());
		return (_Sequences);
	}

	struct determinant_seque : public std::vector<std::pair<std::vector<size_t>, size_t>> {
		using number_type = size_t;
		using seque_type  = std::vector<number_type>;
		using pair_type   = std::pair<seque_type, number_type>;
		using _Mybase     = std::vector<pair_type>;

		determinant_seque() { }

		determinant_seque(const std::vector<size_t>& _Numbers) : _Mybase() {
			auto _Seq = seque(_Numbers);
			for (auto _First = _Seq.begin(); _First != _Seq.end(); ++_First) {
				auto& s = *_First;
				_Mybase::push_back(pair_type(s, count_invseq(s.cbegin(), s.cend())));
			}
			_Mysource = _Numbers;
		}

		std::vector<size_t> _Mysource;
	};

	template<typename _Ty, size_t _Rows, bool _Major, typename _Block, typename _Mtag> inline
	_Ty determinant(const matrixSqu& _Matrix) {
		/* ∑(0,!M): (-1)^t * D[0][s₀] * D[1][s₁] * D[2][s₂]... * D[M-1][s(m-₁)] */
		if _CONSTEXPR_IF(_Rows == 2) {
			return (_Matrix.at(0, 0) * _Matrix.at(1, 1) - _Matrix.at(0, 1) * _Matrix.at(1, 0));
		}

		auto sigma = _Ty(0);
		auto S     = determinant_seque(set_number<determinant_seque::number_type>(0, _Matrix.rows()));
		/*for (size_t i = 0; i != S.size(); ++i) {
			auto _Test = S[i].first;
			for (size_t j = 0; j != _Test.size(); ++j) {
				std::cout << _Test[j] << " ";
			}
			std::cout << std::endl;
		}*/
		assert( S.size() == factorial(_Matrix.rows()) );
		for (size_t i = 0; i != S.size(); ++i) {
			const auto& s = S[i].first;
			const auto& t = S[i].second;
			auto      rou = _Matrix.at(0, s[0]);
			for (size_t j = 1; j != s.size(); ++j) {
				rou *= _Matrix.at(j, s[j]);// high light
			}
			sigma += static_cast<_Ty>(std::pow(-1, t)) * rou;
		}
		return sigma;
	}

#undef matrixMxN
#undef matrixNxM
#undef matrixSqu

}// namespace clmagic



//	//template<typename T> inline
//	//vector3<T> transform_coord(_in(vector3<T>) _Lhs, _in(matrix_<4, 4, T>) _matrix)
//	//	{	// _matrix * Vector[_Lhs, 1.0]
//	//	vector4<T> _Result  = _matrix.row(3);
//	//			   _Result += _matrix.row(2) * _Lhs[2];
//	//			   _Result += _matrix.row(1) * _Lhs[1];
//	//			   _Result += _matrix.row(0) * _Lhs[0];
//
//	//	if ( _Result[3] != T(1) ) 
//	//		{	/*  _Result[3]   = X
//	//				_Result[3]   = X*1
//	//				_Result[3]/X = 1
//	//			*/
//	//		_Result /= _Result[3];
//	//		}
//
//	//	return ( reference_cast<vector3<T>>(_Result) );
//	//	}
//
//	//template<typename T> inline 
//	//unit_vector<T> transform_normal(_in(unit_vector<T>) _Lhs, _in(matrix_<4, 4, T>) _matrix)
//	//	{	// mat3x3(_matrix) * Vec3, igonore translate
//	//	vector4<T> _Result  = _Lhs[0] * _matrix.row(0);
//	//			   _Result += _Lhs[1] * _matrix.row(1);
//	//			   _Result += _Lhs[2] * _matrix.row(2);
//	//	return ( reference_cast<unit_vector<T>>(_Result) );
//	//	}
//
//	//inline vector3<float> screen_to_world(_in(vector3<float>) _Vec, _in(mat4) _Viewmatrix, _in(mat4) _Projmatrix)
//	//	{
//	//	auto _Vec128 = DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&_Vec));
//	//	auto _mat128view = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&_Viewmatrix));
//	//	auto _mat128proj = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&_Projmatrix));
//	//	_Vec128.m128_f32[3] = 1.0f;
//	//	_mat128view = DirectX::XMMatrixInverse(nullptr, _mat128view);
//	//	_mat128proj = DirectX::XMMatrixInverse(nullptr, _mat128proj);
//
//	//	_Vec128 = DirectX::XMVector3TransformCoord(_Vec128, _mat128proj);
//	//	_Vec128 = DirectX::XMVector3TransformCoord(_Vec128, _mat128view);
//	//	_Vec128 = DirectX::XMVector4Normalize(_Vec128);
//
//	//	return (*reinterpret_cast<vector3<float>*>(&_Vec128));
//	//	}
//
//	///* < gen_mat > */
//	//template<typename T> inline 
//	//	matrix_<4, 4, T> scaling_matrix(_in(T) _Width, _in(T) _Height, _in(T) _Depth)
//	//	{
//	//	return ( matrix_<4, 4, T>(
//	//		_Width,    T(0),   T(0), T(0),
//	//		  T(0), _Height,   T(0), T(0),
//	//		  T(0),    T(0), _Depth, T(0),
//	//		  T(0),    T(0),   T(0), T(1)) );
//	//	}
//
//	//template<typename T> inline 
//	//	matrix_<4, 4, T> scaling_matrix(_in(vector3<T>) _sXyz) {
//	//		return ( scaling_matrix(_sXyz[0], _sXyz[1], _sXyz[2]) );
//	//	}
//
//	//template<typename T> inline
//	//matrix_<4, 4, T> rotation_matrix(_in(unit_vector<T>) _Axis, _in(T) _Radians)
//	//	{
//	//	using namespace::DirectX;
//	//	auto _A = _mm_loadu_ps(_Axis.ptr());
//	//	return (*(mat4*)(&DirectX::XMMatrixRotationAxis(_A, _Radians)));
//	//	}
//
//	//template<typename T> inline
//	//matrix_<4, 4, T> rotation_matrix(const T Ax, const T Ay, const T Az, const T rad) {
//	//	return rotation_matrix(unit_vector<T>(Ax, Ay, Az), rad);
//	//}
//
//	//template<typename T> inline
//	//	matrix_<4, 4, T> translation_matrix( _in(T) _X, _in(T) _Y, _in(T) _Z) {
//	//		return ( matrix_<4, 4, T>(
//	//			T(1), T(0), T(0), T(0),
//	//			T(0), T(1), T(0), T(0),
//	//			T(0), T(0), T(1), T(0),
//	//			  _X,   _Y,   _Z, T(1)) );
//	//	}
//
//	//template<typename T> inline
//	//	matrix_<4, 4, T> translation_matrix( _in(vector3<T>) _dXyz) {
//	//		return (translation_matrix(_dXyz[0], _dXyz[1], _dXyz[2]));
//	//	}
//
//	template<typename T> inline
//	matrix<T, 4, 4> Lookat( _in(vector3<T>) _Eye, _in(unit_vector3<T>) _Fwd, _in(unit_vector3<T>) _Up)
//		{
//		vector3<T> f = normalize( _Fwd );
//		vector3<T> r = normalize( cross3(f, _Up) );
//		vector3<T> u = normalize( cross3(r, f) );
//		vector3<T> p = proj<T>(_Eye, r, u, f);
//
//		return ( matrix<T, 4, 4>{
//			 r[0],  u[0], -f[0], T(0),
//			 r[1],  u[1], -f[1], T(0),
//			 r[2],  u[2], -f[2], T(0),
//			-p[0], -p[1],  p[2], T(1)} );
//		/*
//		①:    [ r.x  r.y  r.z ]        [ Right   ]
//			T( [ u.x  u.y  u.z ] ) ~ T( [ Up      ]
//			   [-f.x -f.y -f.z ]        [-Forward ]
//		②: proj(_Eye,  projection to [Right, Up, Forward]'s matrix
//
//		right hand coordinate
//		[  r.x   u.x    -f.x   0 ]
//		[  r.y   u.y    -f.y   0 ]
//		[  r.z   u.z    -f.z   0 ]
//		[-e·r  -e·u   e·f   1 ]
//		*/
//		}
//
//	//template<typename T> inline
//	//	matrix_<4, 4, T> Lookat2(_in(vector3<T>) _Eye, _in(vector3<T>) _Target, _in(unit_vector<T>) _Up) {
//	//		return ( Lookat(_Eye, _Target - _Eye, _Up) );
//	//	}
//
//	//template<typename T> inline
//	//	matrix_<4, 4, T> Ortho( _in(T) l, _in(T) r, _in(T) b, _in(T) t, _in(T) n, _in(T) f) {
//	//		return (matrix_<4, 4, T>(
//	//			T(2)/(r-l),	T(0),	    T(0),       T(0),
//	//			T(1),		T(2)/(t-b), T(0),       T(0),
//	//			T(1),       T(0),		T(2)/(f-n), T(0),
//	//			-(r+l)/(r-l), -(t+b)/(t-b), -(f+n)/(f-n), T(1)) );
//	//		/*
//	//		[    2/(r-l),            0,            0, 0]
//	//		[		   0,      2/(t-b),            0, 0]
//	//		[          0,            0,      2/(f-n), 0]
//	//		[-(r+l)(r-l), -(t+b)/(t-b), -(f+n)/(f-n), 1]
//	//		*/
//	//	}
//
//	template<typename T> inline
//	matrix<T, 4, 4> Perspective( _in(T) _Fov, _in(T) _Aspect, _in(T) _Znear, _in(T) _Zfar) {
//		T _33 = _Zfar / (_Znear - _Zfar);
//		if ( equal(_33, T(-1), T(0.00000002)) ) {
//			throw std::exception("Persepctive near is too small");
//		}
//		T _Sin = sin(_Fov * T(0.5));
//		T _Cos = cos(_Fov * T(0.5));
//		T _Tan = tan(T(1.57) - _Fov * T(0.5));
//
//		return (matrix<T, 4, 4>{
//			_Tan /_Aspect, T(0),        T(0), T( 0),
//			T(0),          _Tan,        T(0), T( 0),
//			T(0),          T(0),         _33, T(-1),
//			T(0),          T(0), _33 *_Znear, T( 0) });
//	/*
//	@_Used to Z:
//		f(x) = (z * _22 + 1 * _32) / -z
//		     = -_22 + _32/-z = z¹
//
//		      -_22 + _32/-z¹ = z
//			   	         _32 = (z + _22) * -z¹
//		f¹(x):-_32/(z + _22) = z¹
//
//	@_Error:
//		f/(n-f) = -1
//        f       = f - n
//		So, please not set "near" is very very small...
//	*/
//	}
//	/* </ gen_mat > */
//
//}// namespace clmagic
//


#endif