//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
// Apache Licene 2.0
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_math_lapack_VECTOR_h_
#define clmagic_math_lapack_VECTOR_h_
#include "../general/simd.h"// block_traits<>, simd_operator
#include "../general/general.h"// include shuffle<>
#include "../real/bitset.h"// include ceil(Number, Bound)
#include "../real/real.h"// include CSTD math
#include <stack>
#include <vector>
#include <string>
#include <algorithm>

namespace clmagic{

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

	/*- - - - - - - - - - - - - - - - - - - _Vector_operation<> - - - - - - - - - - - - - - - - - - - - -*/
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

		using scalar_type  = _Ts;

		template<typename _Fn>
		static _OutIt func_1st(_InIt _First, _InIt _Last, _OutIt _Dest, _Fn _Func, 
			size_t _Inc1 = 1, size_t _Inc2 = 1) {
			for (; _First != _Last; _First+=_Inc1, _Dest+=_Inc2) {
				*_Dest = _Func(*_First);
			}
			return _Dest;
		}

		template<typename _Fn>
		static _OutIt func_2nd(_InIt _First1, _InIt _Last1, _InIt _First2, _OutIt _Dest, _Fn _Func,
			size_t _Inc1 = 1, size_t _Inc2 = 1, size_t _Inc3 = 1) {
			for (; _First1 != _Last1; _First1+=_Inc1, _First2+=_Inc2, _Dest+=_Inc3) {
				*_Dest = _Func(*_First1, *_First2);
			}
			return _Dest;
		}

		/*<afunc>
			<mean>assign-function</mean>
			</afunc>*/
		template<typename _Afn>
		static _Ts& afunc(_InIt _First, _InIt _Last, _Ts& _Result, _Afn _Afunc,
			size_t _Inc1 = 1) {
			for (; _First != _Last; _First+=_Inc1) {
				_Afunc(_Result, *_First);
			}
			return _Result;
		}

		template<typename _Afn, typename _Fn>
		static _Ts& afunc_1st(_InIt _First, _InIt _Last, _Ts& _Result, _Afn _Afunc, _Fn _Func,
			size_t _Inc = 1) {
			// for [_First, _Last): _Afunc(_Result, _Func(*_First))
			// _Afunc = [](auto&, auto&&), for example: [](auto& Res, auto&& X){ Res += X; } ...
			// _Func  = [](const _Ts&), for example: exp2<_Tb> ...
			for ( ; _First != _Last; _First+=_Inc) {
				_Afunc(_Result, _Func(*_First));
			}
			return _Result;
		}

		template<typename _Afn, typename _Fn>
		static _Ts& afunc_2nd(_InIt _First1, _InIt _Last1, _InIt _First2, _Ts& _Result, _Afn _Afunc, _Fn _Func,
			size_t _Inc1 = 1, size_t _Inc2 = 1) {
			// for [_First1, _Last1): _Afunc(_Result, _Func(*_First1, *_First2))
			// _Afunc = [](auto&, auto&&),                for example: [](auto& Res, auto&& X){ Res += X; } ...
			// _Func  = [](const _Ts&, const _Ts&), for example: std::multiplies<_Tb>() ...
			for ( ; _First1 != _Last1; _First1+=_Inc1, _First2+=_Inc2) {
				_Afunc(_Result, _Func(*_First1, *_First2));
			}
			return _Result;
		}

		template<typename _Fn>
		static _Ts& norm(_InIt _First, _InIt _Last, _Ts L, _Ts& _Result,
			size_t _Inc = 1) {
			afunc_1st(_First, _Last, _Result, 
				[](auto& _Res, auto&& _X) { _Res += _X; },
				[](auto&& _X) { return pow(abs(_X), L); }, 
				_Inc
			);

			_Result = pow(_Result, static_cast<_Ts>(1) / L);
			return _Result;
		}

		/*<dot>
			<code>
				auto _Result = static_cast<_Ts>(0);
				auto_vector_operation<_Ts, _Tb>::afunc_2st( _First1, _Last1, _First2, _Result, 
					[](auto& _Res, auto&& _X){ _Res += _X; }, 
					std::multiplies<>() );
			</code>
			</dot>*/
	};

	template<typename _Ts, typename _Tb>
	struct _Vector_operation<_Ts, _Tb, _Vector_accelerate_level::Fastest> {
		using _OutIt = _Tb*;
		using _InIt  = const _Tb *;

		using scalar_type  = _Ts;
		using block_type   = _Tb;
		using block_traits = ::clmagic::block_traits<_Tb>;
		using scalar_operation = _Vector_operation<_Ts, _Tb, _Vector_accelerate_level::Norm>;

		template<typename _Fn>
		static _OutIt func_1st(_InIt _First, _InIt _Last, _OutIt _Dest, _Fn _Func) {
			for (; _First != _Last; ++_First, ++_Dest) {
				*_Dest = _Func(*_First);
			}
			return _Dest;
		}

		template<typename _Fn>
		static _OutIt func_2nd(_InIt _First1, _InIt _Last1, _InIt _First2, _OutIt _Dest, _Fn _Func) {
			for (; _First1 != _Last1; ++_First1, ++_First2, ++_Dest) {
				*_Dest = _Func(*_First1, *_First2);
			}
			return _Dest;
		}

		/*<afunc>
			<mean>assign-function</mean>
			</afunc>*/
		template<typename _Afn>
		static _Ts& afunc(_InIt _First, _InIt _Last, _Ts& _Result, _Afn _Afunc) {
			if (_First != _Last) {
				auto _Temp = *_First++;
				for (; _First != _Last; ++_First) {
					_Afunc(_Temp, *_First);
				}

				auto _First2 = reinterpret_cast<_Ts*>(&_Temp);
				scalar_operation::afunc(_First2, _First2+block_traits::size(), _Result, _Afunc);
			}
			return _Result;
		}

		template<typename _Afn, typename _Fn>
		static _Ts& afunc_1st(_InIt _First, _InIt _Last, _Ts& _Result, _Afn _Afunc, _Fn _Func) {
			// for [_First, _Last): _Afunc(_Result, _Func(*_First))
			// _Afunc = [](auto&, auto&&), for example: [](auto& Res, auto&& X){ Res += X; } ...
			// _Func  = [](const _Tb&), for example: exp2<_Tb> ...
			if (_First != _Last) {
				auto _Temp = _Func(*_First++);
				for ( ; _First != _Last; ++_First) {
					_Afunc(_Temp, _Func(*_First));
				}

				auto _First_temp = reinterpret_cast<_Ts*>(&_Temp);
				scalar_operation::afunc(_First_temp, _First_temp+block_traits::size(), _Result, _Afunc);
			}
			return _Result;
		}

		template<typename _Afn, typename _Fn>
		static _Ts& afunc_2nd(_InIt _First1, _InIt _Last1, _InIt _First2, _Ts& _Result, _Afn _Afunc, _Fn _Func) {
			// for [_First1, _Last1): _Afunc(_Result, _Func(*_First1, *_First2))
			// _Afunc = [](auto&, auto&&),                for example: [](auto& Res, auto&& X){ Res += X; } ...
			// _Func  = [](const _Tb&, const _Tb&), for example: std::multiplies<_Tb>() ...
			if (_First1 != _Last1) {
				auto _Temp = _Func(*_First1++, *_First2++);
				for ( ; _First1 != _Last1; ++_First1, ++_First2) {
					_Afunc(_Temp, _Func(*_First1, *_First2));
				}

				auto _First_temp = reinterpret_cast<_Ts*>(&_Temp);
				scalar_operation::afunc(_First_temp, _First_temp+block_traits::size(), _Result, _Afunc);
			}
			return _Result;
		}

		template<typename _Fn>
		static _Ts& norm(_InIt _First, _InIt _Last, _Ts L, _Ts& _Result) {
			/* abs_pow(A[0]) + abs_pow(A[1]) + abs_pow(A[2]) + ... + abs_pow(A[N]) */
			/******************** equal *******************/
			/*
			(abs_pow(A[0]) + abs_pow(A[4]) + abs_pow(A[8]) + abs_pow(A[12]))
				+ (abs_pow(A[1]) + abs_pow(A[5]) + abs_pow(A[9]) + abs_pow(A[13])) 
				+ ... 
				+ (abs_pow(A[N-12]) + abs_pow(A[N-8]) + abs_pow(A[N-4]) + abs_pow(A[N])) 
			*/
			
			afunc_1st(_First, _Last, _Result, 
				[](auto& _Res, auto&& _X) { _Res += _X; },
				[](auto&& _X) { return pow(abs(_X), L); }
			);

			_Result = pow(_Result, static_cast<_Ts>(1) / L);
			return _Result;
		}

		/*<dot>
			<code>
				auto _Result = static_cast<_Ts>(0);
				auto_vector_operation<_Ts, _Tb>::afunc_2st( _First1, _Last1, _First2, _Result, 
					[](auto& _Res, auto&& _X){ _Res += _X; }, 
					std::multiplies<>() );
			</code>
			</dot>*/
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

		template<typename _Fn>
		static _OutIt func_1st(_InIt _First, _InIt _Last, _OutIt _Dest, _Fn _Func, 
			size_t _Leading_size = 0, size_t _Tail_size = 0) {
			if (_Leading_size != 0) {
				const _Tb       _Temp      = _Func(*(_First - 1));
				const scalar_type* _Last_temp = reinterpret_cast<const _Ts*>((&_Temp) + 1);
				std::copy(_Last_temp-_Leading_size, _Last_temp, ((scalar_type*)_Dest) - _Leading_size );
			}

			for ( ; _First != _Last; ++_First, ++_Dest) {
				*_Dest = _Func(*_First);
			}

			if (_Tail_size != 0) {// vector-length is usually large, when _Tail_size != 0
				const _Tb       _Temp       = _Func(*_First);
				const scalar_type* _First_temp = reinterpret_cast<const _Ts*>(&_Temp);
				std::copy(_First_temp, _First_temp+_Tail_size, (scalar_type*)_Dest);
			}

			return _Dest;// Almost useless
		}
	
		template<typename _Fn>
		static _OutIt func_2nd(_InIt _First1, _InIt _Last1, _InIt _First2, _OutIt _Dest, _Fn _Func, 
			size_t _Leading_size = 0, size_t _Tail_size = 0) {
			if (_Leading_size != 0) {
				const _Tb       _Temp      = _Func(*(_First1 - 1), *(_First2 - 1));
				const scalar_type* _Last_temp = reinterpret_cast<const scalar_type*>((&_Temp) + 1);
				std::copy(_Last_temp-_Leading_size, _Last_temp, ((scalar_type*)_Dest) - _Leading_size );
			}

			for (; _First1 != _Last1; ++_First1, ++_First2, ++_Dest) {
				*_Dest = _Func(*_First1, *_First2);
			}

			if (_Tail_size != 0) {// vector-length is usually large, when _Tail_size != 0
				const _Tb       _Temp       = _Func(*_First1, *_First2);
				const scalar_type* _First_temp = reinterpret_cast<const scalar_type*>(&_Temp);
				std::copy(_First_temp, _First_temp+_Tail_size, (scalar_type*)_Dest);
			}

			return _Dest;// Almost useless
		}
	
		/*<afunc>
			<mean>assign-function</mean>
			</afunc>*/
		template<typename _Afn>
		static _Ts& afunc(_InIt _First, _InIt _Last, _Ts& _Result, _Afn _Afunc, 
			size_t _Leading_size = 0, size_t _Tail_size = 0) {
			if (_Leading_size != 0) {
				auto _Last_temp = reinterpret_cast<const _Ts*>(_First);
				scalar_operation::afunc(_Last_temp-_Leading_size, _Last_temp, _Result, _Afunc);
			}

			if (_First != _Last) {
				auto _Temp = *_First++;
				for ( ; _First != _Last; ++_First) {
					_Afunc(_Temp, *_First);
				}
				auto _First_temp = reinterpret_cast<_Ts*>(&_Temp);
				scalar_operation::afunc(_First_temp, _First_temp+block_traits::size(), _Result, _Afunc);
			}

			if (_Tail_size != 0) {
				auto _First_temp = reinterpret_cast<const _Ts*>(_First);
				scalar_operation::afunc(_First_temp, _First_temp+_Tail_size, _Result, _Afunc);
			}

			return _Result;
		}

		template<typename _Afn, typename _Fn>
		static _Ts& afunc_1st(_InIt _First, _InIt _Last, _Ts& _Result, _Afn _Afunc, _Fn _Func, 
			size_t _Leading_size = 0, size_t _Tail_size = 0) {
			if (_Leading_size != 0) {
				const auto _Temp      = _Func(*(_First - 1));
				auto       _Last_temp = reinterpret_cast<const _Ts*>((&_Temp) + 1);
				scalar_operation::afunc(_Last_temp-_Leading_size, _Last_temp, _Result, _Afunc);
			}

			block_operation::afunc_1st(_First, _Last, _Result, _Afunc, _Func);// impotant!

			if (_Tail_size != 0) {
				const auto _Temp       = _Func(*_Last);
				auto       _First_temp = reinterpret_cast<const _Ts*>(&_Temp);
				scalar_operation::afunc(_First_temp, _First_temp+_Tail_size, _Result, _Afunc);
			}

			return _Result;
		}

		template<typename _Afn, typename _Fn>
		static _Ts& afunc_2nd(_InIt _First1, _InIt _Last1, _InIt _First2, _Ts& _Result, _Afn _Afunc, _Fn _Func,
			size_t _Leading_size = 0, size_t _Tail_size = 0) {
			if (_Leading_size != 0) {
				const auto _Temp      = _Func(*(_First1 - 1), *(_First2 - 1));
				auto       _Last_temp = reinterpret_cast<const _Ts*>((&_Temp) + 1);
				scalar_operation::afunc(_Last_temp-_Leading_size, _Last_temp, _Result, _Afunc);
			}

			if (_First1 != _Last1) {
				auto _Temp = _Func(*_First1++, *_First2++);
				for ( ; _First1 != _Last1; ++_First1, ++_First2) {
					_Afunc(_Temp, _Func(*_First1, *_First2));
				}

				auto _First_temp = reinterpret_cast<_Ts*>(&_Temp);
				scalar_operation::afunc(_First_temp, _First_temp+block_traits::size(), _Result, _Afunc);
			}

			if (_Tail_size != 0) {
				const auto _Temp       = _Func(*_First1, *_First2);
				auto       _First_temp = reinterpret_cast<const _Ts*>(&_Temp);
				scalar_operation::afunc(_First_temp, _First_temp+_Tail_size, _Result, _Afunc);
			}

			return _Result;
		}
	
		template<typename _Fn>
		static _Ts& norm(_InIt _First, _InIt _Last, _Ts L, _Ts& _Result, 
			size_t _Leading_size = 0, size_t _Tail_size = 0) {
			afunc_1st(_First, _Last, _Result, 
				[](auto& _Res, auto&& _X) { _Res += _X; },
				[](auto&& _X) { return pow(abs(_X), L); },
				_Leading_size, _Tail_size
			);

			_Result = pow(_Result, static_cast<_Ts>(1) / L);
			return _Result;
		}
	};

	template<typename _Ts, typename _Tb>
	struct _Vector_operation<_Ts, _Tb, _Vector_accelerate_level::Auto> {
		using _OutIt = void*;
		using _InIt  = const void *;

		using size_type    = ::clmagic::vector_size<_Ts, _Tb>;
		using block_traits = ::clmagic::block_traits<_Tb>;

		using scalar_operation  = _Vector_operation<_Ts, _Tb, _Vector_accelerate_level::Norm>;
		using block_operation   = _Vector_operation<_Ts, _Tb, _Vector_accelerate_level::Fastest>;
		using fastest_operation = block_operation;
		using fast_operation    = _Vector_operation<_Ts, _Tb, _Vector_accelerate_level::Fast>;
		using norm_operation    = scalar_operation;

		template<typename _Fn>
		static _OutIt func_1st(_InIt _First, _InIt _Last, _OutIt _Dest, _Fn _Func, 
			const size_type& _Vsize) {
			if (_Vsize.can_fastest()) {
				return fastest_operation::func_1st(static_cast<const _Tb*>(_First), static_cast<const _Tb*>(_Last),
					static_cast<_Tb*>(_Dest), _Func);
			} else if (!_Vsize.empty()) {
				auto       _First_bk = reinterpret_cast<const _Tb*>( (const _Ts*)(_First) + _Vsize.leading_size );
				const auto _Last_bk  = _First_bk + _Vsize.block_size;
				auto       _Dest_bk  = reinterpret_cast<_Tb*>( (_Ts*)(_Dest) + _Vsize.leading_size );
				return fast_operation::func_1st(_First_bk, _Last_bk, _Dest_bk, _Func,
					_Vsize.leading_size, _Vsize.tail_size);
			}

			return norm_operation::func_1st(static_cast<const _Ts*>(_First), static_cast<const _Ts*>(_Last),
				static_cast<_Ts*>(_Dest), _Func);
		}

		template<typename _Fn>
		static _OutIt func_1st(_InIt _First, _InIt _Last, _OutIt _Dest, _Fn _Func,
			size_t _Inc1 = 1, size_t _Inc2 = 1) {
			if (_Inc1 == 1 && _Inc2 == 1) {// memory-continue
				return func_1st( _First, _Last, _Dest, _Func, size_type((const _Ts*)_First, (const _Ts*)_Last) );
			}

			return norm_operation::func_1st((const _Ts*)(_First), (const _Ts*)(_Last), (_Ts*)(_Dest), _Func, 
				_Inc1, _Inc2);
		}
	
		template<typename _Fn>
		static _OutIt func_2nd(_InIt _First1, _InIt _Last1, _InIt _First2, _OutIt _Dest, _Fn _Func,
			const size_type& _Vsize1, const size_type& _Vsize2) {
			if (_Vsize1 == _Vsize2) {
				if (_Vsize1.can_fastest()) {
					return fastest_operation::func_2nd(static_cast<const _Tb*>(_First1), static_cast<const _Tb*>(_Last1),
						static_cast<const _Tb*>(_First2), static_cast<_Tb*>(_Dest), _Func);
				} else if(!_Vsize1.empty()) {
					auto       _First1_bk = reinterpret_cast<const _Tb*>((const _Ts*)(_First1) + _Vsize1.leading_size);
					const auto _Last1_bk  = _First1_bk + _Vsize1.block_size;
					auto       _First2_bk = reinterpret_cast<const _Tb*>((const _Ts*)(_First2) + _Vsize1.leading_size);
					auto       _Dest_bk   = reinterpret_cast<_Tb*>(      (_Ts*)(_Dest)         + _Vsize1.leading_size);
					return fast_operation::func_2nd(_First1_bk, _Last1_bk, _First2_bk, _Dest_bk, _Func,
						_Vsize1.leading_size, _Vsize1.tail_size);
				}
			}

			return norm_operation::func_2nd(static_cast<const _Ts*>(_First1), static_cast<const _Ts*>(_Last1), 
				static_cast<const _Ts*>(_First2), static_cast<_Ts*>(_Dest), _Func );
		}

		template<typename _Fn>
		static _OutIt func_2nd(_InIt _First1, _InIt _Last1, _InIt _First2, _OutIt _Dest, _Fn _Func, 
			size_t _Inc1 = 1, size_t _Inc2 = 1, size_t _Inc3 = 1) {
			if (_Inc1 == 1 && _Inc2 == 1 && _Inc3 == 1) {// is_continue
				const auto _Diff = std::distance((const _Ts*)(_First1), (const _Ts*)(_Last1));

				return func_2nd( _First1, _Last1, _First2, _Dest, _Func, 
					size_type((const _Ts*)_First1, (const _Ts*)_Last1), 
					size_type((const _Ts*)_First2, (const _Ts*)_First2 + _Diff) );
			}

			return norm_operation::func_2nd(static_cast<const _Ts*>(_First1), static_cast<const _Ts*>(_Last1),
				static_cast<const _Ts*>(_First2), static_cast<_Ts*>(_Dest), _Func, 
				_Inc1, _Inc2, _Inc3);
		}
	
		template<typename _Afn>
		static _Ts& afunc(_InIt _First, _InIt _Last, _Ts& _Result, _Afn _Afunc, const size_type& _Vsize) {
			if ( _Vsize.can_fastest() ) {
				return fastest_operation::afunc((const _Tb*)_First, (const _Tb*)_Last, _Result, _Afunc);
			} else if ( !_Vsize.empty() ) {
				auto _First_bk = reinterpret_cast<const _Tb*>( (const _Ts*)(_First) + _Vsize.leading_size );
				return fast_operation::afunc(_First_bk, _First_bk+_Vsize.block_size, _Result, _Afunc);
			}

			return norm_operation::afunc((const _Ts*)_First, (const _Ts*)_Last, _Result, _Afunc);
		}

		template<typename _Afn>
		static _Ts& afunc(_InIt _First, _InIt _Last, _Ts& _Result, _Afn _Afunc,
			size_t _Inc = 1) {
			if (_Inc == 1) {
				return afunc(_First, _Last, _Result, _Afunc, size_type((const _Ts*)_First, (const _Ts*)_Last));
			}

			return norm_operation::afunc((const _Ts*)_First, (const _Ts*)_Last, _Result, _Afunc,
				_Inc);
		}

		template<typename _Afn, typename _Fn>
		static _Ts& afunc_1st(_InIt _First, _InIt _Last, _Ts& _Result, _Afn _Afunc, _Fn _Func,
			const size_type& _Vsize) {
			if ( _Vsize.can_fastest() ) {
				return fastest_operation::afunc_1st((const _Tb*)_First, (const _Tb*)_Last, _Result, _Afunc, _Func);
			} else if ( !_Vsize.empty() ) {
				auto _First_bk = reinterpret_cast<const _Tb*>( (const _Ts*)(_First) + _Vsize.leading_size );
				return fast_operation::afunc_1st(_First_bk, _First_bk+_Vsize.block_size, _Result, _Afunc, _Func);
			}

			return norm_operation::afunc_1st((const _Ts*)_First, (const _Ts*)_Last, _Result, _Afunc, _Func);
		}

		template<typename _Afn, typename _Fn>
		static _Ts& afunc_1st(_InIt _First, _InIt _Last, _Ts& _Result, _Afn _Afunc, _Fn _Func,
			size_t _Inc = 1) {
			if (_Inc == 1) {
				return afunc_1st(_First, _Last, _Result, _Afunc, _Func, size_type((const _Ts*)_First, (const _Ts*)_Last));
			}

			return norm_operation::afunc_1st((const _Ts*)_First, (const _Ts*)_Last, _Result, _Afunc, _Func, 
				_Inc);
		}

		template<typename _Afn, typename _Fn>
		static _Ts& afunc_2nd(_InIt _First1, _InIt _Last1, _InIt _First2, _Ts& _Result, _Afn _Afunc, _Fn _Func,
			const size_type& _Vsize1, const size_type& _Vsize2) {
			if (_Vsize1 == _Vsize2) {
				if ( _Vsize1.can_fastest() ) {
					return fastest_operation::afunc_2nd(static_cast<const _Tb*>(_First1), static_cast<const _Tb*>(_Last1),
						static_cast<const _Tb*>(_First2), _Result, _Afunc, _Func);
				} else if( !_Vsize1.empty() ) {
					auto       _First1_bk = reinterpret_cast<const _Tb*>((const _Ts*)(_First1) + _Vsize1.leading_size);
					const auto _Last1_bk  = _First1_bk + _Vsize1.block_size;
					auto       _First2_bk = reinterpret_cast<const _Tb*>((const _Ts*)(_First2) + _Vsize1.leading_size);
					return fast_operation::afunc_2nd(_First1_bk, _Last1_bk, _First2_bk, _Result, _Afunc, _Func,
						_Vsize1.leading_size, _Vsize1.tail_size);
				}
			}

			return norm_operation::afunc_2nd(static_cast<const _Ts*>(_First1), static_cast<const _Ts*>(_Last1),
				static_cast<const _Ts*>(_First2), _Result, _Afunc, _Func );
		}

		template<typename _Afn, typename _Fn>
		static _Ts& afunc_2nd(_InIt _First1, _InIt _Last1, _InIt _First2, _Ts& _Result, _Afn _Afunc, _Fn _Func,
			size_t _Inc1 = 1, size_t _Inc2 = 1) {
			if (_Inc1 == 1 && _Inc2 == 1) {
				const auto _Diff = std::distance((const _Ts*)(_First1), (const _Ts*)(_Last1));

				return afunc_2nd( _First1, _Last1, _First2, _Result, _Afunc, _Func,
					size_type((const _Ts*)_First1, (const _Ts*)_Last1), 
					size_type((const _Ts*)_First2, (const _Ts*)_First2 + _Diff) );
			}

			return norm_operation::afunc_2nd(static_cast<const _Ts*>(_First1), static_cast<const _Ts*>(_Last1),
				static_cast<const _Ts*>(_First2), _Result, _Afunc, _Func, _Inc1, _Inc2);
		}
	
		template<typename _Fn>
		static _Ts& norm(_InIt _First, _InIt _Last, _Ts L, _Ts& _Result,
			size_t _Inc1 = 1, size_t _Inc2 = 1) {
			afunc_1st(_First, _Last, _Result, 
				[](auto& _Res, auto&& _X) { _Res += _X; },
				[](auto&& _X) { return pow(abs(_X), L); },
				_Inc1, _Inc2
			);

			_Result = pow(_Result, static_cast<_Ts>(1) / L);
			return _Result;
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

	#define _Generate_block_iter_1st(LEFT, RESULT)        \
	auto       _First = (##LEFT##).begin<_Tb>();       \
	const auto _Last  = _First + (##LEFT##).block_size(); \
	auto       _Dest  = (##RESULT##).begin<_Tb>()

	#define _Generate_block_iter_2nd(LEFT, RIGHT, RESULT)   \
	auto       _First1 = (##LEFT##).begin<_Tb>();        \
	const auto _Last1  = _First1 + (##LEFT##).block_size(); \
	auto       _First2 = (##RIGHT##).begin<_Tb>();       \
	auto       _Dest   = (##RESULT##).begin<_Tb>()

	#define _Generate_block_iter_nodest_1st(LEFT)   \
	auto       _First = (##LEFT##).begin<_Tb>(); \
	const auto _Last  = _First + (##LEFT##).block_size()

	#define _Generate_iter_nodest_2nd(LEFT, RIGHT)          \
	auto       _First1 = (##LEFT##).begin<_Tb>();        \
	const auto _Last1  = _First1 + (##LEFT##).block_size(); \
	auto       _First2 = (##RIGHT##).begin<_Tb>()



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

		static _Ts* allocate(size_t _Count/*scalar*/) {
			return reinterpret_cast<_Ts*>(data().seek_n(calc_block_count(_Count)));
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

		_Tb& top() {
			return ((*this)[_Myseek]);
		}

		_Tb* seek_n(size_t/*block*/ _Count) {
			const auto _Newseek = _Myseek + _Count;
			assert(_Newseek <= _Mybase::size());

			auto _Ptr = &this->top();
			_Myseek = _Newseek;
			return _Ptr;
		}

		size_t _Myseek;
	};


	/*- - - - - - - - - - - - - - - - - - - subvector<> - - - - - - - - - - - - - - - - - - - - - -*/
	// forward declare
	template<typename _Ts, typename _Tb>
		struct const_subvector;
	template<typename _Ts, typename _Tb>
		struct subvector;

	#define SUBVECTOR_OPERATOR_1ST(LHS, FUNC)           \
	size_t _Size   = (##LHS##).size();                  \
	auto   _Memory = clmagic::stack_allocator<_Ts, _Tb>::allocate(_Size); \
	auto   _Result = subvector<_Ts, _Tb>(_Memory, _Memory + _Size, 1, true); \
	auto_vector_operation<_Ts, _Tb>::func_1st((##LHS##).begin(), (##LHS##).end(), _Result.begin(), (##FUNC##), \
	(##LHS##).stride(), _Result.stride()); \
	return std::move(_Result)

	#define SUBVECTOR_OPERATOR_2ND(LHS, RHS, FUNC)      \
	size_t _Size   = (##LHS##).size();                  \
	auto   _Memory = clmagic::stack_allocator<_Ts, _Tb>::allocate(_Size); \
	auto   _Result = subvector<_Ts, _Tb>(_Memory, _Memory + _Size, 1, true); \
	auto_vector_operation<_Ts, _Tb>::func_2nd((##LHS##).begin(), (##LHS##).end(), (##RHS##).begin(), _Result.begin(), (##FUNC##), \
	(##LHS##).stride(), (##RHS##).stride(), _Result.stride()); \
	return std::move(_Result)

	template<typename _Ts, typename _Tb>
	struct const_subvector {// const _Ts
		using iterator_category = std::random_access_iterator_tag;
		using difference_Tspe   = ptrdiff_t;

		using block_type       = const _Tb;
		using block_pointer    = block_type*;
		using scalar_type      = const _Ts;
		using scalar_pointer   = scalar_type*;
		using scalar_reference = scalar_type&;

		using size_type        = ::clmagic::vector_size<_Ts, _Tb>;
		
		using block_traits     = ::clmagic::block_traits<_Tb>;
		using allocator_traits = ::clmagic::stack_allocator<_Ts, _Tb>;

		scalar_pointer begin() const {
			return _Fptr; }
		scalar_pointer end() const {
			return _Lptr; }
		size_t size() const {
			return (_Lptr - _Fptr) / _Nx; }
		size_t stride() const {
			return _Nx; }
		size_type vsize() const {
			return size_type(_Fptr, _Lptr, _Nx); }
		scalar_pointer ptr(size_t _Pos = 0) const {
			return _Fptr + _Pos * _Nx; }
		scalar_reference operator[](size_t _Pos) const {
			return (*ptr(_Pos)); }
		void reset(scalar_pointer _First, scalar_pointer _Last, size_t _Stride = 1, bool _Memory = false) {
			_Fptr  = _First;
			_Lptr  = _Last;
			_Nx    = _Stride;
			_Mem   = _Memory;
		}
		bool empty() const {
			return (_Lptr == _Fptr);
		}
		bool is_continue() const {
			return _Nx == 1;
		}

		const_subvector(const const_subvector&) = delete;
		const_subvector(const_subvector&& _Right) noexcept
			: _Fptr(_Right._Fptr), _Lptr(_Right._Lptr), _Nx(_Right._Nx), _Mem(_Right._Mem) {
			_Right._Mem = false;
		}
		const_subvector(scalar_pointer _First, scalar_pointer _Last)
			: _Fptr(_First), _Lptr(_Last), _Nx(1), _Mem(false) {}
		const_subvector(scalar_pointer _First, scalar_pointer _Last, size_t _Stride)// &_First[1] = &_First[0] + _Stride
			: _Fptr(_First), _Lptr(_Last), _Nx(_Stride), _Mem(false) {}
		~const_subvector() {
			if (_Mem) {
				allocator_traits::deallocate(this->size());
			}
		}

		const_subvector& operator=(const const_subvector&) = delete;
		const_subvector& operator=(const_subvector&& _Right) {
			this->_Fptr  = _Right._Fptr;
			this->_Lptr  = _Right._Lptr;
			this->_Nx    = _Right._Nx;
			this->_Mem   = _Right._Mem;
			_Right._Mem  = false;
			return (*this);
		}

		const_subvector operator()(size_t _First1, size_t _Last1) const {
			return const_subvector(this->ptr(_First1), this->ptr(_Last1), this->stride());
		}
		subvector<_Ts, _Tb> operator-() const {
			assert(!this->empty());
			SUBVECTOR_OPERATOR_1ST(*this, std::negate<>());
		}
		subvector<_Ts, _Tb> operator+(const const_subvector& _Right) const {
			assert(!this->empty());
			assert(this->size() == _Right.size());
			SUBVECTOR_OPERATOR_2ND(*this, _Right, std::plus<>() );
		}
		subvector<_Ts, _Tb> operator-(const const_subvector& _Right) const {
			assert(!this->empty());
			assert(this->size() == _Right.size());
			SUBVECTOR_OPERATOR_2ND(*this, _Right, std::minus<>());
		}
		subvector<_Ts, _Tb> operator*(const const_subvector& _Right) const {
			assert(!this->empty());
			assert(this->size() == _Right.size());
			SUBVECTOR_OPERATOR_2ND(*this, _Right, std::multiplies<>());
		}
		subvector<_Ts, _Tb> operator/(const const_subvector& _Right) const {
			assert(!this->empty());
			assert(this->size() == _Right.size());
			SUBVECTOR_OPERATOR_2ND(*this, _Right, std::divides<>());
		}
		
		subvector<_Ts, _Tb> operator+(const _Ts& _Scalar) const {
			assert(!this->empty());
			const auto _Rhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs + _Rhs; };
			SUBVECTOR_OPERATOR_1ST(*this, _Func);
		}
		subvector<_Ts, _Tb> operator-(const _Ts& _Scalar) const {
			assert(!this->empty());
			const auto _Rhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs - _Rhs; };
			SUBVECTOR_OPERATOR_1ST(*this, _Func);
		}
		subvector<_Ts, _Tb> operator*(const _Ts& _Scalar) const {
			assert(!this->empty());
			const auto _Rhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs * _Rhs; };
			SUBVECTOR_OPERATOR_1ST(*this, _Func);
		}
		subvector<_Ts, _Tb> operator/(const _Ts& _Scalar) const {
			assert(!this->empty());
			const auto _Rhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs / _Rhs; };
			SUBVECTOR_OPERATOR_1ST(*this, _Func);
		}
		friend subvector<_Ts, _Tb> operator+(const _Ts& _Scalar, const const_subvector& _Right) {
			assert(!_Right.empty());
			const auto _Lhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Lhs](auto&& _Rhs) { return _Lhs + _Rhs; };
			SUBVECTOR_OPERATOR_1ST(*this, _Func);
		}
		friend subvector<_Ts, _Tb> operator-(const _Ts& _Scalar, const const_subvector& _Right) {
			assert(!_Right.empty());
			const auto _Lhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Lhs](auto&& _Rhs) { return _Lhs - _Rhs; };
			SUBVECTOR_OPERATOR_1ST(*this, _Func);
		}
		friend subvector<_Ts, _Tb> operator*(const _Ts& _Scalar, const const_subvector& _Right) {
			assert(!_Right.empty());
			const auto _Lhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Lhs](auto&& _Rhs) { return _Lhs * _Rhs; };
			SUBVECTOR_OPERATOR_1ST(*this, _Func);
		}
		friend subvector<_Ts, _Tb> operator/(const _Ts& _Scalar, const const_subvector& _Right) {
			assert(!_Right.empty());
			const auto _Lhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Lhs](auto&& _Rhs) { return _Lhs / _Rhs; };
			SUBVECTOR_OPERATOR_1ST(*this, _Func);
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
			: _Fptr(_First), _Lptr(_Last), _Nx(_Stride), _Mem(_Memory) { }

	private:
		scalar_pointer _Fptr;
		scalar_pointer _Lptr;
		size_t         _Nx;
		bool           _Mem;
	};
	
	// vector OP vector
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> mod(const const_subvector<_Ts, _Tb>& _Left, const const_subvector<_Ts, _Tb>& _Right) {
		assert(!_Left.empty());
		assert(_Left.size() == _Right.size());
		SUBVECTOR_OPERATOR_2ND(_Left, _Right, std::modulus<_Ts>());
	}
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> pow(const const_subvector<_Ts, _Tb>& _Left, const const_subvector<_Ts, _Tb>& _Right) {
		assert(!_Left.empty());
		assert(_Left.size() == _Right.size());
		const auto _Func = [](auto&& A, auto&& B) { return pow(A, B); };
		SUBVECTOR_OPERATOR_2ND(_Left, _Right, _Func);
	}
	
	template<typename _Ts, typename _Tb> inline
	_Ts dot(const const_subvector<_Ts, _Tb>& _Left, const const_subvector<_Ts, _Tb>& _Right) {
		_Ts _Result = static_cast<_Ts>(0);
		return auto_vector_operation<_Ts, _Tb>::afunc_2nd( _Left.begin(), _Left.end(), _Right.begin(), _Result,
			[](auto& _Res, auto&& _X) { _Res += _X; }, std::multiplies<>(), 
			_Left.stride(), _Right.stride() );
	}

	// vector OP scalar
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> mod(const const_subvector<_Ts, _Tb>& _Left, const _Ts& _Scalar) {
		assert(!_Left.empty());
		const auto _Rhs  = clmagic::block_traits<_Ts, _Tb>::set1(_Scalar);
		const auto _Func = [&_Rhs](auto&& _Lhs) { return mod(_Lhs, _Rhs); };
		SUBVECTOR_OPERATOR_1ST(_Left, _Func);
	}
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> pow(const const_subvector<_Ts, _Tb>& _Left, const _Ts& _Scalar) {
		assert(!_Left.empty());
		const auto _Rhs  = clmagic::block_traits<_Ts, _Tb>::set1(_Scalar);
		const auto _Func = [&_Rhs](auto&& _Lhs) { return pow(_Lhs, _Rhs); };
		SUBVECTOR_OPERATOR_1ST(_Left, _Func);
	}
	#undef SUBVECTOR_OPERATOR_1ST
	#undef SUBVECTOR_OPERATOR_2ND


	#define SUBVECTOR_ASSIGN_OPERATOR_1ST(LHS, DST, FUNC) \
	auto_vector_operation<_Ts, _Tb>::func_1st((##LHS##).begin(), (##LHS##).end(), \
	(##DST##).begin(), (##FUNC##), \
	(##LHS##).stride(), (##DST##).stride()); \
	return (*this)

	#define SUBVECTOR_ASSIGN_OPERATOR_2ND(LHS, RHS, DST, FUNC) \
	auto_vector_operation<_Ts, _Tb>::func_2nd((##LHS##).begin(), (##LHS##).end(), (##RHS##).begin(), \
	(##DST##).begin(), (##FUNC##), \
	(##LHS##).stride(), (##RHS##).stride(), (##DST##).stride()); \
	return (*this)

	template<typename _Ts, typename _Tb>
	struct subvector : public const_subvector<_Ts, _Tb> {// default const _Ts
		using _Mybase = const_subvector<_Ts, _Tb>;
		friend _Mybase;

		using iterator_category = std::random_access_iterator_tag;
		using difference_Tspe   = ptrdiff_t;

		using block_type       = _Tb;
		using block_pointer    = block_type*;
		using scalar_type      = _Ts;
		using scalar_pointer   = scalar_type*;
		using scalar_reference = scalar_type&;

		using size_type = typename _Mybase::size_type;

		using block_traits = ::clmagic::block_traits<_Tb>;

		subvector(scalar_pointer _First_arg, scalar_pointer _Last_arg)
			: _Mybase(_First_arg, _Last_arg) {}
		subvector(scalar_pointer _First, scalar_pointer _Last, size_t _Stride)
			: _Mybase(_First, _Last, _Stride) {}
		subvector(subvector<_Ts, _Tb>& _Right) {
			size_t _Size   = _Right.size();
			auto   _Memory = _Mybase::allocator_traits::allocate(_Size);
			_Mybase::reset(_Memory, _Memory+_Size, 1, true);
		}
		subvector(subvector<_Ts, _Tb>&& _Right) noexcept// owner move to this
			: _Mybase(std::move(_Right)) {}

		// full-clone
		subvector& operator=(const const_subvector<_Ts, _Tb>& _Right) {
			auto       _First = _Right.begin();
			const auto _Last  = _Right.end();
			auto       _Dest  = this->begin();
			const auto _Inc1  = _Right.stride();
			const auto _Inc2  = _Mybase::stride();
			for ( ; _First != _Last; _First += _Inc1, _Dest += _Inc2) {
				*_Dest = *_First;
			}
			return *this;
		}
		subvector& operator=(const subvector<_Ts, _Tb>& _Right) { 
			return ( (*this) = static_cast<const const_subvector<_Ts, _Tb>&>(_Right) );
		}
		subvector& operator=(subvector<_Ts, _Tb>&& _Right) noexcept {
			return ((*this) = static_cast<subvector<_Ts, _Tb>&>(_Right));
		}

		subvector operator()(size_t _First, size_t _Last) const {
			return subvector(this->ptr(_First), this->ptr(_Last), this->stride());
		}
		subvector& operator+=(const const_subvector<_Ts, _Tb>& _Right) {
			SUBVECTOR_ASSIGN_OPERATOR_2ND(*this, _Right, *this, std::plus<>());
		}
		subvector& operator-=(const const_subvector<_Ts, _Tb>& _Right) {
			SUBVECTOR_ASSIGN_OPERATOR_2ND(*this, _Right, *this, std::minus<>());
		}
		subvector& operator*=(const const_subvector<_Ts, _Tb>& _Right) {
			SUBVECTOR_ASSIGN_OPERATOR_2ND(*this, _Right, *this, std::multiplies<>());
		}
		subvector& operator/=(const const_subvector<_Ts, _Tb>& _Right) {
			SUBVECTOR_ASSIGN_OPERATOR_2ND(*this, _Right, *this, std::divides<>());
		}
		subvector& operator+=(const scalar_type& _Right) {
			const auto _Rhs  = block_traits::set1(_Right);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs + _Rhs; };
			SUBVECTOR_ASSIGN_OPERATOR_1ST(*this, *this, _Func);
		}
		subvector& operator-=(const scalar_type& _Right) {
			const auto _Rhs  = block_traits::set1(_Right);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs - _Rhs; };
			SUBVECTOR_ASSIGN_OPERATOR_1ST(*this, *this, _Func);
		}
		subvector& operator*=(const scalar_type& _Right) {
			auto       _First = this->begin();
			const auto _Last  = this->end();
			auto       _Dest  = this->begin();
			if (_Mybase::is_continue()) {
				const auto _Vsize = size_type(_First, _Last);
				const auto _Rhs   = block_traits::set1(_Right);
				const auto _Func  = [&_Rhs](auto&& _Lhs) { return _Lhs * _Rhs; };
				if ( _Vsize.can_fastest() ) {
					fastest_vector_operation<_Ts, _Tb>::func_1st(
						reinterpret_cast<const _Tb*>(_First), reinterpret_cast<const _Tb*>(_Last),
						reinterpret_cast<_Tb*>(_Dest),
						_Func);
					return *this;
				} else if( !_Vsize.empty() ) {
					auto       _First_bk = reinterpret_cast<const _Tb*>( (const _Ts*)(_First) + _Vsize.leading_size );
					const auto _Last_bk  = _First_bk + _Vsize.block_size;
					auto       _Dest_bk  = reinterpret_cast<_Tb*>( (_Ts*)(_First) + _Vsize.leading_size );
					fast_vector_operation<_Ts, _Tb>::func_1st(_First_bk, _Last_bk, _Dest_bk, _Func,
						_Vsize.leading_size, _Vsize.tail_size);
					return *this;
				}
			}

			norm_vector_operation<_Ts, _Tb>::func_1st(_First, _Last, _Dest, [&_Right](auto&& _Lhs) { return _Lhs * _Right; }, 
				_Mybase::stride(), _Mybase::stride());
			return *this;
		}
		subvector& operator/=(const scalar_type& _Right) {
			const auto _Rhs  = block_traits::set1(_Right);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs / _Rhs; };
			SUBVECTOR_ASSIGN_OPERATOR_1ST(*this, *this, _Func);
		}

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
		
		void reset(scalar_pointer _First, scalar_pointer _Last, size_t _Stride = 1) {
			_Mybase::reset(_First, _Last, _Stride);
		}
		void fill(const scalar_type& _Val) {
			auto       _Dest = this->begin();
			const auto _Last = this->end();
			const auto _Inc  = _Mybase::stride();
			for (; _Dest != _Last; _Dest += _Inc) {
				*_Dest = _Val;
			}
		}
		template<typename _Iter> 
		void assign(_Iter _First, _Iter _Last) {
			auto       _Dest = this->begin();
			const auto _Inc  = _Mybase::stride();
			for ( ; _First != _Last; ++_First, _Dest+=_Inc) {
				*_Dest = *_First;
			}
		}
	
		struct refernece {
			scalar_pointer _Myfirst;
			scalar_pointer _Mylast;
			size_t         _Mystride;
			vector_size<_Ts, _Tb> _Mysize;
		};
	
		refernece& subvector_ref() {
			return reinterpret_cast<refernece&>(*this);
		}

	protected:
		subvector(scalar_pointer _First, scalar_pointer _Last, size_t _Stride, bool _Memory)
			: _Mybase(_First, _Last, _Stride, _Memory) {}
	};
	#undef SUBVECTOR_ASSIGN_OPERATOR_1ST
	#undef SUBVECTOR_ASSIGN_OPERATOR_2ND

	template<typename _Ts, typename _Tb> inline
	_Ts norm(const_subvector<_Ts, _Tb> _X, const _Ts& _Level) {
		_Ts _Result = static_cast<_Ts>(0);
		return auto_vector_operation<_Ts, _Tb>::norm(_X.begin(), _X.end(), _Level, _Result, _X.stride());
	}
	template<typename _Ts, typename _Tb> inline
	_Ts normL2_square(const_subvector<_Ts, _Tb> _X) {
		return dot(_X, _X);
	}
	template<typename _Ts, typename _Tb> inline
	subvector<_Ts, _Tb> normalize(const const_subvector<_Ts, _Tb>& _Source) {
		const auto _NormL2sq = normL2_square(_Source);
		return ( !approach_equal(_NormL2sq, static_cast<_Ts>(1), std::numeric_limits<_Ts>::epsilon()) ) ? 
			std::move(_Source / sqrt(_NormL2sq)): 
			_Source;
	}


	/*- - - - - - - - - - - - - - - - - - - vector<> - - - - - - - - - - - - - - - - - - - - -*/
	template<typename _Ts, size_t _Size, typename _Tb>
	class __declspec(align(std::alignment_of_v<_Tb>)) _Data_of_vector {
		constexpr static size_t _Real_size = ceil(_Size * sizeof(_Ts), std::alignment_of_v<_Tb>) / sizeof(_Ts);

		_Ts _Mydata[_Real_size];

	public:
		static constexpr size_t size() {
			return _Size; 
		}
		static constexpr size_t tail_size() {
			return _Real_size - _Size; 
		}
		static constexpr size_t block_size() {
			return (_Real_size / block_traits<_Tb>::size());
		}

		template<typename _Ty = _Ts>
		_Ty* ptr() {
			return reinterpret_cast<_Ty*>(_Mydata);
		}
		template<typename _Ty = _Ts>
		_Ty* ptr(size_t _Pos) {
			return reinterpret_cast<_Ty*>(_Mydata + _Pos);
		}
		template<typename _Ty = _Ts>
		_Ty& ref() {
			return ( * ptr<_Ty>() );
		}
		template<typename _Ty = _Ts>
		_Ty& ref(size_t _Pos) {
			return ( * ptr<_Ty>(_Pos) );
		}
		template<typename _Ty = _Ts>
		_Ty& at(size_t _Pos) {
			return (ref<_Ty>(_Pos));
		}

		template<typename _Ty = _Ts>
		const _Ty* ptr() const {
			return reinterpret_cast<const _Ty*>(_Mydata);
		}
		template<typename _Ty = _Ts>
		const _Ty* ptr(size_t _Pos) const {
			return reinterpret_cast<const _Ty*>(_Mydata + _Pos);
		}
		template<typename _Ty = _Ts>
		const _Ty& ref() const {
			return (*ptr<_Ty>());
		}
		template<typename _Ty = _Ts>
		const _Ty& ref(size_t _Pos) const {
			return (*ptr<_Ty>(_Pos));
		}
		template<typename _Ty = _Ts>
		const _Ty& at(size_t _Pos) const {
			return (ref<_Ty>(_Pos));
		}

		template<typename _Ty = _Ts>
		_Ty* begin() {
			return ptr<_Ty>();
		}
		template<typename _Ty = _Ts>
		_Ty* end() {
			return ptr<_Ty>( size() );
		}
		template<typename _Ty = _Ts>
		const _Ty* begin() const {
			return ptr<_Ty>();
		}
		template<typename _Ty = _Ts>
		const _Ty* end() const {
			return ptr<_Ty>(size());
		}

		_Ts& operator[](size_t _Pos) {
			return _Mydata[_Pos]; }
		const _Ts& operator[](size_t _Pos) const {
			return _Mydata[_Pos]; }

		void _Correct_tail_elements() {
			if (_Real_size > _Size) {
				::std::fill(this->ptr(_Size), this->ptr(_Real_size), static_cast<_Ts>(0));
			}
		}
		template<typename _Iter>
		void assign(_Iter _First, _Iter _Last) {
			assert( ::std::distance(_First, _Last) <= ::std::_Iter_diff_t<_Iter>(this->size()) );
			auto _Dest = ::std::copy(_First, _Last, this->begin());
			::std::fill(_Dest, this->ptr(_Real_size), static_cast<_Ts>(0));
		}
		void assign(const _Ts& _Val) {
			::std::fill(this->begin(), this->end(), _Val); 
			_Correct_tail_elements();
		}
	};

	template<typename _Ts, size_t _Size, typename _Tb = _Ts>
	class vector : public _Data_of_vector<_Ts, _Size, _Tb> {
		using _Mybase = _Data_of_vector<_Ts, _Size, _Tb>;

	public:
		//static_assert(std::is_arithmetic_v<_Ts>, "strict math vector");

		using block_type  = _Tb;
		using scalar_type = _Ts;
		using operation   = ::clmagic::fastest_vector_operation<_Ts, _Tb>;// impotant!!!

		using block_traits    = ::clmagic::block_traits<_Tb>;
		//using scalar_traits = clmagic::block_traits<_Ts>;

		using subvector       = ::clmagic::subvector<_Ts, _Tb>;
		using const_subvector = ::clmagic::const_subvector<_Ts, _Tb>;

		constexpr vector() = default;

		explicit vector(int _Val) { _Mybase::assign(static_cast<_Ts>(_Val)); }
		explicit vector(float _Val) { _Mybase::assign(static_cast<_Ts>(_Val)); }
		explicit vector(double _Val) { _Mybase::assign(static_cast<_Ts>(_Val)); }

		template<typename _Iter> 
		vector(_Iter _First, _Iter _Last) { 
			_Mybase::assign(_First, _Last); 
		}
		vector(std::initializer_list<_Ts> _Ilist) { 
			_Mybase::assign(_Ilist.begin(), _Ilist.end()); 
		}

		vector& operator=(std::initializer_list<_Ts> _Ilist) {
			this->assign(_Ilist.begin(), _Ilist.end());
			return *this;
		}

		subvector operator()() {
			return subvector(_Mybase::begin(), _Mybase::end());
		}
		subvector operator()(size_t _First, size_t _Last) {
			return subvector(_Mybase::ptr(_First), _Mybase::ptr(_Last));
		}
		const_subvector operator()() const {
			return const_subvector(_Mybase::begin(), _Mybase::end());
		}
		const_subvector operator()(size_t _First, size_t _Last) const {
			return const_subvector(_Mybase::ptr(_First), _Mybase::ptr(_Last));
		}

		vector neg() const {
			_Return_generate_object( vector, _Result,
				operation::func_1st(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Result.begin<_Tb>(),
					std::negate<_Tb>()) );
		}
		vector add(vector _Right) const {
			_Return_generate_object( vector, _Result,
				operation::func_2nd(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Right.begin<_Tb>(), _Result.begin<_Tb>(),
					std::plus<_Tb>()) );
		}
		vector sub(vector _Right) const {
			_Return_generate_object( vector, _Result,
				operation::func_2nd(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Right.begin<_Tb>(), _Result.begin<_Tb>(),
					std::minus<_Tb>()) );
		}
		vector mul(vector _Right) const {
			_Return_generate_object( vector, _Result,
				operation::func_2nd(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Right.begin<_Tb>(), _Result.begin<_Tb>(),
					std::multiplies<_Tb>()) );
		}
		vector div(vector _Right) const {
			_Return_generate_object( vector, _Result,
				operation::func_2nd(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Right.begin<_Tb>(), _Result.begin<_Tb>(),
					std::divides<_Tb>()) );
		}
		vector mod(vector _Right) const {
			_Return_generate_object( vector, _Result,
				operation::func_2nd(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Right.begin<_Tb>(), _Result.begin<_Tb>(),
					std::modulus<_Tb>()) );
		}
		vector& assign_add(vector _Right) {
			operation::func_2nd(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Right.begin<_Tb>(), 
				this->begin<_Tb>(), std::plus<_Tb>());
			return *this;
		}
		vector& assign_sub(vector _Right) {
			operation::func_2nd(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Right.begin<_Tb>(), 
				this->begin<_Tb>(), std::minus<_Tb>());
			return *this;
		}
		vector& assign_mul(vector _Right) {
			operation::func_2nd(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Right.begin<_Tb>(), 
				this->begin<_Tb>(), std::multiplies<_Tb>());
			return *this;
		}
		vector& assign_div(vector _Right) {
			operation::func_2nd(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Right.begin<_Tb>(), 
				this->begin<_Tb>(), std::divides<_Tb>());
			return *this;
		}
		vector& assign_mod(vector _Right) {
			operation::func_2nd(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Right.begin<_Tb>(), 
				this->begin<_Tb>(), std::modulus<_Tb>());
			return *this;
		}
		
		vector add(_Ts _Scalar) const {
			const _Tb  _Arg1 = block_traits::set1(_Scalar);
			const auto _Func = [&_Arg1](auto&& _Arg0) { return _Arg0 + _Arg1; };
			_Return_generate_object(vector, _Result,
				operation::func_1st(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Result.begin<_Tb>(), _Func));
		}
		vector sub(_Ts _Scalar) const {
			const _Tb  _Arg1 = block_traits::set1(_Scalar);
			const auto _Func = [&_Arg1](auto&& _Arg0) { return _Arg0 - _Arg1; };
			_Return_generate_object(vector, _Result,
				operation::func_1st(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Result.begin<_Tb>(), _Func));
		}
		vector mul(_Ts _Scalar) const {
			const _Tb  _Arg1 = block_traits::set1(_Scalar);
			const auto _Func = [&_Arg1](auto&& _Arg0) { return _Arg0 * _Arg1; };
			_Return_generate_object(vector, _Result,
				operation::func_1st(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Result.begin<_Tb>(), _Func));
		}
		vector div(_Ts _Scalar) const {
			const _Tb  _Arg1 = block_traits::set1(_Scalar);
			const auto _Func = [&_Arg1](auto&& _Arg0) { return _Arg0 / _Arg1; };
			_Return_generate_object(vector, _Result,
				operation::func_1st(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Result.begin<_Tb>(), _Func));
		}
		vector mod(_Ts _Scalar) const {
			const _Tb  _Arg1 = block_traits::set1(_Scalar);
			const auto _Func = [&_Arg1](auto&& _Arg0) { return _Arg0 % _Arg1; };
			_Return_generate_object(vector, _Result,
				operation::func_1st(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Result.begin<_Tb>(), _Func));
		}
		vector added(_Ts _Scalar) const {
			const _Tb  _Arg0 = block_traits::set1(_Scalar);
			const auto _Func = [&_Arg0](auto&& _Arg1) { return _Arg0 + _Arg1; };
			_Return_generate_object(vector, _Result,
				operation::func_1st(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Result.begin<_Tb>(), _Func));
		}
		vector subed(_Ts _Scalar) const {
			const _Tb  _Arg0 = block_traits::set1(_Scalar);
			const auto _Func = [&_Arg0](auto&& _Arg1) { return _Arg0 - _Arg1; };
			_Return_generate_object(vector, _Result,
				operation::func_1st(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Result.begin<_Tb>(), _Func));
		}
		vector muled(_Ts _Scalar) const {
			const _Tb  _Arg0 = block_traits::set1(_Scalar);
			const auto _Func = [&_Arg0](auto&& _Arg1) { return _Arg0 * _Arg1; };
			_Return_generate_object(vector, _Result,
				operation::func_1st(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Result.begin<_Tb>(), _Func));
		}
		vector dived(_Ts _Scalar) const {
			const _Tb  _Arg0 = block_traits::set1(_Scalar);
			const auto _Func = [&_Arg0](auto&& _Arg1) { return _Arg0 / _Arg1; };
			_Return_generate_object(vector, _Result,
				operation::func_1st(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Result.begin<_Tb>(), _Func));
		}
		vector moded(_Ts _Scalar) const {
			const _Tb  _Arg0 = block_traits::set1(_Scalar);
			const auto _Func = [&_Arg0](auto&& _Arg1) { return _Arg0 % _Arg1; };
			_Return_generate_object(vector, _Result,
				operation::func_1st(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Result.begin<_Tb>(), _Func));
		}
		vector& assign_add(_Ts _Scalar) {
			const _Tb  _Arg1 = block_traits::set1(_Scalar);
			const auto _Func = [&_Arg1](auto&& _Arg0) { return _Arg0 + _Arg1; };
			operation::func_1st(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), this->begin<_Tb>(), _Func);
			return *this;
		}
		vector& assign_sub(_Ts _Scalar) {
			const _Tb  _Arg1 = block_traits::set1(_Scalar);
			const auto _Func = [&_Arg1](auto&& _Arg0) { return _Arg0 - _Arg1; };
			operation::func_1st(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), this->begin<_Tb>(), _Func);
			return *this;
		}
		vector& assign_mul(_Ts _Scalar) {
			const _Tb  _Arg1 = block_traits::set1(_Scalar);
			const auto _Func = [&_Arg1](auto&& _Arg0) { return _Arg0 * _Arg1; };
			operation::func_1st(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), this->begin<_Tb>(), _Func);
			return *this;
		}
		vector& assign_div(_Ts _Scalar) {
			const _Tb  _Arg1 = block_traits::set1(_Scalar);
			const auto _Func = [&_Arg1](auto&& _Arg0) { return _Arg0 / _Arg1; };
			operation::func_1st(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), this->begin<_Tb>(), _Func);
			return *this;
		}
		vector& assign_mod(_Ts _Scalar) {
			const _Tb  _Arg1 = block_traits::set1(_Scalar);
			const auto _Func = [&_Arg1](auto&& _Arg0) { return _Arg0 % _Arg1; };
			operation::func_1st(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), this->begin<_Tb>(), _Func);
			return *this;
		}
		
		_Ts sum() const {// X[0] + X[1] + X[2] + ... + X[N]
			const auto _Afunc = [](auto& _Res, auto&& _X) { _Res += _X; };
			_Return_generate_object(_Ts, _Result, _Result = static_cast<_Ts>(0);
				operation::afunc(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Result, _Afunc));
		}
		_Ts product() const {// X[0] * X[1] * X[2] * ... * X[N]
			const auto _Afunc = [](auto& _Res, auto&& _X) { _Res *= _X; };
			_Return_generate_object(_Ts, _Result, _Result = static_cast<_Ts>(0);
				operation::afunc(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Result, _Afunc));
		}
		_Ts norm(_Ts L) const {
			_Return_generate_object(_Ts, _Result, _Result = static_cast<_Ts>(0);
				operation::norm(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), L, _Result));
		}
		_Ts dot(const vector& _Right) const {// X[0]*Y[0] + X[1]*Y[1] + X[2]*Y[2] + ... * X[N]*Y[N]
			const auto _Afunc = [](auto& _Res, auto&& _X) { _Res += _X; };
			_Return_generate_object(_Ts, _Result, _Result = static_cast<_Ts>(0);
				operation::afunc_2nd(this->begin<_Tb>(), this->begin<_Tb>()+this->block_size(), _Right.begin<_Tb>(), _Result,
					_Afunc, std::multiplies<>()) );
		}
		
		vector& normalize() {
			const _Ts _Length_squared = (*this).dot(*this);
			if ( !approach_equal(_Length_squared, static_cast<_Ts>(1), std::numeric_limits<_Ts>::epsilon()) ) {
				*this /= sqrt(_Length_squared);
			}
			return (*this);
		}

		std::string to_string() const {
			return ::clmagic::to_string(this->begin(), this->end());
		}

		vector  operator-() const {
			return this->neg();
		}
		vector  operator+ (const vector& _Right) const {
			return this->add(_Right);
		}
		vector  operator- (const vector& _Right) const {
			return this->sub(_Right);
		}
		vector  operator* (const vector& _Right) const {
			return this->mul(_Right);
		}
		vector  operator/ (const vector& _Right) const {
			return this->div(_Right);
		}
		vector  operator% (const vector& _Right) const {
			return this->mod(_Right);
		}
		vector& operator+=(const vector& _Right) {
			return this->assign_add(_Right);
		}
		vector& operator-=(const vector& _Right) {
			return this->assign_sub(_Right);
		}
		vector& operator*=(const vector& _Right) {
			return this->assign_mul(_Right);
		}
		vector& operator/=(const vector& _Right) {
			return this->assign_div(_Right);
		}
		vector& operator%=(const vector& _Right) {
			return this->assign_mod(_Right);
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

		vector  operator+ (int _Scalar) const {
			return this->add(static_cast<_Ts>(_Scalar));
		}
		vector  operator- (int _Scalar) const {
			return this->sub(static_cast<_Ts>(_Scalar));
		}
		vector  operator* (int _Scalar) const {
			return this->mul(static_cast<_Ts>(_Scalar));
		}
		vector  operator/ (int _Scalar) const {
			return this->div(static_cast<_Ts>(_Scalar));
		}
		vector  operator% (int _Scalar) const {
			return this->mod(static_cast<_Ts>(_Scalar));
		}
		vector& operator+=(int _Scalar) {
			return this->assign_add(static_cast<_Ts>(_Scalar));
		}
		vector& operator-=(int _Scalar) {
			return this->assign_sub(static_cast<_Ts>(_Scalar));
		}
		vector& operator*=(int _Scalar) {
			return this->assign_mul(static_cast<_Ts>(_Scalar));
		}
		vector& operator/=(int _Scalar) {
			return this->assign_div(static_cast<_Ts>(_Scalar));
		}
		vector& operator%=(int _Scalar) {
			return this->assign_mod(static_cast<_Ts>(_Scalar));
		}
		friend vector operator+(int _Scalar, const vector& _Right) {
			return _Right.added(static_cast<_Ts>(_Scalar));
		}
		friend vector operator-(int _Scalar, const vector& _Right) {
			return _Right.subed(static_cast<_Ts>(_Scalar));
		}
		friend vector operator*(int _Scalar, const vector& _Right) {
			return _Right.muled(static_cast<_Ts>(_Scalar));
		}
		friend vector operator/(int _Scalar, const vector& _Right) {
			return _Right.dived(static_cast<_Ts>(_Scalar));
		}
		friend vector operator%(int _Scalar, const vector& _Right) {
			return _Right.moded(static_cast<_Ts>(_Scalar));
		}
		vector  operator+ (float _Scalar) const {
			return this->add(static_cast<_Ts>(_Scalar));
		}
		vector  operator- (float _Scalar) const {
			return this->sub(static_cast<_Ts>(_Scalar));
		}
		vector  operator* (float _Scalar) const {
			return this->mul(static_cast<_Ts>(_Scalar));
		}
		vector  operator/ (float _Scalar) const {
			return this->div(static_cast<_Ts>(_Scalar));
		}
		vector  operator% (float _Scalar) const {
			return this->mod(static_cast<_Ts>(_Scalar));
		}
		vector& operator+=(float _Scalar) {
			return this->assign_add(static_cast<_Ts>(_Scalar));
		}
		vector& operator-=(float _Scalar) {
			return this->assign_sub(static_cast<_Ts>(_Scalar));
		}
		vector& operator*=(float _Scalar) {
			return this->assign_mul(static_cast<_Ts>(_Scalar));
		}
		vector& operator/=(float _Scalar) {
			return this->assign_div(static_cast<_Ts>(_Scalar));
		}
		vector& operator%=(float _Scalar) {
			return this->assign_mod(static_cast<_Ts>(_Scalar));
		}
		friend vector operator+(float _Scalar, const vector& _Right) {
			return _Right.added(static_cast<_Ts>(_Scalar));
		}
		friend vector operator-(float _Scalar, const vector& _Right) {
			return _Right.subed(static_cast<_Ts>(_Scalar));
		}
		friend vector operator*(float _Scalar, const vector& _Right) {
			return _Right.muled(static_cast<_Ts>(_Scalar));
		}
		friend vector operator/(float _Scalar, const vector& _Right) {
			return _Right.dived(static_cast<_Ts>(_Scalar));
		}
		friend vector operator%(float _Scalar, const vector& _Right) {
			return _Right.moded(static_cast<_Ts>(_Scalar));
		}
		vector  operator+ (double _Scalar) const {
			return this->add(static_cast<_Ts>(_Scalar));
		}
		vector  operator- (double _Scalar) const {
			return this->sub(static_cast<_Ts>(_Scalar));
		}
		vector  operator* (double _Scalar) const {
			return this->mul(static_cast<_Ts>(_Scalar));
		}
		vector  operator/ (double _Scalar) const {
			return this->div(static_cast<_Ts>(_Scalar));
		}
		vector  operator% (double _Scalar) const {
			return this->mod(static_cast<_Ts>(_Scalar));
		}
		vector& operator+=(double _Scalar) {
			return this->assign_add(static_cast<_Ts>(_Scalar));
		}
		vector& operator-=(double _Scalar) {
			return this->assign_sub(static_cast<_Ts>(_Scalar));
		}
		vector& operator*=(double _Scalar) {
			return this->assign_mul(static_cast<_Ts>(_Scalar));
		}
		vector& operator/=(double _Scalar) {
			return this->assign_div(static_cast<_Ts>(_Scalar));
		}
		vector& operator%=(double _Scalar) {
			return this->assign_mod(static_cast<_Ts>(_Scalar));
		}
		friend vector operator+(double _Scalar, const vector& _Right) {
			return _Right.added(static_cast<_Ts>(_Scalar));
		}
		friend vector operator-(double _Scalar, const vector& _Right) {
			return _Right.subed(static_cast<_Ts>(_Scalar));
		}
		friend vector operator*(double _Scalar, const vector& _Right) {
			return _Right.muled(static_cast<_Ts>(_Scalar));
		}
		friend vector operator/(double _Scalar, const vector& _Right) {
			return _Right.dived(static_cast<_Ts>(_Scalar));
		}
		friend vector operator%(double _Scalar, const vector& _Right) {
			return _Right.moded(static_cast<_Ts>(_Scalar));
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
		
		friend std::ostream& operator<<(std::ostream& _Ostr, const vector& _Left) {
			return (_Ostr << _Left.to_string());
		}
	};

	template<typename _Ts, typename _Tb = _Ts>
	using vector2 = vector<_Ts, 2, _Tb>;
	template<typename _Ts, typename _Tb = _Ts>
	using vector3 = vector<_Ts, 3, _Tb>;
	template<typename _Ts, typename _Tb = _Ts>
	using vector4 = vector<_Ts, 4, _Tb>;

	template<typename _Ts, size_t _Size, typename _Tb = _Ts>
	using point = vector<_Ts, _Size, _Tb>;
	template<typename _Ts, typename _Tb = _Ts>
	using point2 = vector<_Ts, 2, _Tb>;
	template<typename _Ts, typename _Tb = _Ts>
	using point3 = vector<_Ts, 3, _Tb>;
	
	using point2_size_t = point2<size_t>;

	#define VECTORN ::clmagic::vector<_Ts, _Size, _Tb>
	#define VECTOR2 ::clmagic::vector<_Ts, 2, _Tb>
	#define VECTOR3 ::clmagic::vector<_Ts, 3, _Tb>
	#define VECTOR4 ::clmagic::vector<_Ts, 4, _Tb>



	template<typename _Ts, size_t _Size, typename _Tb> inline
	std::string to_string(const VECTORN& v) {
		return v.to_string();
	}

	template<typename _Ts, size_t _Size, typename _Tb> inline
	_Ts sum(const VECTORN& v) {
		return v.sum();
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	_Ts product(const VECTORN& v) {
		return v.product();
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	_Ts dot(const VECTORN& v1, const VECTORN& v2) {
		return v1.dot(v2);
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	_Ts length(const VECTORN& v) {
		return dot(v, v);
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	_Ts norm(const VECTORN& v, const _Ts& L) {
		return v.norm(L);
	}

	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN normalize(const VECTORN& v) {
		const _Ts _Length_squared = dot(v, v);
		return !approach_equal(_Length_squared, static_cast<_Ts>(1), std::numeric_limits<_Ts>::epsilon()) ?
			v / sqrt(_Length_squared) :
			v;
	}

	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN fmod(const VECTORN& _Left, const VECTORN& _Right) {
		_Return_generate_object( VECTORN, _Result, 
			fast_vector_operation<_Ts, _Tb>::func_2nd(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Right.begin<_Tb>(), _Result.begin<_Tb>(),
				[](const _Tb& _Arg0, const _Tb& _Arg1) { return fmod(_Arg0, _Arg1); }) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN fmod(const VECTORN& _Left, const _Ts& _Scalar) {
		const _Tb  _Arg1 = block_traits<_Tb>::set1(_Scalar);
		const auto _Func = [&_Arg1](const _Tb& _Arg0) { return fmod(_Arg0, _Arg1); };
		_Return_generate_object( VECTORN, _Result, 
			fast_vector_operation<_Ts, _Tb>::func_1st(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Result.begin<_Tb>(), _Func) );
	}

	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN pow(const VECTORN& _Left, const VECTORN& _Right) {
		_Return_generate_object( VECTORN, _Result, 
			fast_vector_operation<_Ts, _Tb>::func_2nd(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Right.begin<_Tb>(), _Result.begin<_Tb>(),
				[](const _Tb& _Arg0, const _Tb& _Arg1) { return pow(_Arg0, _Arg1); }) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN pow(const VECTORN& _Left, const _Ts& _Scalar) {
		const _Tb  _Arg1 = block_traits<_Tb>::set1(_Scalar);
		const auto _Func = [&_Arg1](const _Tb& _Arg0) { return pow(_Arg0, _Arg1); };
		_Return_generate_object( VECTORN, _Result, 
			fast_vector_operation<_Ts, _Tb>::func_1st(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Result.begin<_Tb>(), _Func) );
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
		_Return_generate_object( VECTORN, _Result,
			fast_vector_operation<_Ts, _Tb>::func_1st(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Result.begin<_Tb>(), 
				[](const _Tb& _Arg0) { return abs(_Arg0); }) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN floor(const VECTORN& _Left) {// floor(v)
		_Return_generate_object( VECTORN, _Result,
			fast_vector_operation<_Ts, _Tb>::func_1st(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Result.begin<_Tb>(), 
				[](const _Tb& _Arg0) { return floor(_Arg0); }) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN ceil(const VECTORN& _Left) {// ceil(v)
		_Return_generate_object( VECTORN, _Result,
			fast_vector_operation<_Ts, _Tb>::func_1st(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Result.begin<_Tb>(), 
				[](const _Tb& _Arg0) { return ceil(_Arg0); }) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN trunc(const VECTORN& _Left) {// trunc(v)
		_Return_generate_object( VECTORN, _Result,
			fast_vector_operation<_Ts, _Tb>::func_1st(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Result.begin<_Tb>(), 
				[](const _Tb& _Arg0) { return trunc(_Arg0); }) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN round(const VECTORN& _Left) {// round(v)
		_Return_generate_object( VECTORN, _Result,
			fast_vector_operation<_Ts, _Tb>::func_1st(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Result.begin<_Tb>(), 
				[](const _Tb& _Arg0) { return round(_Arg0); }) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN sqrt(const VECTORN& _Left) {// sqrt(v)
		_Return_generate_object( VECTORN, _Result,
			fast_vector_operation<_Ts, _Tb>::func_1st(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Result.begin<_Tb>(), 
				[](const _Tb& _Arg0) { return sqrt(_Arg0); }) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN cbrt(const VECTORN& _Left) {// cbrt(v)
		_Return_generate_object( VECTORN, _Result,
			fast_vector_operation<_Ts, _Tb>::func_1st(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Result.begin<_Tb>(), 
				[](const _Tb& _Arg0) { return cbrt(_Arg0); }) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN invsqrt(const VECTORN& _Left) {// invsqrt(v)
		_Return_generate_object( VECTORN, _Result,
			fast_vector_operation<_Ts, _Tb>::func_1st(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Result.begin<_Tb>(), 
				[](const _Tb& _Arg0) { return invsqrt(_Arg0); }) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN invcbrt(const VECTORN& _Left) {// invcbrt(v)
		_Return_generate_object( VECTORN, _Result,
			fast_vector_operation<_Ts, _Tb>::func_1st(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Result.begin<_Tb>(), 
				[](const _Tb& _Arg0) { return invcbrt(_Arg0); }) );
	}

	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN sin(const VECTORN& _Left) {// sin(v)
		_Return_generate_object( VECTORN, _Result,
			fast_vector_operation<_Ts, _Tb>::func_1st(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Result.begin<_Tb>(), 
				[](const _Tb& _Arg0) { return sin(_Arg0); }) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN cos(const VECTORN& _Left) {
		_Return_generate_object( VECTORN, _Result,
			fast_vector_operation<_Ts, _Tb>::func_1st(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Result.begin<_Tb>(), 
				[](const _Tb& _Arg0) { return cos(_Arg0); }) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN tan(const VECTORN& _Left) {// tan(v)
		_Return_generate_object( VECTORN, _Result,
			fast_vector_operation<_Ts, _Tb>::func_1st(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Result.begin<_Tb>(), 
				[](const _Tb& _Arg0) { return tan(_Arg0); }) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN asin(const VECTORN& _Left) {// asin(v)
		_Return_generate_object( VECTORN, _Result,
			fast_vector_operation<_Ts, _Tb>::func_1st(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Result.begin<_Tb>(), 
				[](const _Tb& _Arg0) { return asin(_Arg0); }) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN acos(const VECTORN& _Left) {// acos(v)
		_Return_generate_object( VECTORN, _Result,
			fast_vector_operation<_Ts, _Tb>::func_1st(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Result.begin<_Tb>(), 
				[](const _Tb& _Arg0) { return acos(_Arg0); }) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN atan(const VECTORN& _Left) {// atan(v)
		_Return_generate_object( VECTORN, _Result,
			fast_vector_operation<_Ts, _Tb>::func_1st(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Result.begin<_Tb>(), 
				[](const _Tb& _Arg0) { return atan(_Arg0); }) );
	}
	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN atan2(const VECTORN& _Left, const VECTORN& _Right) {// atan(yv, xv)
		_Return_generate_object( VECTORN, _Result, 
			fast_vector_operation<_Ts, _Tb>::func_2nd(_Left.begin<_Tb>(), _Left.begin<_Tb>()+_Left.block_size(), _Right.begin<_Tb>(), _Result.begin<_Tb>(),
				[](const _Tb& _Arg0, const _Tb& _Arg1) { return atan2(_Arg0, _Arg1); }) );
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


	/*- - - - - - - - - - - - - - - - - - - unit_vector<> - - - - - - - - - - - - - - - - - - - - -*/
	/*
	@_unit_vector<T>: as vector3<T>
	@_Traits: length = 1.0
	*/
	template<typename _Ts, size_t _Size, typename _Tb = _Ts>
	struct unit_vector : public vector<_Ts, _Size, _Tb> {
		using _Mybase     = vector<_Ts, _Size, _Tb>;
		using scalar_type = _Ts;
		using vector_Tspe = unit_vector<_Ts, _Size, _Tb>;

		unit_vector() = default;

		unit_vector(std::initializer_list<_Ts> _Ilist, bool _Unitized = false)
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

		_Ts length() const {
			return static_cast<_Ts>(1);
		}
	};

	template<typename _Ts, typename _Tb = _Ts>
	using unit_vector3 = unit_vector<_Ts, 3, _Tb>;

	#define UNIT_VECTORN ::clmagic::unit_vector<_Ts, _Size, _Tb>
	#define UNIT_VECTOR3 ::clmagic::unit_vector3<_Ts, _Tb>

	/*- - - - - - - - - - - - - - - - - - - vector_any<> - - - - - - - - - - - - - - - - - - - - -*/
	/*
	@_vector_any: dynamic-length vector
	*/
	template<typename _Ts, typename _Tb = _Ts, typename _Alloc = std::allocator<_Tb>>
	class vector_any : public clmagic::subvector<_Ts, _Tb> {
		using _Mybase = clmagic::subvector<_Ts, _Tb>;
	public:
		using scalar_type         = _Ts;
		using scalar_pointer         = _Ts*;
		using scalar_const_pointer   = _Ts const*;
		using scalar_reference       = _Ts&;
		using scalar_const_reference = _Ts const&;

		using block_type           = _Tb;
		using block_pointer        = _Tb*;
		using block_const_pointer  = _Tb const*;
		using block_reference       = _Tb&;
		using block_const_reference = _Tb const&;

		using subvector       = clmagic::subvector<_Ts, _Tb>;
		using const_subvector = clmagic::const_subvector<_Ts, _Tb>;

		using block_traits    = clmagic::block_traits<_Tb>;
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
		void assign(const vector_any& _Left, const _Tb& _Right_bk, _Fn _Func) {
			this->assign(_Left,
				[_Func, &_Right_bk](block_reference _Left_bk) { return _Func(_Left_bk, _Right_bk); });
		}

		template<typename _Fn>
		void assign(const _Tb& _Left_bk, const vector_any& _Right, _Fn _Func) {
			this->assign(_Right,
				[_Func, &_Left_bk](block_reference _Right_bk) { return _Func(_Left_bk, _Right_bk); });
		}

		// <Internal-function>
		template<typename _Fn>
		void _Assign(const vector_any& _Left, const _Ts& _Scalar, _Fn _Func) {
			this->assign(_Left, block_traits::set1(_Scalar));
		}

		template<typename _Fn>
		void _Assign(const _Ts& _Scalar, const vector_any& _Right, _Fn _Func) {
			this->assign(block_traits::set1(_Scalar), _Right);
		}
		// </Internal-function>

		template<typename _Iter>
		vector_any(_Iter _First, _Iter _Last) : vector_any() {
			this->assign(_First, _Last);
		}

		vector_any(std::initializer_list<_Ts> _Ilist) : vector_any() {
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
		vector_any(const vector_any& _Left, const _Ts& _Scalar, _Fn _Func) : vector_any() {// construct( this, _Func(_Left, block_tratis::set1(_Scalar)) )
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
		vector_any(const _Ts& _Scalar, const vector_any& _Right, _Fn _Func) : vector_any() {// construct( this, _Func(block_tratis::set1(_Scalar), _Right) )
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
		vector_any operator+(const _Ts& _Scalar) const {
			return std::move(vector_any(*this, _Scalar, std::plus<block_type>()));
		}
		vector_any operator-(const _Ts& _Scalar) const {
			return std::move(vector_any(*this, _Scalar, std::minus<block_type>()));
		}
		vector_any operator*(const _Ts& _Scalar) const {
			return std::move(vector_any(*this, _Scalar, std::multiplies<block_type>()));
		}
		vector_any operator/(const _Ts& _Scalar) const {
			return std::move(vector_any(*this, _Scalar, std::divides<block_type>()));
		}
		vector_any operator%(const _Ts& _Scalar) const {
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
		vector_any& operator+=(const _Ts& _Scalar) {
			this->_Assign(*this, _Scalar, std::plus<block_type>());
			return (*this);
		}
		vector_any& operator-=(const _Ts& _Scalar) {
			this->_Assign(*this, _Scalar, std::minus<block_type>());
			return (*this);
		}
		vector_any& operator*=(const _Ts& _Scalar) {
			this->_Assign(*this, _Scalar, std::multiplies<block_type>());
			return (*this);
		}
		vector_any& operator/=(const _Ts& _Scalar) {
			this->_Assign(*this, _Scalar, std::divides<block_type>());
			return (*this);
		}
		vector_any& operator%=(const _Ts& _Scalar) {
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
				_Myvec._Mysize = vector_size<_Ts, _Tb>(_Myvec._Myfirst, _Myvec._Mylast);// correct vector-size
			} else {
				_Reallocate(_Calculate_growth(this->capacity() + 1));
				emplace_back(std::forward<_Valty...>(_Val...));
			}
		}

		void push_back(const _Ts& _Val) {
			this->emplace_back(_Val);
		}

		void push_back(_Ts&& _Val) {
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
					std::cout << "[vector_any<_Ts, _Tb> memory exception] ->[~vector_any()]" << std::endl;
					//throw std::exception("[vector_any<_Ts, _Tb> memory exception] ->[~vector_any()]");
				}
			}
		}

		std::string to_string() const {
			return ::clmagic::to_string(this->begin(), this->end()); }
	
		friend std::ostream& operator<<(std::ostream& _Ostr, const vector_any& _Left) {
			return (_Ostr << _Left.to_string()); }

	private:

		scalar_pointer _Destroy_range(scalar_pointer _First, scalar_const_pointer _Last) {
			for (; _First != _Last; ++_First) {
				allocate_traits::destroy(_Myalloc, _First);
			}
			return _First;
		}

		scalar_pointer _Construct_range(scalar_pointer _First, scalar_const_pointer _Last, const _Ts& _Val) {
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

		/* learning from C++Stantard-library-vector<_Ts,...> */
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

		/* learning from C++Stantard-library-vector<_Ts,...> */
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

		/* learning from C++Stantard-library-vector<_Ts,...> */
		void _Resize(size_t _Newsize/*scalar*/, const _Ts& _Val) {
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
		_Tb* _Mydata;
		size_t  _Mycapacity;
	};


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



	// dot(_I,_Nref) < 0 ? N : -N
	template<typename _Ts, size_t _Size, typename _Tb> inline
	UNIT_VECTORN faceforward(UNIT_VECTORN _Normal, VECTORN _I, VECTORN _Nref) {
		return (dot(_I, _Nref) < static_cast<_Ts>(0) ? _Normal : -_Normal);
	}

	template<typename _Ts, size_t _Size, typename _Tb> inline
	VECTORN proj(const VECTORN& _Vector, const VECTORN& _Proj) {// return dot(V,||_Proj||) * |Proj| 
		return (dot(_Vector, _Proj) / dot(_Proj, _Proj) * _Proj);
		/*
			|          / |
			|_Vector/
			|    /	     |
			| /
			|-> result<--|▜___________
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