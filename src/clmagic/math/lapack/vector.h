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
#include <stack>
#include <vector>
#include <string>
#include <algorithm>

namespace clmagic {

	/*
	Ruler:  |----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----| 
	Vector: |---------|--------------|--------------|--------------|--------------|---------|           
	       A0        A1														     B0	        B1
	                  |----------block_size---------------------------------------|       

	block_offset = A1 - A0 [bytes]
	leading_size = A1 - A0 [sizeof(scalar)]
	tail_size    = B1 - B0 [sizeof(scalar)]
	*/
	template<typename _Ty, typename _Block>
	struct vector_size {
		size_t leading_size;
		size_t block_size;
		size_t tail_size;

		static void initializer(vector_size& _Dest, const _Ty* _First, const _Ty* _Last) {
			if (_First != nullptr && _Last > _First) {
				const auto _Mysize  = _Last - _First;
				auto       _Mybkoff = reinterpret_cast<size_t>(_First) & alignment_mask_of_v<_Block>;
				if (_Mybkoff % sizeof(_Ty) == 0) {
					_Mybkoff /= sizeof(_Ty);
					const bool _Pass_alignment_point = (_Mybkoff + _Mysize >= block_traits<_Block>::size());
					if (_Pass_alignment_point) {
						_Dest.leading_size = block_traits<_Block>::size() - _Mybkoff;
						_Dest.block_size   = (_Mysize - _Dest.leading_size) / block_traits<_Block>::size();
						_Dest.tail_size    = (_Mysize - _Dest.leading_size) % block_traits<_Block>::size();
					}
				}
			}
		}

		vector_size() : leading_size(0), block_size(0), tail_size(0) {}

		vector_size(const _Ty* _First, const _Ty* _Last) : vector_size() {
			initializer(*this, _First, _Last);
		}

		vector_size(const _Ty* _First, const _Ty* _Last, size_t _Stride) : vector_size() {
			if (_Stride == 1) {
				initializer(*this, _First, _Last);
			}
		}

		bool empty() const {
			return (leading_size == 0 && block_size == 0);
		}

		bool operator==(const vector_size& _Right) const {
			return this->block_size == _Right.block_size &&
				 this->leading_size == _Right.leading_size &&
				    this->tail_size == _Right.tail_size;
		}

		bool operator!=(const vector_size& _Right) const {
			return !(*this == _Right);
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

			int _Initial = _Leading_size != 0 ? 1 :
				           _First != _Last ? 2 :
						    3;

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
				if (_Initial == 2) {// initial 
					_Result = *_First_bk++; 
				}
				for (; _First_bk != _Last_bk; ++_First_bk) {
					_Result = _Func2(_Result, *_First_bk);
				}
			}

			if (_Tail_size != 0) {
				scalar_const_pointer       _First_bk = reinterpret_cast<scalar_const_pointer>(_First);
				const scalar_const_pointer _Last_bk  = _First_bk + _Tail_size;
				if (_Initial == 3) {// initial 
					_Result = *_First_bk++;
				}
				for ( ; _First_bk != _Last_bk; ++_First_bk) {
					_Result = _Func2(_Result, *_First_bk);
				}
			}

			return _Result;
		}

		// sum = comulate(_First, _Last, std::plus<block_type>(), std::plus<scalar_type>(), ...)
		// product = comulate(_First, _Last, std::multiplies<block_type>(), std::multiplies<scalar_type>(), ...)
		static scalar_type norm(block_const_pointer _First, block_const_pointer _Last, scalar_type _Level, size_t _Leading_size, size_t _Tail_size) {
			const auto _Level_block = block_traits::set1(_Level);
			const auto _Invlevel    = static_cast<scalar_type>(1) / _Level;

			const auto _Func1 = [&_Level_block](block_const_reference _Res, block_const_reference _Elm) {
				return _Res + pow(abs(_Elm), _Level_block); };

			const auto _Func2 = [&_Level](scalar_const_reference _Res, scalar_const_reference _Elm) {
				return _Res + pow(abs(_Elm), _Level); };

			return pow( comulate(_First, _Last, _Func1, _Func2, _Leading_size, _Tail_size), _Invlevel );
		}

		template<typename _Fn1, typename _Fn2, typename _Fn3>
		static scalar_type comulate(block_const_pointer _First1, block_const_pointer _Last1, block_const_pointer _First2, 
			_Fn1/*block*/ _Func1, _Fn2/*block*/ _Func2, _Fn3/*scalar*/ _Func3, 
			size_t _Leading_size, size_t _Tail_size) {
			// _Func2(_Result, _Func1(_First1, _First2)), _Func3(_Result, _Result_block_elements)
			scalar_type _Result;

			int _Initial = _Leading_size != 0 ? 1 :
				           _First1 != _Last1 ? 2 : 
						    3;

			if (_Initial == 1) {
				const block_type           _Tmp_bk   = _Func1(*(_First1 - 1), *(_First2 - 1));
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
				if (_Initial == 2) {// initial 
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
				if (_Initial == 3) {// initial
					_Result = *_First_bk++;
				}
				for ( ; _First_bk != _Last_bk; ++_First_bk) {
					_Result = _Func3(_Result, *_First_bk);
				}
			}

			return _Result;
		}
		
		// dot = comulate(_First1, _Last1, _First2, std::multiplies<block_type>(), std::plus<block_type>(), std::plus<scalar_type>(), ...)
	};


	template<typename Iter>
	std::string _To_string(Iter _First, Iter _Last) {
		using std::to_string;
		std::string _Str = "[";
		for (; _First != _Last; ++_First) {
			_Str += to_string(*_First);
			_Str += ' ';
		}
		_Str.back() = ']';
		return _Str;
	}

	template<typename _Ty, typename _Block>
	struct stack_allocator : std::vector<_Block> {
		using _Mybase    = std::vector<_Block>;
		using _This_type = stack_allocator<_Ty, _Block>;

		static _This_type& data() {
			static _This_type _Static_stack;
			return _Static_stack;
		}

		static size_t calc_block_count(size_t _Count/*scalar*/) {
			return ceil(_Count, block_traits<_Block>::size()) / block_traits<_Block>::size();
		/*	const bool _Divide = (_Count % block_traits<_Block>::size()) != 0;
			return _Count / block_traits<_Block>::size() + static_cast<size_t>(_Divide);*/
		}

		static _Ty* allocate(size_t _Count/*scalar*/) {
			return reinterpret_cast<_Ty*>(data().seek_n(calc_block_count(_Count)));
		}

		static void deallocate(size_t _Count/*scalar*/) {
			data()._Myseek -= calc_block_count(_Count);
		}

		static void push(size_t/*block*/ _Count) {
			auto& _Stack = data();
			assert(_Stack._Myseek == 0);
			const auto _Newsize = _Count + _Count / 2;
			_Stack.resize(_Newsize);
		}
		
		static void pop() {
			auto& _Stack = data();
			assert(_Stack._Myseek == 0);
			_Stack.clear();
			_Stack.shrink_to_fit();
		}

		_Block& top() {
			return ((*this)[_Myseek]);
		}

		_Block* seek_n(size_t/*block*/ _Count) {
			const auto _Newseek = _Myseek + _Count;
			assert(_Newseek <= _Mybase::size());

			auto _Ptr = &this->top();
			_Myseek = _Newseek;
			return _Ptr;
		}

		size_t _Myseek;
	};

	template<typename _Ty, typename _Block>
		struct const_subvector;
	template<typename _Ty, typename _Block>
		struct subvector;

	template<typename _Ty, typename _Block>
	struct const_subvector {// const _Ty
		using iterator_category = std::random_access_iterator_tag;
		using difference_type   = ptrdiff_t;

		using block_type       = const _Block;
		using block_pointer    = block_type*;
		using scalar_type      = const _Ty;
		using scalar_pointer   = scalar_type*;
		using scalar_reference = scalar_type&;

		using vector_size = clmagic::vector_size<_Ty, _Block>;
		
		using block_traits     = clmagic::block_traits<_Block>;
		using allocator_traits = clmagic::stack_allocator<_Ty, _Block>;

		scalar_pointer begin() const {
			return _Fptr; }
		scalar_pointer end() const {
			return _Lptr; }
		size_t size() const {
			return (_Lptr - _Fptr) / _Nx; }
		size_t stride() const {
			return _Nx; }
		const vector_size& vsize() const {
			return _Vsize; }
		scalar_pointer ptr(size_t _Pos = 0) const {
			return _Fptr + _Pos * _Nx; }
		scalar_reference operator[](size_t _Pos) const {
			return (*ptr(_Pos)); }

		const_subvector(scalar_pointer _First, scalar_pointer _Last)
			: _Fptr(_First), _Lptr(_Last), _Nx(1), _Vsize(_First, _Last), _Mem(false) {}
		const_subvector(scalar_pointer _First, scalar_pointer _Last, size_t _Stride)// &_First[1] = &_First[0] + _Stride
			: _Fptr(_First), _Lptr(_Last), _Nx(_Stride), _Vsize(_First, _Last, _Stride), _Mem(false) {}
		const_subvector(const_subvector&& _Right) noexcept
			: _Fptr(_Right._Fptr), _Lptr(_Right._Lptr), _Nx(_Right._Nx), _Vsize(_Right._Vsize), _Mem(_Right._Mem) {
			_Right._Mem = false;
		}
		~const_subvector() {
			if (_Mem) {
				allocator_traits::deallocate(this->size());
			}
		}

		const_subvector& operator=(const const_subvector& _Right) = delete;
		const_subvector& operator=(const_subvector&& _Right) {
			this->_Fptr  = _Right._Fptr;
			this->_Lptr  = _Right._Lptr;
			this->_Nx    = _Right._Nx;
			this->_Vsize = _Right._Vsize;
			this->_Mem   = _Right._Mem;
			_Right._Mem  = false;
			return (*this);
		}

		const_subvector operator()(size_t _First1, size_t _Last1) const {
			return const_subvector(this->ptr(_First1), this->ptr(_Last1), this->stride());
		}

		template<typename _Fn>
		static subvector<_Ty, _Block> _Transform_uncheck(const const_subvector& _Left, const const_subvector& _Right, _Fn _Func);
		template<typename _Fn>
		static subvector<_Ty, _Block> _Transform_uncheck(const const_subvector& _Left, const _Ty& _Scalar, _Fn _Func);
		template<typename _Fn>
		static subvector<_Ty, _Block> _Transform_uncheck(const _Ty& _Scalar, const const_subvector& _Right, _Fn _Func);

		subvector<_Ty, _Block> operator+(const const_subvector& _Right) const {
			assert(!this->empty());
			assert(this->size() == _Right.size());
			return std::move(_Transform_uncheck(*this, _Right, std::plus<>()));
		}
		subvector<_Ty, _Block> operator-(const const_subvector& _Right) const {
			assert(!this->empty());
			assert(this->size() == _Right.size());
			return std::move(_Transform_uncheck(*this, _Right, std::minus<>()));
		}
		subvector<_Ty, _Block> operator*(const const_subvector& _Right) const {
			assert(!this->empty());
			assert(this->size() == _Right.size());
			return std::move(_Transform_uncheck(*this, _Right, std::multiplies<>()));
		}
		subvector<_Ty, _Block> operator/(const const_subvector& _Right) const {
			assert(!this->empty());
			assert(this->size() == _Right.size());
			return std::move(_Transform_uncheck(*this, _Right, std::divides<>()));
		}
		subvector<_Ty, _Block> operator+(const _Ty& _Scalar) const {
			assert(!this->empty());
			return std::move(_Transform_uncheck(*this, _Scalar, std::plus<>()));
		}
		subvector<_Ty, _Block> operator-(const _Ty& _Scalar) const {
			assert(!this->empty());
			return std::move(_Transform_uncheck(*this, _Scalar, std::minus<>()));
		}
		subvector<_Ty, _Block> operator*(const _Ty& _Scalar) const {
			assert(!this->empty());
			return std::move(_Transform_uncheck(*this, _Scalar, std::multiplies<>()));
		}
		subvector<_Ty, _Block> operator/(const _Ty& _Scalar) const {
			assert(!this->empty());
			return std::move(_Transform_uncheck(*this, _Scalar, std::divides<>()));
		}
		friend subvector<_Ty, _Block> operator+(const _Ty& _Scalar, const const_subvector& _Right) {
			assert(!_Right.empty());
			return std::move(_Transform_uncheck(_Scalar, _Right, std::plus<>()));
		}
		friend subvector<_Ty, _Block> operator-(const _Ty& _Scalar, const const_subvector& _Right) {
			assert(!_Right.empty());
			return std::move(_Transform_uncheck(_Scalar, _Right, std::minus<>()));
		}
		friend subvector<_Ty, _Block> operator*(const _Ty& _Scalar, const const_subvector& _Right) {
			assert(!_Right.empty());
			return std::move(_Transform_uncheck(_Scalar, _Right, std::multiplies<>()));
		}
		friend subvector<_Ty, _Block> operator/(const _Ty& _Scalar, const const_subvector& _Right) {
			assert(!_Right.empty());
			return std::move(_Transform_uncheck(_Scalar, _Right, std::divides<>()));
		}

		void reset(scalar_pointer _First, scalar_pointer _Last, size_t _Stride = 1) {
			_Fptr  = _First;
			_Lptr  = _Last;
			_Nx    = _Stride;
			_Vsize = vector_size(_First, _Last, _Stride);
			_Mem   = false;
		}

		size_t block_offset() const {// first block offset bytes
			return reinterpret_cast<uintptr_t>(_Fptr) & alignment_mask_of_v<_Block>;
		}

		bool empty() const {
			return (_Lptr == _Fptr);
		}

		bool aligned() const {
			return block_offset() == 0;
		}

		bool pass_alignment_point() const {// is pass alignment point
			return (block_offset() / sizeof(scalar_type) + size() >= block_traits::size());
		}

		bool is_continue() const {
			return _Nx == 1;
		}

		friend std::string to_string(const const_subvector& _Left) {
			using std::to_string;
			const auto  _Inc   = _Left.stride();
			auto        _First = _Left.begin();
			const auto  _Last  = _Left.end();
			std::string _Str   = "[";
			for (; _First != _Last; _First += _Inc) {
				_Str += to_string(*_First);
				_Str += ' ';
			}
			_Str.back() = ']';
			return _Str;
		}
		friend std::ostream& operator<<(std::ostream& _Ostr, const const_subvector& _Left) {
			return (_Ostr << to_string(_Left));
		}

	protected:
		const_subvector(scalar_pointer _First, scalar_pointer _Last, size_t _Stride, bool _Memory)
			: _Fptr(_First), _Lptr(_Last), _Nx(_Stride), _Vsize(_Fptr, _Lptr), _Mem(_Memory) { }

	private:
		scalar_pointer _Fptr;
		scalar_pointer _Lptr;
		size_t         _Nx;
		vector_size    _Vsize;
		bool           _Mem;
	};

	/*
	@_Note: Don't manage memory
	*/
	template<typename _Ty, typename _Block>
	struct subvector : public const_subvector<_Ty, _Block> {// default const _Ty
		using _Mybase = const_subvector<_Ty, _Block>;
		friend _Mybase;

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
		subvector(scalar_pointer _First, scalar_pointer _Last, size_t _Stride)
			: _Mybase(_First, _Last, _Stride) {}
		subvector(subvector<_Ty, _Block>&& _Right) noexcept// owner move to this
			: _Mybase(std::move(_Right)) {}

		// full-clone
		subvector& operator=(const const_subvector<_Ty, _Block>& _Right) {
			auto       _First = _Right.begin();
			const auto _Last  = _Right.end();
			auto       _Dest  = this->begin();
			const auto _Inc1  = _Right.stride();
			const auto _Inc2  = this->stride();
			for ( ; _First != _Last; _First += _Inc1, _Dest += _Inc2) {
				*_Dest = *_First;
			}
			return *this;
		}
		subvector& operator=(const subvector<_Ty, _Block>& _Right) { 
			return ( (*this) = static_cast<const const_subvector<_Ty, _Block>&>(_Right) );
		}
		/*subvector& operator=(const_subvector<_Ty, _Block>&& _Right) noexcept {
			return ((*this) = static_cast<const_subvector<_Ty, _Block>&>(_Right));
		}*/
		subvector& operator=(subvector<_Ty, _Block>&& _Right) noexcept {
			return ((*this) = static_cast<subvector<_Ty, _Block>&>(_Right));
		}

		subvector operator()(size_t _First, size_t _Last) const {
			return subvector(this->ptr(_First), this->ptr(_Last), this->stride());
		}

		subvector& operator+=(const const_subvector<_Ty, _Block>& _Right);
		subvector& operator-=(const const_subvector<_Ty, _Block>& _Right);
		subvector& operator*=(const const_subvector<_Ty, _Block>& _Right);
		subvector& operator/=(const const_subvector<_Ty, _Block>& _Right);
		subvector& operator+=(const scalar_type& _Right);
		subvector& operator-=(const scalar_type& _Right);
		subvector& operator*=(const scalar_type& _Right);
		subvector& operator/=(const scalar_type& _Right);

		scalar_pointer ptr(size_t _Pos = 0) const {
			return  const_cast<scalar_pointer>(_Mybase::ptr(_Pos));
		}
		scalar_pointer begin() const {
			return const_cast<scalar_pointer>(_Mybase::begin()); 
		}
		scalar_pointer end() const {
			return const_cast<scalar_pointer>(_Mybase::end()); 
		}
		scalar_reference operator[](size_t _Pos) const {
			return const_cast<scalar_reference>(_Mybase::operator[](_Pos)); 
		}
		
		void reset(scalar_pointer _First_arg, scalar_pointer _Last_arg, size_t _Stride_arg = 1) {
			_Mybase::reset(_First_arg, _Last_arg, _Stride_arg);
		}

		void fill(scalar_const_reference _Val) {
			auto       _Dest = this->begin();
			const auto _Last = this->end();
			for (; _Dest != _Last; ++_Dest) {
				*_Dest = _Val;
			}
		}

		template<typename _Iter> 
		void assign(_Iter _First, _Iter _Last) {
			auto _Dest = this->begin();
			for (; _First != _Last; ++_First, ++_Dest) {
				*_Dest = *_First;
			}
		}
	
		struct refernece {
			scalar_pointer _Myfirst;
			scalar_pointer _Mylast;
			size_t         _Mystride;
			vector_size<_Ty, _Block> _Mysize;
		};
	
		refernece& subvector_ref() {
			return reinterpret_cast<refernece&>(*this);
		}

	protected:
		subvector(scalar_pointer _First, scalar_pointer _Last, size_t _Stride, bool _Memory)
			: _Mybase(_First, _Last, _Stride, _Memory) {}
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

		using subvector       = clmagic::subvector<_Ty, _Block>;
		using const_subvector = clmagic::const_subvector<_Ty, _Block>;

		using block_traits           = clmagic::block_traits<_Block>;
		using block_accelerate       = clmagic::_Accelerate_block<_Ty, _Block>;
		using vector_size            = clmagic::vector_size<_Ty, _Block>;

		template<typename _Fn1, typename _Fn2>
		static void transform(const const_subvector& _Left, const subvector& _Result,
			/*block*/_Fn1 _Func1, /*scalar*/_Fn2 _Func2) {// _Result = _Func(_Left)
			if (_Result.is_continue() && !_Left.vsize().empty()) {
				const auto& _Vsize = _Left.vsize();
				auto        _First = reinterpret_cast<block_const_pointer>(_Left.begin() + _Vsize.leading_size);
				const auto  _Last  = _First + _Vsize.block_size;
				auto        _Dest  = reinterpret_cast<block_pointer>(_Result.begin() + _Vsize.leading_size);
				block_accelerate::transform(_First, _Last, _Dest, _Func1, 
					_Vsize.leading_size, _Vsize.tail_size);
			} else {// else scalar-operation
				auto       _First = _Left.begin();
				const auto _Last  = _Left.end();
				auto       _Dest  = _Result.begin();
				const auto _Inc1  = _Left.stride();
				const auto _Inc2  = _Result.stride();
				for ( ; _First != _Last; _First += _Inc1, _Dest += _Inc2) {
					*_Dest = _Func2(*_First);
				}
			}
		}

		template<typename _Fn1, typename _Fn2>
		static void transform(const const_subvector& _Left, const const_subvector& _Right, const subvector& _Result,
			/*block*/_Fn1 _Func1, /*scalar*/_Fn2 _Func2) {// _Result = _Func(_Left, _Right)
			const bool _Accelerate = (_Result.is_continue() &&
				!_Left.vsize().empty() && 
				_Left.vsize() == _Right.vsize() );
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
				const auto _Inc1   = _Left.stride();
				const auto _Inc2   = _Right.stride();
				const auto _Inc3   = _Result.stride();
				for ( ; _First1 != _Last1; _First1 += _Inc1, _First2 += _Inc2, _Dest += _Inc3) {
					*_Dest = _Func2(*_First1, *_First2);
				}
			}
		}
	
		template<typename _Fn1, typename _Fn2> inline
		static void transform_right_scalar(const const_subvector& _Left, const subvector& _Result,
			/*block*/_Fn1 _Func1, /*scalar*/_Fn2 _Func2, scalar_const_reference _Scalar) {
			if (_Result.is_continue() && !_Left.vsize().empty()) {
				const auto  B     = block_traits::set1(_Scalar);
				const auto _Func3 = [_Func1, &B](auto&& A) { return _Func1(A, B); };

				const auto& _Vsize = _Left.vsize();
				auto        _First = reinterpret_cast<block_const_pointer>(_Left.begin() + _Vsize.leading_size);
				const auto  _Last  = _First + _Vsize.block_size;
				auto        _Dest  = reinterpret_cast<block_pointer>(_Result.begin() + _Vsize.leading_size);
				block_accelerate::transform(_First, _Last, _Dest, _Func3,
					_Vsize.leading_size, _Vsize.tail_size);
			} else {// else scalar-operation
				auto       _First = _Left.begin();
				const auto _Last  = _Left.end();
				auto       _Dest  = _Result.begin();
				const auto _Inc1  = _Left.stride();
				const auto _Inc2  = _Result.stride();
				for ( ; _First != _Last; _First += _Inc1, _Dest += _Inc2) {
					*_Dest = _Func2(*_First, _Scalar);
				}
			}
		}

		template<typename _Fn1, typename _Fn2> inline
		static void transform_left_scalar(const const_subvector& _Right, const subvector& _Result,
			/*block*/_Fn1 _Func1, /*scalar*/_Fn2 _Func2, scalar_const_reference _Scalar) {
			if (_Result.is_continue() && !_Right.vsize().empty()) {
				const auto  A     = block_traits::set1(_Scalar);
				const auto _Func3 = [_Func1, &A](auto&& B) { return _Func1(A, B); };

				const auto& _Vsize = _Right.vsize();
				auto        _First = reinterpret_cast<block_const_pointer>(_Right.begin() + _Vsize.leading_size);
				const auto  _Last  = _First + _Vsize.block_size;
				auto        _Dest  = reinterpret_cast<block_pointer>(_Result.begin() + _Vsize.leading_size);
				block_accelerate::transform(_First, _Last, _Dest, _Func3,
					_Vsize.leading_size, _Vsize.tail_size);
			} else {// else scalar-operation
				auto       _First = _Right.begin();
				const auto _Last  = _Right.end();
				auto       _Dest  = _Result.begin();
				const auto _Inc1  = _Right.stride();
				const auto _Inc2  = _Result.stride();
				for ( ; _First != _Last; _First += _Inc1, _Dest += _Inc2) {
					*_Dest = _Func2(*_First, _Scalar);
				}
			}
		}
	
		template<typename _Fn1, typename _Fn2> inline
		static scalar_type comulate(const const_subvector& _Left, _Fn1 _Func1, _Fn2 _Func2) {
			if ( !_Left.vsize().empty() ) {
				const auto& _Vsize = _Left.vsize();
				auto        _First = reinterpret_cast<block_const_pointer>(_Left.begin() + _Vsize.leading_size);
				const auto  _Last  = _First + _Vsize.block_size;
				return block_accelerate::comulate(_First, _Last, _Func1, _Func2, 
					_Vsize.leading_size, _Vsize.tail_size);
			} else {
				auto        _First  = _Left.begin();
				const auto  _Last   = _Left.end();
				const auto  _Inc    = _Left.stride();
				scalar_type _Result = *_First; _First += _Inc;
				for (; _First != _Last; _First += _Inc) {
					_Result = _Func2(_Result, *_First);
				}
				return _Result;
			}
		}
	
		static scalar_type norm(const const_subvector& _Left, scalar_type _Level)  {
			const auto _Level_block = block_traits::set1(_Level);
			const auto _Invlevel    = static_cast<scalar_type>(1) / _Level;

			const auto _Func1 = [&_Level_block](auto&& _Res, auto&& _Elm) {
				return _Res + pow(abs(_Elm), _Level_block); };

			const auto _Func2 = [&_Level](auto&& _Res, auto&& _Elm) {
				return _Res + pow(abs(_Elm), _Level); };

			return pow( comulate(_Left, _Func1, _Func2), _Invlevel );
		}

		template<typename _Fn1, typename _Fn2, typename _Fn3, typename _Fn4> inline
		static scalar_type comulate(const const_subvector& _Left, const const_subvector& _Right, _Fn1 _Func1, _Fn2 _Func2, _Fn3 _Func3, _Fn4 _Func4) {
			const bool _Accelerate = !_Left.vsize().empty() && 
				_Left.vsize() == _Right.vsize();

			if (_Accelerate) {
				const auto& _Vsize  = _Left.vsize();
				auto        _First1 = reinterpret_cast<block_const_pointer>(_Left.begin() + _Vsize.leading_size);
				const auto  _Last1  = _First1 + _Vsize.block_size;
				auto        _First2 = reinterpret_cast<block_const_pointer>(_Right.begin() + _Vsize.leading_size);
				return block_accelerate::comulate(_First1, _Last1, _First2, _Func1, _Func3, _Func4, 
					_Vsize.leading_size, _Vsize.tail_size);
			} else {
				auto        _First1 = _Left.begin();
				const auto  _Last1  = _Left.end();
				auto        _First2 = _Right.begin();
				const auto  _Inc1   = _Left.stride();
				const auto  _Inc2   = _Right.stride();
				scalar_type _Result = _Func2(*_First1, *_First2); _First1 += _Inc1; _First2 += _Inc2;
				for (; _First1 != _Last1; _First1 += _Inc1, _First2 += _Inc2) {
					_Result = _Func4(_Result, _Func2(*_First1, *_First2));
				}
				return _Result;
			}
		}
	};


	template<typename _Ty, typename _Block> template<typename _Fn> inline
	subvector<_Ty, _Block> const_subvector<_Ty, _Block>::_Transform_uncheck(const const_subvector& _Left, const const_subvector& _Right, _Fn _Func) {
		auto _Memory = allocator_traits::allocate(_Left.size());
		auto _Result = subvector<_Ty, _Block>(_Memory, _Memory + _Left.size(), 1, true);
		_Accelerate_subvector<_Ty, _Block>::transform(_Left, _Right, _Result, _Func, _Func);
		return std::move(_Result);
	}
	template<typename _Ty, typename _Block> template<typename _Fn> inline
	subvector<_Ty, _Block> const_subvector<_Ty, _Block>::_Transform_uncheck(const const_subvector& _Left, const _Ty& _Scalar, _Fn _Func) {
		auto _Memory = allocator_traits::allocate(_Left.size());
		auto _Result = subvector<_Ty, _Block>(_Memory, _Memory + _Left.size(), 1, true);
		_Accelerate_subvector<_Ty, _Block>::transform_right_scalar(_Left, _Result, _Func, _Func, _Scalar);
		return std::move(_Result);
	}
	template<typename _Ty, typename _Block> template<typename _Fn> inline
	subvector<_Ty, _Block> const_subvector<_Ty, _Block>::_Transform_uncheck(const _Ty& _Scalar, const const_subvector& _Right, _Fn _Func) {
		auto _Memory = allocator_traits::allocate(_Right.size());
		auto _Result = subvector<_Ty, _Block>(_Memory, _Memory + _Right.size(), 1, true);
		_Accelerate_subvector<_Ty, _Block>::transform_left_scalar(_Right, _Result, _Func, _Func, _Scalar);
		return std::move(_Result);
	}

	template<typename _Ty, typename _Block> inline
	subvector<_Ty, _Block>& subvector<_Ty, _Block>::operator+=(const const_subvector<_Ty, _Block>& _Right) {
		_Accelerate_subvector<_Ty, _Block>::transform(*this, _Right, *this, std::plus<>(), std::plus<>());
		return *this;
	}
	template<typename _Ty, typename _Block> inline
	subvector<_Ty, _Block>& subvector<_Ty, _Block>::operator-=(const const_subvector<_Ty, _Block>& _Right) {
		_Accelerate_subvector<_Ty, _Block>::transform(*this, _Right, *this, std::minus<>(), std::minus<>());
		return *this;
	}
	template<typename _Ty, typename _Block> inline
	subvector<_Ty, _Block>& subvector<_Ty, _Block>::operator*=(const const_subvector<_Ty, _Block>& _Right) {
		_Accelerate_subvector<_Ty, _Block>::transform(*this, _Right, *this, std::multiplies<>(), std::multiplies<>());
		return *this;
	}
	template<typename _Ty, typename _Block> inline
	subvector<_Ty, _Block>& subvector<_Ty, _Block>::operator/=(const const_subvector<_Ty, _Block>& _Right) {
		_Accelerate_subvector<_Ty, _Block>::transform(*this, _Right, *this, std::divides<>(), std::divides<>());
		return *this;
	}
	template<typename _Ty, typename _Block> inline
	subvector<_Ty, _Block>& subvector<_Ty, _Block>::operator+=(const scalar_type& _Scalar) {
		_Accelerate_subvector<_Ty, _Block>::transform_right_scalar(*this, *this, std::plus<>(), std::plus<>(), _Scalar);
		return *this;
	}
	template<typename _Ty, typename _Block> inline
	subvector<_Ty, _Block>& subvector<_Ty, _Block>::operator-=(const scalar_type& _Scalar) {
		_Accelerate_subvector<_Ty, _Block>::transform_right_scalar(*this, *this, std::minus<>(), std::minus<>(), _Scalar);
		return *this;
	}
	template<typename _Ty, typename _Block> inline
	subvector<_Ty, _Block>& subvector<_Ty, _Block>::operator*=(const scalar_type& _Scalar) {
		_Accelerate_subvector<_Ty, _Block>::transform_right_scalar(*this, *this, std::multiplies<>(), std::multiplies<>(), _Scalar);
		return *this;
	}
	template<typename _Ty, typename _Block> inline
	subvector<_Ty, _Block>& subvector<_Ty, _Block>::operator/=(const scalar_type& _Scalar) {
		_Accelerate_subvector<_Ty, _Block>::transform_right_scalar(*this, *this, std::divides<>(), std::divides<>(), _Scalar);
		return *this;
	}


	// vector OP vector
	template<typename _Ty, typename _Block> inline
	void mod(const_subvector<_Ty, _Block> _Left, const_subvector<_Ty, _Block> _Right, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform(_Left, _Right, _Result, std::modulus<_Block>(), std::modulus<_Ty>());
	}
	template<typename _Ty, typename _Block> inline
	void pow(const_subvector<_Ty, _Block> _Left, const_subvector<_Ty, _Block> _Right, subvector<_Ty, _Block> _Result) {
		const auto _Func = [](auto&& A, auto&& B) { return pow(A, B); };
		_Accelerate_subvector<_Ty, _Block>::transform(_Left, _Right, _Result, _Func, _Func);
	}
	template<typename _Ty, typename _Block> inline
	_Ty dot(const_subvector<_Ty, _Block> _Left, const_subvector<_Ty, _Block> _Right) {
		return _Accelerate_subvector<_Ty, _Block>::comulate(_Left, _Right, 
			std::multiplies<_Block>(), std::multiplies<_Ty>(), std::plus<_Block>(), std::plus<_Ty>());
	}

	// vector OP scalar
	template<typename _Ty, typename _Block> inline
	void mod(const_subvector<_Ty, _Block> _Left, const _Ty& _Scalar, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform_right_scalar(_Left, _Result, std::modulus<_Block>(), std::modulus<_Ty>(), _Scalar);
	}
	template<typename _Ty, typename _Block> inline
	void pow(const_subvector<_Ty, _Block> _Left, const _Ty& _Scalar, subvector<_Ty, _Block> _Result) {
		const auto _Func = [](auto&& A, auto&& B) { return pow(A, B); };
		_Accelerate_subvector<_Ty, _Block>::transform_right_scalar(_Left, _Result, _Func, _Func, _Scalar);
	}

	// scalar OP vector
	template<typename _Ty, typename _Block> inline
	void mod(const _Ty& _Scalar, const_subvector<_Ty, _Block> _Right, subvector<_Ty, _Block> _Result) {
		_Accelerate_subvector<_Ty, _Block>::transform_left_scalar(_Right, _Result, std::modulus<_Block>(), std::modulus<_Ty>(), _Scalar);
	}

	template<typename _Ty, typename _Block> inline
	_Ty norm(const_subvector<_Ty, _Block> _X, const _Ty& _Level) {
		return _Accelerate_subvector<_Ty, _Block>::norm(_X, _Level);
	}
	template<typename _Ty, typename _Block> inline
	_Ty normL2_square(const_subvector<_Ty, _Block> _X) {
		return dot(_X, _X);
	}
	template<typename _Ty, typename _Block> inline
	void normalize(const const_subvector<_Ty, _Block>& _Source, subvector<_Ty, _Block>& _Dest) {
		const auto _NormL2sq = normL2_square(_Source);
		if ( !approach_equal(_NormL2sq, static_cast<_Ty>(1), std::numeric_limits<_Ty>::epsilon()) ) {
			div(_Source, sqrt(_NormL2sq), _Dest);
		} else {
			std::copy(_Source.begin(), _Source.end(), _Dest.begin());
		}
	}

	

	enum Coordinates {
		_LH_,
		_RH_
	};

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
		template<typename _Iter>
		void assign(_Iter _First, _Iter _Last) {
			assert( std::distance(_First, _Last) <= std::_Iter_diff_t<_Iter>(this->size()) );
			auto _Dest = std::copy(_First, _Last, this->begin());
			std::fill(_Dest, this->end(), static_cast<scalar_type>(0));
		}
		void assign(const scalar_type& _Val) {
			std::fill(this->begin(), this->end(), _Val); 
		}
		void assign(std::initializer_list<scalar_type> _Ilist) {
			assert(_Ilist.size() <= this->size());
			auto _Dest = std::copy(_Ilist.begin(), _Ilist.end(), this->begin());
			std::fill(_Dest, this->end(), static_cast<scalar_type>(0));
		}

		template<typename _Fn>
		vector<bool, _Size>&& compare(const vector& _Right, _Fn _Pred) const {
			vector<bool, _Size> _Result;
			std::transform(this->begin(), this->end(), _Right.begin(), _Result.begin(), _Pred);
			return std::move(_Result);
		}

		template<typename _Fn>
		vector<bool, _Size>&& compare(const _Ty& _Scalar, _Fn _Pred) const {
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
		template<typename _Iter> 
		vector(_Iter _First, _Iter _Last) { this->assign(_First, _Last); }
		constexpr vector() : _Mydata{ 0 } {}
		explicit vector(const scalar_type& _Val) { this->assign(_Val); }
		vector(std::initializer_list<scalar_type> _Ilist) { this->assign(_Ilist); }
		
		vector& operator=(std::initializer_list<scalar_type> _Ilist) {
			this->assign(_Ilist);
			return *this;
		}

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
		vector<bool, _Size> operator< (const vector& _Right) const {
			return this->compare(_Right, std::less<scalar_type>());
		}
		vector<bool, _Size> operator> (const vector& _Right) const {
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
		vector<bool, _Size> operator< (const scalar_type& _Scalar) const {
			return this->compare(_Scalar, std::less<scalar_type>());
		}
		vector<bool, _Size> operator> (const scalar_type& _Scalar) const {
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
			return vector(*this, block_traits::set1(_Scalar), std::plus<block_type>());
		}
		vector operator-(const scalar_type& _Scalar) const {
			return vector(*this, block_traits::set1(_Scalar), std::minus<block_type>());
		}
		vector operator*(const scalar_type& _Scalar) const {
			return vector(*this, block_traits::set1(_Scalar), std::multiplies<block_type>());
		}
		vector operator/(const scalar_type& _Scalar) const {
			return vector(*this, block_traits::set1(_Scalar), std::divides<block_type>());
		}
		vector operator%(const scalar_type& _Scalar) const {
			return vector(*this, block_traits::set1(_Scalar), std::modulus<block_type>());
		}
		vector& operator+=(const vector& _Right) {
			_Assign(*this, _Right, std::plus<block_type>()); 
			return (*this); 
		}
		vector& operator-=(const vector& _Right) {
			_Assign(*this, _Right, std::minus<block_type>());
			return (*this); 
		}
		vector& operator*=(const vector& _Right) {
			_Assign(*this, _Right, std::multiplies<block_type>());
			return (*this);
		}
		vector& operator/=(const vector& _Right) {
			_Assign(*this, _Right, std::divides<block_type>());
			return (*this); 
		}
		vector& operator%=(const vector& _Right) {
			_Assign(*this, _Right, std::modulus<block_type>());
			return (*this); 
		}
		vector& operator+=(const scalar_type& _Scalar) {
			_Assign(*this, block_traits::set1(_Scalar), std::plus<block_type>());
			return (*this); 
		}
		vector& operator-=(const scalar_type& _Scalar) {
			_Assign(*this, block_traits::set1(_Scalar), std::minus<block_type>());
			return (*this);
		}
		vector& operator*=(const scalar_type& _Scalar) {
			_Assign(*this, block_traits::set1(_Scalar), std::multiplies<block_type>());
			return (*this);
		}
		vector& operator/=(const scalar_type& _Scalar) {
			_Assign(*this, block_traits::set1(_Scalar), std::divides<block_type>());
			return (*this);
		}
		vector& operator%=(const scalar_type& _Scalar) {
			_Assign(*this, block_traits::set1(_Scalar), std::modulus<block_type>());
			return (*this); 
		}

		friend vector operator+(const scalar_type& _Scalar, const vector& _Right) {
			return vector(block_traits::set1(_Scalar), _Right, std::plus<block_type>());
		}
		friend vector operator-(const scalar_type& _Scalar, const vector& _Right) {
			return vector(block_traits::set1(_Scalar), _Right, std::minus<block_type>());
		}
		friend vector operator*(const scalar_type& _Scalar, const vector& _Right) {
			return vector(block_traits::set1(_Scalar), _Right, std::multiplies<block_type>());
		}
		friend vector operator/(const scalar_type& _Scalar, const vector& _Right) {
			return vector(block_traits::set1(_Scalar), _Right, std::divides<block_type>());
		}
		friend vector operator%(const scalar_type& _Scalar, const vector& _Right) {
			return vector(block_traits::set1(_Scalar), _Right, std::modulus<block_type>());
		}

		vector& normalize() {
			const auto _NormL2sq = normL2_square(*this);
			if (!approach_equal(_NormL2sq, static_cast<scalar_type>(1), std::numeric_limits<scalar_type>::epsilon())) {
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

	protected:
		template<typename _Fn>
		void _Assign(const vector& _Left, _Fn _Func) {
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
		void _Assign(const vector& _Left, const vector& _Right, _Fn _Func) {
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
		vector(const vector& _Left, _Fn _Func) {
			this->_Assign(_Left, _Func); 
		}
		template<typename _Fn> 
		vector(const vector& _Left, const vector& _Right, _Fn _Func) {
			this->_Assign(_Left, _Right, _Func); 
		}
		
		template<typename _Fn>
		void _Assign(const vector& _Left, const block_type& _Right_block, _Fn _Func) {
			this->_Assign(_Left,
				[_Func, &_Right_block](block_const_reference _Left_block) {
					return _Func(_Left_block, _Right_block);
				} );
		}
		template<typename _Fn>
		void _Assign(const block_type& _Left_block, const vector& _Right, _Fn _Func) {
			this->_Assign(_Right,
				[_Func, &_Left_block] (block_const_reference _Right_block) {
					return _Func(_Left_block, _Right_block);
				} );
		}
		template<typename _Fn> 
		vector(const vector& _Left, const block_type& _Right_block, _Fn _Func) {
			this->_Assign(_Left, _Right_block, _Func); 
		}
		template<typename _Fn> 
		vector(const block_type& _Left_block, const vector& _Right, _Fn _Func) {
			this->_Assign(_Left_block, _Right, _Func); 
		}
	
	public:
		template<typename _Fn>
		static vector& _Transform(const vector& _Left, vector& _Result, _Fn _Func) {
			_Result._Assign(_Left, _Func);
			return _Result;
		}
		template<typename _Fn>
		static vector& _Transform(const vector& _Left, const vector& _Right, vector& _Result, _Fn _Func) {
			_Result._Assign(_Left, _Right, _Func);
			return _Result;
		}
		template<typename _Fn>
		static vector _Transform_copy(const vector& _Left, _Fn _Func) {
			return vector(_Left, _Func);
		}
		template<typename _Fn>
		static vector _Transform_copy(const vector& _Left, const vector& _Right, _Fn _Func) {
			return vector(_Left, _Right, _Func);
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
		const auto _Func = std::plus<>();
		return _Accelerate_block<_Ty, _Block>::comulate(_X.block_begin(), _X.block_end(), _Func, _Func, 0, _X.tail_size());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	_Ty product(const vectorN& _X) {// X[0] * X[1] * X[2] * ... * X[N]
		const auto _Func = std::multiplies<>();
		return _Accelerate_block<_Ty, _Block>::comulate(_X.block_begin(), _X.block_end(), _Func, _Func, 0, _X.tail_size());
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
		return !approach_equal(_NormL2sq, static_cast<_Ty>(1), std::numeric_limits<_Ty>::epsilon()) ?
			_X / sqrt(_NormL2sq) : 
			_X;
	}

	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN mod(const vectorN& _Left, const vectorN& _Right) {
		return vectorN::_Transform_copy(_Left, _Right, std::modulus<>());
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN pow(const vectorN& _Left, const vectorN& _Right) {
		return vectorN::_Transform_copy(_Left, _Right,
			[](auto&& A, auto&& B) { return pow(A, B); });
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN cross3(const vectorN& _Left, const vectorN& _Right) {
		return vectorN{
			_Left[1] * _Right[2] - _Left[2] * _Right[1],
			_Left[2] * _Right[0] - _Left[0] * _Right[2],
			_Left[0] * _Right[1] - _Left[1] * _Right[0]
		};
	}

	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN abs(const vectorN& _Left) {
		return vectorN::_Transform_copy(_Left,
			[](auto&& bk) { return abs(bk); });
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN floor(const vectorN& _Left) {
		return vectorN::_Transform_copy(_Left,
			[](auto&& bk) { return floor(bk); });
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN ceil(const vectorN& _Left) {
		return vectorN::_Transform_copy(_Left,
			[](auto&& bk) { return ceil(bk); });
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN trunc(const vectorN& _Left) {
		return vectorN::_Transform_copy(_Left,
			[](auto&& bk) { return trunc(bk); });
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN round(const vectorN& _Left) {
		return vectorN::_Transform_copy(_Left,
			[](auto&& bk) { return round(bk); });
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN sqrt(const vectorN& _Left) {
		return vectorN::_Transform_copy(_Left,
			[](auto&& bk) { return sqrt(bk); });
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN cbrt(const vectorN& _Left) {
		return vectorN::_Transform_copy(_Left,
			[](auto&& bk) { return cbrt(bk); });
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN invsqrt(const vectorN& _Left) {
		return vectorN::_Transform_copy(_Left,
			[](auto&& bk) { return invsqrt(bk); });
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN invcbrt(const vectorN& _Left) {
		return vectorN::_Transform_copy(_Left,
			[](auto&& bk) { return invcbrt(bk); });
	}

	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN sin(const vectorN& _Left) {
		return vectorN::_Transform_copy(_Left,
			[](auto&& bk) { return sin(bk); });
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN cos(const vectorN& _Left) {
		return vectorN::_Transform_copy(_Left,
			[](auto&& bk) { return cos(bk); });
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN tan(const vectorN& _Left) {
		return vectorN::_Transform_copy(_Left,
			[](auto&& bk) { return tan(bk); });
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN asin(const vectorN& _Left) {
		return vectorN::_Transform_copy(_Left,
			[](auto&& bk) { return asin(bk); });
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN acos(const vectorN& _Left) {
		return vectorN::_Transform_copy(_Left,
			[](auto&& bk) { return acos(bk); });
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN atan(const vectorN& _Left) {
		return vectorN::_Transform_copy(_Left,
			[](auto&& bk) { return atan(bk); });
	}
	template<typename _Ty, size_t _Size, typename _Block> inline
	vectorN atan2(const vectorN& _Y, const vectorN& _X) {
		return vectorN::_Transform_copy(_Y, _X,
			[](auto&& bky, auto&& bkx) { return atan2(bky, bkx); } );
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
		using block_reference       = _Block&;
		using block_const_reference = _Block const&;

		using subvector       = clmagic::subvector<_Ty, _Block>;
		using const_subvector = clmagic::const_subvector<_Ty, _Block>;

		using block_traits    = clmagic::block_traits<_Block>;
		using allocate_traits = std::allocator_traits<_Alloc>;

		vector_any() : _Mybase(nullptr, nullptr), _Myalloc(), _Mydata(nullptr), _Mycapacity(0) { }

		vector_any(const vector_any& _Right) : vector_any() {// copy-constructor
			this->assign(_Right.begin(), _Right.end());
		}

		vector_any(vector_any&& _Right)
			: _Mybase(std::move(_Right)), _Myalloc(_Right._Myalloc), 
			_Mydata(_Right._Mydata), _Mycapacity(_Right._Mycapacity) {// move-constructor
			_Right._Mydata         = nullptr;
			_Right._Mycapacity     = 0;
			static_cast<subvector&&>(_Right).reset(nullptr, nullptr);
		}

		template<typename _Iter>
		void assign(_Iter _First, _Iter _Last) {
			const auto _Distance    = std::distance(_First, _Last);
			const auto _Newcapacity = _Distance / block_traits::size() + static_cast<size_t>(_Distance % block_traits::size() != 0);
			this->_Reallocate(_Newcapacity);

			scalar_pointer _Myfirst = this->begin();
			scalar_pointer _Mylast  = this->end();
			const auto     _Mysize  = _Mylast - _Myfirst;
			if (_Mysize < _Distance) {// copy [_First, _Last) to [_Myfirst, ...), construct [..., _Last) to [..., _Mylast)
				for (; _Myfirst != _Mylast; ++_Myfirst, ++_First) {
					*_Myfirst = *_First;
				}
				for (; _First != _Last; ++_First, ++_Mylast/*correct _Mylast*/) {
					allocate_traits::construct(_Myalloc, _Mylast, *_First);
				}
			} else {
				for (; _Myfirst != _Mylast; ++_Myfirst, ++_First) {
					*_Myfirst = *_First;
				}
				_Destroy_range(_Myfirst, _Mylast);
				_Mylast = _Myfirst;// correct _Mylast
			}
			_Mybase::reset(this->begin(), _Mylast);
		}

		template<typename _Fn>
		void assign(const vector_any& _Left, _Fn _Func) {// memory aligned
			assert(_Left.aligned());
			this->_Resize(_Left.size());
			assert(this->aligned());
			std::transform(_Left.block_begin(), _Left.block_end(), this->block_begin(), _Func);
		}

		template<typename _Fn>
		void assign(const vector_any& _Left, const vector_any& _Right, _Fn _Func) {
			assert(_Left.size() == _Right.size());
			assert(_Left.aligned() && _Right.aligned());
			this->_Resize(_Left.size());
			assert(this->aligned());
			std::transform(_Left.block_begin(), _Left.block_end(), _Right.block_begin(), this->block_begin(), _Func);
		}

		template<typename _Fn>
		void assign(const vector_any& _Left, const _Block& _Right_bk, _Fn _Func) {
			this->assign(_Left,
				[_Func, &_Right_bk](block_reference _Left_bk) { return _Func(_Left_bk, _Right_bk); });
		}

		template<typename _Fn>
		void assign(const _Block& _Left_bk, const vector_any& _Right, _Fn _Func) {
			this->assign(_Right,
				[_Func, &_Left_bk](block_reference _Right_bk) { return _Func(_Left_bk, _Right_bk); });
		}

		// <Internal-function>
		template<typename _Fn>
		void _Assign(const vector_any& _Left, const _Ty& _Scalar, _Fn _Func) {
			this->assign(_Left, block_traits::set1(_Scalar));
		}

		template<typename _Fn>
		void _Assign(const _Ty& _Scalar, const vector_any& _Right, _Fn _Func) {
			this->assign(block_traits::set1(_Scalar), _Right);
		}
		// </Internal-function>

		template<typename _Iter>
		vector_any(_Iter _First, _Iter _Last) : vector_any() {
			this->assign(_First, _Last);
		}

		vector_any(std::initializer_list<_Ty> _Ilist) : vector_any() {
			this->assign(_Ilist.begin(), _Ilist.end());
		}

		template<typename _Fn>
		vector_any(const vector_any& _Left, _Fn _Func) : vector_any() {
			assert(_Left.aligned());
			auto       _First = _Left.block_begin();
			const auto _Last  = _Left.block_end();
			this->_Reallocate(_Last - _First);
			assert(this->aligned());

			auto _Dest = this->block_begin();
			for (; _First != _Last; ++_First, ++_Dest) {
				allocate_traits::construct(_Myalloc, _Dest, _Func(*_First));
			}

			_Mybase::reset(this->begin(), this->begin() + _Left.size());// 
		}

		template<typename _Fn>
		vector_any(vector_any&& _Left, _Fn _Func) : vector_any(_Left) {
			std::transform(this->block_begin(), this->block_end(), this->block_begin(), _Func);
		}

		template<typename _Fn>
		vector_any(const vector_any& _Left, const vector_any& _Right, _Fn _Func) : vector_any() {// construct(this, _Func(_Left, _Right))
			assert(_Left.size() == _Right.size());
			assert(_Left.aligned() && _Right.aligned());
			auto       _First1 = _Left.block_begin();
			const auto _Last1  = _Left.block_end();
			this->_Reallocate(_Last1 - _First1);
			assert(this->aligned());

			auto _First2 = _Right.block_begin();
			auto _Dest   = this->block_begin();
			for (; _First1 != _Last1; ++_First1, ++_First2, ++_Dest) {
				allocate_traits::construct(_Myalloc, _Dest, _Func(*_First1, *_First2));
			}

			_Mybase::reset(this->begin(), this->begin() + _Left.size());// 
		}
		
		// <Internal-function>
		template<typename _Fn>
		vector_any(const vector_any& _Left, const _Ty& _Scalar, _Fn _Func) : vector_any() {// construct( this, _Func(_Left, block_tratis::set1(_Scalar)) )
			assert(_Left.aligned());
			auto       _First = _Left.block_begin();
			const auto _Last  = _Left.block_end();
			this->_Reallocate(_Last - _First);
			assert(this->aligned());

			const auto _Right_bk = block_traits::set1(_Scalar);
			auto       _Dest     = this->block_begin();
			for (; _First != _Last; ++_First, ++_Dest) {
				allocate_traits::construct(_Myalloc, _Dest, _Func(*_First, _Right_bk));
			}

			_Mybase::reset(this->begin(), this->begin() + _Left.size());// 
		}

		template<typename _Fn>
		vector_any(const _Ty& _Scalar, const vector_any& _Right, _Fn _Func) : vector_any() {// construct( this, _Func(block_tratis::set1(_Scalar), _Right) )
			assert(_Right.aligned());
			auto       _First = _Right.block_begin();
			const auto _Last  = _Right.block_end();
			this->_Reallocate(_Last - _First);
			assert(this->aligned());

			const auto _Left_bk = block_traits::set1(_Scalar);
			auto       _Dest    = this->block_begin();
			for (; _First != _Last; ++_First, ++_Dest) {
				allocate_traits::construct(_Myalloc, _Dest, _Func(_Left_bk, *_First));
			}

			_Mybase::reset(this->begin(), this->begin() + _Right.size());// 
		}
		// </Internal-function>
		
		vector_any& operator=(const vector_any& _Right) {
			this->assign(_Right.begin(), _Right.end());
			return *this;
		}
		vector_any& operator=(vector_any&& _Right) {
			_Mybase::subvector_ref() = _Right.subvector_ref();
			_Mydata     = _Right._Mydata;
			_Mycapacity = _Right._Mycapacity;
			_Right._Mydata         = nullptr;
			_Right._Mycapacity     = 0;
			_Right.subvector_ref() = { 0, 0, 1, {0, 0, 0} };
			return *this;
		}

		subvector operator()(size_t _First, size_t _Last) {
			return subvector(this->ptr(_First), this->ptr(_Last));
		}
		const_subvector operator()(size_t _First, size_t _Last) const {
			return const_subvector(this->ptr(_First), this->ptr(_Last));
		}

		vector_any operator-() const {
			return std::move(vector_any(*this, std::negate<block_type>()));
		}
		vector_any operator+(const vector_any& _Right) const {
			return std::move(vector_any(*this, _Right, std::plus<block_type>()));
		}
		vector_any operator-(const vector_any& _Right) const {
			return std::move(vector_any(*this, _Right, std::minus<block_type>()));
		}
		vector_any operator*(const vector_any& _Right) const {
			return std::move(vector_any(*this, _Right, std::multiplies<block_type>()));
		}
		vector_any operator/(const vector_any& _Right) const {
			return std::move(vector_any(*this, _Right, std::divides<block_type>()));
		}
		vector_any operator%(const vector_any& _Right) const {
			return std::move(vector_any(*this, _Right, std::modulus<block_type>()));
		}
		vector_any operator+(const _Ty& _Scalar) const {
			return std::move(vector_any(*this, _Scalar, std::plus<block_type>()));
		}
		vector_any operator-(const _Ty& _Scalar) const {
			return std::move(vector_any(*this, _Scalar, std::minus<block_type>()));
		}
		vector_any operator*(const _Ty& _Scalar) const {
			return std::move(vector_any(*this, _Scalar, std::multiplies<block_type>()));
		}
		vector_any operator/(const _Ty& _Scalar) const {
			return std::move(vector_any(*this, _Scalar, std::divides<block_type>()));
		}
		vector_any operator%(const _Ty& _Scalar) const {
			return std::move(vector_any(*this, _Scalar, std::modulus<block_type>()));
		}
		vector_any& operator+=(const vector_any& _Right) {
			this->assign(*this, _Right, std::plus<block_type>());
			return (*this);
		}
		vector_any& operator-=(const vector_any& _Right) {
			this->assign(*this, _Right, std::minus<block_type>());
			return (*this);
		}
		vector_any& operator*=(const vector_any& _Right) {
			this->assign(*this, _Right, std::multiplies<block_type>());
			return (*this);
		}
		vector_any& operator/=(const vector_any& _Right) {
			this->assign(*this, _Right, std::divides<block_type>());
			return (*this);
		}
		vector_any& operator%=(const vector_any& _Right) {
			this->assign(*this, _Right, std::modulus<block_type>());
			return (*this);
		}
		vector_any& operator+=(const _Ty& _Scalar) {
			this->_Assign(*this, _Scalar, std::plus<block_type>());
			return (*this);
		}
		vector_any& operator-=(const _Ty& _Scalar) {
			this->_Assign(*this, _Scalar, std::minus<block_type>());
			return (*this);
		}
		vector_any& operator*=(const _Ty& _Scalar) {
			this->_Assign(*this, _Scalar, std::multiplies<block_type>());
			return (*this);
		}
		vector_any& operator/=(const _Ty& _Scalar) {
			this->_Assign(*this, _Scalar, std::divides<block_type>());
			return (*this);
		}
		vector_any& operator%=(const _Ty& _Scalar) {
			this->_Assign(*this, _Scalar, std::modulus<block_type>());
			return (*this);
		}
		
		scalar_const_pointer ptr(size_t _Pos = 0) const {
			return _Mybase::ptr(_Pos); }
		scalar_const_pointer begin() const {
			return _Mybase::begin(); 
		}
		scalar_const_pointer end() const {
			return _Mybase::end(); 
		}
		scalar_const_reference operator[](size_t _Pos) const {
			return _Mybase::operator[](_Pos); }
		scalar_pointer ptr(size_t _Pos = 0) {
			return _Mybase::ptr(_Pos);
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
		size_t size() const {
			return _Mybase::size();
		}
		size_t capacity() const {
			return _Mycapacity; 
		}
		bool empty() const {// size is Zero
			return _Mybase::empty();
		}
		bool aligned() const {
			return _Mybase::aligned();
		}

		block_pointer block_begin() {
			return _Mydata;
		}
		block_pointer block_end() {
			return _Mydata + this->size() / block_traits::size() + static_cast<size_t>(this->size() % block_traits::size() != 0);
		}
		block_const_pointer block_begin() const {
			return _Mydata;
		}
		block_const_pointer block_end() const {
			return _Mydata + this->size() / block_traits::size() + static_cast<size_t>(this->size() % block_traits::size() != 0);
		}

		void clear() {// clear _Mybase, don't modify capacity
			auto       _First = this->begin();
			const auto _Last  = this->end();
			_Destroy_range(_First, _Last);
			_Mybase::reset(_First, _First);
		}

		template<typename... _Valty>
		void emplace_back(_Valty&&... _Val) {
			if ( _Has_capacity(this->size() + 1) ) {
				auto&           _Myvec  = _Mybase::subvector_ref();
				scalar_pointer& _Mylast = _Myvec._Mylast;
				allocate_traits::construct(_Myalloc, _Mylast, std::forward<_Valty...>(_Val...));
				++_Mylast;// correct _Mylast
				_Myvec._Mysize = vector_size<_Ty, _Block>(_Myvec._Myfirst, _Myvec._Mylast);// correct vector-size
			} else {
				_Reallocate(_Calculate_growth(this->capacity() + 1));
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
				auto& _Myvec = _Mybase::subvector_ref();
				--_Myvec._Mylast;
				allocate_traits::destroy(_Myalloc, _Myvec._Mylast);
			}
		}

		~vector_any() {// destroy[_First, _last) and delete[] _Mydata
			if (_Mydata != nullptr) {
				_Destroy_range(this->begin(), this->end());
				allocate_traits::deallocate(_Myalloc, _Mydata, _Mycapacity);
				_Mydata     = nullptr;
				_Mycapacity = 0;
				_Mybase::reset(nullptr, nullptr);
			} else {// _Mydata == nullptr
				if (!this->empty()) {
					std::cout << "[vector_any<_Ty, _Block> memory exception] ->[~vector_any()]" << std::endl;
					//throw std::exception("[vector_any<_Ty, _Block> memory exception] ->[~vector_any()]");
				}
			}
		}

		friend std::string to_string(const vector_any& _Left) {
			return _To_string(_Left.begin(), _Left.end()); }
		friend std::ostream& operator<<(std::ostream& _Ostr, const vector_any& _Left) {
			return (_Ostr << to_string(_Left)); }

	private:

		scalar_pointer _Destroy_range(scalar_pointer _First, scalar_const_pointer _Last) {
			for (; _First != _Last; ++_First) {
				allocate_traits::destroy(_Myalloc, _First);
			}
			return _First;
		}

		scalar_pointer _Construct_range(scalar_pointer _First, scalar_const_pointer _Last, const _Ty& _Val) {
			for (; _First != _Last; ++_First) {
				allocate_traits::construct(_Myalloc, _First, _Val);
			}
			return _First;
		}

		size_t max_size() const {
			return std::numeric_limits<size_t>::max();
		}

		bool _Has_capacity(size_t _Size) {// _Size scalar-sizes  <= _My scalar_sizes
			return (_Size <= _Mycapacity * block_traits::size());
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
		void _Reallocate(size_t _Newcapacity/*block*/) {
			if (_Newcapacity != _Mycapacity) {
				scalar_pointer _Myfirst = this->begin();
				scalar_pointer _Mylast  = this->end();
				const size_t   _Mysize  = _Mylast - _Myfirst;

				block_pointer  _Newdata  = allocate_traits::allocate(_Myalloc, _Newcapacity); // reallocate;
				scalar_pointer _Newfirst = reinterpret_cast<scalar_pointer>(_Newdata);
				scalar_pointer _Newlast  = _Newfirst;

				if (_Mysize != 0) {// move [_Myfirst, ...) to [_Newfirst, _Newlast) and delete _Olddata
					auto _Last = _Myfirst + std::min(_Mysize, _Newcapacity * block_traits::size());
					_Newlast = std::move(_Myfirst, _Last, _Newfirst);
					_Destroy_range(_Last, _Mylast);
					allocate_traits::deallocate(_Myalloc, _Mydata, _Mycapacity);
				}
				_Mydata     = _Newdata;
				_Mycapacity = _Newcapacity;
				_Mybase::reset(_Newfirst, _Newlast);
			}
		}

		/* learning from C++Stantard-library-vector<_Ty,...> */
		void _Resize(size_t _Newsize/*scalar*/, const _Ty& _Val) {
			if (_Newsize != this->size()) {
				if ( !_Has_capacity(_Newsize) ) {
					_Reallocate(_Calculate_growth(_Newsize / block_traits::size() + 1));
				}

				scalar_pointer _Myfirst = this->begin();
				scalar_pointer _Mylast  = this->end();
				if (_Newsize < this->size()) {// destroy [_Newlast, _Mylast)
					auto _Newlast = _Myfirst + _Newsize;
					_Mylast = _Destroy_range(_Newlast, _Mylast);
				} else {// construct [_Mylast, _Newlast)
					auto _Newlast = _Myfirst + _Newsize;
					_Mylast = _Construct_range(_Mylast, _Newlast, _Val);
				}
				_Mybase::reset(_Myfirst, _Mylast);
			}
		}

		_Alloc  _Myalloc;
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

		unit_vector(std::initializer_list<_Ty> _Ilist, bool _Unitized = false)
			: _Mybase(_Ilist) {
			if (!_Unitized) this->normalize();
		}

		unit_vector(const _Mybase& _Vector, bool _Unitized = false)
			: _Mybase(_Unitized ? _Vector : normalize(_Vector)) {
			// 
		}

		unit_vector operator-() const {
			return unit_vector(-static_cast<const _Mybase&>(*this), true);
		}
	};

	template<typename _Ty, typename _Block = _Ty>
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


}// namespace clmagic

#endif