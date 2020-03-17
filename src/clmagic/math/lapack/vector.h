//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_math_lapack_VECTOR_h_
#define clmagic_math_lapack_VECTOR_h_
#include "../general/clmagic.h"// include is_*<>, shuffle<>
#include "../general/general.h"// include abs() minval() maxval()
#include "../general/simd.h"// SIMD
#include "../real/real.h"// include CSTD math
#include <string>
#include <algorithm>

namespace clmagic {

	/*
	|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
	     |---------|--------------|--------------|--------------|--------------|---------|
	               
	A0   A1        |----------block_size---------------------------------------|        B1
	              F0                                                           B0

	block_offset = A1 - A0
	leading_size = F0 - A1
	tail_size    = B1 - B0
	*/
	template<typename _Ty, typename _Block>
	struct vector_size {
		size_t block_size;
		size_t leading_size;
		size_t tail_size;
	};

	template<typename _Ty, typename _Block>
	vector_size<_Ty, _Block> make_vector_size(const _Ty* _First, const _Ty* _Last) {
		// _Mysize = _Block_size * block_traits::size() + _Leading_size + _Tail_size
		if (_First != nullptr && _Last >= _First) {
			const auto _Mysize  = _Last - _First;
			const auto _Mybkoff = offset_align_of(std::alignment_of_v<_Block>, _First) / sizeof(_Ty);
			const bool _Has_block_begin = (_Mybkoff + _Mysize >= block_traits<_Block>::size());

			size_t _Leading_size = _Has_block_begin ?
				block_traits<_Block>::size() - _Mybkoff :
				_Mysize;
			size_t _Tail_size  = _Has_block_begin ? 
				(_Mysize - _Leading_size) % block_traits<_Block>::size() :
				0;
			size_t _Block_size = (_Mysize - _Leading_size) / block_traits<_Block>::size();

			return vector_size<_Ty, _Block>{ _Block_size, _Leading_size, _Tail_size };
		} else {
			return vector_size<_Ty, _Block>{0, 0, 0};
		}
	}


	template<typename _Ty, typename _Block>
	struct const_subvector {// const _Ty
		using iterator_category = std::random_access_iterator_tag;
		using difference_type   = ptrdiff_t;

		using block_type       = const _Block;
		using block_pointer    = block_type*;
		using scalar_type      = const _Ty;
		using scalar_pointer   = scalar_type*;
		using scalar_reference = scalar_type&;

		using block_traits  = clmagic::block_traits<_Block>;
		using vector_size   = clmagic::vector_size<_Ty, _Block>;

		const_subvector(scalar_pointer _First_arg, scalar_pointer _Last_arg)
			: _First(_First_arg), _Last(_Last_arg), _Vsize(make_vector_size<_Ty, _Block>(_First_arg, _Last_arg)) {}

		size_t size() const {
			return (_Last - _First);
		}

		const vector_size& vsize() const {
			return _Vsize; 
		}

		size_t block_offset() const {// first block offset bytes
			return offset_align_of(std::alignment_of_v<block_type>, _First); 
		}

		bool empty() const {
			return (_Last - _First) == 0;
		}

		bool pass_alignment_point() const {// is pass alignment point
			return (block_offset() / sizeof(scalar_type) + size() >= block_traits::size());
		}

		scalar_pointer begin() const {
			return _First; 
		}
		scalar_pointer end() const {
			return _Last;
		}
		scalar_reference operator[](size_t _Pos) const {
			return *(_First + _Pos); 
		}

	private:
		scalar_pointer _First;
		scalar_pointer _Last;
		vector_size    _Vsize;
	};

	/*
	@_Note: Don't manage memory
	*/
	template<typename _Ty, typename _Block>
	struct subvector : public const_subvector<_Ty, _Block> {// default const _Ty
		using _Mybase = const_subvector<_Ty, _Block>;

		using iterator_category = std::random_access_iterator_tag;
		using difference_type   = ptrdiff_t;

		using block_type       = _Block;
		using block_pointer    = block_type*;
		using scalar_type      = _Ty;
		using scalar_pointer   = scalar_type*;
		using scalar_reference = scalar_type&;

		using block_traits = clmagic::block_traits<_Block>;

		using block_const_pointer    = block_type const*;
		using scalar_const_pointer   = scalar_type const*;
		using scalar_const_reference = scalar_type const&;

		subvector(scalar_pointer _First_arg, scalar_pointer _Last_arg)
			: _Mybase(_First_arg, _Last_arg) {}

		scalar_pointer begin() {
			return const_cast<scalar_pointer>(_Mybase::begin()); }
		scalar_pointer end() {
			return const_cast<scalar_pointer>(_Mybase::end()); }
		scalar_const_pointer begin() const {
			return _Mybase::begin(); }
		scalar_const_pointer end() const {
			return _Mybase::end(); }

		scalar_reference operator[](size_t _Pos) {
			return const_cast<scalar_reference>(_Mybase::operator[](_Pos)); }
		scalar_const_reference operator[](size_t _Pos) const {
			return _Mybase::operator[](_Pos); }
		
		void fill(scalar_const_reference _Val) {
			std::fill(this->begin(), this->end(), _Val); }

		template<typename _Iter> 
		void assign(_Iter _First, _Iter _Last) {
			std::copy(_First, _Last, this->begin()); }
	
		struct refernece {
			scalar_pointer _Myfirst;
			scalar_pointer _Mylast;
			vector_size<_Ty, _Block> _Mysize;
		};
	
		refernece& self_ref() {
			return reinterpret_cast<refernece&>(*this);
		}
	};


	/*
	---+---+---|---+---+---|---+---+---|---+---+---|---+---+---|---+---+---|---+---+---+---+---+---
	        ---|---+---+---|---+---+---|---+---+---|---+---+---|---+---+---|---+---
		   |mod|
			            ---|---+---+---|---+---+---|---+---+---|---+---+---|---+---|---+---+---
					   |mod|
	mod == block_offset
	*/
	template<typename _Ty, typename _Block>
	struct _Accelerate_block {
		using scalar_type          = _Ty;
		using scalar_pointer       = _Ty*;
		using scalar_const_pointer = _Ty const*;
		using scalar_const_reference = _Ty const&;

		using block_type          = _Block;
		using block_pointer       = _Block*;
		using block_const_pointer = _Block const*;// const _Block* 
		using block_const_reference = _Block const&;

		using block_traits = clmagic::block_traits<block_type>;

		// [_First, _Last)
		template<typename _Fn>
		static void transform(block_const_pointer _First, block_const_pointer _Last, block_pointer _Dest, _Fn _Func, 
			size_t _Leading_size, size_t _Tail_size) {
			// transform [_First, _Last) with _Func, addition [..., _First) and [_Last, ...)
			if (_Leading_size != 0) {
				const block_type           _Temp        = _Func(*(_First - 1));
				scalar_pointer             _Dest_lead  = reinterpret_cast<scalar_pointer>(_Dest) - _Leading_size;
				const scalar_const_pointer _Last_lead  = reinterpret_cast<scalar_const_pointer>((&_Temp) + 1);
				scalar_const_pointer       _First_lead = _Last_lead - _Leading_size;
				std::copy(_First_lead, _Last_lead, _Dest_lead);
			}

			for ( ; _First != _Last; ++_First, ++_Dest) {
				*_Dest = _Func(*_First);
			}

			if (_Tail_size != 0) {
				const block_type           _Temp       = _Func(*_First);
				scalar_pointer             _Dest_tail  = reinterpret_cast<scalar_pointer>(_Dest);
				scalar_const_pointer       _First_tail = reinterpret_cast<scalar_const_pointer>(&_Temp);
				const scalar_const_pointer _Last_tail  = _First_tail + _Tail_size;
				std::copy(_First_tail, _Last_tail, _Dest_tail);
			}
		}

		template<typename _Fn>
		static void transform(block_const_pointer _First1, block_const_pointer _Last1, block_const_pointer _First2, block_pointer _Dest, _Fn _Func,
			size_t _Leading_size, size_t _Tail_size) {
			// transform [_First1, _Last1) and [_First2, ...) with _Func, addition [..., _First) and [_Last, ...)
			if (_Leading_size != 0) {
				const block_type           _Tmp        = _Func(*(_First1 - 1), *(_First2 - 1));
				scalar_pointer             _Dest_lead  = reinterpret_cast<scalar_pointer>(_Dest) - _Leading_size;
				const scalar_const_pointer _Last_lead  = reinterpret_cast<scalar_const_pointer>((&_Tmp) + 1);
				scalar_const_pointer       _First_lead = _Last_lead - _Leading_size;
				std::copy(_First_lead, _Last_lead, _Dest_lead);
			}

			for ( ; _First1 != _Last1; ++_First1, ++_First2, ++_Dest) {
				*_Dest = _Func(*_First1, *_First2);
			}

			if (_Tail_size != 0) {
				const block_type           _Tmp        = _Func(*_First1, *_First2);
				scalar_pointer             _Dest_tail  = reinterpret_cast<scalar_pointer>(_Dest);
				scalar_const_pointer       _First_tail = reinterpret_cast<scalar_const_pointer>(&_Tmp);
				const scalar_const_pointer _Last_tail  = _First_tail + _Tail_size;
				std::copy(_First_tail, _Last_tail, _Dest_tail);
			}
		}
	
		template<typename _Fn1, typename _Fn2>
		static scalar_type comulate(block_const_pointer _First, block_const_pointer _Last, _Fn1/*block*/ _Func1, _Fn2/*scalar*/ _Func2,
			size_t _Leading_size, size_t _Tail_size) {
			// comulate [_First, _Last), addition [_First_pre, _First) and [_Last, _Last_aft) with _Func
			scalar_type _Result;

			if (_Leading_size != 0) {
				const scalar_const_pointer _Last_bk  = reinterpret_cast<scalar_const_pointer>(_First);
				scalar_const_pointer       _First_bk = _Last_bk - _Leading_size;
				_Result = *_First_bk++;// initial
				for ( ; _First_bk != _Last_bk; ++_First_bk) {
					_Result = _Func2(_Result, *_First_bk);// scalar_operation
				}
			}

			if (_First != _Last) {
				block_type _Comulate_block = *_First++;
				for ( ; _First != _Last; ++_First) {
					_Comulate_block = _Func1(_Comulate_block, *_First);// block_operation
				}

				scalar_const_pointer       _First_bk = reinterpret_cast<scalar_const_pointer>(&_Comulate_block);
				const scalar_const_pointer _Last_bk  = _First_bk + block_traits::size();
				if (_Leading_size == 0) {// initial 
					_Result = *_First_bk++; 
				}
				for (; _First_bk != _Last_bk; ++_First_bk) {
					_Result = _Func2(_Result, *_First_bk);
				}
			}

			if (_Tail_size != 0) {
				scalar_const_pointer       _First_bk = reinterpret_cast<scalar_const_pointer>(_First);
				const scalar_const_pointer _Last_bk  = _First_bk + _Tail_size;
				for ( ; _First_bk != _Last_bk; ++_First_bk) {
					_Result = _Func2(_Result, *_First_bk);
				}
			}

			return _Result;
		}

		// sum = comulate(_First, _Last, std::plus<block_type>(), std::plus<scalar_type>(), ...)
		// product = comulate(_First, _Last, std::multiplies<block_type>(), std::multiplies<scalar_type>(), ...)
		static scalar_type norm(block_const_pointer _First, block_const_pointer _Last, scalar_type _Level, size_t _Leading_size, size_t _Tail_size) {
			const auto _Func3       = std::plus<block_type>();
			const auto _Func4       = std::plus<scalar_type>();
			const auto _Level_block = block_traits::set1(_Level);
			const auto _Invlevel    = static_cast<scalar_type>(1) / _Level;

			const auto _Func1 = [_Func3, &_Level_block](block_const_reference _Res, block_const_reference _Elm) {
				return _Func3(_Res, pow(abs(_Elm), _Level_block)); };

			const auto _Func2 = [_Func4, &_Level](scalar_const_reference _Res, scalar_const_reference _Elm) {
				return _Func4(_Res, pow(abs(_Elm), _Level)); };

			return pow( comulate(_First, _Last, _Func1, _Func2, _Leading_size, _Tail_size), _Invlevel );
		}

		template<typename _Fn1, typename _Fn2, typename _Fn3>
		static scalar_type comulate(block_const_pointer _First1, block_const_pointer _Last1, block_const_pointer _First2, 
			_Fn1/*block*/ _Func1, _Fn2/*block*/ _Func2, _Fn3/*scalar*/ _Func3, 
			size_t _Leading_size, size_t _Tail_size) {
			// comulate( _Func1( [_First1, _Last1), [_First2, ...) ) ) with _Func2 _Func3
			scalar_type _Result;

			if (_Leading_size != 0) {
				const scalar_type          _Tmp_bk   = _Func1(*(_First1 - 1), *(_First2 - 1));
				const scalar_const_pointer _Last_bk  = reinterpret_cast<scalar_const_pointer>((&_Tmp_bk) + 1);
				scalar_const_pointer       _First_bk = _Last_bk - _Leading_size;
				_Result = *_First_bk++;// initial
				for ( ; _First_bk != _Last_bk; ++_First_bk) {
					_Result = _Func3(_Result , *_First_bk);// scalar_operation
				}
			}

			if (_First1 != _Last1) {
				block_type _Comulate_block = _Func1(*_First1++, *_First2++);
				for ( ; _First1 != _Last1; ++_First1, ++_First2) {
					_Comulate_block = _Func2( _Comulate_block, _Func1(*_First1, *_First2) );// block_operation
				}

				scalar_const_pointer       _First_bk = reinterpret_cast<scalar_const_pointer>(&_Comulate_block);
				const scalar_const_pointer _Last_bk  = _First_bk + block_traits::size();
				if (_Leading_size == 0) {// initial 
					_Result = *_First_bk++; 
				}
				for (; _First_bk != _Last_bk; ++_First_bk) {
					_Result = _Func3(_Result, *_First_bk);
				}
			}

			if (_Tail_size != 0) {
				const block_type           _Tmp_bk   = _Func1(*_First1, *_First2);
				scalar_const_pointer       _First_bk = reinterpret_cast<scalar_const_pointer>(&_Tmp_bk);
				const scalar_const_pointer _Last_bk  = _First_bk + _Tail_size;
				for ( ; _First_bk != _Last_bk; ++_First_bk) {
					_Result = _Func3(_Result, *_First_bk);
				}
			}

			return _Result;
		}
		
		// dot = comulate(_First1, _Last1, _First2, std::multiplies<block_type>(), std::plus<block_type>(), std::plus<scalar_type>(), ...)
	};


	template<typename _Ty, typename _Block>
	struct _Accelerate_subvector {
		using scalar_type          = _Ty;
		using scalar_pointer       = _Ty*;
		using scalar_const_pointer = _Ty const*;
		using scalar_const_reference = _Ty const&;

		using block_type            = _Block;
		using block_pointer         = _Block*;
		using block_const_pointer   = _Block const*;
		using block_const_reference = _Block const&;

		using vector_reference       = subvector<_Ty, _Block>;
		using vector_const_reference = const_subvector<_Ty, _Block>;

		using block_traits           = clmagic::block_traits<_Block>;
		using block_accelerate       = _Accelerate_block<_Ty, _Block>;
		using vector_size            = clmagic::vector_size<_Ty, _Block>;

		template<typename _Fn1, typename _Fn2>
		static void transform(vector_const_reference _Left, vector_reference _Result,
			/*block*/_Fn1 _Func1, /*scalar*/_Fn2 _Func2) {// _Result = _Func(_Left)
			assert(_Left.size() == _Result.size());
			// _Result can not be aligned

			if (_Left.block_offset() % sizeof(scalar_type) == 0 && _Left.pass_alignment_point()) {
				const auto& _Vsize = _Left.vsize();
				auto        _First = reinterpret_cast<block_const_pointer>(_Left.begin() + _Vsize.leading_size);
				const auto  _Last  = _First + _Vsize.block_size;
				auto        _Dest  = reinterpret_cast<block_pointer>(_Result.begin() + _Vsize.leading_size);
				block_accelerate::transform(_First, _Last, _Dest, _Func1, _Vsize.leading_size, _Vsize.tail_size);
			} else {// else scalar-operation
				auto       _First = _Left.begin();
				const auto _Last  = _Left.end();
				auto       _Dest  = _Result.begin();
				for (; _First != _Last; ++_First, ++_Dest) {
					*_Dest = _Func2(*_First);
				}
			}
		}

		template<typename _Fn1, typename _Fn2>
		static void transform(vector_const_reference _Left, vector_const_reference _Right, vector_reference _Result,
			/*block*/_Fn1 _Func1, /*scalar*/_Fn2 _Func2) {// _Result = _Func(_Left, _Right)
			assert(_Left.size() == _Right.size() && _Right.size() == _Result.size());

			const bool _Accelerate = (_Left.block_offset() == _Right.block_offset()/*A≡B(mod alignment_of_v<block_type>)*/ && 
									  _Left.block_offset() % sizeof(scalar_type) == 0 &&
									  _Left.pass_alignment_point() );
			// _Result can not be aligned

			if (_Accelerate) {
				const auto& _Vsize  = _Left.vsize();
				auto        _First1 = reinterpret_cast<block_const_pointer>(_Left.begin() + _Vsize.leading_size);
				const auto  _Last1  = _First1 + _Vsize.block_size;
				auto        _First2 = reinterpret_cast<block_const_pointer>(_Right.begin() + _Vsize.leading_size);
				auto        _Dest   = reinterpret_cast<block_pointer>(_Result.begin() + _Vsize.leading_size);
				block_accelerate::transform(_First1, _Last1, _First2, _Dest, _Func1, 
					_Vsize.leading_size, _Vsize.tail_size);
			} else {// else scalar-operation
				auto       _First1 = _Left.begin();
				const auto _Last1  = _Left.end();
				auto       _First2 = _Right.begin();
				auto       _Dest   = _Result.begin();
				std::transform(_First1, _Last1, _First2, _Dest, _Func2);
			}
		}
	
		template<typename _Fn1, typename _Fn2> inline
		static void transform_right_scalar(vector_const_reference _Left, vector_reference _Result,
			/*block*/_Fn1 _Func1, /*scalar*/_Fn2 _Func2, scalar_const_reference _Scalar) {
			const auto _Scalar_block = block_traits<block_type>::set1(_Scalar);
			transform(_Left, _Result, 
				[_Func1, &_Scalar_block] (block_const_reference _X) { return _Func1(_X, _Scalar_block); },
				[_Func2, &_Scalar] (scalar_const_reference _X) { return _Func2(_X, _Scalar); } );
		}

		template<typename _Fn1, typename _Fn2> inline
		static void transform_left_scalar(vector_const_reference _Right, vector_reference _Result,
			/*block*/_Fn1 _Func1, /*scalar*/_Fn2 _Func2, scalar_const_reference _Scalar) {
			const auto _Scalar_block = block_traits<block_type>::set1(_Scalar);
			transform(_Right, _Result,
				[_Func1, &_Scalar_block](block_const_reference _X) { return _Func1(_Scalar_block, _X); },
				[_Func2, &_Scalar](scalar_const_reference _X) { return _Func2(_Scalar, _X); } );
		}
	
		template<typename _Fn1, typename _Fn2> inline
		static scalar_type comulate(vector_const_reference _Left, _Fn1 _Func1, _Fn2 _Func2) {
			assert(!_Left.empty());

			if (_Left.pass_alignment_point() && _Left.block_offset() % sizeof(scalar_type) == 0) {
				const auto& _Vsize = _Left.vsize();
				auto        _First = reinterpret_cast<block_const_pointer>(_Left.begin() + _Vsize.leading_size);
				const auto  _Last  = _First + _Vsize.block_size;
				return block_accelerate::comulate(_First, _Last, _Func1, _Func2, _Vsize.leading_size, _Vsize.tail_size);
			} else {
				auto        _First  = _Left.begin();
				const auto  _Last   = _Left.end();
				scalar_type _Result = *_First++;
				for (; _First != _Last; ++_First) {
					_Result = _Func2(_Result, *_First);
				}
				return _Result;
			}
		}
	
		static scalar_type norm(vector_const_reference _Left, scalar_type _Level) {
			const auto _Func3       = std::plus<block_type>();
			const auto _Func4       = std::plus<scalar_type>();
			const auto _Level_block = block_traits::set1(_Level);
			const auto _Invlevel    = static_cast<scalar_type>(1) / _Level;

			const auto _Func1 = [_Func3, &_Level_block](block_const_reference _Res, block_const_reference _Elm) {
				return _Func3(_Res, pow(abs(_Elm), _Level_block)); };

			const auto _Func2 = [_Func4, &_Level](scalar_const_reference _Res, scalar_const_reference _Elm) {
				return _Func4(_Res, pow(abs(_Elm), _Level)); };

			return pow( comulate(_Left, _Func1, _Func2), _Invlevel );
		}

		template<typename _Fn1, typename _Fn2, typename _Fn3, typename _Fn4> inline
		static scalar_type comulate(vector_const_reference _Left, vector_const_reference _Right, _Fn1 _Func1, _Fn2 _Func2, _Fn3 _Func3, _Fn4 _Func4) {
			assert(_Left.size() == _Right.size());
			assert(!_Left.empty());
			const bool _Accelerate = _Left.block_offset() == _Right.block_offset() &&
									 _Left.block_offset() % sizeof(scalar_type) == 0 &&
									 _Left.pass_alignment_point();
			if (_Accelerate) {
				const auto& _Vsize  = _Left.vsize();
				auto        _First1 = reinterpret_cast<block_const_pointer>(_Left.begin() + _Vsize.leading_size);
				const auto  _Last1  = _First1 + _Vsize.block_size;
				auto        _First2 = reinterpret_cast<block_const_pointer>(_Right.begin() + _Vsize.leading_size);
				return block_accelerate::comulate(_First1, _Last1, _First2, _Func1, _Func3, _Func4, _Vsize.leading_size, _Vsize.tail_size);
			} else {
				auto        _First1 = _Left.begin();
				const auto  _Last1  = _Left.end();
				auto        _First2 = _Right.begin();
				scalar_type _Result = _Func2(*_First1, *_First2);
				for (; _First1 != _Last1; ++_First1, ++_First2) {
					_Result = _Func4(_Result, _Func2(*_First1, *_First2));
				}
				return _Result;
			}
		}
	};

	// vector OP vector
	template<typename _Ty, typename _Block> inline
	void add(const_subvector<_Ty, _Block> _Left, const_subvector<_Ty, _Block> _Right, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform(_Left, _Right, _Result, std::plus<_Block>(), std::plus<_Ty>());
	}
	template<typename _Ty, typename _Block> inline
	void sub(const_subvector<_Ty, _Block> _Left, const_subvector<_Ty, _Block> _Right, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform(_Left, _Right, _Result, std::minus<_Block>(), std::minus<_Ty>());
	}
	template<typename _Ty, typename _Block> inline
	void mul(const_subvector<_Ty, _Block> _Left, const_subvector<_Ty, _Block> _Right, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform(_Left, _Right, _Result, std::multiplies<_Block>(), std::multiplies<_Ty>());
	}
	template<typename _Ty, typename _Block> inline
	void div(const_subvector<_Ty, _Block> _Left, const_subvector<_Ty, _Block> _Right, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform(_Left, _Right, _Result, std::divides<_Block>(), std::divides<_Ty>());
	}
	template<typename _Ty, typename _Block> inline
	void mod(const_subvector<_Ty, _Block> _Left, const_subvector<_Ty, _Block> _Right, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform(_Left, _Right, _Result, std::modulus<_Block>(), std::modulus<_Ty>());
	}
	template<typename _Ty, typename _Block> inline
	void pow(const_subvector<_Ty, _Block> _Left, const_subvector<_Ty, _Block> _Right, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform(_Left, _Right, _Result, std::func_pow<_Block>(), std::func_pow<_Ty>());
	}
	template<typename _Ty, typename _Block> inline
	_Ty dot(const_subvector<_Ty, _Block> _Left, const_subvector<_Ty, _Block> _Right) {
		return _Accelerate_subvector<_Ty, _Block>::comulate(_Left, _Right, 
			std::multiplies<_Block>(), std::multiplies<_Ty>(), std::plus<_Block>(), std::plus<_Ty>());
	}

	// vector OP scalar
	template<typename _Ty, typename _Block> inline
	void add(const_subvector<_Ty, _Block> _Left, const _Ty& _Scalar, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform_right_scalar(_Left, _Result, std::plus<_Block>(), std::plus<_Ty>(), _Scalar);
	}
	template<typename _Ty, typename _Block> inline
	void sub(const_subvector<_Ty, _Block> _Left, const _Ty& _Scalar, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform_right_scalar(_Left, _Result, std::minus<_Block>(), std::minus<_Ty>(), _Scalar);
	}
	template<typename _Ty, typename _Block> inline
	void mul(const_subvector<_Ty, _Block> _Left, const _Ty& _Scalar, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform_right_scalar(_Left, _Result, std::multiplies<_Block>(), std::multiplies<_Ty>(), _Scalar);
	}
	template<typename _Ty, typename _Block> inline
	void div(const_subvector<_Ty, _Block> _Left, const _Ty& _Scalar, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform_right_scalar(_Left, _Result, std::divides<_Block>(), std::divides<_Ty>(), _Scalar);
	}
	template<typename _Ty, typename _Block> inline
	void mod(const_subvector<_Ty, _Block> _Left, const _Ty& _Scalar, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform_right_scalar(_Left, _Result, std::modulus<_Block>(), std::modulus<_Ty>(), _Scalar);
	}
	template<typename _Ty, typename _Block> inline
	void pow(const_subvector<_Ty, _Block> _Left, const _Ty& _Scalar, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform_right_scalar(_Left, _Result, std::func_pow<_Block>(), std::func_pow<_Ty>(), _Scalar);
	}

	// scalar OP vector
	template<typename _Ty, typename _Block> inline
	void add(const _Ty& _Scalar, const_subvector<_Ty, _Block> _Right, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform_left_scalar(_Right, _Result, std::plus<_Block>(), std::plus<_Ty>(), _Scalar);
	}
	template<typename _Ty, typename _Block> inline
	void sub(const _Ty& _Scalar, const_subvector<_Ty, _Block> _Right, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform_left_scalar(_Right, _Result, std::minus<_Block>(), std::minus<_Ty>(), _Scalar);
	}
	template<typename _Ty, typename _Block> inline
	void mul(const _Ty& _Scalar, const_subvector<_Ty, _Block> _Right, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform_left_scalar(_Right, _Result, std::multiplies<_Block>(), std::multiplies<_Ty>(), _Scalar);
	}
	template<typename _Ty, typename _Block> inline
	void div(const _Ty& _Scalar, const_subvector<_Ty, _Block> _Right, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform_left_scalar(_Right, _Result, std::divides<_Block>(), std::divides<_Ty>(), _Scalar);
	}
	template<typename _Ty, typename _Block> inline
	void mod(const _Ty& _Scalar, const_subvector<_Ty, _Block> _Right, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform_left_scalar(_Right, _Result, std::modulus<_Block>(), std::modulus<_Ty>(), _Scalar);
	}

	

	template<typename Iter>
	std::string _To_string(Iter _First, Iter _Last) {
		using std::to_string;
		std::string _Str = "[";
		for ( ; _First != _Last; ++_First) {
			_Str += to_string(*_First);
			_Str += ' ';
		}
		_Str.back() = ']';
		return _Str;
	}


	/*
	----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+---- float = scalar = block
	-------------------+-------------------+-------------------+-------------------+-------------------+-------------------+ __m128 = block
	Vector is a lot of block, Its scalar is minimum block
	*/
	template<typename _Ty, size_t _Size, typename _Block = _Ty>
	struct __declspec(align(std::alignment_of_v<_Block>)) vector {
		using block_type           = _Block;
		using block_pointer        = block_type*;
		using block_const_pointer  = block_type const*;
		using block_const_reference = block_type const&;

		using scalar_type            = _Ty;
		using scalar_pointer         = scalar_type*;
		using scalar_const_pointer   = scalar_type const*;
		using scalar_reference       = scalar_type&;
		using scalar_const_reference = scalar_type const&;

		using subvector       = clmagic::subvector<_Ty, _Block>;
		using const_subvector = clmagic::const_subvector<_Ty, _Block>;

		using block_traits     = clmagic::block_traits<_Block>;
		using block_accelerate = _Accelerate_block<_Ty, _Block>;
		//using scalar_traits = clmagic::block_traits<scalar_type>;

		static constexpr size_t size() {
			return _Size; }
		static constexpr size_t tail_size() {
			return _Size % block_traits::size(); }
		static constexpr size_t block_size() {
			return _Size / block_traits::size(); }
		
		block_pointer block_begin() {
			return reinterpret_cast<block_pointer>(_Mydata); }
		block_pointer block_end() {
			return reinterpret_cast<block_pointer>(_Mydata) + block_size(); }
		block_const_pointer block_begin() const {
			return reinterpret_cast<block_const_pointer>(_Mydata); }
		block_const_pointer block_end() const {
			return reinterpret_cast<block_const_pointer>(_Mydata) + block_size(); }

		scalar_pointer ptr(size_t _Pos = 0) {
			return _Mydata + _Pos; }
		scalar_pointer begin() {
			return _Mydata; }
		scalar_pointer end() {
			return _Mydata + this->size(); }
		scalar_const_pointer ptr(size_t _Pos = 0) const {
			return _Mydata + _Pos; }
		scalar_const_pointer begin() const {
			return _Mydata; }
		scalar_const_pointer end() const {
			return _Mydata + this->size(); }

		scalar_reference operator[](size_t _Pos) {
			return _Mydata[_Pos]; }
		scalar_const_reference operator[](size_t _Pos) const {
			return _Mydata[_Pos]; }

		// assgin(...)
		void assign(scalar_const_reference _Val) {
			std::fill(this->begin(), this->end(), _Val); 
		}

		void assign(std::initializer_list<scalar_type> _Ilist) {
			assert(_Ilist.size() <= this->size());
			auto _Dest = std::copy(_Ilist.begin(), _Ilist.end(), this->begin());
			std::fill(_Dest, this->end(), static_cast<scalar_type>(0));
		}

		template<typename _Iter>
		void assign(_Iter _First, _Iter _Last) {
			assert( std::distance(_First, _Last) <= std::_Iter_diff_t<_Iter>(this->size()) );
			auto _Dest = std::copy(_First, _Last, this->begin());
			std::fill(_Dest, this->end(), static_cast<scalar_type>(0));
		}

		template<typename _Fn>
		void assign(const vector& _Left, _Fn _Func) {
			if _CONSTEXPR_IF(vector::size() < block_traits::size()) {
				const auto _Tmp_bk = _Func(*_Left.block_begin());
				auto       _First  = reinterpret_cast<scalar_const_pointer>(&_Tmp_bk);
				const auto _Last   = _First + this->size();
				std::copy(_First, _Last, this->begin());
			} else {
				auto       _First = _Left.block_begin();
				const auto _Last  = _Left.block_end();
				auto       _Dest  = this->block_begin();
				for ( ; _First != _Last; ++_First, ++_Dest) {
					*_Dest = _Func(*_First);
				}

				if _CONSTEXPR_IF(vector::size() % block_traits::size()) {
					const auto  _Tmp_bk = _Func(*_First);
					auto        _First2 = reinterpret_cast<scalar_const_pointer>(&_Tmp_bk);
					auto        _Dest2  = reinterpret_cast<scalar_pointer>(_Dest);
					const auto  _Last2  = this->end();
					for ( ; _Dest2 != _Last2; ++_Dest2, ++_First2) {
						*_Dest2 = *_First2;
					}
				}
			}
		}

		template<typename _Fn>
		void assign(const vector& _Left, const vector& _Right, _Fn _Func) {
			if _CONSTEXPR_IF(vector::size() < block_traits::size()) {
				const auto _Tmp_bk = _Func(*_Left.block_begin(), *_Right.block_begin());
				auto       _First  = reinterpret_cast<scalar_const_pointer>(&_Tmp_bk);
				const auto _Last   = _First + this->size();
				std::copy(_First, _Last, this->begin());
			} else {
				auto       _First1 = _Left.block_begin();
				auto       _First2 = _Right.block_begin();
				const auto _Last1  = _Left.block_end();
				auto       _Dest1   = this->block_begin();
				for ( ; _First1 != _Last1; ++_First1, ++_First2, ++_Dest1) {
					*_Dest1 = _Func(*_First1, *_First2);
				}

				if _CONSTEXPR_IF(vector::size() % block_traits::size()) {
					const auto _Tmp_bk = _Func(*_First1, *_First2);
					auto       _First  = reinterpret_cast<scalar_const_pointer>(&_Tmp_bk);
					auto       _Dest   = reinterpret_cast<scalar_pointer>(_Dest1);
					const auto _Last   = this->end();
					for ( ; _Dest != _Last; ++_Dest, ++_First) {// [_Dest2, _Last2)
						*_Dest = *_First;
					}
				}
			}
		}

		template<typename _Fn>
		void assign(const vector& _Left, const scalar_type& _Scalar, _Fn _Func) {
			const auto _Right_bk = block_traits::set1(_Scalar);
			this->assign(_Left,
				[_Func, &_Right_bk](block_const_reference _Left_bk) {
					return _Func(_Left_bk, _Right_bk);
				} );
		}

		template<typename _Fn>
		void assign(const scalar_type& _Scalar, const vector& _Right, _Fn _Func) {
			const auto _Left_bk = block_traits::set1(_Scalar);
			this->assign(_Right, 
				[_Func, &_Left_bk] (block_const_reference _Right_bk) {
					return _Func(_Left_bk, _Right_bk);
				} );
		}

		template<typename _Fn>
		vector<bool, _Size>&& compare(const vector& _Right, _Fn _Pred) const {
			vector<bool, _Size> _Result;
			std::transform(this->begin(), this->end(), _Right.begin(), _Result.begin(), _Pred);
			return std::move(_Result);
		}

		template<typename _Fn>
		vector<bool, _Size>&& compare(const scalar_type& _Scalar, _Fn _Pred) const {
			vector<bool, _Size> _Result;
			auto       _First = this->begin();
			const auto _Last  = this->end();
			auto       _Dest  = _Result.begin();
			for (; _First != _Last; ++_First, ++_Dest) {
				*_Dest = _Pred(*_First, _Scalar);
			}
			return std::move(_Result);
		}
		
		// constructor
		constexpr vector() : _Mydata{ 0 } {}

		explicit vector(scalar_const_reference _Val) { 
			this->assign(_Val); }

		vector(std::initializer_list<scalar_type> _Ilist) { 
			this->assign(_Ilist); }

		template<typename _Iter> vector(_Iter _First, _Iter _Last) { 
			this->assign(_First, _Last); }

		template<typename _Fn> vector(const vector& _Left, _Fn _Func) {
			this->assign(_Left, _Func); }

		template<typename _Fn> vector(const vector& _Left, const vector& _Right, _Fn _Func) {
			this->assign(_Left, _Right, _Func); }

		template<typename _Fn> vector(const vector& _Left, const scalar_type& _Scalar, _Fn _Func) {
			this->assign(_Left, _Scalar, _Func); }

		template<typename _Fn> vector(const scalar_type& _Scalar, const vector& _Right, _Fn _Func) {
			this->assign(_Scalar, _Right, _Func); }


		subvector operator()(size_t _First, size_t _Last) {
			return subvector(this->ptr(_First), this->ptr(_Last)); 
		}
		const_subvector operator()(size_t _First, size_t _Last) const {
			return const_subvector(this->ptr(_First), this->ptr(_Last)); 
		}

		vector<bool, _Size> operator==(const vector& _Right) const {
			return this->compare(_Right, std::equal_to<scalar_type>());
		}
		vector<bool, _Size> operator!=(const vector& _Right) const {
			return this->compare(_Right, std::not_equal_to<scalar_type>());
		}
		vector<bool, _Size> operator<(const vector& _Right) const {
			return this->compare(_Right, std::less<scalar_type>());
		}
		vector<bool, _Size> operator>(const vector& _Right) const {
			return this->compare(_Right, std::greater<scalar_type>());
		}
		vector<bool, _Size> operator<=(const vector& _Right) const {
			return this->compare(_Right, std::less_equal<scalar_type>());
		}
		vector<bool, _Size> operator>=(const vector& _Right) const {
			return this->compare(_Right, std::greater_equal<scalar_type>());
		}
		vector<bool, _Size> operator==(const scalar_type& _Scalar) const {
			return this->compare(_Scalar, std::equal_to<scalar_type>());
		}
		vector<bool, _Size> operator!=(const scalar_type& _Scalar) const {
			return this->compare(_Scalar, std::not_equal_to<scalar_type>());
		}
		vector<bool, _Size> operator<(const scalar_type& _Scalar) const {
			return this->compare(_Scalar, std::less<scalar_type>());
		}
		vector<bool, _Size> operator>(const scalar_type& _Scalar) const {
			return this->compare(_Scalar, std::greater<scalar_type>());
		}
		vector<bool, _Size> operator<=(const scalar_type& _Scalar) const {
			return this->compare(_Scalar, std::less_equal<scalar_type>());
		}
		vector<bool, _Size> operator>=(const scalar_type& _Scalar) const {
			return this->compare(_Scalar, std::greater_equal<scalar_type>());
		}

		vector operator-() const {
			return vector(*this, std::negate<block_type>()); 
		}
		vector operator+(const vector& _Right) const {
			return vector(*this, _Right, std::plus<block_type>()); 
		}
		vector operator-(const vector& _Right) const {
			return vector(*this, _Right, std::minus<block_type>()); 
		}
		vector operator*(const vector& _Right) const {
			return vector(*this, _Right, std::multiplies<block_type>());
		}
		vector operator/(const vector& _Right) const {
			return vector(*this, _Right, std::divides<block_type>()); 
		}
		vector operator%(const vector& _Right) const {
			return vector(*this, _Right, std::modulus<block_type>()); 
		}
		vector operator+(const scalar_type& _Scalar) const {
			return vector(*this, _Scalar, std::plus<block_type>()); 
		}
		vector operator-(const scalar_type& _Scalar) const {
			return vector(*this, _Scalar, std::minus<block_type>()); 
		}
		vector operator*(const scalar_type& _Scalar) const {
			return vector(*this, _Scalar, std::multiplies<block_type>()); 
		}
		vector operator/(const scalar_type& _Scalar) const {
			return vector(*this, _Scalar, std::divides<block_type>()); 
		}
		vector operator%(const scalar_type& _Scalar) const {
			return vector(*this, _Scalar, std::modulus<block_type>()); 
		}
		vector& operator+=(const vector& _Right) {
			assign(*this, _Right, std::plus<block_type>()); 
			return (*this); 
		}
		vector& operator-=(const vector& _Right) {
			assign(*this, _Right, std::minus<block_type>()); 
			return (*this); 
		}
		vector& operator*=(const vector& _Right) {
			assign(*this, _Right, std::multiplies<block_type>()); 
			return (*this);
		}
		vector& operator/=(const vector& _Right) {
			assign(*this, _Right, std::divides<block_type>()); 
			return (*this); 
		}
		vector& operator%=(const vector& _Right) {
			assign(*this, _Right, std::modulus<block_type>()); 
			return (*this); 
		}
		vector& operator+=(const scalar_type& _Scalar) {
			assign(*this, _Scalar, std::plus<block_type>()); 
			return (*this); 
		}
		vector& operator-=(const scalar_type& _Scalar) {
			assign(*this, _Scalar, std::minus<block_type>()); 
			return (*this);
		}
		vector& operator*=(const scalar_type& _Scalar) {
			assign(*this, _Scalar, std::multiplies<block_type>()); 
			return (*this);
		}
		vector& operator/=(const scalar_type& _Scalar) {
			assign(*this, _Scalar, std::divides<block_type>()); 
			return (*this);
		}
		vector& operator%=(const scalar_type& _Scalar) {
			assign(*this, _Scalar, std::modulus<block_type>());
			return (*this); 
		}

		friend vector operator+(const scalar_type& _Scalar, const vector& _Right) {
			return vector(_Scalar, _Right, std::plus<block_type>()); 
		}
		friend vector operator-(const scalar_type& _Scalar, const vector& _Right) {
			return vector(_Scalar, _Right, std::minus<block_type>()); 
		}
		friend vector operator*(const scalar_type& _Scalar, const vector& _Right) {
			return vector(_Scalar, _Right, std::multiplies<block_type>()); 
		}
		friend vector operator/(const scalar_type& _Scalar, const vector& _Right) {
			return vector(_Scalar, _Right, std::divides<block_type>()); 
		}
		friend vector operator%(const scalar_type& _Scalar, const vector& _Right) {
			return vector(_Scalar, _Right, std::modulus<block_type>());
		}

		vector& normalize() {
			const auto _NormL2sq = normL2_square(*this);
			if (approach_equal(_NormL2sq, static_cast<scalar_type>(1), std::numeric_limits<scalar_type>::epsilon())) {
				*this /= sqrt(_NormL2sq);
			}
			return (*this);
		}

		friend std::string to_string(const vector& _Left) {
			return _To_string(_Left.begin(), _Left.end()); 
		}
		friend std::ostream& operator<<(std::ostream& _Ostr, const vector& _Left) {
			return (_Ostr << to_string(_Left)); 
		}

	private:
		scalar_type _Mydata[_Size];
	};

	template<typename _Ty, typename _Block = _Ty>
		using vector2 = vector<_Ty, 2, _Block>;
	template<typename _Ty, typename _Block = _Ty>
		using vector3 = vector<_Ty, 3, _Block>;
	template<typename _Ty, typename _Block = _Ty>
		using vector4 = vector<_Ty, 4, _Block>;

	template<size_t _Size>
		using bvector = vector<bool, _Size>;

	template<typename _Ty, size_t _Size, typename _Block = _Ty>
		using point = vector<_Ty, _Size, _Block>;
	template<typename _Ty, typename _Block = _Ty>
		using point2 = vector<_Ty, 2, _Block>;
	template<typename _Ty, typename _Block = _Ty>
		using point3 = vector<_Ty, 3, _Block>;
	
	using point2_size_t = point2<size_t>;


#define vectorN vector<_Ty, _Size, _Block>
	template<typename _Ty, size_t _Size, typename _Block> inline
	_Ty sum(const vectorN& _X) {// X[0] + X[1] + X[2] + ... + X[N]
		const auto _Func1 = std::plus<_Block>();
		const auto _Func2 = std::plus<_Ty>();
		return _Accelerate_block<_Ty, _Block>::comulate(_X.block_begin(), _X.block_end(), _Func1, _Func2, 0, _X.tail_size());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	_Ty product(const vectorN& _X) {// X[0] * X[1] * X[2] * ... * X[N]
		const auto _Func1 = std::multiplies<_Block>();
		const auto _Func2 = std::multiplies<_Ty>();
		return _Accelerate_block<_Ty, _Block>::comulate(_X.block_begin(), _X.block_end(), _Func1, _Func2, 0, _X.tail_size());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	_Ty dot(const vectorN& _X, const vectorN& _Y) {// X[0]*Y[0] + X[1]*Y[1] + X[2]*Y[2] + ... * X[N]*Y[N]
		const auto _Func1 = std::multiplies<_Block>();
		const auto _Func2 = std::plus<_Block>();
		const auto _Func3 = std::plus<_Ty>();
		return _Accelerate_block<_Ty, _Block>::comulate(_X.block_begin(), _X.block_end(), _Y.block_begin(), _Func1, _Func2, _Func3, 0, _X.tail_size());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	_Ty normL2_square(const vectorN& _X) {// X[0]² + X[1]² + X[2]² + ... * X[N]²
		return dot(_X, _X);
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	_Ty length(const vectorN& _X) {// sqrt(dot(X, X))
		return sqrt(normL2_square(_X));
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	_Ty norm(const vectorN& _X, const _Ty& _Level) {// sqrt(dot(X, X))
		return _Accelerate_block<_Ty, _Block>::norm(_X.block_begin(), _X.block_end(), _Level, 0, _X.tail_size());
	}

	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN normalize(const vectorN& _X) {
		const auto _NormL2sq = normL2_square(_X);
		return approach_equal(_NormL2sq, static_cast<_Ty>(1), std::numeric_limits<_Ty>::epsilon()) ?
			_X :
			_X / sqrt(_NormL2sq);
	}

	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN mod(const vectorN& _Left, const vectorN& _Right) {
		return vector(_Left, _Right, std::modulus<_Block>());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN pow(const vectorN& _Left, const vectorN& _Right) {
		return vector(_Left, _Right, std::func_pow<_Block>());
	}

	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN abs(const vectorN& _Left) {
		return vector(_Left, std::func_abs<_Block>());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN floor(const vectorN& _Left) {
		return vector(_Left, std::func_floor<_Block>());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN ceil(const vectorN& _Left) {
		return vector(_Left, std::func_ceil<_Block>());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN trunc(const vectorN& _Left) {
		return vector(_Left, std::func_trunc<_Block>());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN round(const vectorN& _Left) {
		return vector(_Left, std::func_round<_Block>());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN sqrt(const vectorN& _Left) {
		return vector(_Left, std::func_sqrt<_Block>());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN cbrt(const vectorN& _Left) {
		return vector(_Left, std::func_cbrt<_Block>());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN invsqrt(const vectorN& _Left) {
		return vector(_Left, std::func_invsqrt<_Block>());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN invcbrt(const vectorN& _Left) {
		return vector(_Left, std::func_invcbrt<_Block>());
	}

	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN sin(const vectorN& _Left) {
		return vector(_Left, std::func_sin<_Block>());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN cos(const vectorN& _Left) {
		return vector(_Left, std::func_cos<_Block>());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN tan(const vectorN& _Left) {
		return vector(_Left, std::func_tan<_Block>());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN asin(const vectorN& _Left) {
		return vector(_Left, std::func_asin<_Block>());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN acos(const vectorN& _Left) {
		return vector(_Left, std::func_acos<_Block>());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN atan(const vectorN& _Left) {
		return vector(_Left, std::func_atan<_Block>());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN atan2(const vectorN& _Y, const vectorN& _X) {
		return vector(_Y, _X, std::func_atan2<_Block>());
	}

	// compare function
	template<size_t _Size, typename _Block>
	bool all_of(const vector<bool, _Size, _Block>& _X) {
		for (auto _First = _X.begin(); _First != _X.end(); ++_First) {
			if (!(*_First)) {
				return false;
			}
		}
		return true;
	}
	template<size_t _Size, typename _Block>
	bool any_of(const vector<bool, _Size, _Block>& _X) {
		for (auto _First = _X.begin(); _First != _X.end(); ++_First) {
			if (*_First) {
				return true;
			}
		}
		return false;
	}
	template<size_t _Size, typename _Block>
	bool none_of(const vector<bool, _Size, _Block>& _X) {
		for (auto _First = _X.begin(); _First != _X.end(); ++_First) {
			if ((*_First)) {
				return false;
			}
		}
		return true;
	}

#undef vectorN


	/*
	@_vector_any: dynamic-length vector
	*/
	template<typename _Ty, typename _Block = _Ty, typename _Alloc = std::allocator<_Block>>
	class vector_any : public clmagic::subvector<_Ty, _Block> {
		using _Mybase = clmagic::subvector<_Ty, _Block>;
	public:
		using scalar_type            = _Ty;
		using scalar_pointer         = _Ty*;
		using scalar_const_pointer   = _Ty const*;
		using scalar_reference       = _Ty&;
		using scalar_const_reference = _Ty const&;

		using block_type           = _Block;
		using block_pointer        = _Block*;
		using block_const_pointer  = _Block const*;

		using subvector       = clmagic::subvector<_Ty, _Block>;
		using const_subvector = clmagic::const_subvector<_Ty, _Block>;

		using block_traits    = clmagic::block_traits<_Block>;
		using allocate_traits = std::allocator_traits<_Alloc>;

		vector_any() : _Mybase(nullptr, nullptr), _Myalloc(), _Mydata(nullptr), _Mycapacity(0) { }

		template<typename _Iter>
		void assign(_Iter _First, _Iter _Last) {
			const size_t _Newcapacity = std::distance(_First, _Last) / block_traits::size() + 1;
			this->_Realocate(_Newcapacity, 0);

			auto&           _Myvec  = _Mybase::self_ref();
			scalar_pointer& _Mylast = _Myvec._Mylast;
			for (; _First != _Last; ++_First) {
				allocate_traits::construct(_Myalloc, _Mylast, *_First);
				++_Mylast;// correct _Mylast
			}
			_Myvec._Mysize = make_vector_size<_Ty, _Block>(_Myvec._Myfirst, _Myvec._Mylast);// correct _Myvsize
		}

		template<typename _Iter>
		vector_any(_Iter _First, _Iter _Last) : vector_any() {
			this->assign(_First, _Last);
		}

		vector_any(std::initializer_list<scalar_type> _Ilist) : vector_any(_Ilist.begin(), _Ilist.end()) { 
			// do nothing
		}

		subvector operator()(size_t _First, size_t _Last) {
			return subvector(this->ptr(_First), this->ptr(_Last));
		}
		const_subvector operator()(size_t _First, size_t _Last) const {
			return const_subvector(this->ptr(_First), this->ptr(_Last));
		}

		size_t size() const {
			return _Mybase::size();
		}
		size_t capacity() const {
			return _Mycapacity; 
		}
		bool empty() const {
			return (this->size() == 0);
		}

		scalar_pointer ptr(size_t _Pos = 0) {
			return _Mybase::begin() + _Pos;
		}
		scalar_pointer begin() {
			return _Mybase::begin(); 
		}
		scalar_pointer end() {
			return _Mybase::end(); 
		}
		scalar_reference operator[](size_t _Pos) {
			return _Mybase::operator[](_Pos); 
		}
		
		scalar_const_pointer ptr(size_t _Pos = 0) const {
			return _Mybase::begin() + _Pos; }
		scalar_const_pointer begin() const {
			return _Mybase::begin(); 
		}
		scalar_const_pointer end() const {
			return _Mybase::end(); 
		}
		scalar_const_reference operator[](size_t _Pos) const {
			return _Mybase::operator[](_Pos); }

		void clear() {// don't modify capcity
			auto& _Myvec = _Mybase::self_ref();
			_Destroy_range(_Myvec._Myfirst, _Myvec._Mylast);
			_Myvec._Mylast = _Myvec._Myfirst;
			_Myvec._Mysize = { 0, 0, 0 };
		}

		template<typename... _Valty>
		void emplace_back(_Valty&&... _Val) {
			if (_Has_capacity(1)) {
				auto&           _Myvec  = _Mybase::self_ref();
				scalar_pointer& _Mylast = _Myvec._Mylast;
				allocate_traits::construct(_Myalloc, _Mylast, std::forward<_Valty...>(_Val...));
				++_Mylast;// correct _Mylast
				_Myvec._Mysize = make_vector_size<_Ty, _Block>(_Myvec._Myfirst, _Myvec._Mylast);// correct _Myvsize
			} else {
				_Realocate(_Calculate_growth(this->capacity() + 1), this->size());
				emplace_back(std::forward<_Valty...>(_Val...));
			}
		}

		void push_back(const _Ty& _Val) {
			this->emplace_back(_Val);
		}

		void push_back(_Ty&& _Val) {
			this->emplace_back(std::move(_Val)); }

		void pop_back() {
			if ( !this->empty() ) {
				auto& _Myvec = _Mybase::self_ref();
				--_Myvec._Mylast;
				allocate_traits::destroy(_Myalloc, _Myvec._Mylast);
			}
		}

		~vector_any() {// destroy[_First, _last) and delete[] _Mydata
			auto& _Myvec = _Mybase::self_ref();
			_Destroy_range(_Myvec._Myfirst, _Myvec._Mylast);
			allocate_traits::deallocate(_Myalloc, _Mydata, _Mycapacity);
			_Mydata = nullptr;
		}

		friend std::string to_string(const vector_any& _Left) {
			return _To_string(_Left.begin(), _Left.end()); }
		friend std::ostream& operator<<(std::ostream& _Ostr, const vector_any& _Left) {
			return (_Ostr << to_string(_Left)); }

	private:
		void _Destroy_range(scalar_pointer _First, scalar_const_pointer _Last) {
			for (; _First != _Last; ++_First) {
				//_First->~_Ty();
				allocate_traits::destroy(_Myalloc, _First);
			}
		}

		size_t max_size() const {
			return std::numeric_limits<size_t>::max();
		}

		bool _Has_capacity(size_t _Addition) {
			return (this->size() + _Addition <= this->capacity() * block_traits::size());
		}

		/* learning from C++Stantard-library-vector<_Ty,...> */
		size_t _Calculate_growth(const size_t _Newsize) const {
			// given _Oldcapacity and _Newsize, calculate geometric growth
			const auto _Oldcapacity = capacity();

			if (_Oldcapacity > max_size() - _Oldcapacity / 2) {
				return _Newsize; // geometric growth would overflow
			}

			const size_t _Geometric = std::max(size_t(2), _Oldcapacity + _Oldcapacity / 2);

			if (_Geometric < _Newsize) {
				return _Newsize; // geometric growth would be insufficient
			}

			return _Geometric; // geometric growth is sufficient
		}

		/* learning from C++Stantard-library-vector<_Ty,...> */
		void _Realocate(size_t _Newcapacity/*block*/, size_t _Newsize/*scalar*/) {
			auto&           _Myvec   = _Mybase::self_ref();
			scalar_pointer& _Myfirst = _Myvec._Myfirst;
			scalar_pointer& _Mylast  = _Myvec._Mylast;
			auto&           _Myvsize = _Myvec._Mysize;

			if (_Newcapacity != _Mycapacity) {
				block_pointer  _Newdata  = allocate_traits::allocate(_Myalloc, _Newcapacity); // reallocate;
				scalar_pointer _Newfirst = reinterpret_cast<scalar_pointer>(_Newdata);

				size_t _Oldsize = _Mylast - _Myfirst;
				if (_Oldsize != 0) {
					auto _Last = _Myfirst + std::min(_Oldsize, _Newsize);
					std::move(_Myfirst, _Last, _Newfirst);
					_Destroy_range(_Last, _Mylast);
					allocate_traits::deallocate(_Myalloc, _Mydata, _Mycapacity);
				}// move [_Myfirst, ...) to [_Newfirst, ...) and delete _Mydata
				_Mydata     = _Newdata;
				_Myfirst    = _Newfirst;
				_Mycapacity = _Newcapacity;
			}

			_Mylast  = _Myfirst + _Newsize;
			_Myvsize = make_vector_size<_Ty, _Block>(_Myfirst, _Mylast);
		}

		_Alloc _Myalloc;
		_Block* _Mydata;
		size_t  _Mycapacity;
	};



	template<typename _Ty>
		constexpr bool is_vector_v = false;
	template<typename _Ty, size_t _Size, typename _Block>
		constexpr bool is_vector_v< vector<_Ty, _Size, _Block> > = true;
	template<typename _Ty, typename _Block>
		constexpr bool is_vector_v< vector_any<_Ty, _Block> > = true;

	template<typename _Ty, size_t _Size, typename _Block>
		constexpr bool is_support_scalar_operator< vector<_Ty, _Size, _Block> > = true;
	template<typename _Ty, typename _Block>
		constexpr bool is_support_scalar_operator< vector_any<_Ty, _Block> > = true;




	/*
	@_unit_vector<T>: as vector3<T>
	@_Traits: length = 1.0
	*/
	template<typename _Ty, size_t _Size, typename _Block = _Ty>
	struct unit_vector : public vector<_Ty, _Size, _Block> {
		using _Mybase     = vector<_Ty, _Size, _Block>;
		using scalar_type = _Ty;
		using vector_type = unit_vector<_Ty, _Size, _Block>;

		unit_vector() = default;

		unit_vector(_STD initializer_list<scalar_type> _Ilist, bool _Unitized = false)
			: _Mybase(_Ilist) {
			if (!_Unitized) this->normalize();
		}

		unit_vector(const _Mybase& _Vector, bool _Unitized = false)
			: _Mybase(_Unitized ? _Vector : normalize(_Vector)) {
			// 
		}

		vector_type operator-() const {
			return vector_type(std::negate<>(*this), true);
		}
	};

	template<typename _Ty = real_t, typename _Block = _Ty>
		using unit_vector3 = unit_vector<_Ty, 3, _Block>;








	/*template<typename _Ty, size_t _Size, typename _Vtag> inline
	auto distance(const vector<_Ty, _Size, _Vtag>& _A, const vector<_Ty, _Size, _Vtag>& _B) 
		-> decltype(length(_B - _A)) {
		return length(_B - _A);
	}*/

	//template<size_t N, typename T> inline
	//	T radians(_in(vector<N, T>) _A, _in(vector<N, T>) _B) 
	//	{	// return dot(A,B)÷|A|÷|B|
	//	return (dot(_A, _B) / _A.length() / _B.length());
	//	}

	//template<typename _Ty, size_t _Size, typename _Vtag> inline
	//unit_vectorN faceforward(const unit_vectorN& _Normal, const vectorN& _I, const vectorN& _Nref) {// dot(_I,_Nref) < 0 ? N : -N
	//	return (dot(_I, _Nref) < static_cast<_Ty>(0) ? _Normal : -_Normal);
	//}

	//template<typename _Ty, size_t _Size, typename _Vtag> inline
	//vectorN proj(const vectorN& _Vector, const vectorN& _Proj) {// return dot(V,||_Proj||) * |Proj| 
	//	return (dot(_Vector, _Proj) / dot(_Proj, _Proj) * _Proj);
	//	/*
	//		|          / |
	//		|_Vector/
	//		|    /	     |
	//		| /
	//		|-> result<--|▜___________
	//		| N |       N is n|malized
	//		| ->   _Proj   <- |

	//		proj_length = (N dot _Lhs)
	//		proj_length = (normalize(_Proj) dot _Lhs)
	//		proj = proj_length * normalize(_Proj)

	//		proj = (_Lhs dot normalize(_Proj)) * normalize(_Proj)
	//		proj = (_Lhs * _Proj / length(_Proj)) dot (_Proj / length(_Proj) * _Proj / length(_Proj))
	//		proj = (_Lhs * _Proj / length(_Proj)) dot (_Proj^2 /length(_Proj)^2)
	//		proj = (_Lhs dot _Proj) / dot(_Proj) * _Proj
	//	*/
	//}

	//template<typename _Ty, size_t _Size, typename _Vtag> inline
	//vectorN proj(const vectorN& _Vector, const unit_vectorN& _Proj) {// return dot(V,N)*N
	//	return (dot(_Vector, _Proj) * _Proj);
	//}

	//template<typename _Ty, typename _Vtag> inline
	//vector3<_Ty, _Vtag> proj(const vector3<_Ty, _Vtag>& _Vector, const unit_vector3<_Ty, _Vtag>& _Rt, 
	//	const unit_vector3<_Ty, _Vtag>& _Up, const unit_vector3<_Ty, _Vtag>& _Fwd) {// return [dot3(V,r), dot(V,u), dot(V,f)]
	//	return vector3<_Ty, _Vtag>{ dot(_Vector, _Rt), dot(_Vector, _Up), dot(_Vector, _Fwd) };
	//}

	//template<typename _Ty, size_t _Size, typename _Vtag> inline
	//vectorN reflect(const vectorN& _Direction, const unit_vectorN& _Normal) {
	//	return (static_cast<_Ty>(2) * proj(_Direction, _Normal) - _Direction);
	//	/*
	//		/|\
	//	   / 2.0f * proj(Dir, _Normal)
	//	  /  |  \
	//	 /   |
	//	/____|    \
	//	\    |    / Result
	//	Dir  |   /
	//	  \ Norm/
	//	   \ | /
	//	____\|/_________
	//	*/
	//}

	//template<typename T>
	//vector2<T> perp(_in(vector2<T>) _Lhs) {
	//	vector2<T>(-_Lhs[1], _Lhs[0]);
	//}

	//template<typename T>
	//void orthogonal(_inout(_STD vector<vector3<T>>) _Vectors) {
	//	for (size_t i = 1; i != _Vectors.size(); ++i) {
	//		for (size_t j = 0; j != i; ++j) {
	//			_Vectors[i] -= proj(_Vectors[i], _Vectors[j]);
	//		}
	//	}
	//}


	/*- - - - - - - - - - - - - - - - - - DIFINITION - - - - - - - - - - - - - - - -  - - - - - -*/
#define vectorN vector<_Ty, _Size, _Block>

	


#undef vectorN

}// namespace clmagic

#endif