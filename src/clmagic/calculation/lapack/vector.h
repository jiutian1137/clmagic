//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
// Apache License 2.0
//
// Table of contents
// 0.   graph -------------------------------------------------------------- L38
// 1.   block_traits *------------------------------------------------------ L55
// 2.1. memory ------------------------------------------------------------- L82
// 2.2. align -------------------------------------------------------------- L136
// 3.1. vector_size -------------------------------------------------------- L160
// 3.2. core vector_operation ---------------------------------------------- L233
// 4.1. binary operator with constant -------------------------------------- L680
// 4.2. RET_LAMBDA_ -------------------------------------------------------- L754
// 5.   shuffle *----------------------------------------------------------- L759
// 6.   subvector *--------------------------------------------------------- L820
// 7.   vector *------------------------------------------------------------ L1318
// 8.   unit_vector *------------------------------------------------------- L1947
// 9.   vector_any --------------------------------------------------------- L1989
// 10.  vector_cast *------------------------------------------------------- L2334
// 11.  undetermined ------------------------------------------------------- L2361
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_math_lapack_VECTOR_h_
#define clmagic_math_lapack_VECTOR_h_
#include "../numeric/real.h"// include CSTD math

#include <cassert>
#include <string>// to_string
#include <numeric>// std::accumulate<>, std::transform_reduce<>
#include <algorithm>// std::transform<>
#include <functional>// std::plus<>, std::multiplies, ...

#include <memory>// vector_any<>
#include <vector>// vector_any<>

/*<graph>
	array:  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----| unit:[scalars]
	vector: |-----|-----------------|-----------------|-----------------|-----------------|-----| unit:[blocks]         
			A0   A1														                  B0    B1
			|-led-|---------------------------- block_size -------------------------------|-tai-|       
	
	A0 = first
	B1 = last

	block_offset = A0 & alignment_mask_of_v<_Tb> unit:[bytes]
	leading_size = A1 - A0 unit:[scalars]
	block_size   = B0 - A1 unit:[blocks]
	tail_size    = B1 - B0 unit:[scalars]    
</graph>*/

namespace clmagic{

// 1.   block_traits *------------------------------------------------------ L55

	template<typename _Ty>
	struct block_traits {// default smallest_block
		using block_type  = _Ty;
		using scalar_type = _Ty;

		static constexpr size_t size() {
			return 1;
		}

		static block_type construct0() {
			return block_type();
		}

		static block_type construct1(const scalar_type& _Val) {
			return block_type(_Val);
		}

		static scalar_type at0(const block_type& _Bval) {
			return _Bval;
		}
	};

	template<typename _Ty> constexpr
	bool is_smallest_block_v = std::is_same_v<typename block_traits<_Ty>::block_type, 
		                                      typename block_traits<_Ty>::scalar_type>;

// 2.1. memory ------------------------------------------------------------- L82

	// { stride == 1 }
	template<typename _Ty>
	struct continuous_memory {
		_Ty* first;
		_Ty* last;

		continuous_memory() : first(nullptr), last(nullptr) {}

		continuous_memory(_Ty* _First, _Ty* _Last) : first(_First), last(_Last) { assert(_First <= _Last); }

		bool empty() const {
			return (first == last);
		}

		size_t size() const {
			return (last - first);
		}
	};

	// { any_memory = continuous_memory + discontinuous_memory }
	template<typename _Ty>
	struct any_memory {
		_Ty* first;
		_Ty* last;
		ptrdiff_t stride;

		any_memory() : first(nullptr), last(nullptr), stride(1) {}

		any_memory(_Ty* _First, _Ty* _Last) : first(nullptr), last(nullptr), stride(1) {}

		any_memory(continuous_memory<_Ty> _Mem) : first(_Mem.first), last(_Mem.last), stride(1) {}

		any_memory(_Ty* _First, _Ty* _Last, ptrdiff_t _Stride) : first(_First), last(_Last), stride(_Stride) {}

		bool empty() const {
			return (first == last);
		}

		bool continuous() const {
			return (stride == 1);
		}

		size_t size() const {
			return (last - first) / stride;
		}

		continuous_memory<_Ty> as_continuous_memory() {
			assert( continuous() );
			return continuous_memory<_Ty>( first, last );
		}
	};

// 2.2. align -------------------------------------------------------------- L136

	template<typename _Ty> constexpr
	size_t alignment_mask_of_v = std::alignment_of_v<_Ty> - 1;

	template<size_t _Val, size_t _Bound>
	struct aligned_ceil {
		constexpr static size_t value = (_Val + (_Bound - 1)) & (~(_Bound - 1));
	};

	template<typename _Storety, size_t _Size, typename _Alignty>
	struct aligned_traits {
		constexpr static size_t unaligned_bytes = sizeof(_Storety) * _Size;
		constexpr static size_t unaligned_size  = _Size;

		constexpr static size_t aligned_bound   = std::alignment_of_v<_Alignty>;
		constexpr static size_t aligned_bytes   = aligned_ceil<unaligned_bytes, aligned_bound>::value;
		constexpr static size_t aligned_size    = aligned_bytes / sizeof(_Storety);
		// aligned_size >= unaligned_size

		constexpr static size_t valid_size   = unaligned_size;
		constexpr static size_t invalid_size = aligned_size - unaligned_size;
	};

// 3.1. vector_size -------------------------------------------------------- L160

	// { array_size = leading_size + block_size*block_traits<_Tb> + tail_size }
	template<typename _Ts, typename _Tb>
	struct vector_size {
		size_t leading_size;
		size_t block_size;
		size_t tail_size;

		static bool construct(vector_size& _Dest, continuous_memory<const _Ts> _Source) {
			// 1. check empty
			if (!_Source.empty()) {
				const auto _My_scalar_size  = _Source.size();// unit:[scalars]
				auto       _My_block_offset = reinterpret_cast<size_t>(_Source.first) & alignment_mask_of_v<_Tb>;// unit:[bytes]

				// 2. check memory_error
				if (_My_block_offset % sizeof(_Ts) == 0) {
					_My_block_offset /= sizeof(_Ts);// unit:[scalars]

					// 3. check through_alignement_point
					const bool _Through_alignment_point = (_My_block_offset + _My_scalar_size >= block_traits<_Tb>::size());
					if (_Through_alignment_point) {
						_Dest.leading_size  = (_My_block_offset == 0) ? 0 : block_traits<_Tb>::size() - _My_block_offset;

						auto _My_block_size = _My_scalar_size - _Dest.leading_size;// unit:[scalars]
						_Dest.block_size    = _My_block_size / block_traits<_Tb>::size();
						_Dest.tail_size     = _My_block_size % block_traits<_Tb>::size();
						return true;
					}

				} else {
					throw std::exception("clmagic::error::bad_memory");
				}
			}

			return false;
		}

		vector_size() : leading_size(0), block_size(0), tail_size(0) {}

		vector_size(const _Ts* _First, const _Ts* _Last)
			: leading_size(0), block_size(0), tail_size(0) {
			// construct by continuous_memory<const _Ts>
			construct(*this, continuous_memory<const _Ts>(_First, _Last));
		}

		vector_size(const _Ts* _First, const _Ts* _Last, ptrdiff_t _Stride)
			: leading_size(0), block_size(0), tail_size(0) {
			// construct by any_memory<const _Ts>
			if (_Stride == 1) {
				construct(*this, continuous_memory<const _Ts>(_First, _Last));
			}
		}

		bool empty() const {
			return (block_size == 0 && leading_size == 0 && tail_size == 0);
		}
		
		bool can_fastest() const {
			return (leading_size == 0 && tail_size == 0);
		}

		bool operator==(const vector_size& _Right) const {
			return (this->leading_size == _Right.leading_size &&
				    this->block_size   == _Right.block_size &&
					this->tail_size    == _Right.tail_size);
		}

		bool operator!=(const vector_size& _Right) const {
			return !(*this == _Right);
		}
	};

// 3.2. core vector_operation ---------------------------------------------- L233
	
	enum class vector_accelerate_level {
		_FASTEST,
		_FAST,
		_NORM,
		_AUTO
	};

	// { norm_vector_operation }
	template<typename _Ts, typename _Tb, vector_accelerate_level/*default: Norm*/>
	struct _Vector_operation {
		using _OutIt = _Ts*;
		using _InIt  = const _Ts *;

		using scalar_type = _Ts;

		template<typename _UnaryOp>
		static _OutIt transform(_InIt _First, _InIt _Last, _OutIt _Dest, _UnaryOp _Transform_op,
			ptrdiff_t _Inc1 = 1, ptrdiff_t _Inc2 = 1) {
			for ( ; _First != _Last; _First+=_Inc1, _Dest+=_Inc2) {
				*_Dest = _Transform_op(*_First);
			}
			return _Dest;
		}

		template<typename _BinOp>
		static _OutIt transform(_InIt _First1, _InIt _Last1, _InIt _First2, _OutIt _Dest, _BinOp _Transform_op,
			ptrdiff_t _Inc1 = 1, ptrdiff_t _Inc2 = 1, ptrdiff_t _Inc3 = 1) {
			for ( ; _First1 != _Last1; _First1+=_Inc1, _First2+=_Inc2, _Dest+=_Inc3) {
				*_Dest = _Transform_op(*_First1, *_First2);
			}
			return _Dest;
		}

		template<typename _BinOp>
		static scalar_type accumulate(_InIt _First, _InIt _Last, scalar_type _Val, _BinOp _Reduce_op,
			ptrdiff_t _Inc1 = 1) {
			for (; _First != _Last; _First+=_Inc1) {
				_Val = _Reduce_op(_Val, *_First);
			}
			return _Val;
		}

		template<typename _BinOp, typename _UnaryOp>
		static scalar_type transform_accumulate(_InIt _First, _InIt _Last, scalar_type _Val, _BinOp _Reduce_op, _UnaryOp _Transform_op,
			ptrdiff_t _Inc = 1) {
			for ( ; _First != _Last; _First+=_Inc) {
				_Val = _Reduce_op(_Val, _Transform_op(*_First));
			}
			return _Val;
		}

		template<typename _BinOp1, typename _BinOp2>
		static scalar_type transform_accumulate(_InIt _First1, _InIt _Last1, _InIt _First2, scalar_type _Val, _BinOp1 _Reduce_op, _BinOp2 _Transform_op,
			ptrdiff_t _Inc1 = 1, ptrdiff_t _Inc2 = 1) {
			for ( ; _First1 != _Last1; _First1+=_Inc1, _First2+=_Inc2) {
				_Val = _Reduce_op(_Val, _Transform_op(*_First1, *_First2));
			}
			return _Val;
		}
	};

	// { fastest_vector_operation }
	template<typename _Ts, typename _Tb>
	struct _Vector_operation<_Ts, _Tb, vector_accelerate_level::_FASTEST> {
		using _OutIt = _Tb*;
		using _InIt  = const _Tb *;

		using scalar_type  = _Ts;
		using block_type   = _Tb;
		using block_traits = clmagic::block_traits<_Tb>;
		
		using scalar_operation = _Vector_operation<_Ts, _Tb, vector_accelerate_level::_NORM>;

		template<typename _UnaryOp>
		static _OutIt transform(_InIt _First, _InIt _Last, _OutIt _Dest, _UnaryOp _Func) {
			return std::transform(_First, _Last, _Dest, _Func);
		}

		template<typename _BinOp>
		static _OutIt transform(_InIt _First1, _InIt _Last1, _InIt _First2, _OutIt _Dest, _BinOp _Func) {
			return std::transform(_First1, _Last1, _First2, _Dest, _Func);
		}

		template<typename _BinOp>
		static scalar_type accumulate(_InIt _First, _InIt _Last, scalar_type _Val, _BinOp _Reduce_op) {
			if (_First == _Last) {
				return _Val;
			}
			block_type _Block_result = std::accumulate(std::next(_First, 1), _Last, *_First, _Reduce_op);
			auto       _Sfirst       = reinterpret_cast<const scalar_type*>(&_Block_result);
			const auto _Slast        = _Sfirst + block_traits::size();
			return scalar_operation::accumulate(_Sfirst, _Slast, _Val, _Reduce_op);
		}

		template<typename _BinOp, typename _UnaryOp>
		static scalar_type transform_accumulate(_InIt _First, _InIt _Last, scalar_type _Initval, _BinOp _Reduce_op, _UnaryOp _Transform_op) {
			if (_First == _Last) {
				return _Initval;
			}
			block_type _Block_initval = _Transform_op(*_First++);
			block_type _Block_result  = _STD transform_reduce(_First, _Last, _Block_initval, _Reduce_op, _Transform_op);
			auto       _Sfirst        = reinterpret_cast<const scalar_type*>(&_Block_result);
			const auto _Slast         = _Sfirst + block_traits::size();
			return scalar_operation::accumulate(_Sfirst, _Slast, _Initval, _Reduce_op);
		}

		template<typename _BinOp1, typename _BinOp2>
		static scalar_type transform_accumulate(_InIt _First1, _InIt _Last1, _InIt _First2, scalar_type _Initval, _BinOp1 _Reduce_op, _BinOp2 _Transform_op) {
			if (_First1 == _Last1) {
				return _Initval;
			}
			block_type _Block_initval = _Transform_op(*_First1++, *_First2++);
			block_type _Block_result  = _STD transform_reduce(_First1, _Last1, _First2, _Block_initval, _Reduce_op, _Transform_op);
			auto       _Sfirst        = reinterpret_cast<const scalar_type*>(&_Block_result);
			const auto _Slast         = _Sfirst + block_traits::size();
			return scalar_operation::accumulate(_Sfirst, _Slast, _Initval, _Reduce_op);
		}
	};
	
	// { fast_vector_operation }
	template<typename _Ts, typename _Tb>
	struct _Vector_operation<_Ts, _Tb, vector_accelerate_level::_FAST> {
		using _OutIt = _Tb*;
		using _InIt  = const _Tb*;

		using scalar_type  = _Ts;
		using block_type   = _Tb;
		using block_traits = ::clmagic::block_traits<_Tb>;
		
		using scalar_operation = _Vector_operation<_Ts, _Tb, vector_accelerate_level::_NORM>;
		using block_operation  = _Vector_operation<_Ts, _Tb, vector_accelerate_level::_FASTEST>;

		/*<viewer>
			<read-mode> practical meaning of each paragraph is the same </read-mode>
			<paragraph>
				if(_Leading_size != 0){ PARAGRAPH } 
				if(_First != _Last){ PARAGRAPH }
				if(_Tail_size != 0){ PARAGRAPH }
				for(...) is also PARAGRAPH
			</paragraph>
			</viewer>*/

		static void _Copy_from_last(const scalar_type* _Last, scalar_type* _Dest, size_t _Count) {// [..., _Last)
			while (_Count--) {
				--_Last;
				--_Dest;
				*_Dest = *_Last;
			}
		}

		template<typename _UnaryOp>
		static _OutIt transform(_InIt _First, _InIt _Last, _OutIt _Dest, _UnaryOp _Transform_op,
			size_t _Leading_size = 0, size_t _Tail_size = 0) {
			if (_Leading_size != 0) {
				const block_type   _Temp      = _Transform_op(*std::prev(_First,1));
				const scalar_type* _Last_temp = reinterpret_cast<const scalar_type*>((&_Temp) + 1);
				_Copy_from_last(_Last_temp, (scalar_type*)_Dest, _Leading_size);
			}

			for ( ; _First != _Last; ++_First, ++_Dest) {
				*_Dest = _Transform_op(*_First);
			}

			if (_Tail_size != 0) {// vector-length is usually large, when _Tail_size != 0
				const block_type   _Temp       = _Transform_op(*_First);
				const scalar_type* _First_temp = reinterpret_cast<const scalar_type*>(&_Temp);
				std::copy(_First_temp, _First_temp+_Tail_size, (scalar_type*)_Dest);
			}

			return _Dest;// Almost useless
		}
	
		template<typename _BinOp>
		static _OutIt transform(_InIt _First1, _InIt _Last1, _InIt _First2, _OutIt _Dest, _BinOp _Transform_op,
			size_t _Leading_size = 0, size_t _Tail_size = 0) {
			if (_Leading_size != 0) {
				const block_type   _Temp      = _Transform_op( *std::prev(_First1, 1), *std::prev(_First2, 1) );
				const scalar_type* _Last_temp = reinterpret_cast<const scalar_type*>((&_Temp) + 1);
				_Copy_from_last(_Last_temp, (scalar_type*)_Dest, _Leading_size);
			}

			for (; _First1 != _Last1; ++_First1, ++_First2, ++_Dest) {
				*_Dest = _Transform_op(*_First1, *_First2);
			}

			if (_Tail_size != 0) {// vector-length is usually large, when _Tail_size != 0
				const _Tb          _Temp       = _Transform_op(*_First1, *_First2);
				const scalar_type* _First_temp = reinterpret_cast<const scalar_type*>(&_Temp);
				std::copy(_First_temp, _First_temp+_Tail_size, (scalar_type*)_Dest);
			}

			return _Dest;// Almost useless
		}
	
		template<typename _BinOp>
		static scalar_type accumulate(_InIt _First, _InIt _Last, scalar_type _Val, _BinOp _Reduce_op,
			size_t _Leading_size = 0, size_t _Tail_size = 0) {
			if (_Leading_size != 0) {
				auto _Slast  = reinterpret_cast<const scalar_type*>(_First);
				auto _Sfirst = _Slast - _Leading_size;
				_Val = std::accumulate(_Sfirst, _Slast, _Val, _Reduce_op);
			}

			_Val = block_operation::accumulate(_First, _Last, _Val, _Reduce_op);// impotant!

			if (_Tail_size != 0) {
				auto       _Sfirst = reinterpret_cast<const _Ts*>(_Last);
				const auto _Slast  = _Sfirst + _Tail_size;
				_Val = std::accumulate(_Sfirst, _Slast, _Val, _Reduce_op);
			}

			return _Val;
		}

		template<typename _BinOp, typename _UnaryOp>
		static scalar_type transform_accumulate(_InIt _First, _InIt _Last, scalar_type _Val, _BinOp _Reduce_op, _UnaryOp _Transform_op,
			size_t _Leading_size = 0, size_t _Tail_size = 0) {
			if (_Leading_size != 0) {
				const auto _Temp   = _Transform_op(*std::prev(_First,1));
				auto       _Slast  = reinterpret_cast<const scalar_type*>((&_Temp) + 1);
				auto       _Sfirst = _Slast - _Leading_size;
				_Val = std::accumulate(_Sfirst, _Slast, _Val, _Reduce_op);
			}

			_Val = block_operation::transform_accumulate(_First, _Last, _Val, _Reduce_op, _Transform_op);// impotant!

			if (_Tail_size != 0) {
				const auto _Temp   = _Transform_op(*_Last);
				auto       _Sfirst = reinterpret_cast<const _Ts*>(&_Temp);
				const auto _Slast  = _Sfirst + _Tail_size;
				_Val = std::accumulate(_Sfirst, _Slast, _Val, _Reduce_op);
			}

			return _Val;
		}

		template<typename _BinOp1, typename _BinOp2>
		static scalar_type transform_accumulate(_InIt _First1, _InIt _Last1, _InIt _First2, scalar_type _Val, _BinOp1 _Reduce_op, _BinOp2 _Transform_op,
			size_t _Leading_size = 0, size_t _Tail_size = 0) {
			if (_Leading_size != 0) {
				const auto _Temp   = _Transform_op( *std::prev(_First1,1), *std::prev(_First2,1) );
				auto       _Slast  = reinterpret_cast<const scalar_type*>((&_Temp) + 1);
				auto       _Sfirst = _Slast - _Leading_size;
				_Val = std::accumulate(_Sfirst, _Slast, _Val, _Reduce_op);
			}

			if (_First1 != _Last1) {
				block_type _Block_val = _Transform_op(*_First1++, *_First2++);
				for ( ; _First1 != _Last1; ++_First1, ++_First2) {
					_Block_val = _Reduce_op(_Block_val, _Transform_op(*_First1, *_First2));
				}

				auto       _Sfirst = reinterpret_cast<const scalar_type*>(&_Block_val);
				const auto _Slast  = _Sfirst + block_traits::size();
				_Val = std::accumulate(_Sfirst, _Slast, _Val, _Reduce_op);
			}

			if (_Tail_size != 0) {
				const auto _Temp   = _Transform_op(*_First1, *_First2);
				auto       _Sfirst = reinterpret_cast<const scalar_type*>(&_Temp);
				const auto _Slast  = _Sfirst + _Tail_size;
				_Val = std::accumulate(_Sfirst, _Slast, _Val, _Reduce_op);
			}

			return _Val;
		}	
	};

	// { auto_vector_operation }
	template<typename _Ts, typename _Tb>
	struct _Vector_operation<_Ts, _Tb, vector_accelerate_level::_AUTO> {
		using _OutIt = void*;
		using _InIt  = const void *;

		using scalar_type  = _Ts;
		using block_type   = _Tb;
		using size_type    = clmagic::vector_size<_Ts, _Tb>;
		using block_traits = clmagic::block_traits<_Tb>;

		using fastest_operation = _Vector_operation<_Ts, _Tb, vector_accelerate_level::_FASTEST>;;
		using fast_operation    = _Vector_operation<_Ts, _Tb, vector_accelerate_level::_FAST>;
		using norm_operation    = _Vector_operation<_Ts, _Tb, vector_accelerate_level::_NORM>;;

		template<typename _UnaryOp>
		static _OutIt transform(_InIt _First, _InIt _Last, _OutIt _Dest, _UnaryOp _Transform_op,
			ptrdiff_t _Inc1 = 1, ptrdiff_t _Inc2 = 1) {
			// transform [_First, _Last) with _Transform_op

			if ( _Inc1 == 1 && _Inc2 == 1 ) {// continuous_memory
				const auto _Vsize = size_type((const scalar_type*)_First, (const scalar_type*)_Last);

				if ( !_Vsize.empty() ) {
					if ( _Vsize.can_fastest() ) {
						return fastest_operation::transform(static_cast<typename fastest_operation::_InIt>(_First),
															static_cast<typename fastest_operation::_InIt>(_Last), 
															static_cast<typename fastest_operation::_OutIt>(_Dest), _Transform_op);
					} else {
						auto       _Bfirst = reinterpret_cast<const block_type*>( (const scalar_type*)_First + _Vsize.leading_size );
						const auto _Blast  = _Bfirst + _Vsize.block_size;
						auto       _Bdest  = reinterpret_cast<block_type*>( (scalar_type*)_Dest + _Vsize.leading_size );
						return fast_operation::transform(_Bfirst, _Blast, _Bdest, _Transform_op, _Vsize.leading_size, _Vsize.tail_size);
					}
				}
			}

			// discontinuous_memory
			return norm_operation::transform(static_cast<typename norm_operation::_InIt>(_First), 
												static_cast<typename norm_operation::_InIt>(_Last),
												static_cast<typename norm_operation::_OutIt>(_Dest), _Transform_op, _Inc1, _Inc2);
		}
	
		template<typename _BinOp>
		static _OutIt transform(_InIt _First1, _InIt _Last1, _InIt _First2, _OutIt _Dest, _BinOp _Transform_op,
			ptrdiff_t _Inc1 = 1, ptrdiff_t _Inc2 = 1, ptrdiff_t _Inc3 = 1) {
			// transform [_First1, _Last1) and [_Frist2, ...) with _Transform_op

			if ( _Inc1 == 1 && _Inc2 == 1 && _Inc3 == 1 ) {// continuous_memory
				const auto _Diff   = std::distance((const scalar_type*)_First1, (const scalar_type*)_Last1);
				const auto _Vsize1 = size_type((const scalar_type*)_First1, (const scalar_type*)_Last1);
				const auto _Vsize2 = size_type((const scalar_type*)_First2, (const scalar_type*)_First2 + _Diff);

				if ( _Vsize1 == _Vsize2 && !_Vsize1.empty() ) {
					if ( _Vsize1.can_fastest() ) {
						return fastest_operation::transform(static_cast<typename fastest_operation::_InIt>(_First1), 
															static_cast<typename fastest_operation::_InIt>(_Last1), 
															static_cast<typename fastest_operation::_InIt>(_First2), 
															static_cast<typename fastest_operation::_OutIt>(_Dest), _Transform_op);
					} else {
						auto       _Bfirst1 = reinterpret_cast<const block_type*>( (const scalar_type*)(_First1) + _Vsize1.leading_size );
						const auto _Blast1  = _Bfirst1 + _Vsize1.block_size;
						auto       _Bfirst2 = reinterpret_cast<const block_type*>( (const scalar_type*)(_First2) + _Vsize1.leading_size );
						auto       _Bdest   = reinterpret_cast<block_type*>( (scalar_type*)(_Dest) + _Vsize1.leading_size );
						return fast_operation::transform(_Bfirst1, _Blast1, _Bfirst2, _Bdest, _Transform_op, _Vsize1.leading_size, _Vsize1.tail_size);
					}
				}
			}

			// discontinuous_memory
			return norm_operation::transform(static_cast<typename norm_operation::_InIt>(_First1), 
												static_cast<typename norm_operation::_InIt>(_Last1),
												static_cast<typename norm_operation::_InIt>(_First2), 
												static_cast<typename norm_operation::_OutIt>(_Dest), _Transform_op, _Inc1, _Inc2, _Inc3);
		}

		template<typename _BinOp>
		static scalar_type accumulate(_InIt _First, _InIt _Last, scalar_type _Val, _BinOp _Reduce_op,
			ptrdiff_t _Inc = 1) {
			// accumulate [_First, _Last) with _Reduce_op

			if (_Inc == 1) {// continuous_memory
				const auto _Vsize = size_type((const scalar_type*)_First, (const scalar_type*)_Last);

				if ( !_Vsize.empty() ){
					if ( _Vsize.can_fastest() ) {
						return fastest_operation::accumulate(static_cast<typename fastest_operation::_InIt>(_First),
																static_cast<typename fastest_operation::_InIt>(_Last), _Val, _Reduce_op);
					} else {
						auto       _Bfirst = reinterpret_cast<const block_type*>( (const scalar_type*)(_First) + _Vsize.leading_size );
						const auto _Blast  = _Bfirst + _Vsize.block_size;
						return fast_operation::accumulate(_Bfirst, _Blast, _Val, _Reduce_op);
					}
				}
			}

			// discontinuous_memory
			return norm_operation::accumulate(static_cast<typename norm_operation::_InIt>(_First), 
												static_cast<typename norm_operation::_InIt>(_Last), _Val, _Reduce_op, _Inc);
		}

		template<typename _BinOp, typename _UnaryOp>
		static scalar_type transform_accumulate(_InIt _First, _InIt _Last, scalar_type _Val, _BinOp _Reduce_op, _UnaryOp _Transform_op,
			ptrdiff_t _Inc = 1) {
			// transform_accumulate [_First, _Last) with _Transform_op and _Reduce_op

			if (_Inc == 1) {// continuous_memory
				const auto _Vsize = size_type( (const scalar_type*)_First, (const scalar_type*)_Last );

				if ( !_Vsize.empty() ){
					if ( _Vsize.can_fastest() ) {
						return fastest_operation::transform_accumulate(static_cast<typename fastest_operation::_InIt>(_First), 
																		static_cast<typename fastest_operation::_InIt>(_First), _Val, _Reduce_op, _Transform_op);
					} else {
						auto       _Bfirst = reinterpret_cast<const block_type*>( (const scalar_type*)_First + _Vsize.leading_size );
						const auto _Blast  = _Bfirst + _Vsize.block_size;
						return fast_operation::transform_accumulate(_Bfirst, _Blast, _Val, _Reduce_op, _Transform_op);
					}
				}
			}

			// discontinuous_memory
			return norm_operation::transform_accumulate(static_cast<typename norm_operation::_InIt>(_First), 
														static_cast<typename norm_operation::_InIt>(_Last), _Val, _Reduce_op, _Transform_op, _Inc);
		}

		template<typename _BinOp1, typename _BinOp2>
		static scalar_type transform_accumulate(_InIt _First1, _InIt _Last1, _InIt _First2, scalar_type _Val, _BinOp1 _Reduce_op, _BinOp2 _Transform_op,
			ptrdiff_t _Inc1 = 1, ptrdiff_t _Inc2 = 1) {
			// transform_accumulate [_First1, _Last1) and [_First2, ...) with _Transform_op and _Reduce_op

			if (_Inc1 == 1 && _Inc2 == 1) {// continuous_memory
				const auto _Diff   = std::distance((const scalar_type*)_First1, (const scalar_type*)_Last1);
				const auto _Vsize1 = size_type((const scalar_type*)_First1, (const scalar_type*)_Last1);
				const auto _Vsize2 = size_type((const scalar_type*)_First2, (const scalar_type*)_First2 + _Diff);

				if ( _Vsize1 == _Vsize2 && !_Vsize1.empty() ) {
					if ( _Vsize1.can_fastest() ) {
						return fastest_operation::transform_accumulate(static_cast<typename fast_operation::_InIt>(_First1), 
																		static_cast<typename fast_operation::_InIt>(_Last1), 
																		static_cast<typename fast_operation::_InIt>(_First2), _Val, _Reduce_op, _Transform_op);
					} else {
						auto       _Bfirst1 = reinterpret_cast<const block_type*>((const scalar_type*)(_First1) + _Vsize1.leading_size);
						const auto _Blast1  = _Bfirst1 + _Vsize1.block_size;
						auto       _Bfirst2 = reinterpret_cast<const block_type*>((const scalar_type*)(_First2) + _Vsize1.leading_size);
						return fast_operation::transform_accumulate(_Bfirst1, _Blast1, _Bfirst2, _Val, _Reduce_op, _Transform_op, _Vsize1.leading_size, _Vsize1.tail_size);
					}
				}
			}

			// discontinuous_memory
			return norm_operation::transform_accumulate(static_cast<typename norm_operation::_InIt>(_First1), static_cast<typename norm_operation::_InIt>(_Last1),
				static_cast<typename norm_operation::_InIt>(_First2), _Val, _Reduce_op, _Transform_op, _Inc1, _Inc2);
		}
	};

	// { auto_vector_operation }
	template<typename _Ts>
	struct _Vector_operation<_Ts, _Ts, vector_accelerate_level::_AUTO>
		: public _Vector_operation<_Ts, _Ts, vector_accelerate_level::_NORM>{
		// 
	};

	template<typename _Ts, typename _Tb>
	using fastest_vector_operation = _Vector_operation<_Ts, _Tb, vector_accelerate_level::_FASTEST>;

	template<typename _Ts, typename _Tb>
	using fast_vector_operation = _Vector_operation<_Ts, _Tb, vector_accelerate_level::_FAST>;
	
	template<typename _Ts, typename _Tb>
	using norm_vector_operation = _Vector_operation<_Ts, _Tb, vector_accelerate_level::_NORM>;
	
	template<typename _Ts, typename _Tb>
	using auto_vector_operation = _Vector_operation<_Ts, _Tb, vector_accelerate_level::_AUTO>;

// 4.1. binary operator with constant -------------------------------------- L680

	namespace Internal {
		// { varying at left }
		template<typename _BinOp, typename _Ty, bool _Is_smb/* = is_smallest_block_v<_Ty>*/>
		struct _Left_varying_op_with_constant {
			_Left_varying_op_with_constant(_BinOp _Arg0, const _Ty& _Arg1) : _Func(_Arg0), _Right(_Arg1) {}

			auto operator()(const _Ty& _Left) const {
				return _Func( _Left, _Right );
			}

			_BinOp _Func;
			_Ty    _Right;
		};

		template<typename _BinOp, typename _Ty>
		struct _Left_varying_op_with_constant<_BinOp, _Ty, false> {
			_Left_varying_op_with_constant(_BinOp _Arg0, const _Ty& _Arg1) : _Func(_Arg0), _Right(_Arg1) {}

			auto operator()(const typename block_traits<_Ty>::scalar_type& _Left) const  {
				return _Func( _Left, block_traits<_Ty>::at0(_Right) );
			}

			auto operator()(const typename block_traits<_Ty>::block_type& _Left) const {
				return _Func( _Left, _Right );
			}

			_BinOp _Func;
			_Ty    _Right;
		};

		// { varying at right }
		template<typename _BinOp, typename _Ty, bool _Is_smb/* = is_smallest_block_v<_Ty>*/>
		struct _Right_varying_op_with_constant {
			_Right_varying_op_with_constant(_BinOp _Arg0, const _Ty& _Arg1) : _Func(_Arg0), _Left(_Arg1) {}

			auto operator()(const _Ty& _Right) const {
				return _Func(_Left, _Right);
			}

			_BinOp _Func;
			_Ty    _Left;
		};

		template<typename _BinOp, typename _Ty>
		struct _Right_varying_op_with_constant<_BinOp, _Ty, false> {
			_Right_varying_op_with_constant(_BinOp _Arg0, const _Ty& _Arg1) : _Func(_Arg0), _Left(_Arg1) {}

			auto operator()(const typename block_traits<_Ty>::scalar_type& _Right) const {
				return _Func( block_traits<_Ty>::at0(_Left), _Right );
			}

			auto operator()(const typename block_traits<_Ty>::block_type& _Right) const {
				return _Func( _Left, _Right );
			}

			_BinOp _Func;
			_Ty    _Left;
		};
	}

	// { varying at left }
	template< typename _BinOp, typename _Ty, bool _Is_smb = is_smallest_block_v<_Ty> > inline
	Internal::_Left_varying_op_with_constant<_BinOp, _Ty, _Is_smb> left_varying_op_by(_BinOp _Func, const _Ty& _Constant) {
		return Internal::_Left_varying_op_with_constant<_BinOp, _Ty, _Is_smb>(_Func, _Constant);
	}

	// { varying at right }
	template< typename _BinOp, typename _Ty, bool _Is_smb = is_smallest_block_v<_Ty> > inline
	Internal::_Right_varying_op_with_constant<_BinOp, _Ty, _Is_smb> right_varying_op_by(_BinOp _Func, const _Ty& _Constant) {
		return Internal::_Right_varying_op_with_constant<_BinOp, _Ty, _Is_smb>(_Func, _Constant);
	}

// 4.2. RET_LAMBDA_ -------------------------------------------------------- L754

	#define RET_LAMBDA_1ST(FNAME, TYPE) [](TYPE _X){ return FNAME(_X); }
	#define RET_LAMBDA_2ND(FNAME, TYPE1, TYPE2) [](TYPE1 _X, TYPE2 _Y){ return FNAME(_X, _Y); }

// 5.   shuffle *----------------------------------------------------------- L759

	template<typename _OutTy, typename _InTy> inline
	void _Shuffle_fill(_OutTy& _Dest, size_t _Dest_index, const _InTy& _Source, size_t _Source_index) {
		_Dest[_Dest_index] = _Source[_Source_index];
	}
	
	template<typename _OutTy, typename _InTy, typename ..._Tys>
	void _Shuffle_fill(_OutTy& _Dest, size_t _Dest_index, const _InTy& _Source, size_t _Source_index, _Tys... _Args) {
		_Dest[_Dest_index] = _Source[_Source_index];
		_Shuffle_fill(_Dest, _Dest_index + 1, _Source, _Args...);
	}

	template<typename _OutTy, typename _InTy> inline
	_OutTy shuffle(const _InTy& _Source, size_t i0) {
		return _OutTy{ _Source[i0] };
	}

	template<typename _OutTy, typename _InTy> inline
	_OutTy shuffle(const _InTy& _Source, size_t i0, size_t i1) {
		return _OutTy{ _Source[i0], _Source[i1] };
	}

	template<typename _OutTy, typename _InTy> inline
	_OutTy shuffle(const _InTy& _Source, size_t i0, size_t i1, size_t i2) {
		return _OutTy{ _Source[i0], _Source[i1], _Source[i2] };
	}

	template<typename _OutTy, typename _InTy> inline
	_OutTy shuffle(const _InTy& _Source, size_t i0, size_t i1, size_t i2, size_t i3) {
		return _OutTy{ _Source[i0], _Source[i1], _Source[i2], _Source[i3] };
	}

	template<typename _OutTy, typename _InTy> inline
	_OutTy shuffle(const _InTy& _Source, size_t i0, size_t i1, size_t i2, size_t i3, size_t i4) {
		return _OutTy{ _Source[i0], _Source[i1], _Source[i2], _Source[i3], _Source[i4] };
	}

	template<typename _OutTy, typename _InTy> inline
	_OutTy shuffle(const _InTy& _Source, size_t i0, size_t i1, size_t i2, size_t i3, size_t i4, size_t i5) {
		return _OutTy{ _Source[i0], _Source[i1], _Source[i2], _Source[i3], _Source[i4], _Source[i5] };
	}

	template<typename _OutTy, typename _InTy, typename ..._Tys> inline
	_OutTy shuffle(const _InTy& _Source, size_t i0, size_t i1, size_t i2, size_t i3, size_t i4, size_t i5, _Tys... _Selector) {
		_OutTy _Dest = shuffle<_OutTy>(_Source, i0, i1, i2, i3, i4, i5);
		_Shuffle_fill(_Dest, 6, _Source, _Selector...);
		return (_Dest);
	}

	template<typename _OutTy, typename _InTy, typename ..._Tys> inline
	void shuffle(const _InTy& _Source, _OutTy& _Dest, _Tys... _Selector) {
		_Shuffle_fill(_Dest, 0, _Source, _Selector...);
	}

	// <shuffle> is often used in vector mathematics
	// auto v2 = shuffle<vector3>(v1, 1, 0, 1);

	template<typename ..._Tys> constexpr 
	size_t types_size_v = std::tuple_size_v<std::tuple<_Tys...>>;

// 6.   subvector *--------------------------------------------------------- L820
	
	namespace Internal {
		// { Internal struct }
		template<typename _Ts, typename _Tb>
		struct _Subvector_data {
		protected:
			_Ts*   _My_first  = nullptr;
			_Ts*   _My_last   = nullptr;
			size_t _My_stride = 1;
			bool   _Is_owner  = false;
		public:

			~_Subvector_data() {
				if (_Is_owner) {
					delete[] _My_first;
					_My_first = nullptr;
					_My_last  = nullptr;
					_Is_owner = false;
				}
			}
		
			// property
			size_t size() const {
				return (_My_last - _My_first) / _My_stride; 
			}
			size_t stride() const {
				return _My_stride;
			}
			bool empty() const {
				return (_My_first == _My_last);
			}
			bool continuous() const {
				return (_My_stride == 1);
			}
			vector_size<_Ts, _Tb> vsize() const {
				return vector_size<_Ts, _Tb>(_My_first, _My_last, _My_stride); 
			}
		
			// iterator
			_Ts* begin() {
				return _My_first;
			}
			_Ts* end() {
				return _My_last;
			}
			const _Ts* begin() const {
				return _My_first;
			}
			const _Ts* end() const {
				return _My_last;
			}
		
			// index operation
			_Ts* ptr(size_t _Pos = 0) {
				return (_My_first + _Pos * _My_stride);
			}
			_Ts& at(size_t _Pos) {
				return *(ptr(_Pos));
			}
			_Ts& operator[](size_t _Pos) {
				return at(_Pos);
			}
			const _Ts* ptr(size_t _Pos = 0) const {
				return (_My_first + _Pos * _My_stride);
			}
			const _Ts& at(size_t _Pos) const {
				return *(ptr(_Pos));
			}
			const _Ts& operator[](size_t _Pos) const {
				return at(_Pos);
			}
		
			// string
			std::string to_string() const {
				assert(!this->empty());
				auto       _First = this->begin();
				const auto _Last  = this->end();
				std::string _Str  = std::to_string(*_First); _First += _My_stride;
				for (; _First != _Last; _First += _My_stride) {
					_Str += ',';
					_Str += std::to_string(*_First);
				}
				return std::move('{' + _Str + '}');
			}
		};
	}

	// { Internal struct }
	template<typename _Ts, typename _Tb>
	class subvector : public Internal::_Subvector_data<_Ts, _Tb> {
		using _Mybase = Internal::_Subvector_data<_Ts, _Tb>;

	public:
		using block_type   = _Tb;
		using scalar_type  = _Ts;
		using block_traits = clmagic::block_traits<block_type>;

		// for_each
		template<typename _UnaryOp>
		void for_each(_UnaryOp _Transform_op) {
			auto_vector_operation<scalar_type, block_type>::transform(
				this->begin()/*_First*/, 
				this->end()  /*_Last*/,
				this->begin()/*_Dest*/, _Transform_op, this->stride(), this->stride());
		}
		template<typename _BinOp>
		void for_each(const subvector& _Args2, _BinOp _Transform_op) {
			assert(this->size() == _Args2.size());
			auto_vector_operation<scalar_type, block_type>::transform(
				this->begin()/*_First1*/,
				this->end()  /*_Last1*/,
			   _Args2.begin()/*_First2*/,
				this->begin()/*_Dest*/, _Transform_op, this->stride(), _Args2.stride(), this->stride());
		}
		template<typename _UnaryOp>
		subvector for_each_copy(_UnaryOp _Transform_op) const {
			subvector _Result = subvector(this->size());
			auto_vector_operation<scalar_type, block_type>::transform(
				this->begin() /*_First*/, 
				this->end()   /*_Last*/,
			   _Result.begin()/*_Dest*/, _Transform_op, this->stride(), _Result.stride());
			return std::move(_Result);
		}
		template<typename _BinOp>
		subvector for_each_copy(const subvector& _Args2, _BinOp _Transform_op) const {
			assert( this->size() == _Args2.size() );
			subvector _Result = subvector(this->size());
			auto_vector_operation<scalar_type, block_type>::transform(
				this->begin()/*_First1*/,
				this->end()  /*_Last1*/,
			   _Args2.begin()/*_First2*/,
			   _Result.begin()/*_Dest*/, _Transform_op, this->stride(), _Args2.stride(), _Result.stride());
			return std::move(_Result);
		}
		
		// accmulate	
		template<typename _BinOp>
		scalar_type accumulate(scalar_type _Val, _BinOp _Reduce_op) const {
			return auto_vector_operation<scalar_type, block_type>::accumulate(
				this->begin()/*_First*/, 
				this->end()  /*_Last*/, _Val, _Reduce_op, this->stride());
		}
		template<typename _BinOp, typename _UnaryOp>
		scalar_type accumulate(scalar_type _Val, _BinOp _Reduce_op, _UnaryOp _Transform_op) const {
			return auto_vector_operation<scalar_type, block_type>::transform_accumulate(
				this->begin()/*_First*/, 
				this->end()  /*_Last*/, _Val, _Reduce_op, _Transform_op, this->stride());
		}
		template<typename _BinOp1, typename _BinOp2>
		scalar_type accumulate(const subvector& _Args2, scalar_type _Val, _BinOp1 _Reduce_op, _BinOp2 _Transform_op) const {
			return auto_vector_operation<scalar_type, block_type>::transform_accumulate(
				this->begin()/*_First1*/,
				this->end()  /*_Last1*/,
			   _Args2.begin()/*_First2*/, _Val, _Reduce_op, _Transform_op, this->stride(), _Args2.stride());
		}

		// constructor
		subvector() = default;
		subvector(const subvector& _Right) = default;
		subvector(subvector&& _Right) noexcept {
			*this = std::move(_Right);
		}
		subvector(scalar_type* _First, scalar_type* _Last) {
			_Mybase::_My_first  = _First;
			_Mybase::_My_last   = _Last;
			_Mybase::_My_stride = 1;
			_Mybase::_Is_owner  = false;
		}
		subvector(scalar_type* _First, scalar_type* _Last, size_t _Stride) {
			_Mybase::_My_first  = _First;
			_Mybase::_My_last   = _Last;
			_Mybase::_My_stride = _Stride;
			_Mybase::_Is_owner  = false;
		}
		explicit subvector(size_t _Count) {
			_Mybase::_Is_owner  = true;
			_Mybase::_My_first  = new scalar_type[_Count];
			_Mybase::_My_last   = _Mybase::_My_first + _Count;
			_Mybase::_My_stride = 1;
		}
		
		// assign operator
		subvector& operator=(const subvector& _Right) {
			if (&_Right == this) {
				return *this;
			}

			if (_Mybase::_My_first != nullptr) {
				assert( this->size() == _Right.size() );
				auto       _First = _Right.begin();
				const auto _Last  = _Right.end();
				auto       _Dest  = this->begin();
				for ( ; _First != _Last; _First += _Right.stride(), _Dest += this->stride()) {
					*_Dest = *_First;
				}
				return *this;
			} else {
				auto _New_vec = subvector(_Right.size());
				     _New_vec = _Right;
				return ( *this = std::move(_New_vec) );
			}
		}
		subvector& operator=(subvector&& _Right) noexcept {
			_Mybase::_My_first  = _Right._My_first;
			_Mybase::_My_last   = _Right._My_last;
			_Mybase::_My_stride = _Right._My_stride;
			_Mybase::_Is_owner  = _Right._Is_owner;
			_Right._My_first = nullptr;
			_Right._My_last  = nullptr;
			_Right._Is_owner = false;
			return *this;
		}
		
		// arthmetic operator	
		subvector  operator- () const {
			return std::move( this->for_each_copy(std::negate<>()) );
		}
		subvector  operator+ (const subvector& _Right) const {
			return std::move( this->for_each_copy(_Right, std::plus<>()) );
		}
		subvector  operator- (const subvector& _Right) const {
			return std::move( this->for_each_copy(_Right, std::minus<>()) );
		}
		subvector  operator* (const subvector& _Right) const {
			return std::move( this->for_each_copy(_Right, std::multiplies<>()) );
		}
		subvector  operator/ (const subvector& _Right) const {
			return std::move( this->for_each_copy(_Right, std::divides<>()) );
		}
		subvector  operator% (const subvector& _Right) const {
			return std::move( this->for_each_copy(_Right, std::modulus<>()) );
		}
		subvector& operator+=(const subvector& _Right) {
			this->for_each(_Right, std::plus<>());
			return *this;
		}
		subvector& operator-=(const subvector& _Right) {
			this->for_each(_Right, std::minus<>());
			return *this;
		}
		subvector& operator*=(const subvector& _Right) {
			this->for_each(_Right, std::multiplies<>());
			return *this;
		}
		subvector& operator/=(const subvector& _Right) {
			this->for_each(_Right, std::divides<>());
			return *this;
		}
		subvector& operator%=(const subvector& _Right) {
			this->for_each(_Right, std::modulus<>());
			return *this;
		}
		subvector  operator+ (const scalar_type& _Scalar) const {
			return std::move(
				this->for_each_copy(
					left_varying_op_by( std::plus<>(), block_traits::construct1(_Scalar) )
				)
			);
		}
		subvector  operator- (const scalar_type& _Scalar) const {
			return std::move(
				this->for_each_copy(
					left_varying_op_by( std::minus<>(), block_traits::construct1(_Scalar) )
				)
			);
		}
		subvector  operator* (const scalar_type& _Scalar) const {
			return std::move(
				this->for_each_copy(
					left_varying_op_by( std::multiplies<>(), block_traits::construct1(_Scalar) )
				)
			);
		}
		subvector  operator/ (const scalar_type& _Scalar) const {
			return std::move(
				this->for_each_copy(
					left_varying_op_by( std::divides<>(), block_traits::construct1(_Scalar) )
				)
			);
		}
		subvector  operator% (const scalar_type& _Scalar) const {
			return std::move(
				this->for_each_copy(
					left_varying_op_by( std::modulus<>(), block_traits::construct1(_Scalar) )
				)
			);
		}
		subvector& operator+=(const scalar_type& _Scalar) {
			this->for_each( left_varying_op_by( std::plus<>(), block_traits::construct1(_Scalar) ) );
			return *this;
		}
		subvector& operator-=(const scalar_type& _Scalar) {
			this->for_each( left_varying_op_by( std::minus<>(), block_traits::construct1(_Scalar) ) );
			return *this;
		}
		subvector& operator*=(const scalar_type& _Scalar) {
			this->for_each( left_varying_op_by( std::multiplies<>(), block_traits::construct1(_Scalar) ) );
			return *this;
		}
		subvector& operator/=(const scalar_type& _Scalar) {
			this->for_each( left_varying_op_by( std::divides<>(), block_traits::construct1(_Scalar) ) );
			return *this;
		}
		subvector& operator%=(const scalar_type& _Scalar) {
			this->for_each( left_varying_op_by( std::modulus<>(), block_traits::construct1(_Scalar) ) );
			return *this;
		}
		friend subvector operator+(const scalar_type& _Left, const subvector& _Right) {
			return std::move(
				_Right.for_each_copy(
					right_varying_op_by( std::plus<>(), block_traits::construct1(_Left) )
				)
			);
		}
		friend subvector operator-(const scalar_type& _Left, const subvector& _Right) {
			return std::move(
				_Right.for_each_copy(
					right_varying_op_by( std::minus<>(), block_traits::construct1(_Left) )
				)
			);
		}
		friend subvector operator*(const scalar_type& _Left, const subvector& _Right) {
			return std::move(
				_Right.for_each_copy(
					right_varying_op_by( std::multiplies<>(), block_traits::construct1(_Left) )
				)
			);
		}
		friend subvector operator/(const scalar_type& _Left, const subvector& _Right) {
			return std::move(
				_Right.for_each_copy(
					right_varying_op_by( std::divides<>(), block_traits::construct1(_Left) )
				)
			);
		}
		friend subvector operator%(const scalar_type& _Left, const subvector& _Right) {
			return std::move(
				_Right.for_each_copy(
					right_varying_op_by( std::modulus<>(), block_traits::construct1(_Left) )
				)
			);
		}
		
		// vector operator
		scalar_type dot(const subvector& _Right) const {
			return this->accumulate(_Right, static_cast<scalar_type>(0), std::plus<>()/*_Reduce_op*/, std::multiplies<>()/*_Transform_op*/);
		}
		scalar_type norm(const scalar_type& L) const {
			const auto _Transform_op = left_varying_op_by( [](auto&& _Left, auto&& _Right){ return pow(_Left, _Right); }, block_traits::construct1(L) );
			const auto _Result       = this->accumulate(static_cast<scalar_type>(0), std::plus<>(), _Transform_op);
			return pow(_Result, 1/L);
		}
		scalar_type length() const {
			scalar_type _Length_sqr = this->dot(*this);
			return sqrt(_Length_sqr);
		}
		void normalize() {
			scalar_type _Length_sqr = this->dot(*this);
			scalar_type _Error      = _Length_sqr - 1;
			if (abs(_Error) > std::numeric_limits<scalar_type>::epsilon()) {
				(*this) /= sqrt(_Length_sqr);
			}
		}
		
		// IO operator
		friend std::istream& operator>>(std::istream& _Istr, subvector _Left) {
			auto       _First = _Left.begin();
			const auto _Last  = _Left.end();
			for ( ; _First != _Last; _First += _Left.stride()) {
				_Istr >> *_First;
			}
			return _Istr;
		}
		friend std::ostream& operator<<(std::ostream& _Ostr, const subvector& _Left) {
			return (_Ostr << _Left.to_string());
		}
	};

	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> fmod(const subvector<_Ts, _Tb>& _Left, const subvector<_Ts, _Tb>& _Right) {// requires: finded function fmod(_Tb, _Tb)
		assert( !_Left.empty() );
		assert( _Left.size() == _Right.size() );
		return std::move( 
			_Left.for_each_copy( _Right, RET_LAMBDA_2ND(fmod,auto&&,auto&&) )
		);
	}
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> fmod(const subvector<_Ts, _Tb>& _Left, _Ts _Right) {// requires: finded function fmod(_Tb, _Tb)
		assert( !_Left.empty() );
		return std::move(
			_Left.for_each_copy(
				left_varying_op_by( RET_LAMBDA_2ND(fmod,auto&&,auto&&), block_traits<_Tb>::construct1(_Right) )
			));
	}
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> pow(const subvector<_Ts, _Tb>& _Left, const subvector<_Ts, _Tb>& _Right) {// requires: finded function pow(_Tb, _Tb)
		assert( !_Left.empty() );
		assert( _Left.size() == _Right.size() );
		return std::move( 
			_Left.for_each_copy( _Right, RET_LAMBDA_2ND(pow,auto&&,auto&&) )
		);
	}
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> pow(const subvector<_Ts, _Tb>& _Left, _Ts _Right) {// requires: finded function pow(_Tb, _Tb)
		assert( !_Left.empty() );
		return std::move(
			_Left.for_each_copy(
				left_varying_op_by( RET_LAMBDA_2ND(pow,auto&&,auto&&), block_traits<_Tb>::construct1(_Right) )
			));
	}
	// numeric0
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> abs(const subvector<_Ts, _Tb>& _Left) {
		return std::move( _Left.for_each_copy(RET_LAMBDA_1ST(abs, auto&&)) );
	}
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> trunc(const subvector<_Ts, _Tb>& _Left) {
		return std::move( _Left.for_each_copy(RET_LAMBDA_1ST(trunc, auto&&)) );
	}
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> floor(const subvector<_Ts, _Tb>& _Left) {
		return std::move( _Left.for_each_copy(RET_LAMBDA_1ST(floor, auto&&)) );
	}
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> ceil(const subvector<_Ts, _Tb>& _Left) {
		return std::move( _Left.for_each_copy(RET_LAMBDA_1ST(ceil, auto&&)) );
	}
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> round(const subvector<_Ts, _Tb>& _Left) {
		return std::move( _Left.for_each_copy(RET_LAMBDA_1ST(round, auto&&)) );
	}
	// numeric1
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> sqrt(const subvector<_Ts, _Tb>& _Left) {
		return std::move( _Left.for_each_copy(RET_LAMBDA_1ST(sqrt, auto&&)) );
	}
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> cbrt(const subvector<_Ts, _Tb>& _Left) {
		return std::move( _Left.for_each_copy(RET_LAMBDA_1ST(cbrt, auto&&)) );
	}
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> invsqrt(const subvector<_Ts, _Tb>& _Left) {
		return std::move( _Left.for_each_copy(RET_LAMBDA_1ST(invsqrt, auto&&)) );
	}
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> invcbrt(const subvector<_Ts, _Tb>& _Left) {
		return std::move( _Left.for_each_copy(RET_LAMBDA_1ST(invcbrt, auto&&)) );
	}
	// trigonometric
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> sin(const subvector<_Ts, _Tb>& _Left) {// requires: finded function sin(_Ts|_Tb)
		return std::move( _Left.for_each_copy( RET_LAMBDA_1ST(sin, auto&&) ) );
	}
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> cos(const subvector<_Ts, _Tb>& _Left) {// requires: finded function cos(_Ts|_Tb)
		return std::move( _Left.for_each_copy( RET_LAMBDA_1ST(cos, auto&&) ) );
	}
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> tan(const subvector<_Ts, _Tb>& _Left) {// requires: finded function tan(_Ts|_Tb)
		return std::move( _Left.for_each_copy( RET_LAMBDA_1ST(tan, auto&&) ) );
	}
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> asin(const subvector<_Ts, _Tb>& _Left) {// requires: finded function asin(_Ts|_Tb)
		return std::move( _Left.for_each_copy( RET_LAMBDA_1ST(asin, auto&&) ) );
	}
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> acos(const subvector<_Ts, _Tb>& _Left) {// requires: finded function acos(_Ts|_Tb)
		return std::move( _Left.for_each_copy( RET_LAMBDA_1ST(acos, auto&&) ) );
	}
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> atan(const subvector<_Ts, _Tb>& _Left) {// requires: finded function atan(_Ts|_Tb)
		return std::move( _Left.for_each_copy( RET_LAMBDA_1ST(atan, auto&&) ) );
	}
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> atan2(const subvector<_Ts, _Tb>& _Left, const subvector<_Ts, _Tb>& _Right) {// requires: finded function atan2(_Ts, _Ts|_Tb, _Tb)
		return std::move( _Left.for_each_copy(_Right, RET_LAMBDA_2ND(atan2, auto&&, auto&&)) );
	}
	// vector
	template<typename _Ts, typename _Tb> inline
	_Ts norm(const subvector<_Ts, _Tb>& _Left, _Ts L) {
		return _Left.norm(L);
	}

// 7.   vector *------------------------------------------------------------ L1319

	// { vector, static size }
	template<size_t _Size, typename _Ts = scalar_t<void>, typename _Tb = _Ts>
	class __declspec(align(std::alignment_of_v<_Tb>))
		vector {
	public:
		static_assert(_Size != 0, "clmagic::error");
		//static_assert(std::is_arithmetic_v<_Ts>, "strict math vector");

		using block_type     = _Tb;
		using scalar_type    = _Ts;
		using subvector_type = clmagic::subvector< scalar_type, block_type >;

		using _My_block_traits   = clmagic::block_traits<_Tb>;
		using _My_aligned_traits = clmagic::aligned_traits<_Ts, _Size, _Tb>;

		_Ts _Mydata[ _My_aligned_traits::aligned_size ];

#pragma region property
		static constexpr size_t size() {
			return _Size; 
		}
		static constexpr size_t block_size() {
			return (_Size / _My_block_traits::size());
		}
		static constexpr size_t tail_size() {
			return (_Size - block_size() * _My_block_traits::size());
		}
#pragma endregion
		
#pragma region traverse
		scalar_type* ptr(size_t _Pos = 0) {
			return (_Mydata + _Pos);
		}
		scalar_type& at(size_t _Pos) {
			assert(_Pos < _Size);
			return _Mydata[_Pos];
		}
		scalar_type& operator[](size_t _Pos) {
			return this->at(_Pos);
		}
		const scalar_type* ptr(size_t _Pos = 0) const {
			return (_Mydata + _Pos);
		}
		const scalar_type& at(size_t _Pos) const {
			assert(_Pos < _Size);
			return _Mydata[_Pos];
		}
		const scalar_type& operator[](size_t _Pos) const {
			return this->at(_Pos);
		}

		template<typename _Ty = scalar_type>
		_Ty* begin() {
			return reinterpret_cast<_Ty*>(_Mydata);
		}
		template<typename _Ty = scalar_type>
		_Ty* end() {
			return reinterpret_cast<_Ty*>(_Mydata + _Size);
		}
		template<typename _Ty = scalar_type>
		const _Ty* begin() const {
			return reinterpret_cast<const _Ty*>(_Mydata);
		}
		template<typename _Ty = scalar_type>
		const _Ty* end() const {
			return reinterpret_cast<const _Ty*>(_Mydata + _Size);
		}
#pragma endregion

#pragma region assign
		void _Correct_tail_elements() {
			if _CONSTEXPR_IF ( _My_aligned_traits::invalid_size != 0 ) {
				std::fill(end(), end()+_My_aligned_traits::invalid_size, static_cast<scalar_type>(0));
			}
		}
		template<typename _Iter>
		void assign(_Iter _First, _Iter _Last) {
			assert( _STD distance(_First, _Last) <= _STD _Iter_diff_t<_Iter>(this->size()) );
			auto _Dest = std::copy(_First, _Last, this->begin<scalar_type>());
			std::fill(_Dest, this->end<scalar_type>(), static_cast<scalar_type>(0));
		}
		void assign(const scalar_type& _Val) {
			std::fill(this->begin<scalar_type>(), this->end<scalar_type>(), _Val);
		}
#pragma endregion

#pragma region string
		std::string to_string() const {
			auto       _First = this->begin<scalar_type>();
			const auto _Last  = this->end<scalar_type>();
			std::string _Str  = std::to_string(*_First++);
			for ( ; _First != _Last; ++_First) {
				_Str += ',';
				_Str += std::to_string(*_First);
			}
			return '{' + _Str + '}';
		}
#pragma endregion

#pragma region for_each
		template<typename _UnaryOp>
		void for_each_scalar(_UnaryOp _Transform_op) {// for each scalar by (_Transform_op*)(scalar_type)
			std::transform(this->begin<scalar_type>(), 
				           this->end<scalar_type>(), 
				           this->begin<scalar_type>(), _Transform_op);
		}
		template<typename _BinOp>
		void for_each_scalar(const vector& _Args2, _BinOp _Transform_op) {
			std::transform(this->begin<scalar_type>(),
				           this->end<scalar_type>(), 
				          _Args2.begin<scalar_type>(), 
				           this->end<scalar_type>(), _Transform_op);
		}
		template<typename _UnaryOp>
		vector for_each_scalar_copy(_UnaryOp _Transform_op) const {
			vector _Result;
			std::transform(this->begin<scalar_type>(), 
				           this->end<scalar_type>(), 
				          _Result.begin<scalar_type>(), _Transform_op);
			_Result._Correct_tail_elements();
			return _Result;
		}
		template<typename _BinOp>
		vector for_each_scalar_copy(const vector& _Args2, _BinOp _Transform_op) const {
			vector _Result;
			std::transform(this->begin<scalar_type>(), 
				           this->end<scalar_type>(), 
				          _Args2.begin<scalar_type>(), 
				          _Result.begin<scalar_type>(), _Transform_op);
			_Result._Correct_tail_elements();
			return _Result;
		}
		template<typename _UnaryOp>
		void for_each_block(_UnaryOp _Transform_op) {// for each block by (_Transform_op*)(block_type)
			if _CONSTEXPR_IF( this->tail_size() == 0 ){
				fastest_vector_operation<scalar_type, block_type>::transform(
					this->begin<block_type>()/*_First*/, 
					this->end<block_type>()  /*_Last*/, 
					this->begin<block_type>()/*_Dest*/, _Transform_op );
			} else{
				fast_vector_operation<scalar_type, block_type>::transform(
					this->begin<block_type>(),
					this->begin<block_type>() + this->block_size(),
					this->begin<block_type>(), _Transform_op, 0/*leading_size*/, this->tail_size());
			}
		}
		template<typename _BinOp>
		void for_each_block(const vector& _Args2, _BinOp _Transform_op) {
			if _CONSTEXPR_IF( this->tail_size() == 0 ) {
				fastest_vector_operation<scalar_type, block_type>::transform(
					this->begin<block_type>()/*_First1*/, 
					this->end<block_type>()  /*_Last1*/, 
				   _Args2.begin<block_type>()/*_First2*/,
					this->begin<block_type>()/*_Dest*/, _Transform_op );
			} else {
				fast_vector_operation<scalar_type, block_type>::transform(
					this->begin<block_type>(),
					this->begin<block_type>() + this->block_size(),
				   _Args2.begin<block_type>(),
					this->begin<block_type>(), _Transform_op, 0/*leading_size*/, this->tail_size());
			}
		}
		template<typename _UnaryOp>
		vector for_each_block_copy(_UnaryOp _Transform_op) const {
			vector _Result;
			if _CONSTEXPR_IF( this->tail_size() == 0 ) {
				fastest_vector_operation<scalar_type, block_type>::transform(
					this->begin<block_type>(),
					this->end<block_type>(),
				   _Result.begin<block_type>(), _Transform_op);
			} else {
				fast_vector_operation<scalar_type, block_type>::transform(
					this->begin<block_type>(),
					this->begin<block_type>() + this->block_size(),
				   _Result.begin<block_type>(), _Transform_op, 0/*leading_size*/, this->tail_size());
			}
			return _Result;
		}
		template<typename _BinOp>
		vector for_each_block_copy(const vector& _Args2, _BinOp _Transform_op) const {
			vector _Result;
			if _CONSTEXPR_IF( this->tail_size() == 0 ) {
				fastest_vector_operation<scalar_type, block_type>::transform(
					this->begin<block_type>(),
					this->end<block_type>(),
				   _Args2.begin<block_type>(),
				   _Result.begin<block_type>(), _Transform_op);
			} else {
				fast_vector_operation<scalar_type, block_type>::transform(
					this->begin<block_type>(),
					this->begin<block_type>() + this->block_size(),
				   _Args2.begin<block_type>(),
				   _Result.begin<block_type>(), _Transform_op, 0/*leading_size*/, this->tail_size());
			}
			return _Result;
		}
		
		// accmulate	
		template<typename _BinOp>
		scalar_type accumulate(scalar_type _Val, _BinOp _Reduce_op) const {
			if _CONSTEXPR_IF( this->tail_size() == 0 ){
				return fastest_vector_operation<scalar_type, block_type>::accumulate(
					this->begin<block_type>()/*_First*/, 
					this->end<block_type>()  /*_Last*/, _Val, _Reduce_op);
			} else{
				return fast_vector_operation<scalar_type, block_type>::accumulate(
					this->begin<block_type>(),
					this->begin<block_type>() + this->block_size(), _Val, _Reduce_op, 0/*leading_size*/, this->tail_size());
			}
		}
		template<typename _BinOp, typename _UnaryOp>
		scalar_type accumulate(scalar_type _Val, _BinOp _Reduce_op, _UnaryOp _Transform_op) const {
			if _CONSTEXPR_IF( this->tail_size() == 0 ){
				return fastest_vector_operation<scalar_type, block_type>::transform_accumulate(
					this->begin<block_type>()/*_First*/, 
					this->end<block_type>()  /*_Last*/, _Val, _Reduce_op, _Transform_op);
			} else{
				return fast_vector_operation<scalar_type, block_type>::transform_accumulate(
					this->begin<block_type>(),
					this->begin<block_type>() + this->block_size(), _Val, _Reduce_op, _Transform_op, 0/*leading_size*/, this->tail_size());
			}
		}
		template<typename _BinOp1, typename _BinOp2>
		scalar_type accumulate(const vector& _Args2, scalar_type _Val, _BinOp1 _Reduce_op, _BinOp2 _Transform_op) const {
			if _CONSTEXPR_IF( this->tail_size() == 0 ){
				return fastest_vector_operation<scalar_type, block_type>::transform_accumulate(
					this->begin<block_type>()/*_First*/, 
					this->end<block_type>()  /*_Last*/,
				   _Args2.begin<block_type>(), _Val, _Reduce_op, _Transform_op);
			} else{
				return fast_vector_operation<scalar_type, block_type>::transform_accumulate(
					this->begin<block_type>(),
					this->begin<block_type>() + this->block_size(),
				   _Args2.begin<block_type>(), _Val, _Reduce_op, _Transform_op, 0/*leading_size*/, this->tail_size());
			}
		}
#pragma endregion

#pragma region constructor
		vector() = default;
		explicit vector(const scalar_type& _Val) { 
			this->assign(_Val); 
			this->_Correct_tail_elements();
		}
		template<typename ..._Tys>
		vector(const scalar_type& x0, const scalar_type& x1, const _Tys&... xN) : _Mydata{ x0, x1, xN... } {
			static_assert((2 + types_size_v<_Tys...>) <= _Size, "clmagic::Error");
			this->_Correct_tail_elements();
		}
		template<typename _Iter> 
		vector(_Iter _First, _Iter _Last) { 
			this->assign(_First, _Last); 
			this->_Correct_tail_elements();
		}
#pragma endregion

#pragma region assign_operator
		vector& operator=(const vector&) = default;
		vector& operator=(std::initializer_list<scalar_type> _Ilist) {
			this->assign(_Ilist.begin(), _Ilist.end());
			return *this;
		}
#pragma endregion

#pragma region function_operator
		subvector_type operator()() {
			return subvector_type(this->begin(), this->end());
		}
		subvector_type operator()(size_t _First, size_t _Last) {
			return subvector_type(this->ptr(_First), this->ptr(_Last));
		}
		const subvector_type operator()() const {
			return const_cast<vector&>(*this).operator()();
		}
		const subvector_type operator()(size_t _First, size_t _Last) const {
			return const_cast<vector&>(*this).operator()(_First, _Last);
		}
#pragma endregion

#pragma region compare_operator
		bool operator==(const vector& _Right) const {
			for (size_t i = 0; i != this->size(); ++i) {
				const scalar_type _Error = (*this)[i] - _Right[i];
				if (abs(_Error) > std::numeric_limits<scalar_type>::epsilon()) {
					return false;
				}
			}
			return true;
		}
		bool operator!=(const vector& _Right) const {
			return !((*this) == _Right);
		}
		bool operator< (const vector& _Right) const {
			return this->length_squared() < _Right.length_squared();
		}
		bool operator> (const vector& _Right) const {
			return this->length_squared() > _Right.length_squared();
		}
		bool operator<=(const vector& _Right) const {
			return !((*this) > _Right);
		}
		bool operator>=(const vector& _Right) const {
			return !((*this) < _Right);
		}
#pragma endregion
		
#pragma region arthmetic_operator
		vector  operator-() const {
			return this->for_each_block_copy( std::negate<block_type>() );
		}
		vector  operator+ (const vector& _Right) const {
			return this->for_each_block_copy(_Right, std::plus<block_type>());
		}
		vector  operator- (const vector& _Right) const {
			return this->for_each_block_copy(_Right, std::minus<block_type>());
		}
		vector  operator* (const vector& _Right) const {
			return this->for_each_block_copy(_Right, std::multiplies<block_type>());
		}
		vector  operator/ (const vector& _Right) const {
			return this->for_each_block_copy(_Right, std::divides<block_type>());
		}
		vector  operator% (const vector& _Right) const {
			return this->for_each_block_copy(_Right, std::modulus<block_type>());
		}
		vector& operator+=(const vector& _Right) {
			this->for_each_block(_Right, std::plus<block_type>());
			return *this;
		}
		vector& operator-=(const vector& _Right) {
			this->for_each_block(_Right, std::minus<block_type>());
			return *this;
		}
		vector& operator*=(const vector& _Right) {
			this->for_each_block(_Right, std::multiplies<block_type>());
			return *this;
		}
		vector& operator/=(const vector& _Right) {
			this->for_each_block(_Right, std::divides<block_type>());
			return *this;
		}
		vector& operator%=(const vector& _Right) {
			this->for_each_block(_Right, std::modulus<block_type>());
			return *this;
		}
		vector  operator+ (const scalar_type& _Scalar) const {
			return this->for_each_block_copy(
				left_varying_op_by(std::plus<block_type>(), _My_block_traits::construct1(_Scalar)) );
		}
		vector  operator- (const scalar_type& _Scalar) const {
			return this->for_each_block_copy(
				left_varying_op_by(std::minus<block_type>(), block_traits::construct1(_Scalar)) );
		}
		vector  operator* (const scalar_type& _Scalar) const {
			return this->for_each_block_copy(
				left_varying_op_by(std::multiplies<block_type>(), _My_block_traits::construct1(_Scalar)) );
		}
		vector  operator/ (const scalar_type& _Scalar) const {
			return this->for_each_block_copy(
				left_varying_op_by(std::divides<block_type>(), _My_block_traits::construct1(_Scalar)) );
		}
		vector  operator% (const scalar_type& _Scalar) const {
			return this->for_each_block_copy(
				left_varying_op_by(std::modulus<block_type>(), _My_block_traits::construct1(_Scalar)) );
		}
		vector& operator+=(const scalar_type& _Scalar) {
			this->for_each_block(
				left_varying_op_by(std::plus<block_type>(), _My_block_traits::construct1(_Scalar)) );
			return *this;
		}
		vector& operator-=(const scalar_type& _Scalar) {
			this->for_each_block(
				left_varying_op_by(std::minus<block_type>(), _My_block_traits::construct1(_Scalar)) );
			return *this;
		}
		vector& operator*=(const scalar_type& _Scalar) {
			this->for_each_block(
				left_varying_op_by(std::multiplies<block_type>(), _My_block_traits::construct1(_Scalar)) );
			return *this;
		}
		vector& operator/=(const scalar_type& _Scalar) {
			this->for_each_block(
				left_varying_op_by(std::divides<block_type>(), _My_block_traits::construct1(_Scalar)) );
			return *this;
		}
		vector& operator%=(const scalar_type& _Scalar) {
			this->for_each_block(
				left_varying_op_by(std::modulus<block_type>(), _My_block_traits::construct1(_Scalar)) );
			return *this;
		}
		friend vector operator+(const scalar_type& _Scalar, const vector& _Right) {
			return _Right.for_each_block_copy(
				right_varying_op_by(std::plus<block_type>(), _My_block_traits::construct1(_Scalar)) );
		}
		friend vector operator-(const scalar_type& _Scalar, const vector& _Right) {
			return _Right.for_each_block_copy(
				right_varying_op_by(std::minus<block_type>(), block_traits::construct1(_Scalar)) );
		}
		friend vector operator*(const scalar_type& _Scalar, const vector& _Right) {
			return _Right.for_each_block_copy(
				right_varying_op_by(std::multiplies<block_type>(), block_traits::construct1(_Scalar)) );
		}
		friend vector operator/(const scalar_type& _Scalar, const vector& _Right) {
			return _Right.for_each_block_copy(
				right_varying_op_by(std::divides<block_type>(), block_traits::construct1(_Scalar)) );
		}
		friend vector operator%(const scalar_type& _Scalar, const vector& _Right) {
			return _Right.for_each_block_copy(
				right_varying_op_by(std::modulus<block_type>(), block_traits::construct1(_Scalar)) );
		}
#pragma endregion

#pragma region vector_operator
		scalar_type dot(const vector& _Right) const {// sum_of( (*this)[N]*_Right[N] )
			return this->accumulate(_Right, static_cast<scalar_type>(0), std::plus<>(), std::multiplies<>());
		}
		scalar_type norm(const scalar_type& L) const {// pow( sum_of( pow((*this)[N], L), 1/L )
			const auto _Transform_op = left_varying_op_by( [](block_type _Left, block_type _Right){ return pow(_Left, _Right); }, block_traits::construct1(L) );
			const auto _Result       = this->accumulate(static_cast<scalar_type>(0), std::plus<>(), _Transform_op);
			return pow(_Result, 1/L);
		}
		scalar_type length() const {// sqrt( dot(*this,*this) )
			scalar_type _Length_sqr = this->dot(*this);
			return sqrt(_Length_sqr);
		}
		scalar_type length_squared() const {
			return this->dot(*this);
		}
		void normalize() {
			scalar_type _Length_sqr = this->dot(*this);
			scalar_type _Error      = _Length_sqr - 1;
			if ( abs(_Error) > std::numeric_limits<scalar_type>::epsilon() ) {
				(*this) /= sqrt(_Length_sqr);
			}
		}
#pragma endregion

#pragma region IO_operator
		friend std::istream& operator>>(std::istream& _Istr, vector& _Left) {
			auto       _First = _Left.begin<_Ts>();
			const auto _Last  = _Left.end<_Ts>();
			for (; _First != _Last; ++_First) {
				_Istr >> (*_First);
			}
			return _Istr;
		}
		friend std::ostream& operator<<(std::ostream& _Ostr, const vector& _Left) {
			return (_Ostr << _Left.to_string());
		}
#pragma endregion
	};

	template<typename _Ts = scalar_t<void>, typename _Tb = _Ts>
		using vector2 = vector<2, _Ts, _Tb>;

	template<typename _Ts = scalar_t<void>, typename _Tb = _Ts>
		using vector3 = vector<3, _Ts, _Tb>;
	
	template<typename _Ts = scalar_t<void>, typename _Tb = _Ts>
		using vector4 = vector<4, _Ts, _Tb>;
	
	using point2_size_t = vector2<size_t>;

	#define VECTORN ::clmagic::vector<_Size, _Ts, _Tb>
	#define VECTOR2 ::clmagic::vector<2, _Ts, _Tb>
	#define VECTOR3 ::clmagic::vector<3, _Ts, _Tb>
	#define VECTOR4 ::clmagic::vector<4, _Ts, _Tb>

	template<typename _Ts, size_t _Size, typename _Tb> inline
	std::string to_string(const VECTORN& _Left) {
		return _Left.to_string();
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN fmod(const VECTORN& _Left, const VECTORN& _Right) {
		return _Left.for_each_block_copy( _Right, RET_LAMBDA_2ND(fmod, const _Tb&, const _Tb&) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN fmod(const VECTORN& _Left, const SCALAR& _Scalar) {
		return _Left.for_each_block_copy( 
			left_varying_op_by( RET_LAMBDA_2ND(fmod, const _Tb&, const _Tb&), block_traits<_Tb>::construct1(_Scalar) ) 
		);
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN pow(const VECTORN& _Left, const VECTORN& _Right) {
		return _Left.for_each_block_copy( _Right, RET_LAMBDA_2ND(pow, const _Tb&, const _Tb&) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN pow(const VECTORN& _Left, const SCALAR& _Scalar) {
		return _Left.for_each_block_copy(
			left_varying_op_by( RET_LAMBDA_2ND(pow, const _Tb&, const _Tb&), block_traits<_Tb>::construct1(_Scalar) ) 
		);
	}	
	// numeric0
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN abs(const VECTORN& _Left) {// abs(v)
		return _Left.for_each_block_copy( RET_LAMBDA_1ST(abs, const _Tb&) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN floor(const VECTORN& _Left) {// floor(v)
		return _Left.for_each_block_copy( RET_LAMBDA_1ST(floor, const _Tb&)  );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN ceil(const VECTORN& _Left) {// ceil(v)
		return _Left.for_each_block_copy( RET_LAMBDA_1ST(ceil, const _Tb&) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN trunc(const VECTORN& _Left) {// trunc(v)
		return _Left.for_each_block_copy( RET_LAMBDA_1ST(trunc, const _Tb&) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN round(const VECTORN& _Left) {// round(v)
		return _Left.for_each_block_copy( RET_LAMBDA_1ST(round, const _Tb&) );
	}
	// numeric1
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN sqrt(const VECTORN& _Left) {// sqrt(v)
		return _Left.for_each_block_copy( RET_LAMBDA_1ST(sqrt, const _Tb&) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN cbrt(const VECTORN& _Left) {// cbrt(v)
		return _Left.for_each_block_copy( RET_LAMBDA_1ST(cbrt, const _Tb&) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN invsqrt(const VECTORN& _Left) {// invsqrt(v)
		return _Left.for_each_block_copy( RET_LAMBDA_1ST(invsqrt, const _Tb&) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN invcbrt(const VECTORN& _Left) {// invcbrt(v)
		return _Left.for_each_block_copy( RET_LAMBDA_1ST(invcbrt, const _Tb&) );
	}
	// trigonometric
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN sin(const VECTORN& _Left) {
		return _Left.for_each_block_copy( RET_LAMBDA_1ST(sin, const _Tb&) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN cos(const VECTORN& _Left) {
		return _Left.for_each_block_copy( RET_LAMBDA_1ST(cos, const _Tb&) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN tan(const VECTORN& _Left) {
		return _Left.for_each_block_copy( RET_LAMBDA_1ST(tan, const _Tb&) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN asin(const VECTORN& _Left) {
		return _Left.for_each_block_copy( RET_LAMBDA_1ST(asin, const _Tb&) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN acos(const VECTORN& _Left) {
		return _Left.for_each_block_copy( RET_LAMBDA_1ST(acos, const _Tb&) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN atan(const VECTORN& _Left) {
		return _Left.for_each_block_copy( RET_LAMBDA_1ST(atan, const _Tb&) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN atan2(const VECTORN& _Left, const VECTORN& _Right) {
		return _Left.for_each_block_copy( RET_LAMBDA_2ND(atan2, const _Tb&, const _Tb&) );
	}
	// vector
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN cross3(const VECTORN& _Left, const VECTORN& _Right) {
		return VECTORN{
			_Left[1] * _Right[2] - _Left[2] * _Right[1],
			_Left[2] * _Right[0] - _Left[0] * _Right[2],
			_Left[0] * _Right[1] - _Left[1] * _Right[0]
		};
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	SCALAR norm(const VECTORN& _Left, const SCALAR& L) {
		return _Left.norm(L);
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	SCALAR length_squared(const VECTORN& _Left) {
		return _Left.length_squared();
	}

// 8.   unit_vector *------------------------------------------------------- L1947

	// { unit_vector, static size }
	template<size_t _Size, typename _Ts = scalar_t<void>, typename _Tb = _Ts>
	class unit_vector : public vector<_Size, _Ts, _Tb> {
		using _Mybase = vector<_Size, _Ts, _Tb>;

	public:
		using block_type  = _Tb;
		using scalar_type = _Ts;

		unit_vector() = default;
		unit_vector(scalar_type xi, scalar_type yj, scalar_type zk, bool _Unitized = false) : _Mybase(xi, yj, zk) {
			if (!_Unitized) this->normalize();
		}
		unit_vector(const _Mybase& _Vector, bool _Unitized = false)
			: _Mybase(_Unitized ? _Vector : normalize(_Vector)) {}

		unit_vector operator-() const {
			return unit_vector(-static_cast<const _Mybase&>(*this), true);
		}

		scalar_type length() const {
			return static_cast<scalar_type>(1);
		}
	};
	
	template<typename _Ts = scalar_t<void>, typename _Tb = _Ts>
	using unit_vector3 = unit_vector<3, _Ts, _Tb>;

	#define UNIT_VECTORN ::clmagic::unit_vector<_Size, _Ts, _Tb>
	#define UNIT_VECTOR3 ::clmagic::unit_vector3<_Ts, _Tb>

	template<typename _Ts, size_t _Size, typename _Tb>
	UNIT_VECTORN abs(const UNIT_VECTORN& _Left) {
		return UNIT_VECTORN(-_Left, true);
	}
	template<typename _Ts, size_t _Size, typename _Tb>
	SCALAR length(const UNIT_VECTORN& _Left) {
		return static_cast<_Ts>(1);
	}

// 9.   vector_any --------------------------------------------------------- L1989

	namespace Internal {
		// { Internal struct }
		template<typename _Ts, typename _Tb = _Ts>
		class _Vector_any_data {
			std::vector<_Tb> _My_blocks;
			size_t           _My_size;
		public:
			using block_type   = _Tb;
			using scalar_type  = _Ts;
			using block_traits = clmagic::block_traits<block_type>;

			// property
			size_t size() const {
				return _My_size;
			}
			size_t real_size() const {
				return _My_blocks.size() * block_traits::size();
			}
			size_t tail_size() const {
				return (real_size() - size());
			}
			size_t block_size() const {
				return _My_blocks.size();
			}
		
			// iterator
			template<typename _Ty = scalar_type>
			_Ty* begin() {
				return reinterpret_cast<_Ty*>(_My_blocks.data());
			}
			template<typename _Ty = scalar_type>
			_Ty* end() {
				return begin() + size();
			}
			template<typename _Ty = scalar_type>
			const _Ty* begin() const {
				return reinterpret_cast<const _Ty*>(_My_blocks.data());
			}
			template<typename _Ty = scalar_type>
			const _Ty* end() const {
				return begin() + size();
			}
		
			// index_operation
			scalar_type* ptr(size_t _Pos = 0) {
				return reinterpret_cast<scalar_type*>(_My_blocks.data()) + _Pos;
			}
			scalar_type& at(size_t _Pos) {
				assert( _Pos < size() );
				return *(ptr(_Pos));
			}
			scalar_type& operator[](size_t _Pos) {
				return at(_Pos);
			}
			const scalar_type* ptr(size_t _Pos = 0) const {
				return reinterpret_cast<const scalar_type*>(_My_blocks.data()) + _Pos;
			}
			const scalar_type& at(size_t _Pos) const {
				assert( _Pos < size() );
				return *(ptr(_Pos));
			}
			const scalar_type& operator[](size_t _Pos) const {
				return at(_Pos);
			}
		
			// memory_operation
			void _Correct_tail_elements() {
				const size_t _My_real_size = this->real_size();
				if (_My_real_size > _My_size) {
					std::fill(this->ptr(_My_size), this->ptr(_My_real_size), static_cast<scalar_type>(0));
				}
			}
			void swap(_Vector_any_data& _Right) noexcept {
				std::swap(_My_blocks, _Right._My_blocks);
				std::swap(_My_size, _Right._My_size);
			}
			void release() noexcept {
				if (_My_size != 0) {
					std::swap(std::vector<block_type>(), std::move(_My_blocks));// noexcept
					_My_size = 0;
				}
			}
			void resize(size_t _Newsize) {
				size_t _Real_newsize = (_Newsize + (block_traits::size()-1)) & (~(block_traits::size() - 1));
				_My_blocks.resize(_Real_newsize / block_traits::size());
				_My_size = _Newsize;
			}
			template<typename _Iter>
			void assign(_Iter _First, _Iter _Last) {
				size_t _Diff = std::distance(_First, _Last);

				_Vector_any_data _New_data;
				_New_data.resize(_Diff);
				auto _Dest = std::copy(_First, _Last, _New_data.begin<scalar_type>());
				std::fill(_Dest, _New_data.end<scalar_type>(), static_cast<scalar_type>(0));

				_New_data.swap(*this);
			}
			void assign(const scalar_type& _Val) {
				std::fill(this->begin<scalar_type>(), this->end<scalar_type>(), _Val);
			}
		
			// string
			std::string to_string() const {
				auto       _First = this->begin<scalar_type>();
				const auto _Last  = this->end<scalar_type>();
				std::string _Str  = std::to_string(*_First++);
				for ( ; _First != _Last; ++_First) {
					_Str += ',';
					_Str += std::to_string(*_First);
				}
				return std::move('{' + _Str + '}');
			}
		};
	}

	// { vector, dynamic size }
	template<typename _Ts, typename _Tb = _Ts>
	class vector_any : public Internal::_Vector_any_data<_Ts, _Tb> {
		using _Mybase = Internal::_Vector_any_data<_Ts, _Tb>;

	public:
		using block_type   = _Tb;
		using scalar_type  = _Ts;
		using subvector    = clmagic::subvector<scalar_type, block_type>;
		using block_traits = clmagic::block_traits<block_type>;

		// for_each
		template<typename _UnaryOp>
		void for_each_scalar(_UnaryOp _Transform_op) {// for each scalar by (_Transform_op*)(scalar_type)
			std::transform(this->begin<scalar_type>(), 
				           this->end<scalar_type>(), 
				           this->begin<scalar_type>(), _Transform_op);
		}
		template<typename _BinOp>
		void for_each_scalar(const vector_any& _Args2, _BinOp _Transform_op) {
			std::transform(this->begin<scalar_type>(),
				           this->end<scalar_type>(), 
				          _Args2.begin<scalar_type>(), 
				           this->end<scalar_type>(), _Transform_op);
		}
		template<typename _UnaryOp>
		vector_any for_each_scalar_copy(_UnaryOp _Transform_op) const {
			vector_any _Result; 
			_Result.resize(this->size());
			std::transform(this->begin<scalar_type>(), 
				           this->end<scalar_type>(), 
				          _Result.begin<scalar_type>(), _Transform_op);
			return std::move(_Result);
		}
		template<typename _BinOp>
		vector_any for_each_scalar_copy(const vector_any& _Args2, _BinOp _Transform_op) const {
			vector_any _Result;
			_Result.resize(this->size());
			std::transform(this->begin<scalar_type>(), 
				           this->end<scalar_type>(), 
				          _Args2.begin<scalar_type>(), 
				          _Result.begin<scalar_type>(), _Transform_op);
			return std::move(_Result);
		}
		template<typename _UnaryOp>
		void for_each_block(_UnaryOp _Transform_op) {// for each block by (_Transform_op*)(block_type)
			std::transform(this->begin<block_type>(), 
				           this->begin<block_type>() + this->block_size(), 
				           this->begin<block_type>(), _Transform_op);
		}
		template<typename _BinOp>
		void for_each_block(const vector_any& _Args2, _BinOp _Transform_op) {
			std::transform(this->begin<block_type>(), 
				           this->begin<block_type>() + this->block_size(), 
				          _Args2.begin<block_type>(), 
				           this->begin<block_type>(), _Transform_op);
		}
		template<typename _UnaryOp>
		vector_any for_each_block_copy(_UnaryOp _Transform_op) const {
			vector_any _Result;
			_Result.resize(this->size());
			std::transform(this->begin<block_type>(), 
				           this->begin<block_type>()+this->block_size(), 
				           _Result.begin<block_type>(), _Transform_op);
			return std::move(_Result);
		}
		template<typename _BinOp>
		vector_any for_each_block_copy(const vector_any& _Args2, _BinOp _Transform_op) const {
			vector_any _Result;
			_Result.resize(this->size());
			std::transform(this->begin<block_type>(), 
				           this->begin<block_type>()+this->block_size(), 
				          _Args2.begin<block_type>(), 
				          _Result.begin<block_type>(), _Transform_op);
			return std::move(_Result);
		}
		
		// constructor
		vector_any() = default;
		vector_any(const vector_any& _Right) = default;
		vector_any(vector_any&& _Right) noexcept {// move-constructor
			_Right.swap(*this);
			_Right.release();
		}
		vector_any(size_t _Count, scalar_type _Val) {
			_Mybase::resize(_Count);
			// noexcept
			_Mybase::assign(_Val);
			_Mybase::_Correct_tail_elements();
		}
		template<typename _Iter>
		vector_any(_Iter _First, _Iter _Last) {
			_Mybase::assign(_First, _Last);
			_Mybase::_Correct_tail_elements();
		}
		vector_any(std::initializer_list<scalar_type> _Ilist) {
			_Mybase::assign(_Ilist.begin(), _Ilist.end());
			_Mybase::_Correct_tail_elements();
		}
		
		// assign_operator
		vector_any& operator=(const vector_any& _Right) = default;
		vector_any& operator=(vector_any&& _Right) {
			_Right.swap(*this);
			_Right.release();
		}
		
		// function_operator
		subvector operator()() {
			return subvector(this->begin(), this->end());
		}
		subvector operator()(size_t _First, size_t _Last) {
			return subvector(this->ptr(_First), this->ptr(_Last));
		}
		const subvector operator()() const {
			return const_cast<vector_any&>(*this).operator()( );
		}
		const subvector operator()(size_t _First, size_t _Last) const {
			return const_cast<vector_any&>(*this).operator()(_First, _Last);
		}
		
		// arthmetic_operator
		vector_any operator-() const {
			return std::move( this->for_each_block_copy(std::negate<block_type>()) );
		}
		vector_any operator+(const vector_any& _Right) const {
			return std::move( this->for_each_block_copy(_Right, std::plus<block_type>()) );
		}
		vector_any operator-(const vector_any& _Right) const {
			return std::move( this->for_each_block_copy(_Right, std::minus<block_type>()) );
		}
		vector_any operator*(const vector_any& _Right) const {
			return std::move( this->for_each_block_copy(_Right, std::multiplies<block_type>()) );
		}
		vector_any operator/(const vector_any& _Right) const {
			return std::move( this->for_each_block_copy(_Right, std::divides<block_type>()) );
		}
		vector_any operator%(const vector_any& _Right) const {
			return std::move( this->for_each_block_copy(_Right, std::modulus<block_type>()) );
		}
		vector_any operator+(const scalar_type& _Scalar) const {
			const block_type _Right        = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left + _Right; };
			return std::move( this->for_each_block_copy(_Transform_op) );
		}
		vector_any operator-(const scalar_type& _Scalar) const {
			const block_type _Right        = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left - _Right; };
			return std::move( this->for_each_block_copy(_Transform_op) );
		}
		vector_any operator*(const scalar_type& _Scalar) const {
			const block_type _Right        = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left * _Right; };
			return std::move( this->for_each_block_copy(_Transform_op) );
		}
		vector_any operator/(const scalar_type& _Scalar) const {
			const block_type _Right        = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left / _Right; };
			return std::move( this->for_each_block_copy(_Transform_op) );
		}
		vector_any operator%(const scalar_type& _Scalar) const {
			const block_type _Right        = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left % _Right; };
			return std::move( this->for_each_block_copy(_Transform_op) );
		}
		vector_any& operator+=(const vector_any& _Right) {
			this->for_each_block(_Right, std::plus<block_type>());
			return *this;
		}
		vector_any& operator-=(const vector_any& _Right) {
			this->for_each_block(_Right, std::minus<block_type>());
			return *this;
		}
		vector_any& operator*=(const vector_any& _Right) {
			this->for_each_block(_Right, std::multiplies<block_type>());
			return *this;
		}
		vector_any& operator/=(const vector_any& _Right) {
			this->for_each_block(_Right, std::divides<block_type>());
			return *this;
		}
		vector_any& operator%=(const vector_any& _Right) {
			this->for_each_block(_Right, std::modulus<block_type>());
			return *this;
		}
		vector_any& operator+=(const scalar_type& _Scalar) {
			const block_type _Right         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left + _Right; };
			this->for_each_block(_Transform_op);
			return *this;
		}
		vector_any& operator-=(const scalar_type& _Scalar) {
			const block_type _Right         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left - _Right; };
			this->for_each_block(_Transform_op);
			return *this;
		}
		vector_any& operator*=(const scalar_type& _Scalar) {
			const block_type _Right         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left * _Right; };
			this->for_each_block(_Transform_op);
			return *this;
		}
		vector_any& operator/=(const scalar_type& _Scalar) {
			const block_type _Right         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left / _Right; };
			this->for_each_block(_Transform_op);
			return *this;
		}
		vector_any& operator%=(const scalar_type& _Scalar) {
			const block_type _Right         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left % _Right; };
			this->for_each_block(_Transform_op);
			return *this;
		}
		
		// IO_operator
		friend std::ostream& operator<<(std::ostream& _Ostr, const vector_any& _Left) {
			return (_Ostr << _Left.to_string()); 
		}
	};

	template<typename _Ts, typename _Tb> inline
	std::string to_string(const vector_any<_Ts, _Tb>& _Left) {
		return _Left.to_string();
	}

// 10.  vector_cast *------------------------------------------------------- L2334

	template<typename _OutVec, typename _InVec>
	struct _Vector_cast {
		_OutVec operator()(const _InVec& _Source) const {
			return _OutVec(_Source.begin(), _Source.end());
		}
	};

	template<size_t _Size, typename _Ts, typename _Tb, typename _InVec>
	struct _Vector_cast< vector<_Size, _Ts, _Tb>, _InVec > {
		vector<_Size,_Ts,_Tb> operator()(const _InVec& _Source) const {
			return vector<_Size,_Ts,_Tb>( _Source.begin(), std::next(_Source.begin(), std::min(_Source.size(), _Size)) );
		}
	};
	
	template<typename _OutVec, typename _InVec> inline
	_OutVec vector_cast(const _InVec& _Source) {
		return _Vector_cast<_OutVec, _InVec>()(_Source);
	}

	template<typename _InVec, typename _OutVec> inline
	void vector_cast(const _InVec& _Source, _OutVec& _Destination) {
		_Destination = vector_cast<_OutVec>(_Source);
	}

	template<typename _InVstream, typename _OutVstream>
	void vector_stream_cast(const _InVstream& _Source, _OutVstream& _Destination) {
		auto        _First = _Source.begin();
		const auto  _Last  = _Source.end();
		auto        _Dest  = _Destination.begin();
		for ( ; _First != _Last; ++_First, ++_Dest) {
			vector_cast(*_First, *_Dest);
		}
	}

	template<typename _OutVstream, typename _InVstream>
	_OutVstream vector_stream_cast(const _InVstream& _Source) {
		_OutVstream _Destination = _OutVstream( _Source.size() );// invoke _OutVstream::_OutVstream(size_t _Count)
		vector_stream_cast<_InVstream, _OutVstream>( _Source, _Destination );
		return std::move( _Destination );
	}

	template<size_t i0, size_t i1, size_t i2, typename _InVec3, typename _OutVec3> inline
	void vector3_cast(const _InVec3& _Source, _OutVec3& _Destination) {
		_Destination[0] = _Source[i0];
		_Destination[1] = _Source[i1];
		_Destination[2] = _Source[i2];
	}

	template<typename _OutVec3, size_t i0, size_t i1, size_t i2, typename _InVec3> inline
	_OutVec3 vector3_cast(const _InVec3& _Source) {
		return _OutVec3{ _Source[i0],  _Source[i1],  _Source[i2] };
	}

	template<size_t i0, size_t i1, size_t i2, typename _InVstream, typename _OutVstream>
	void vector3_stream_cast(const _InVstream& _Source, _OutVstream& _Destination) {
		auto       _First = _Source.begin();
		const auto _Last  = _Source.end();
		auto       _Dest  = _Destination.begin();
		for ( ; _First != _Last; ++_First, ++_Dest) {
			vector3_cast<i0, i1, i2>(*_First, *_Dest);
		}
	}

	template<typename _OutVstream, size_t i0, size_t i1, size_t i2, typename _InVstream>
	_OutVstream vector3_stream_cast(const _InVstream& _Source) {
		_OutVstream _Destination = _OutVstream( _Source.size() );
		vector3_stream_cast<i0, i1, i2, _InVstream, _OutVstream>( _Source, _Destination );
		return std::move( _Destination );
	}

// 11.  undetermined ------------------------------------------------------- L2361

	template<typename _Ty, bool _Is_scalar = std::is_scalar_v<_Ty>>
	struct inner_product {
		auto operator()(const _Ty& _Left, const _Ty& _Right) const -> decltype( _Left.dot(_Right) ) {
			// requires member_function, because finded dot in any_namespace
			return _Left.dot(_Right);
		}
	};
	
	template<typename _Ty>
	struct inner_product<_Ty, true> {
		_Ty operator()(const _Ty& _Left, const _Ty& _Right) const {
			return _Left * _Right;
		}
	};

	template<typename _Ty> inline
	auto dot(const _Ty& _Left, const _Ty& _Right) {
		return inner_product<_Ty>()(_Left, _Right);
	}

	template<typename _Ty, bool _Is_scalar = std::is_scalar_v<_Ty>>
	struct _Normalize_operator {
		_Ty operator()(const _Ty& _Source) const {
			// requires member_function: auto _Ty::dot(_Ty)
			const auto _Length_squared = dot(_Source, _Source);
			const auto _Error          = _Length_squared - static_cast<decltype(_Length_squared)>(1);
			return abs(_Error) < std::numeric_limits<decltype(_Error)>::epsilon()
				? _Source : _Source / sqrt(_Length_squared);
		}
	};

	template<typename _Ty>
	struct _Normalize_operator<_Ty, true> {
		_Ty operator()(const _Ty& _Source) {
			return static_cast<_Ty>(1);
		}
	};

	template<typename _Ty> inline
	_Ty normalize(const _Ty& _Source) {
		return _Normalize_operator<_Ty>()(_Source);
	}

	template<typename _Ty> inline
	auto length_squared(const _Ty& _Source) -> decltype( dot(_Source, _Source) ) {
		return dot(_Source, _Source);
	}

	template<typename _Ty> inline
	auto length(const _Ty& _Source) -> decltype( sqrt(length_squared(_Source)) ) {
		return sqrt(length_squared(_Source));
	}

	namespace Internal {
		// { specialization, calculation/real/real.h, Line:28 }
		template<size_t _Size, typename _Ts, typename _Tb>
		struct _Scalar_t< vector<_Size, _Ts, _Tb> > {
			using type = typename vector<_Size, _Ts, _Tb>::scalar_type;
		};

		// { specialization, calculation/real/real.h, Line:28 }
		template<size_t _Size, typename _Ts, typename _Tb>
		struct _Scalar_t< unit_vector<_Size, _Ts, _Tb> > {
			using type = typename unit_vector<_Size, _Ts, _Tb>::scalar_type;
		};

		// { specialization, calculation/real/real.h, Line:28 }
		template<typename _Ts, typename _Tb>
		struct _Scalar_t< vector_any<_Ts, _Tb> > {
			using type = typename vector_any<_Ts, _Tb>::scalar_type;
		};
	}

	// compare function
	/*template<size_t _Size, typename _Tb>
	bool all_of(const vector<bool, _Size, _Tb>& _X) {
		for (auto _First = _X.begin(); _First != _X.end(); ++_First) {
			if (!(*_First)) {
				return false;
			}
		}
		return true;
	}
	template<size_t _Size, typename _Tb>
	bool any_of(const vector<bool, _Size, _Tb>& _X) {
		for (auto _First = _X.begin(); _First != _X.end(); ++_First) {
			if (*_First) {
				return true;
			}
		}
		return false;
	}
	template<size_t _Size, typename _Tb>
	bool none_of(const vector<bool, _Size, _Tb>& _X) {
		for (auto _First = _X.begin(); _First != _X.end(); ++_First) {
			if ((*_First)) {
				return false;
			}
		}
		return true;
	}*/

	// dot(_I,_Nref) < 0 ? N : -N
	template<typename _Ts, size_t _Size, typename _Tb> inline
	UNIT_VECTORN faceforward(UNIT_VECTORN _Normal, VECTORN _I, VECTORN _Nref) {
		return (dot(_I, _Nref) < 0 ? _Normal : -_Normal);
	}

	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN proj(const VECTORN& _Vector, const VECTORN& _Proj) {// return dot(V,||_Proj||) * |Proj| 
		return (dot(_Vector, _Proj) / dot(_Proj, _Proj) * _Proj);
		/*
			|          / |
			|_Vector/
			|    /	     |
			| /
			|-> result<--|___________
			| N |       N is n|malized
			| ->   _Proj   <- |

			proj_length = (N dot _Lhs)
			proj_length = (normalize(_Proj) dot _Lhs)
			proj = proj_length * normalize(_Proj)

			proj = (_Lhs dot normalize(_Proj)) * normalize(_Proj)
			proj = (_Lhs * _Proj / length(_Proj)) dot (_Proj / length(_Proj) * _Proj / length(_Proj))
			proj = (_Lhs * _Proj / length(_Proj)) dot (_Proj^2 /length(_Proj)^2)
			proj = (_Lhs dot _Proj) / dot(_Proj) * _Proj
		*/
	}



	//template<typename _Ts, typename _Tb, size_t _Npatch = 4, size_t _Nattri = 2>
	//struct geometry {
	//	enum {
	//		// requires
	//		V = 0,
	//		VT = 1,
	//		// free rein
	//		TAN = 2,
	//		BTAN = 3
	//	};

	//	struct face {
	//		VECTOR3 position[_Npatch];
	//		VECTOR3 normal;
	//		VECTOR3 tangent;
	//		VECTOR3 bitangent;
	//		VECTOR3 texcoord[_Npatch];
	//	};

	//	using index_type = vector<_Nattri, uint32_t>;
	//	using patch_type = vector<_Npatch, index_type>;
	//	
	//	std::vector<VECTOR3> attributes[_Nattri];
	//	std::vector<patch_type> faces;

	//	const face operator[](size_t f) const {
	//		face        _Face;
	//		const auto& _Patch = faces[f];
	//		for (size_t i = 0; i != _Npatch; ++i) {
	//			_Face.position[i] = attributes[V][ _Patch[i][V] ];
	//			_Face.texcoord[i] = attributes[VT][ _Patch[i][VT] ];
	//		}
	//		if (_Npatch >= 3) {// greater than triangle
	//			_Face.normal = cross3(_Face.position[1] - _Face.position[0], _Face.position[2] - _Face.position[0]);
	//			_Face.normal.normalize();
	//		}

	//		return _Face;
	//	}

	//};


	/*template<typename _Ts, size_t _Size, typename _Vtag> inline
	auto distance(const vector<_Ts, _Size, _Vtag>& _A, const vector<_Ts, _Size, _Vtag>& _B) 
		-> decltype(length(_B - _A)) {
		return length(_B - _A);
	}*/

	//template<size_t N, typename T> inline
	//	T radians(_in(vector<N, T>) _A, _in(vector<N, T>) _B) 
	//	{	// return dot(A,B)÷|A|÷|B|
	//	return (dot(_A, _B) / _A.length() / _B.length());
	//	}



	//template<typename _Ts, size_t _Size, typename _Vtag> inline
	//vectorN proj(const vectorN& _Vector, const unit_vectorN& _Proj) {// return dot(V,N)*N
	//	return (dot(_Vector, _Proj) * _Proj);
	//}

	//template<typename _Ts, typename _Vtag> inline
	//vector3<_Ts, _Vtag> proj(const vector3<_Ts, _Vtag>& _Vector, const unit_vector3<_Ts, _Vtag>& _Rt, 
	//	const unit_vector3<_Ts, _Vtag>& _Up, const unit_vector3<_Ts, _Vtag>& _Fwd) {// return [dot3(V,r), dot(V,u), dot(V,f)]
	//	return vector3<_Ts, _Vtag>{ dot(_Vector, _Rt), dot(_Vector, _Up), dot(_Vector, _Fwd) };
	//}

	//template<typename _Ts, size_t _Size, typename _Vtag> inline
	//vectorN reflect(const vectorN& _Direction, const unit_vectorN& _Normal) {
	//	return (static_cast<_Ts>(2) * proj(_Direction, _Normal) - _Direction);
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