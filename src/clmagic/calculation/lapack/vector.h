//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
// Apache Licene 2.0
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_math_lapack_VECTOR_h_
#define clmagic_math_lapack_VECTOR_h_
//#include "../general/simd.h"// block_traits<>, simd_operator
#include "../general/general.h"// include shuffle<>
#include "../real/bitset.h"// include ceil(Number, Bound)
#include "../real/real.h"// include CSTD math
#include <stack>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>

namespace clmagic{
#ifndef clmagic_calculation_general_SIMD_h_
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
#endif

	/*- - - - - - - - - - - - - - - - - - - - vector_size<> - - - - - - - - - - - - - - - - - - - - - - -*/
	/*
	Ruler:  |----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----| 
	Vector: |---------|--------------|--------------|--------------|--------------|---------|           
			A0        A1														     B0	        B1
						|----------block_size---------------------------------------|       

	block_offset = A1 - A0 [bytes]
	leading_size = A1 - A0 [sizeof(scalar)]
	tail_size    = B1 - B0 [sizeof(scalar)]
	*/
	template<typename _Ts, typename _Tb>
	struct vector_size {
		size_t leading_size;
		size_t block_size;
		size_t tail_size;

		static void initializer(vector_size& _Dest, const _Ts* _First, const _Ts* _Last) {
			if (_First != nullptr && _Last > _First) {
				const auto _Mysize  = _Last - _First;
				auto       _Mybkoff = reinterpret_cast<size_t>(_First) & clmagic::alignment_mask_of_v<_Tb>;
				if (_Mybkoff % sizeof(_Ts) == 0) {
					_Mybkoff /= sizeof(_Ts);
					const bool _Pass_alignment_point = (_Mybkoff + _Mysize >= block_traits<_Tb>::size());
					if (_Pass_alignment_point) {// leading_size != 0 || block_size != 0
						_Dest.leading_size = (_Mybkoff == 0) ? 0 : block_traits<_Tb>::size() - _Mybkoff;
						_Dest.block_size   = (_Mysize - _Dest.leading_size) / block_traits<_Tb>::size();
						_Dest.tail_size    = (_Mysize - _Dest.leading_size) % block_traits<_Tb>::size();
					}
				}
			}
		}

		vector_size() : leading_size(0), block_size(0), tail_size(0) {}

		vector_size(const _Ts* _First, const _Ts* _Last) : vector_size() {
			initializer(*this, _First, _Last);
		}

		vector_size(const _Ts* _First, const _Ts* _Last, size_t _Stride) : vector_size() {
			if (_Stride == 1) {
				initializer(*this, _First, _Last);
			}
		}

		bool empty() const {
			return (leading_size == 0 && block_size == 0);
		}
		
		bool can_fastest() const {
			return (leading_size == 0 && tail_size == 0);
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

	/*- - - - - - - - - - - - - - - - - - - * _Vector_operation<> * - - - - - - - - - - - - - - - - - - - - -*/
	enum class _Vector_accelerate_level {
		Fastest,
		Fast,
		Norm,
		Auto
	};

	/*
	---+---+---|---+---+---|---+---+---|---+---+---|---+---+---|---+---+---|---+---+---+---+---+---
			---|---+---+---|---+---+---|---+---+---|---+---+---|---+---+---|---+---
			|mod|
						---|---+---+---|---+---+---|---+---+---|---+---+---|---+---|---+---+---
						|mod|
	mod == block_offset
	*/
	template<typename _Ts, typename _Tb, _Vector_accelerate_level/*default: Norm*/>
	struct _Vector_operation {
		using _OutIt = _Ts*;
		using _InIt  = const _Ts *;

		using scalar_type = _Ts;

		template<typename _UnaryOp>
		static _OutIt transform(_InIt _First, _InIt _Last, _OutIt _Dest, _UnaryOp _Transform_op,
			size_t _Inc1 = 1, size_t _Inc2 = 1) {
			for ( ; _First != _Last; _First+=_Inc1, _Dest+=_Inc2) {
				*_Dest = _Transform_op(*_First);
			}
			return _Dest;
		}

		template<typename _BinOp>
		static _OutIt transform(_InIt _First1, _InIt _Last1, _InIt _First2, _OutIt _Dest, _BinOp _Transform_op,
			size_t _Inc1 = 1, size_t _Inc2 = 1, size_t _Inc3 = 1) {
			for ( ; _First1 != _Last1; _First1+=_Inc1, _First2+=_Inc2, _Dest+=_Inc3) {
				*_Dest = _Transform_op(*_First1, *_First2);
			}
			return _Dest;
		}

		template<typename _BinOp>
		static scalar_type accumulate(_InIt _First, _InIt _Last, scalar_type _Val, _BinOp _Reduce_op,
			size_t _Inc1 = 1) {
			for (; _First != _Last; _First+=_Inc1) {
				_Val = _Reduce_op(_Val, *_First);
			}
			return _Val;
		}

		template<typename _BinOp, typename _UnaryOp>
		static scalar_type transform_accumulate(_InIt _First, _InIt _Last, scalar_type _Val, _BinOp _Reduce_op, _UnaryOp _Transform_op,
			size_t _Inc = 1) {
			for ( ; _First != _Last; _First+=_Inc) {
				_Val = _Reduce_op(_Val, _Transform_op(*_First));
			}
			return _Val;
		}

		template<typename _BinOp1, typename _BinOp2>
		static scalar_type transform_accumulate(_InIt _First1, _InIt _Last1, _InIt _First2, scalar_type _Val, _BinOp1 _Reduce_op, _BinOp2 _Transform_op,
			size_t _Inc1 = 1, size_t _Inc2 = 1) {
			// for [_First1, _Last1): _Afunc(_Result, _Func(*_First1, *_First2))
			// _Afunc = [](auto&, auto&&),                for example: [](auto& Res, auto&& X){ Res += X; } ...
			// _Func  = [](const _Ts&, const _Ts&), for example: std::multiplies<_Tb>() ...
			for ( ; _First1 != _Last1; _First1+=_Inc1, _First2+=_Inc2) {
				_Val = _Reduce_op(_Val, _Transform_op(*_First1, *_First2));
			}
			return _Val;
		}
	};

	template<typename _Ts, typename _Tb>
	struct _Vector_operation<_Ts, _Tb, _Vector_accelerate_level::Fastest> {
		using _OutIt = _Tb*;
		using _InIt  = const _Tb *;

		using scalar_type  = _Ts;
		using block_type   = _Tb;
		using block_traits = clmagic::block_traits<_Tb>;
		using scalar_operation = _Vector_operation<_Ts, _Tb, _Vector_accelerate_level::Norm>;

		template<typename _UnaryOp>
		static _OutIt transform(_InIt _First, _InIt _Last, _OutIt _Dest, _UnaryOp _Func) {
			return std::transform(_First, _Last, _Dest, _Func);
		}

		template<typename _BinOp>
		static _OutIt transform(_InIt _First1, _InIt _Last1, _InIt _First2, _OutIt _Dest, _BinOp _Func) {
			return std::transform(_First1, _Last1, _First2, _Dest, _Func);
		}

		/*template<typename _BinOp>
		static block_type accumulate(_InIt _First, _InIt _Last, block_type _Initval, _BinOp _Reduce_op) {
			return std::accumulate(_First, _Last, _Initval, _Reduce_op);
		}*/

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

		/*template<typename _BinOp, typename _UnaryOp>
		static block_type transform_accumulate(_InIt _First, _InIt _Last, block_type _Initval, _BinOp _Reduce_op, _UnaryOp _Transform_op) {
			return std::transform_reduce(_First, _Last, _Initval, _Reduce_op, _Transform_op);
		}*/

		template<typename _BinOp, typename _UnaryOp>
		static scalar_type transform_accumulate(_InIt _First, _InIt _Last, scalar_type _Initval, _BinOp _Reduce_op, _UnaryOp _Transform_op) {
			if (_First == _Last) {
				return _Initval;
			}
			block_type _Block_initval = _Transform_op(*_First++);
			block_type _Block_result  = std::transform_reduce(_First, _Last, _Block_initval, _Reduce_op, _Transform_op);
			auto       _Sfirst        = reinterpret_cast<const scalar_type*>(&_Block_result);
			const auto _Slast         = _Sfirst + block_traits::size();
			return scalar_operation::accumulate(_Sfirst, _Slast, _Initval, _Reduce_op);
		}

		/*template<typename _BinOp1, typename _BinOp2>
		static block_type transform_accumulate(_InIt _First1, _InIt _Last1, _InIt _First2, block_type _Initval, _BinOp1 _Reduce_op, _BinOp2 _Transform_op) {
			return std::transform_reduce(_First1, _Last1, _First2, _Initval, _Reduce_op, _Transform_op);
		}*/

		template<typename _BinOp1, typename _BinOp2>
		static scalar_type transform_accumulate(_InIt _First1, _InIt _Last1, _InIt _First2, scalar_type _Initval, _BinOp1 _Reduce_op, _BinOp2 _Transform_op) {
			if (_First1 == _Last1) {
				return _Initval;
			}
			block_type _Block_initval = _Transform_op(*_First1++, *_First2++);
			block_type _Block_result  = std::transform_reduce(_First1, _Last1, _First2, _Block_initval, _Reduce_op, _Transform_op);
			auto       _Sfirst        = reinterpret_cast<const scalar_type*>(&_Block_result);
			const auto _Slast         = _Sfirst + block_traits::size();
			return scalar_operation::accumulate(_Sfirst, _Slast, _Initval, _Reduce_op);
		}
	};

	template<typename _Ts, typename _Tb>
	struct _Vector_operation<_Ts, _Tb, _Vector_accelerate_level::Fast> {
		using _OutIt = _Tb*;
		using _InIt  = const _Tb*;

		using scalar_type  = _Ts;
		using block_type   = _Tb;
		using block_traits = ::clmagic::block_traits<_Tb>;
		using scalar_operation = _Vector_operation<_Ts, _Tb, _Vector_accelerate_level::Norm>;
		using block_operation  = _Vector_operation<_Ts, _Tb, _Vector_accelerate_level::Fastest>;

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

	template<typename _Ts, typename _Tb>
	struct _Vector_operation<_Ts, _Tb, _Vector_accelerate_level::Auto> {
		using _OutIt = void*;
		using _InIt  = const void *;

		using scalar_type  = _Ts;
		using block_type   = _Tb;
		using size_type    = clmagic::vector_size<_Ts, _Tb>;
		using block_traits = clmagic::block_traits<_Tb>;

		using scalar_operation  = _Vector_operation<_Ts, _Tb, _Vector_accelerate_level::Norm>;
		using block_operation   = _Vector_operation<_Ts, _Tb, _Vector_accelerate_level::Fastest>;
		using fastest_operation = block_operation;
		using fast_operation    = _Vector_operation<_Ts, _Tb, _Vector_accelerate_level::Fast>;
		using norm_operation    = scalar_operation;

		template<typename _UnaryOp>
		static _OutIt transform(_InIt _First, _InIt _Last, _OutIt _Dest, _UnaryOp _Transform_op,
			const size_type& _Vsize) {
			if (_Vsize.can_fastest()) {
				auto       _Bfirst = static_cast<typename fastest_operation::_InIt>(_First);
				const auto _Blast  = static_cast<typename fastest_operation::_InIt>(_Last);
				auto       _Dest   = static_cast<typename fastest_operation::_OutIt>(_Last);
				return fastest_operation::transform(_Bfirst, _Blast, _Dest, _Transform_op);
			} else if (!_Vsize.empty()) {
				auto       _Bfirst = reinterpret_cast<const block_type*>( (const scalar_type*)(_First) + _Vsize.leading_size );
				const auto _Blast  = _Bfirst + _Vsize.block_size;
				auto       _Bdest  = reinterpret_cast<block_type*>( (scalar_type*)(_Dest) + _Vsize.leading_size );
				return fast_operation::transform(_Bfirst, _Blast, _Bdest, _Transform_op, _Vsize.leading_size, _Vsize.tail_size);
			}
			return norm_operation::transform(static_cast<typename norm_operation::_InIt>(_First), static_cast<typename norm_operation::_InIt>(_Last),
				static_cast<typename norm_operation::_OutIt>(_Dest), _Transform_op);
		}

		template<typename _UnaryOp>
		static _OutIt transform(_InIt _First, _InIt _Last, _OutIt _Dest, _UnaryOp _Transform_op,
			size_t _Inc1 = 1, size_t _Inc2 = 1) {
			if (_Inc1 == 1 && _Inc2 == 1) {// memory-continue
				return transform( _First, _Last, _Dest, _Transform_op, 
					size_type((const _Ts*)_First, (const _Ts*)_Last) );
			}
			return norm_operation::transform(static_cast<typename norm_operation::_InIt>(_First), static_cast<typename norm_operation::_InIt>(_Last),
				static_cast<typename norm_operation::_OutIt>(_Dest), _Transform_op, _Inc1, _Inc2);
		}
	
		template<typename _BinOp>
		static _OutIt transform(_InIt _First1, _InIt _Last1, _InIt _First2, _OutIt _Dest, _BinOp _Transform_op,
			const size_type& _Vsize1, const size_type& _Vsize2) {
			if (_Vsize1 == _Vsize2) {
				if (_Vsize1.can_fastest()) {
					auto       _Bfirst1 = static_cast<typename fastest_operation::_InIt>(_First1);
					const auto _Blast1  = static_cast<typename fastest_operation::_InIt>(_Last1);
					auto       _Bfirst2 = static_cast<typename fastest_operation::_InIt>(_First2);
					auto       _Bdest   = static_cast<typename fastest_operation::_OutIt>(_Dest);
					return fastest_operation::transform(_Bfirst1, _Blast1, _Bfirst2, _Bdest, _Transform_op);
				} else if(!_Vsize1.empty()) {
					auto       _Bfirst1 = reinterpret_cast<const block_type*>((const scalar_type*)(_First1) + _Vsize1.leading_size);
					const auto _Blast1  = _Bfirst1 + _Vsize1.block_size;
					auto       _Bfirst2 = reinterpret_cast<const block_type*>((const scalar_type*)(_First2) + _Vsize1.leading_size);
					auto       _Bdest   = reinterpret_cast<block_type*>(      (scalar_type*)(_Dest)         + _Vsize1.leading_size);
					return fast_operation::transform(_Bfirst1, _Blast1, _Bfirst2, _Bdest, _Transform_op, _Vsize1.leading_size, _Vsize1.tail_size);
				}
			}

			return norm_operation::transform(static_cast<typename norm_operation::_InIt>(_First1), static_cast<typename norm_operation::_InIt>(_Last1),
				static_cast<typename norm_operation::_InIt>(_First2), static_cast<typename norm_operation::_OutIt>(_Dest), _Transform_op );
		}

		template<typename _BinOp>
		static _OutIt transform(_InIt _First1, _InIt _Last1, _InIt _First2, _OutIt _Dest, _BinOp _Transform_op,
			size_t _Inc1 = 1, size_t _Inc2 = 1, size_t _Inc3 = 1) {
			if (_Inc1 == 1 && _Inc2 == 1 && _Inc3 == 1) {// is_continue
				const auto _Diff = std::distance((const _Ts*)(_First1), (const _Ts*)(_Last1));
				return transform( _First1, _Last1, _First2, _Dest, _Transform_op,
					size_type((const _Ts*)_First1, (const _Ts*)_Last1), 
					size_type((const _Ts*)_First2, (const _Ts*)_First2 + _Diff) );
			}
			return norm_operation::transform(static_cast<typename norm_operation::_InIt>(_First1), static_cast<typename norm_operation::_InIt>(_Last1),
				static_cast<typename norm_operation::_InIt>(_First2), static_cast<typename norm_operation::_OutIt>(_Dest), _Transform_op, _Inc1, _Inc2, _Inc3);
		}
	
		template<typename _BinOp>
		static scalar_type accumulate(_InIt _First, _InIt _Last, scalar_type _Val, _BinOp _Reduce_op, const size_type& _Vsize) {
			if ( _Vsize.can_fastest() ) {
				auto       _Bfirst = static_cast<typename fastest_operation::_InIt>(_First);
				const auto _Blast  = static_cast<typename fastest_operation::_InIt>(_Last);
				return fastest_operation::accumulate(_Bfirst, _Blast, _Val, _Reduce_op);
			} else if ( !_Vsize.empty() ) {
				auto       _Bfirst = reinterpret_cast<const block_type*>( (const scalar_type*)(_First) + _Vsize.leading_size );
				const auto _Blast  = _Bfirst + +_Vsize.block_size;
				return fast_operation::accumulate(_Bfirst, _Blast, _Val, _Reduce_op);
			}
			return norm_operation::accumulate(static_cast<typename norm_operation::_InIt>(_First), static_cast<typename norm_operation::_InIt>(_Last),
				_Val, _Reduce_op);
		}

		template<typename _BinOp>
		static scalar_type accumulate(_InIt _First, _InIt _Last, scalar_type _Val, _BinOp _Reduce_op,
			size_t _Inc = 1) {
			if (_Inc == 1) {// memory-continue
				return accumulate( _First, _Last, _Val, _Reduce_op, size_type((const _Ts*)_First, (const _Ts*)_Last) );
			}
			return norm_operation::accumulate(static_cast<typename norm_operation::_InIt>(_First), static_cast<typename norm_operation::_InIt>(_Last),
				_Val, _Reduce_op, _Inc);
		}

		template<typename _BinOp, typename _UnaryOp>
		static scalar_type transform_accumulate(_InIt _First, _InIt _Last, scalar_type _Val, _BinOp _Reduce_op, _UnaryOp _Transform_op,
			const size_type& _Vsize) {
			if ( _Vsize.can_fastest() ) {
				auto       _Bfirst = static_cast<typename fastest_operation::_InIt>(_First);
				const auto _Blast  = static_cast<typename fastest_operation::_InIt>(_First);
				return fastest_operation::transform_accumulate(_Bfirst, _Blast, _Val, _Reduce_op, _Transform_op);
			} else if ( !_Vsize.empty() ) {
				auto       _Bfirst = reinterpret_cast<const block_type*>( (const scalar_type*)(_First) + _Vsize.leading_size );
				const auto _Blast  = _Bfirst + _Vsize.block_size;
				return fast_operation::transform_accumulate(_Bfirst, _Blast, _Val, _Reduce_op, _Transform_op);
			}
			return norm_operation::transform_accumulate(static_cast<typename norm_operation::_InIt>(_First), static_cast<typename norm_operation::_InIt>(_Last),
				_Val, _Reduce_op, _Transform_op);
		}

		template<typename _BinOp, typename _UnaryOp>
		static scalar_type transform_accumulate(_InIt _First, _InIt _Last, scalar_type _Val, _BinOp _Reduce_op, _UnaryOp _Transform_op,
			size_t _Inc = 1) {
			if (_Inc == 1) {// memory-continue
				return transform_accumulate(_First, _Last, _Val, _Reduce_op, _Transform_op, 
					size_type((const _Ts*)_First, (const _Ts*)_Last) );
			}
			return norm_operation::transform_accumulate(static_cast<typename norm_operation::_InIt>(_First), static_cast<typename norm_operation::_InIt>(_Last),
				_Val, _Reduce_op, _Transform_op, _Inc);
		}

		template<typename _BinOp1, typename _BinOp2>
		static scalar_type transform_accumulate(_InIt _First1, _InIt _Last1, _InIt _First2, scalar_type _Val, _BinOp1 _Reduce_op, _BinOp2 _Transform_op,
			const size_type& _Vsize1, const size_type& _Vsize2) {
			if (_Vsize1 == _Vsize2) {
				if ( _Vsize1.can_fastest() ) {
					auto       _Bfirst1 = static_cast<typename fast_operation::_InIt>(_First1);
					const auto _Blast1  = static_cast<typename fast_operation::_InIt>(_Last1);
					auto       _Bfirst2 = static_cast<typename fast_operation::_InIt>(_First2);
					return fastest_operation::transform_accumulate(_Bfirst1, _Blast1, _Bfirst2, _Val, _Reduce_op, _Transform_op);
				} else if( !_Vsize1.empty() ) {
					auto       _Bfirst1 = reinterpret_cast<const block_type*>((const scalar_type*)(_First1) + _Vsize1.leading_size);
					const auto _Blast1  = _Bfirst1 + _Vsize1.block_size;
					auto       _Bfirst2 = reinterpret_cast<const block_type*>((const scalar_type*)(_First2) + _Vsize1.leading_size);
					return fast_operation::transform_accumulate(_Bfirst1, _Blast1, _Bfirst2, _Val, _Reduce_op, _Transform_op, _Vsize1.leading_size, _Vsize1.tail_size);
				}
			}
			return norm_operation::transform_accumulate(static_cast<typename norm_operation::_InIt>(_First1), static_cast<typename norm_operation::_InIt>(_Last1),
				static_cast<typename norm_operation::_InIt>(_First2), _Val, _Reduce_op, _Transform_op );
		}

		template<typename _BinOp1, typename _BinOp2>
		static scalar_type transform_accumulate(_InIt _First1, _InIt _Last1, _InIt _First2, scalar_type _Val, _BinOp1 _Reduce_op, _BinOp2 _Transform_op,
			size_t _Inc1 = 1, size_t _Inc2 = 1) {
			if (_Inc1 == 1 && _Inc2 == 1) {// memory-continue
				const auto _Diff = std::distance((const _Ts*)(_First1), (const _Ts*)(_Last1));
				return transform_accumulate( _First1, _Last1, _First2, _Val, _Reduce_op, _Transform_op,
					size_type((const _Ts*)_First1, (const _Ts*)_Last1), 
					size_type((const _Ts*)_First2, (const _Ts*)_First2 + _Diff) );
			}
			return norm_operation::transform_accumulate(static_cast<typename norm_operation::_InIt>(_First1), static_cast<typename norm_operation::_InIt>(_Last1),
				static_cast<typename norm_operation::_InIt>(_First2), _Val, _Reduce_op, _Transform_op, _Inc1, _Inc2);
		}
	};

	template<typename _Ts>
	struct _Vector_operation<_Ts, _Ts, _Vector_accelerate_level::Auto>
		: public _Vector_operation<_Ts, _Ts, _Vector_accelerate_level::Norm>{
		// 
	};

	template<typename _Ts, typename _Tb>
		using fastest_vector_operation = _Vector_operation<_Ts, _Tb, _Vector_accelerate_level::Fastest>;
	template<typename _Ts, typename _Tb>
		using fast_vector_operation = _Vector_operation<_Ts, _Tb, _Vector_accelerate_level::Fast>;
	template<typename _Ts, typename _Tb>
		using norm_vector_operation = _Vector_operation<_Ts, _Tb, _Vector_accelerate_level::Norm>;
	template<typename _Ts, typename _Tb>
		using auto_vector_operation = _Vector_operation<_Ts, _Tb, _Vector_accelerate_level::Auto>;

	/*- - - - - - - - - - - - - - - - - - - _Tb_t<> - - - - - - - - - - - - - - - - - - - - -*/
	template<typename _Ts>
	struct _SIMD4_enum {
		using type = _Ts;
	};
	template<typename _Ts>
	struct _SIMD8_enum {
		using type = _Ts;
	};
#ifdef clmagic_calculation_general_SIMD_h_
	template<>
	struct _SIMD4_enum<float> {
		using type = __m128;
	};
	template<>
	struct _SIMD4_enum<double> {
		using type = __m256d;
	};
	template<>
	struct _SIMD4_enum<int> {
		using type = __m128i;
	};
	template<>
	struct _SIMD8_enum<float> {
		using type = __m256;
	};
	template<>
	struct _SIMD8_enum<double> {
		using type = __m512d;
	};
	template<>
	struct _SIMD8_enum<int> {
		using type = __m256i;
	};
#endif
	template<typename _Ts>
		using _SIMD4_t = typename _SIMD4_enum<_Ts>::type;
	template<typename _Ts>
		using _SIMD8_t = typename _SIMD8_enum<_Ts>::type;

	/*- - - - - - - - - - - - - - - - - - stack_allocator<> - - - - - - - - - - - - - - - - - - - - -*/
	template<typename _Ts, typename _Tb>
	struct stack_allocator : std::vector<_Tb> {
		using _Mybase       = std::vector<_Tb>;
		using _This_type = stack_allocator<_Ts, _Tb>;

		static _This_type& data() {
			static _This_type _Static_stack;
			return _Static_stack;
		}

		static size_t calc_block_count(size_t _Count/*scalar*/) {
			return ceil(_Count, block_traits<_Tb>::size()) / block_traits<_Tb>::size();
		/*	const bool _Divide = (_Count % block_traits<_Tb>::size()) != 0;
			return _Count / block_traits<_Tb>::size() + static_cast<size_t>(_Divide);*/
		}

		static _Ts* allocate(size_t _Scalar_count) {
			return reinterpret_cast<_Ts*>(data().seek_n(calc_block_count(_Scalar_count)));
		}

		static void deallocate(size_t _Scalar_count) {
			data()._Myseek -= calc_block_count(_Scalar_count);
		}

		static void push(size_t _Block_count) {
			auto& _Stack = data();
			assert(_Stack.empty());
			const auto _Newsize = _Block_count + _Block_count / 2;
			_Stack.resize(_Newsize);
		}
		
		static void pop() {
			auto& _Stack = data();
			assert(_Stack.empty());
			_Stack.clear();
			_Stack.shrink_to_fit();
		}

		_Tb& top() {
			return ((*this)[_Myseek]);
		}

		_Tb* seek_n(size_t _Count/*block*/) {
			const auto _Newseek = _Myseek + _Count;
			if (_Newseek > _Mybase::size()) {
				_Mybase::resize(_Newseek);
			}

			auto* _Ptr = &(this->top());
			_Myseek = _Newseek;
			return _Ptr;
		}

		bool empty() const {
			return _Myseek == 0;
		}

		size_t _Myseek;
	};


	/*- - - - - - - - - - - - - - - - - - - subvector<> - - - - - - - - - - - - - - - - - - - - - -*/
	template<typename _Ts, typename _Tb>
	struct _Subvector_data {
		~_Subvector_data() {
			if (_Is_owner) {
				delete[] _My_first;
				_My_first = nullptr;
				_My_last  = nullptr;
				_Is_owner = false;
			}
		}

		_Ts* ptr() {
			return _My_first;
		}
		_Ts* ptr(size_t _Pos) {
			return _My_first + _Pos * _My_stride;
		}
		_Ts* begin() {
			return _My_first;
		}
		_Ts* end() {
			return _My_last;
		}
		const _Ts* ptr() const {
			return _My_first;
		}
		const _Ts* ptr(size_t _Pos) const {
			return (_My_first + _Pos * _My_stride);
		}
		const _Ts* begin() const {
			return _My_first;
		}
		const _Ts* end() const {
			return _My_last;
		}
		
		size_t size() const {
			return (_My_last - _My_first) / _My_stride; 
		}
		size_t stride() const {
			return _My_stride;
		}
		bool empty() const {
			return (_My_first == _My_last);
		}
		bool is_continue() const {
			return (_My_stride == 1);
		}
		vector_size<_Ts, _Tb> vsize() const {
			return vector_size<_Ts, _Tb>(_My_first, _My_last, _My_stride); 
		}
		
		std::string to_string() const {
			assert(!this->empty());
			auto       _First = this->begin();
			const auto _Last  = this->end();
			std::string _Str  = std::to_string(*_First); _First += _My_stride;
			for (; _First != _Last; _First += _My_stride) {
				_Str += ' ';
				_Str += std::to_string(*_First);
			}
			return std::move(_Str);
		}

	protected:
		_Ts*   _My_first  = nullptr;
		_Ts*   _My_last   = nullptr;
		size_t _My_stride = 1;
		bool   _Is_owner  = false;
	};

	template<typename _Ts, typename _Tb>
	class subvector : public _Subvector_data<_Ts, _Tb> {
		using _Mybase = _Subvector_data<_Ts, _Tb>;
	public:
		using block_type   = _Tb;
		using scalar_type  = _Ts;
		using block_traits = clmagic::block_traits<block_type>;

		subvector() = default;
		
		subvector(const subvector& _Right) {
			_Mybase::_My_first  = _Right._My_first;
			_Mybase::_My_last   = _Right._My_last;
			_Mybase::_My_stride = _Right._My_stride;
			_Mybase::_Is_owner  = false;
		}

		subvector(subvector&& _Right) {
			_Mybase::_My_first  = _Right._My_first;
			_Mybase::_My_last   = _Right._My_last;
			_Mybase::_My_stride = _Right._My_stride;
			_Mybase::_Is_owner  = _Right._Is_owner;
			_Right._My_first = nullptr;
			_Right._My_last  = nullptr;
			_Right._Is_owner = false;
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
		
		subvector(size_t _Count) {
			_Mybase::_My_first  = new scalar_type[_Count];
			_Mybase::_My_last   = _Mybase::_My_first + _Count;
			_Mybase::_My_stride = 1;
			_Mybase::_Is_owner  = true;
		}

		subvector& operator=(const subvector& _Right) {
			if (_Mybase::_My_first == nullptr) {
				_Mybase::_My_first  = new scalar_type[_Right.size()];
				_Mybase::_My_last   = _Mybase::_My_first + _Right.size();
				_Mybase::_My_stride = 1;
				_Mybase::_Is_owner  = true;
			}
			
			assert( this->size() == _Right.size() );
			auto       _First = _Right.begin();
			const auto _Last  = _Right.end();
			auto       _Dest  = this->begin();
			for ( ; _First != _Last; _First += _Right.stride(), _Dest += this->stride()) {
				*_Dest = *_First;
			}
			return *this;
		}

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
			assert(this->size() == _Args2.size());
			subvector _Result = subvector(this->size());
			auto_vector_operation<scalar_type, block_type>::transform(
				this->begin()/*_First1*/,
				this->end()  /*_Last1*/,
			   _Args2.begin()/*_First2*/,
			   _Result.begin()/*_Dest*/, _Transform_op, this->stride(), _Args2.stride(), _Result.stride());
			return std::move(_Result);
		}

		subvector neg() const {
			return std::move( this->for_each_copy(std::negate<>()) );
		}

		subvector add(const subvector& _Right) const {
			return std::move( this->for_each_copy(_Right, std::plus<>()) );
		}

		subvector sub(const subvector& _Right) const {
			return std::move( this->for_each_copy(_Right, std::minus<>()) );
		}

		subvector mul(const subvector& _Right) const {
			return std::move( this->for_each_copy(_Right, std::multiplies<>()) );
		}

		subvector div(const subvector& _Right) const {
			return std::move( this->for_each_copy(_Right, std::divides<>()) );
		}

		subvector mod(const subvector& _Right) const {
			return std::move( this->for_each_copy(_Right, std::modulus<>()) );
		}

		subvector add(scalar_type _Scalar) const {
			const block_type _Right        = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left + _Right; };
			return std::move( this->for_each_copy(_Transform_op) );
		}

		subvector sub(scalar_type _Scalar) const {
			const block_type _Right        = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left - _Right; };
			return std::move( this->for_each_copy(_Transform_op) );
		}

		subvector mul(scalar_type _Scalar) const {
			const block_type _Right        = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left * _Right; };
			return std::move( this->for_each_copy(_Transform_op) );
		}

		subvector div(scalar_type _Scalar) const {
			const block_type _Right        = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left / _Right; };
			return std::move( this->for_each_copy(_Transform_op) );
		}

		subvector mod(scalar_type _Scalar) const {
			const block_type _Right        = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left % _Right; };
			return std::move( this->for_each_copy(_Transform_op) );
		}
		
		subvector& assign_add(const subvector& _Right) {
			this->for_each(_Right, std::plus<>());
			return *this;
		}

		subvector& assign_sub(const subvector& _Right) {
			this->for_each(_Right, std::minus<>());
			return *this;
		}

		subvector& assign_mul(const subvector& _Right) {
			this->for_each(_Right, std::multiplies<>());
			return *this;
		}

		subvector& assign_div(const subvector& _Right) {
			this->for_each(_Right, std::divides<>());
			return *this;
		}

		subvector& assign_mod(const subvector& _Right) {
			this->for_each(_Right, std::modulus<>());
			return *this;
		}

		subvector& assign_add(scalar_type _Scalar) {
			const block_type _Right        = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left + _Right; };
			this->for_each(_Transform_op);
			return *this;
		}

		subvector& assign_sub(scalar_type _Scalar) {
			const block_type _Right        = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left - _Right; };
			this->for_each(_Transform_op);
			return *this;
		}

		subvector& assign_mul(scalar_type _Scalar) {
			const block_type _Right        = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left * _Right; };
			this->for_each(_Transform_op);
			return *this;
		}

		subvector& assign_div(scalar_type _Scalar) {
			const block_type _Right        = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left / _Right; };
			this->for_each(_Transform_op);
			return *this;
		}

		subvector& assign_mod(scalar_type _Scalar) {
			const block_type _Right        = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left % _Right; };
			this->for_each(_Transform_op);
			return *this;
		}
		
		subvector added(scalar_type _Scalar) const {
			const block_type _Left         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Left](auto&& _Right) { return _Left + _Right; };
			return std::move( this->for_each_copy(_Transform_op) );
		}

		subvector subed(scalar_type _Scalar) const {
			const block_type _Left         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Left](auto&& _Right) { return _Left - _Right; };
			return std::move( this->for_each_copy(_Transform_op) );
		}

		subvector muled(scalar_type _Scalar) const {
			const block_type _Left         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Left](auto&& _Right) { return _Left * _Right; };
			return std::move( this->for_each_copy(_Transform_op) );
		}

		subvector dived(scalar_type _Scalar) const {
			const block_type _Left         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Left](auto&& _Right) { return _Left / _Right; };
			return std::move( this->for_each_copy(_Transform_op) );
		}

		subvector moded(scalar_type _Scalar) const {
			const block_type _Left         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Left](auto&& _Right) { return _Left % _Right; };
			return std::move( this->for_each_copy(_Transform_op) );
		}

		scalar_type dot(const subvector& _Right) const {
			return auto_vector_operation<scalar_type, block_type>::transform_accumulate(
				this->begin() /*_First1*/,
				this->end()   /*_Last1*/,
				_Right.begin()/*_First2*/, static_cast<scalar_type>(0), std::plus<>()/*_Reduce_op*/, std::multiplies<>()/*_Transform_op*/,
				this->stride(), _Right.stride() );
		}

		scalar_type norm(scalar_type L) const {
			const auto _Arg1         = block_traits::set1(L);
			const auto _Transform_op = [&_Arg1](auto&& _Arg0) { return pow(_Arg0, _Arg1); };

			const scalar_type _Result = auto_vector_operation<_Ts, _Tb>::transform_accumulate(
				this->begin()/*_First*/, 
				this->end()  /*_Last*/, static_cast<scalar_type>(0), std::plus<>(), _Transform_op, this->stride());
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

		subvector operator-() const {
			return std::move(this->neg());
		}

		subvector operator+(const subvector& _Right) const {
			return std::move(this->add(_Right));
		}

		subvector operator-(const subvector& _Right) const {
			return std::move(this->sub(_Right));
		}

		subvector operator*(const subvector& _Right) const {
			return std::move(this->mul(_Right));
		}

		subvector operator/(const subvector& _Right) const {
			return std::move(this->div(_Right));
		}

		subvector operator%(const subvector& _Right) const {
			return std::move(this->mod(_Right));
		}

		subvector operator+(scalar_type _Right) const {
			return std::move(this->add(_Right));
		}

		subvector operator-(scalar_type _Right) const {
			return std::move(this->sub(_Right));
		}

		subvector operator*(scalar_type _Right) const {
			return std::move(this->mul(_Right));
		}

		subvector operator/(scalar_type _Right) const {
			return std::move(this->div(_Right));
		}

		subvector operator%(scalar_type _Right) const {
			return std::move(this->mod(_Right));
		}

		subvector& operator+=(const subvector& _Right) {
			return this->assign_add(_Right);
		}

		subvector& operator-=(const subvector& _Right) {
			return this->assign_sub(_Right);
		}

		subvector& operator*=(const subvector& _Right) {
			return this->assign_mul(_Right);
		}

		subvector& operator/=(const subvector& _Right) {
			return this->assign_div(_Right);
		}

		subvector& operator%=(const subvector& _Right) {
			return this->assign_mod(_Right);
		}

		subvector& operator+=(scalar_type _Right) {
			return this->assign_add(_Right);
		}

		subvector& operator-=(scalar_type _Right) {
			return this->assign_sub(_Right);
		}

		subvector& operator*=(scalar_type _Right) {
			return this->assign_mul(_Right);
		}

		subvector& operator/=(scalar_type _Right) {
			return this->assign_div(_Right);
		}

		subvector& operator%=(scalar_type _Right) {
			return this->assign_mod(_Right);
		}

		friend subvector operator+(scalar_type _Left, const subvector& _Right) {
			return std::move( _Right.added(_Left) );
		}

		friend subvector operator-(scalar_type _Left, const subvector& _Right) {
			return std::move(_Right.subed(_Left));
		}

		friend subvector operator*(scalar_type _Left, const subvector& _Right) {
			return std::move(_Right.muled(_Left));
		}

		friend subvector operator/(scalar_type _Left, const subvector& _Right) {
			return std::move(_Right.dived(_Left));
		}

		friend subvector operator%(scalar_type _Left, const subvector& _Right) {
			return std::move(_Right.moded(_Left));
		}
	
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
	subvector<_Ts, _Tb> fmod(const subvector<_Ts, _Tb>& _Left, const subvector<_Ts, _Tb>& _Right) {// must find function fmod(_Tb, _Tb)
		assert( !_Left.empty() );
		assert( _Left.size() == _Right.size() );
		const auto _Transform_op = [](auto&& _Arg0, auto&& _Arg1) { return fmod(_Arg0, _Arg1); };
		return std::move( _Left.for_each_copy(_Right, _Transform_op) );
	}

	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> fmod(const subvector<_Ts, _Tb>& _Left, _Ts _Right) {// must find function fmod(_Tb, _Tb)
		assert( !_Left.empty() );
		const auto _Arg1         = block_traits<_Tb>::set1(_Right);
		const auto _Transform_op = [&_Arg1](auto&& _Arg0) { return fmod(_Arg0, _Arg1); };
		return std::move( _Left.for_each_copy(_Transform_op) );
	}

	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> pow(const subvector<_Ts, _Tb>& _Left, const subvector<_Ts, _Tb>& _Right) {// must find function pow(_Tb, _Tb)
		assert( !_Left.empty() );
		assert( _Left.size() == _Right.size() );
		const auto _Transform_op = [](auto&& _Arg0, auto&& _Arg1) { return pow(_Arg0, _Arg1); };
		return std::move( _Left.for_each_copy(_Right, _Transform_op) );
	}

	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> pow(const subvector<_Ts, _Tb>& _Left, _Ts _Right) {// must find function pow(_Tb, _Tb)
		assert( !_Left.empty() );
		const auto _Arg1         = block_traits<_Tb>::set1(_Right);
		const auto _Transform_op = [&_Arg1](auto&& _Arg0) { return pow(_Arg0, _Arg1); };
		return std::move( _Left.for_each_copy(_Transform_op) );
	}
	
	template<typename _Ts, typename _Tb> inline
	_Ts dot(const subvector<_Ts, _Tb>& _Left, const subvector<_Ts, _Tb>& _Right) {
		return _Left.dot(_Right);
	}

	template<typename _Ts, typename _Tb> inline
	_Ts norm(const subvector<_Ts, _Tb>& _Left, _Ts L) {
		return _Left.norm(L);
	}

	template<typename _Ts, typename _Tb> inline
	_Ts length(const subvector<_Ts, _Tb>& _Left) {
		return _Left.length();
	}

	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> normalize(const subvector<_Ts, _Tb>& _Left) {
		subvector<_Ts, _Tb> _Result;
		_Result = _Left;
		_Result.normalize();
		return std::move(_Result);
	}

	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> sin(const subvector<_Ts, _Tb>& _Left) {// must find function sin(_Tb)
		const auto _Transform_op = [](auto&& x) { return sin(x); };
		return std::move( _Left.for_each_copy(_Transform_op) );
	}

	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> cos(const subvector<_Ts, _Tb>& _Left) {// must find function cos(_Tb)
		const auto _Transform_op = [](auto&& x) { return cos(x); };
		return std::move( _Left.for_each_copy(_Transform_op) );
	}

	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> tan(const subvector<_Ts, _Tb>& _Left) {// must find function tan(_Tb)
		const auto _Transform_op = [](auto&& x) { return tan(x); };
		return std::move( _Left.for_each_copy(_Transform_op) );
	}

	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> asin(const subvector<_Ts, _Tb>& _Left) {// must find function asin(_Tb)
		const auto _Transform_op = [](auto&& x) { return asin(x); };
		return std::move( _Left.for_each_copy(_Transform_op) );
	}

	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> acos(const subvector<_Ts, _Tb>& _Left) {// must find function acos(_Tb)
		const auto _Transform_op = [](auto&& x) { return acos(x); };
		return std::move( _Left.for_each_copy(_Transform_op) );
	}

	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> atan(const subvector<_Ts, _Tb>& _Left) {// must find function atan(_Tb)
		const auto _Transform_op = [](auto&& x) { return atan(x); };
		return std::move( _Left.for_each_copy(_Transform_op) );
	}

	/*- - - - - - - - - - - - - - - - - - - * vector * - - - - - - - - - - - - - - - - - - - - -*/
	template<typename _Ts, size_t _Size, typename _Tb>
	class __declspec(align(std::alignment_of_v<_Tb>)) _Vector_data {
		constexpr static size_t _Real_size = ceil(_Size * sizeof(_Ts), std::alignment_of_v<_Tb>) / sizeof(_Ts);
	public:
		_Ts _Mydata[_Real_size];

		using block_type  = _Tb;
		using scalar_type = _Ts;

		static constexpr size_t size() {
			return _Size; 
		}
		static constexpr size_t real_size() {
			return _Real_size;
		}
		static constexpr size_t tail_size() {
			return _Real_size - _Size; 
		}
		static constexpr size_t block_size() {
			return (_Real_size / block_traits<_Tb>::size());
		}

		template<typename _Ty = scalar_type>
		_Ty* ptr() {
			return reinterpret_cast<_Ty*>(_Mydata);
		}
		template<typename _Ty = scalar_type>
		_Ty* ptr(size_t _Pos) {
			return reinterpret_cast<_Ty*>(_Mydata + _Pos);
		}
		template<typename _Ty = scalar_type>
		_Ty& at(size_t _Pos) {
			assert(_Pos < _Size);
			return _Mydata[_Pos];
		}

		template<typename _Ty = scalar_type>
		const _Ty* ptr() const {
			return reinterpret_cast<const _Ty*>(_Mydata);
		}
		template<typename _Ty = scalar_type>
		const _Ty* ptr(size_t _Pos) const {
			return reinterpret_cast<const _Ty*>(_Mydata + _Pos);
		}
		template<typename _Ty = scalar_type>
		const _Ty& at(size_t _Pos) const {
			assert(_Pos < _Size);
			return _Mydata[_Pos];
		}

		template<typename _Ty = scalar_type>
		_Ty* begin() {
			return ptr<_Ty>();
		}
		template<typename _Ty = scalar_type>
		_Ty* end() {
			return reinterpret_cast<_Ty*>(_Mydata + _Size);
		}
		template<typename _Ty = scalar_type>
		const _Ty* begin() const {
			return ptr<_Ty>();
		}
		template<typename _Ty = scalar_type>
		const _Ty* end() const {
			return reinterpret_cast<const _Ty*>(_Mydata + _Size);
		}

		scalar_type& operator[](size_t _Pos) {
			assert(_Pos < _Size);
			return _Mydata[_Pos]; 
		}
		const scalar_type& operator[](size_t _Pos) const {
			assert(_Pos < _Size);
			return _Mydata[_Pos]; 
		}

		void _Correct_tail_elements() {
			if (_Real_size > _Size) {
				std::fill(this->ptr(_Size), this->ptr(_Real_size), static_cast<scalar_type>(0));
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
	
		template<typename _UnaryOp>
		void for_each_scalar(_UnaryOp _Transform_op) {// for each scalar by (_Transform_op*)(scalar_type)
			std::transform(this->begin<scalar_type>(), 
				           this->end<scalar_type>(), 
				           this->begin<scalar_type>(), _Transform_op);
		}

		template<typename _BinOp>
		void for_each_scalar(const _Vector_data& _Args2, _BinOp _Transform_op) {
			std::transform(this->begin<scalar_type>(),
				           this->end<scalar_type>(), 
				          _Args2.begin<scalar_type>(), 
				           this->end<scalar_type>(), _Transform_op);
		}
		
		template<typename _UnaryOp>
		void for_each_block(_UnaryOp _Transform_op) {// for each block by (_Transform_op*)(block_type)
			std::transform(this->begin<block_type>(), 
				           this->begin<block_type>() + this->block_size(), 
				           this->begin<block_type>(), _Transform_op);
		}

		template<typename _BinOp>
		void for_each_block(const _Vector_data& _Args2, _BinOp _Transform_op) {
			std::transform(this->begin<block_type>(), 
				           this->begin<block_type>() + this->block_size(), 
				          _Args2.begin<block_type>(), 
				           this->begin<block_type>(), _Transform_op);
		}
	
		std::string to_string() const {
			auto       _First = this->begin<scalar_type>();
			const auto _Last  = this->end<scalar_type>();
			std::string _Str  = std::to_string(*_First++);
			for ( ; _First != _Last; ++_First) {
				_Str += ' ';
				_Str += std::to_string(*_First);
			}
			return _Str;
		}
	};

	template<typename _Ts, size_t _Size, typename _Tb = _SIMD4_t<_Ts>>
	class vector : public _Vector_data<_Ts, _Size, _Tb> {
		using _Mybase = _Vector_data<_Ts, _Size, _Tb>;
	public:
		static_assert(_Size != 0, "clmagic::Error");
		//static_assert(std::is_arithmetic_v<_Ts>, "strict math vector");

		using block_type  = _Tb;
		using scalar_type = _Ts;

		using block_traits    = clmagic::block_traits<_Tb>;
		//using scalar_traits = clmagic::block_traits<_Ts>;

		vector() = default;
		
		explicit vector(scalar_type _Val) { 
			_Mybase::assign(_Val); 
			_Mybase::_Correct_tail_elements();
		}

		template<typename ..._Tys>
		vector(scalar_type x0, scalar_type x1, _Tys... xN) : _Mybase{ x0, x1, xN... } {
			static_assert((2 + types_size_v<_Tys...>) <= _Size, "clmagic::Error");
			_Mybase::_Correct_tail_elements();
		}

		template<typename _Iter> 
		vector(_Iter _First, _Iter _Last) { 
			_Mybase::assign(_First, _Last); 
			_Mybase::_Correct_tail_elements();
		}
		
		vector(std::initializer_list<scalar_type> _Ilist) {
			_Mybase::assign(_Ilist.begin(), _Ilist.end()); 
			_Mybase::_Correct_tail_elements();
		}

		vector& operator=(const vector&) = default;
		
		vector& operator=(std::initializer_list<scalar_type> _Ilist) {
			this->assign(_Ilist.begin(), _Ilist.end());
			return *this;
		}

		subvector<scalar_type, block_type> operator()() {
			return subvector<scalar_type, block_type>(this->begin(), this->end());
		}
		
		subvector<scalar_type, block_type> operator()(size_t _First, size_t _Last) {
			return subvector<scalar_type, block_type>(this->ptr(_First), this->ptr(_Last));
		}
		
		const subvector<scalar_type, block_type> operator()() const {
			return subvector<scalar_type, block_type>(
				const_cast<scalar_type*>(this->begin()), 
				const_cast<scalar_type*>(this->end()) );
		}
		
		const subvector<scalar_type, block_type> operator()(size_t _First, size_t _Last) const {
			return subvector<scalar_type, block_type>(
				const_cast<scalar_type*>(this->begin()), 
				const_cast<scalar_type*>(this->end()) );
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
		vector for_each_block_copy(_UnaryOp _Transform_op) const {
			vector _Result;
			std::transform(this->begin<block_type>(), 
				           this->begin<block_type>()+this->block_size(), 
				           _Result.begin<block_type>(), _Transform_op);
			return _Result;
		}

		template<typename _BinOp>
		vector for_each_block_copy(const vector& _Args2, _BinOp _Transform_op) const {
			vector _Result;
			std::transform(this->begin<block_type>(), 
				           this->begin<block_type>()+this->block_size(), 
				           _Args2.begin<block_type>(), 
				           _Result.begin<block_type>(), _Transform_op);
			return _Result;
		}
				
		scalar_type sum() const {// sum_of( (*this)[N] )
			return fastest_vector_operation<scalar_type, block_type>::accumulate(
				this->begin<block_type>(),
				this->begin<block_type>() + this->block_size(), static_cast<scalar_type>(0), std::plus<>());
		}
		
		scalar_type product() const {// product_of( (*this)[N] )
			return fastest_vector_operation<scalar_type, block_type>::accumulate(
				this->begin<block_type>(), 
				this->begin<block_type>() + this->block_size(), static_cast<scalar_type>(0), std::multiplies<>());
		}
		
		scalar_type dot(const vector& _Right) const {// sum_of( (*this)[N]*_Right[N] )
			return fastest_vector_operation<scalar_type, block_type>::transform_accumulate(
				this->begin<block_type>(), 
				this->begin<block_type>() + this->block_size(), 
			   _Right.begin<block_type>(), static_cast<scalar_type>(0), std::plus<>(), std::multiplies<>());
		}
		
		scalar_type norm(scalar_type L) const {// pow( sum_of( pow((*this)[N], L), 1/L )
			const block_type _Arg1         = block_traits::set1(L);
			const auto       _Transform_op = [&_Arg1](auto&& _Arg0) { return pow(_Arg0, _Arg1); };

			const auto _Result = fastest_vector_operation<scalar_type, block_type>::transform_accumulate(
				this->begin<block_type>(), 
				this->begin<block_type>() + this->block_size(), static_cast<scalar_type>(0), std::plus<>(), _Transform_op);
			return pow(_Result, 1/L);
		}
		
		scalar_type length() const {// sqrt( dot(*this,*this) )
			scalar_type _Length_sqr = this->dot(*this);
			return sqrt(_Length_sqr);
		}

		void normalize() {
			scalar_type _Length_sqr = this->dot(*this);
			scalar_type _Error      = _Length_sqr - 1;
			if ( abs(_Error) > std::numeric_limits<scalar_type>::epsilon() ) {
				(*this) /= sqrt(_Length_sqr);
			}
		}

		vector<bool, _Size> operator==(const vector& _Right) const {
			using bvec = vector<bool, _Size>;
			_Return_generate_object( bvec, _Result, 
				std::transform(this->begin(), this->end(), _Right.begin(), _Result.begin(), std::equal_to<>()) );
		}
		vector<bool, _Size> operator!=(const vector& _Right) const {
			using bvec = vector<bool, _Size>;
			_Return_generate_object(bvec, _Result,
				std::transform(this->begin(), this->end(), _Right.begin(), _Result.begin(), std::not_equal_to<>()));
		}
		vector<bool, _Size> operator< (const vector& _Right) const {
			using bvec = vector<bool, _Size>;
			_Return_generate_object(bvec, _Result,
				std::transform(this->begin(), this->end(), _Right.begin(), _Result.begin(), std::less<>()));
		}
		vector<bool, _Size> operator> (const vector& _Right) const {
			using bvec = vector<bool, _Size>;
			_Return_generate_object(bvec, _Result,
				std::transform(this->begin(), this->end(), _Right.begin(), _Result.begin(), std::greater<>()));
		}
		vector<bool, _Size> operator<=(const vector& _Right) const {
			using bvec = vector<bool, _Size>;
			_Return_generate_object(bvec, _Result,
				std::transform(this->begin(), this->end(), _Right.begin(), _Result.begin(), std::less_equal<>()));
		}
		vector<bool, _Size> operator>=(const vector& _Right) const {
			using bvec = vector<bool, _Size>;
			_Return_generate_object(bvec, _Result,
				std::transform(this->begin(), this->end(), _Right.begin(), _Result.begin(), std::greater_equal<>()));
		}
		vector<bool, _Size> operator==(const _Ts& _Scalar) const {
			const auto _Func = [&_Scalar](auto&& _Arg0) { return _Arg0 == _Scalar; };
			using bvec = vector<bool, _Size>;
			_Return_generate_object(bvec, _Result,
				std::transform(this->begin(), this->end(), _Result.begin(), _Func));
		}
		vector<bool, _Size> operator!=(const _Ts& _Scalar) const {
			const auto _Func = [&_Scalar](auto&& _Arg0) { return _Arg0 != _Scalar; };
			using bvec = vector<bool, _Size>;
			_Return_generate_object(bvec, _Result,
				std::transform(this->begin(), this->end(), _Result.begin(), _Func));
		}
		vector<bool, _Size> operator< (const _Ts& _Scalar) const {
			const auto _Func = [&_Scalar](auto&& _Arg0) { return _Arg0 < _Scalar; };
			using bvec = vector<bool, _Size>;
			_Return_generate_object(bvec, _Result,
				std::transform(this->begin(), this->end(), _Result.begin(), _Func));
		}
		vector<bool, _Size> operator> (const _Ts& _Scalar) const {
			const auto _Func = [&_Scalar](auto&& _Arg0) { return _Arg0 > _Scalar; };
			using bvec = vector<bool, _Size>;
			_Return_generate_object(bvec, _Result,
				std::transform(this->begin(), this->end(), _Result.begin(), _Func));
		}
		vector<bool, _Size> operator<=(const _Ts& _Scalar) const {
			const auto _Func = [&_Scalar](auto&& _Arg0) { return _Arg0 <= _Scalar; };
			using bvec = vector<bool, _Size>;
			_Return_generate_object(bvec, _Result,
				std::transform(this->begin(), this->end(), _Result.begin(), _Func));
		}
		vector<bool, _Size> operator>=(const _Ts& _Scalar) const {
			const auto _Func = [&_Scalar](auto&& _Arg0) { return _Arg0 >= _Scalar; };
			using bvec = vector<bool, _Size>;
			_Return_generate_object(bvec, _Result,
				std::transform(this->begin(), this->end(), _Result.begin(), _Func));
		}
		
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
		vector  operator+ (scalar_type _Scalar) const {
			const block_type _Right         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left + _Right; };
			return this->for_each_block_copy(_Transform_op);
		}
		vector  operator- (scalar_type _Scalar) const {
			const block_type _Right         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left - _Right; };
			return this->for_each_block_copy(_Transform_op);
		}
		vector  operator* (scalar_type _Scalar) const {
			const block_type _Right         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left * _Right; };
			return this->for_each_block_copy(_Transform_op);
		}
		vector  operator/ (scalar_type _Scalar) const {
			const block_type _Right         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left / _Right; };
			return this->for_each_block_copy(_Transform_op);
		}
		vector  operator% (scalar_type _Scalar) const {
			const block_type _Right         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left % _Right; };
			return this->for_each_block_copy(_Transform_op);
		}
		vector& operator+=(scalar_type _Scalar) {
			const block_type _Right         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left + _Right; };
			this->for_each_block(_Transform_op);
			return *this;
		}
		vector& operator-=(scalar_type _Scalar) {
			const block_type _Right         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left - _Right; };
			this->for_each_block(_Transform_op);
			return *this;
		}
		vector& operator*=(scalar_type _Scalar) {
			const block_type _Right         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left * _Right; };
			this->for_each_block(_Transform_op);
			return *this;
		}
		vector& operator/=(scalar_type _Scalar) {
			const block_type _Right         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left / _Right; };
			this->for_each_block(_Transform_op);
			return *this;
		}
		vector& operator%=(scalar_type _Scalar) {
			const block_type _Right         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Right](auto&& _Left) { return _Left % _Right; };
			this->for_each_block(_Transform_op);
			return *this;
		}
		friend vector operator+(scalar_type _Scalar, const vector& _Right) {
			const block_type _Left         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Left](auto&& _Right) { return _Left + _Right; };
			return _Right.for_each_block_copy(_Transform_op);
		}
		friend vector operator-(scalar_type _Scalar, const vector& _Right) {
			const block_type _Left         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Left](auto&& _Right) { return _Left - _Right; };
			return _Right.for_each_block_copy(_Transform_op);
		}
		friend vector operator*(scalar_type _Scalar, const vector& _Right) {
			const block_type _Left         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Left](auto&& _Right) { return _Left * _Right; };
			return _Right.for_each_block_copy(_Transform_op);
		}
		friend vector operator/(scalar_type _Scalar, const vector& _Right) {
			const block_type _Left         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Left](auto&& _Right) { return _Left / _Right; };
			return _Right.for_each_block_copy(_Transform_op);
		}
		friend vector operator%(scalar_type _Scalar, const vector& _Right) {
			const block_type _Left         = block_traits::set1(_Scalar);
			const auto       _Transform_op = [&_Left](auto&& _Right) { return _Left % _Right; };
			return _Right.for_each_block_copy(_Transform_op);
		}

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
	};

#define VECTORN ::clmagic::vector<_Ts, _Size, _Tb>
#define VECTOR2 ::clmagic::vector<_Ts, 2, _Tb>
#define VECTOR3 ::clmagic::vector<_Ts, 3, _Tb>
#define VECTOR4 ::clmagic::vector<_Ts, 4, _Tb>

	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN fmod(const VECTORN& _Left, const VECTORN& _Right) {
		auto _Transform_op = [](const _Tb& _Arg0, const _Tb& _Arg1) { return fmod(_Arg0, _Arg1); };
		return _Left.for_each_block_copy(_Right, _Transform_op);
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN fmod(const VECTORN& _Left, SCALAR _Scalar) {
		const _Tb  _Arg1 = block_traits<_Tb>::set1(_Scalar);
		const auto _Func = [&_Arg1](const _Tb& _Arg0) { return fmod(_Arg0, _Arg1); };
		return _Left.for_each_block_copy(_Func);
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN pow(const VECTORN& _Left, const VECTORN& _Right) {
		auto _Transform_op = [](const _Tb& _Arg0, const _Tb& _Arg1) { return pow(_Arg0, _Arg1); };
		return _Left.for_each_block_copy(_Right, _Transform_op);
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN pow(const VECTORN& _Left, SCALAR _Scalar) {
		const _Tb  _Arg1 = block_traits<_Tb>::set1(_Scalar);
		const auto _Func = [&_Arg1](const _Tb& _Arg0) { return pow(_Arg0, _Arg1); };
		return _Left.for_each_block_copy(_Func);
	}	

	template<typename _Ts, size_t _Size, typename _Tb> inline
	SCALAR dot(const VECTORN& _Left, const VECTORN& _Right) {
		return _Left.dot(_Right);
	}
	
	template<typename _Ts, size_t _Size, typename _Tb> inline
	SCALAR norm(const VECTORN& _Left, SCALAR L) {
		return _Left.norm(L);
	}
	
	template<typename _Ts, size_t _Size, typename _Tb> inline
	SCALAR length(const VECTORN& _Left) {
		return _Left.length();
	}

	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN normalize(const VECTORN& _Left) {
		const SCALAR _Length_sqr = dot(_Left, _Left);
		const SCALAR _Error      = _Length_sqr - 1;
		return (abs(_Error) > std::numeric_limits<_Ts>::epsilon()) ? _Left/sqrt(_Length_sqr) : _Left;
	}

	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN cross3(const VECTORN& _Left, const VECTORN& _Right) {
		return VECTORN{
			_Left[1] * _Right[2] - _Left[2] * _Right[1],
			_Left[2] * _Right[0] - _Left[0] * _Right[2],
			_Left[0] * _Right[1] - _Left[1] * _Right[0]
		};
	}

	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN abs(const VECTORN& _Left) {// abs(v)
		return _Left.for_each_block_copy( [](const _Tb& x){ return abs(x); } );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN floor(const VECTORN& _Left) {// floor(v)
		return _Left.for_each_block_copy( [](const _Tb& x){ return floor(x); } );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN ceil(const VECTORN& _Left) {// ceil(v)
		return _Left.for_each_block_copy( [](const _Tb& x){ return ceil(x); } );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN trunc(const VECTORN& _Left) {// trunc(v)
		return _Left.for_each_block_copy( [](const _Tb& x){ return trunc(x); } );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN round(const VECTORN& _Left) {// round(v)
		return _Left.for_each_block_copy( [](const _Tb& x){ return round(x); } );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN sqrt(const VECTORN& _Left) {// sqrt(v)
		return _Left.for_each_block_copy( [](const _Tb& x){ return sqrt(x); } );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN cbrt(const VECTORN& _Left) {// cbrt(v)
		return _Left.for_each_block_copy( [](const _Tb& x){ return cbrt(x); } );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN invsqrt(const VECTORN& _Left) {// invsqrt(v)
		return _Left.for_each_block_copy( [](const _Tb& x){ return invsqrt(x); } );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN invcbrt(const VECTORN& _Left) {// invcbrt(v)
		return _Left.for_each_block_copy( [](const _Tb& x){ return invcbrt(x); } );
	}

	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN sin(const VECTORN& _Left) {// sin(v)
		return _Left.for_each_block_copy( [](const _Tb& x){ return sin(x); } );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN cos(const VECTORN& _Left) {
		return _Left.for_each_block_copy( [](const _Tb& x){ return cos(x); } );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN tan(const VECTORN& _Left) {// tan(v)
		return _Left.for_each_block_copy( [](const _Tb& x){ return tan(x); } );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN asin(const VECTORN& _Left) {// asin(v)
		return _Left.for_each_block_copy( [](const _Tb& x){ return asin(x); } );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN acos(const VECTORN& _Left) {// acos(v)
		return _Left.for_each_block_copy( [](const _Tb& x){ return acos(x); } );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN atan(const VECTORN& _Left) {// atan(v)
		return _Left.for_each_block_copy( [](const _Tb& x){ return atan(x); } );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN atan2(const VECTORN& _Left, const VECTORN& _Right) {// atan(yv, xv)
		return _Left.for_each_block_copy( [](const _Tb& y, const _Tb& x){ return atan2(y, x); } );
	}

	/*- - - - - - - - - - - - - - - - - - - * unit_vector * - - - - - - - - - - - - - - - - - - - - -*/
	template<typename _Ts, size_t _Size, typename _Tb = _SIMD4_t<_Ts>>
	struct unit_vector : public vector<_Ts, _Size, _Tb> {
		using _Mybase     = vector<_Ts, _Size, _Tb>;
		using block_type  = _Tb;
		using scalar_type = _Ts;

		unit_vector() = default;
		unit_vector(std::initializer_list<scalar_type> _Ilist, bool _Unitized = false)
			: _Mybase(_Ilist) {
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
	
#define UNIT_VECTORN ::clmagic::unit_vector<_Ts, _Size, _Tb>
#define UNIT_VECTOR3 ::clmagic::unit_vector3<_Ts, _Tb>

	template<typename _Ts, size_t _Size, typename _Tb>
	UNIT_VECTORN abs(const UNIT_VECTORN& _Left) {
		return UNIT_VECTORN(-_Left, true);
	}

	template<typename _Ts, size_t _Size, typename _Tb>
	SCALAR length(const UNIT_VECTORN& _Left) {
		return static_cast<_Ts>(1);
	}

	/*- - - - - - - - - - - - - - - - - - - * vector_any * - - - - - - - - - - - - - - - - - - - - -*/
	template<typename _Ts, typename _Tb = _SIMD4_t<_Ts>>
	class _Vector_any_data {
		std::vector<_Tb> _My_blocks;
		size_t           _My_size;
	public:
		using block_type   = _Tb;
		using scalar_type  = _Ts;
		using block_traits = clmagic::block_traits<block_type>;

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

		template<typename _Ty = scalar_type>
		_Ty* ptr() {
			return reinterpret_cast<_Ty*>(_My_blocks.data());
		}
		
		template<typename _Ty = scalar_type>
		const _Ty* ptr() const {
			return reinterpret_cast<const _Ty*>(_My_blocks.data());
		}
		
		template<typename _Ty = scalar_type>
		_Ty* ptr(size_t _Pos) {
			return reinterpret_cast<_Ty*>(this->ptr<scalar_type>() + _Pos);
		}
		
		template<typename _Ty = scalar_type>
		const _Ty* ptr(size_t _Pos) const {
			return reinterpret_cast<const _Ty*>(this->ptr<scalar_type>() + _Pos);
		}
		
		template<typename _Ty = scalar_type>
		_Ty& at(size_t _Pos) {
			assert(_Pos < this->size());
			return *(this->ptr<_Ty>(_Pos));
		}
		
		template<typename _Ty = scalar_type>
		const _Ty& at(size_t _Pos) const {
			assert(_Pos < this->size());
			return *(this->ptr<_Ty>(_Pos));
		}

		template<typename _Ty = scalar_type>
		_Ty* begin() {
			return this->ptr<_Ty>();
		}
		
		template<typename _Ty = scalar_type>
		const _Ty* begin() const {
			return this->ptr<_Ty>();
		}
		
		template<typename _Ty = scalar_type>
		_Ty* end() {
			return reinterpret_cast<_Ty*>(this->ptr<scalar_type>() + this->size());
		}
		
		template<typename _Ty = scalar_type>
		const _Ty* end() const {
			return reinterpret_cast<const _Ty*>(this->ptr<scalar_type>() + this->size());
		}

		scalar_type& operator[](size_t _Pos) {
			return this->at(_Pos); 
		}
		const scalar_type& operator[](size_t _Pos) const {
			return this->at(_Pos); 
		}

		void _Correct_tail_elements() {
			const size_t _My_real_size = this->real_size();
			if (_My_real_size > _My_size) {
				std::fill(this->ptr(_My_size), this->ptr(_My_real_size), static_cast<scalar_type>(0));
			}
		}
		
		void swap(_Vector_any_data& _Right) noexcept {
			std::swap(std::move(_My_blocks), std::move(_Right._My_blocks));
			std::swap(_My_size, _Right._My_size);
		}

		void release() noexcept {
			if (_My_size != 0) {
				std::swap(std::vector<block_type>(), std::move(_My_blocks));// noexcept
				_My_size = 0;
			}
		}

		void resize(size_t _Newsize) {
			size_t _Real_newsize = ceil(_Newsize, block_traits::size());
			_My_blocks.resize(_Real_newsize / block_traits::size());
			_My_size = _Newsize;
		}

		template<typename _Iter>
		void assign(_Iter _First, _Iter _Last) {
			size_t _Diff = distance(_First, _Last);

			_Vector_any_data _New_data;
			_New_data.resize(_Diff);
			auto _Dest = std::copy(_First, _Last, _New_data.begin<scalar_type>());
			std::fill(_Dest, _New_data.end<scalar_type>(), static_cast<scalar_type>(0));

			_New_data.swap(*this);
		}
		
		void assign(const scalar_type& _Val) {
			std::fill(this->begin<scalar_type>(), this->end<scalar_type>(), _Val);
		}
	
		template<typename _UnaryOp>
		void for_each_scalar(_UnaryOp _Transform_op) {// for each scalar by (_Transform_op*)(scalar_type)
			std::transform(this->begin<scalar_type>(), 
				           this->end<scalar_type>(), 
				           this->begin<scalar_type>(), _Transform_op);
		}

		template<typename _BinOp>
		void for_each_scalar(const _Vector_any_data& _Args2, _BinOp _Transform_op) {
			std::transform(this->begin<scalar_type>(),
				           this->end<scalar_type>(), 
				          _Args2.begin<scalar_type>(), 
				           this->end<scalar_type>(), _Transform_op);
		}
		
		template<typename _UnaryOp>
		void for_each_block(_UnaryOp _Transform_op) {// for each block by (_Transform_op*)(block_type)
			std::transform(this->begin<block_type>(), 
				           this->begin<block_type>() + this->block_size(), 
				           this->begin<block_type>(), _Transform_op);
		}

		template<typename _BinOp>
		void for_each_block(const _Vector_any_data& _Args2, _BinOp _Transform_op) {
			std::transform(this->begin<block_type>(), 
				           this->begin<block_type>() + this->block_size(), 
				          _Args2.begin<block_type>(), 
				           this->begin<block_type>(), _Transform_op);
		}
	
		std::string to_string() const {
			auto       _First = this->begin<scalar_type>();
			const auto _Last  = this->end<scalar_type>();
			std::string _Str  = std::to_string(*_First++);
			for ( ; _First != _Last; ++_First) {
				_Str += ' ';
				_Str += std::to_string(*_First);
			}
			return _Str;
		}
	};

	template<typename _Ts, typename _Tb = _SIMD4_t<_Ts>>
	class vector_any : public _Vector_any_data<_Ts, _Tb> {
		using _Mybase = _Vector_any_data<_Ts, _Tb>;
	public:
		using block_type   = _Tb;
		using scalar_type  = _Ts;
		using block_traits = clmagic::block_traits<block_type>;

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
		
		vector_any& operator=(const vector_any& _Right) = default;
		
		vector_any& operator=(vector_any&& _Right) {
			_Right.swap(*this);
			_Right.release();
		}

		subvector<scalar_type, block_type> operator()(size_t _First, size_t _Last) {
			return subvector<scalar_type, block_type>(this->ptr(_First), this->ptr(_Last));
		}
		
		const subvector<scalar_type, block_type> operator()(size_t _First, size_t _Last) const {
			return subvector<scalar_type, block_type>(
				const_cast<scalar_type*>(this->ptr(_First)), const_cast<scalar_type*>(this->ptr(_Last)) );
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
		
		friend std::ostream& operator<<(std::ostream& _Ostr, const vector_any& _Left) {
			return (_Ostr << _Left.to_string()); }
	};

	/*- - - - - - - - - - - - - - - - - - - _Vector_cast - - - - - - - - - - - - - - - - - - - - -*/
	template<typename _OutVec, typename _InVec>
	struct _Vector_cast {
		using dest_type = _OutVec;
		using src_type  = _InVec;

		dest_type operator()(const src_type& v) const {
			return dest_type(v.begin(), v.end());
		}
	};

	template<typename _Ts, size_t _Size, typename _Tb, typename _InVec>
	struct _Vector_cast<vector<_Ts, _Size, _Tb>, _InVec> {
		using dest_type = vector<_Ts, _Size, _Tb>;
		using src_type  = _InVec;

		dest_type operator()(const src_type& v) const {
			return dest_type( v.begin(), v.begin() + min(v.size(), dest_type::size()) );
		}
	};
	
	template<typename _OutVec, typename _InVec>
	_OutVec vector_cast(const _InVec& v) {
		return _Vector_cast<_OutVec, _InVec>()(v);
	}

	/*- - - - - - - - - - - - - - - - - - - vector_traits - - - - - - - - - - - - - - - - - - - - -*/
	template<typename _Ts>
		constexpr bool is_vector_v = false;
	template<typename _Ts, size_t _Size, typename _Tb>
		constexpr bool is_vector_v< vector<_Ts, _Size, _Tb> > = true;
	template<typename _Ts, typename _Tb>
		constexpr bool is_vector_v< vector_any<_Ts, _Tb> > = true;

	template<typename _Ts, size_t _Size, typename _Tb>
		constexpr bool is_support_scalar_operator< vector<_Ts, _Size, _Tb> > = true;
	template<typename _Ts, typename _Tb>
		constexpr bool is_support_scalar_operator< vector_any<_Ts, _Tb> > = true;

	/*- - - - - - - - - - - - - - - - - - - vector_alias - - - - - - - - - - - - - - - - - - - - -*/
	template<typename _Ts, typename _Tb = _SIMD4_t<_Ts>> 
		using vector2 = vector<_Ts, 2, _Tb>;
	template<typename _Ts, typename _Tb = _SIMD4_t<_Ts>> 
		using vector3 = vector<_Ts, 3, _Tb>;
	template<typename _Ts, typename _Tb = _SIMD4_t<_Ts>> 
		using vector4 = vector<_Ts, 4, _Tb>;
	template<typename _Ts, typename _Tb = _SIMD4_t<_Ts>> 
		using unit_vector3 = unit_vector<_Ts, 3, _Tb>;
	using point2_size_t = vector2<size_t>;

	/*- - - - - - - - - - - - - - - - - - - vector_function - - - - - - - - - - - - - - - - - - - - -*/
	template<typename _Ts, size_t _Size, typename _Tb> inline
	std::string to_string(const VECTORN& v) {
		return v.to_string();
	}
	template<typename _Ts, typename _Tb> inline
	std::string to_string(const vector_any<_Ts, _Tb>& v) {
		return v.to_string();
	}

	// compare function
	template<size_t _Size, typename _Tb>
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
	}

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