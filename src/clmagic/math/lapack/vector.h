//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_math_lapack_VECTOR_h_
#define clmagic_math_lapack_VECTOR_h_
#include "../general/clmagic.h"// include is_*<>, shuffle<>
#include "../general/general.h"// include abs() minval() maxval()
#include "../general/bitset.h"// include ceil(Number, Bound)
#include "../general/real.h"// include CSTD math
#include "../general/simd.h"// block_traits<>, simd_operator
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
	template<typename _SclTy, typename _BlkTy>
	struct vector_size {
		size_t leading_size;
		size_t block_size;
		size_t tail_size;

		static void initializer(vector_size& _Dest, const _SclTy* _First, const _SclTy* _Last) {
			if (_First != nullptr && _Last > _First) {
				const auto _Mysize  = _Last - _First;
				auto       _Mybkoff = reinterpret_cast<size_t>(_First) & clmagic::alignment_mask_of_v<_BlkTy>;
				if (_Mybkoff % sizeof(_SclTy) == 0) {
					_Mybkoff /= sizeof(_SclTy);
					const bool _Pass_alignment_point = (_Mybkoff + _Mysize >= block_traits<_BlkTy>::size());
					if (_Pass_alignment_point) {// leading_size != 0 || block_size != 0
						_Dest.leading_size = block_traits<_BlkTy>::size() - _Mybkoff;
						_Dest.block_size   = (_Mysize - _Dest.leading_size) / block_traits<_BlkTy>::size();
						_Dest.tail_size    = (_Mysize - _Dest.leading_size) % block_traits<_BlkTy>::size();
					}
				}
			}
		}

		vector_size() : leading_size(0), block_size(0), tail_size(0) {}

		vector_size(const _SclTy* _First, const _SclTy* _Last) : vector_size() {
			initializer(*this, _First, _Last);
		}

		vector_size(const _SclTy* _First, const _SclTy* _Last, size_t _Stride) : vector_size() {
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
	template<typename _SclTy, typename _BlkTy, _Vector_accelerate_level/*default: Norm*/>
	struct _Vector_operation {
		using _OutIt = _SclTy*;
		using _InIt  = const _SclTy *;

		using scalar_type  = _SclTy;

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
		static _SclTy& afunc(_InIt _First, _InIt _Last, _SclTy& _Result, _Afn _Afunc,
			size_t _Inc1 = 1) {
			for (; _First != _Last; _First+=_Inc1) {
				_Afunc(_Result, *_First);
			}
			return _Result;
		}

		template<typename _Afn, typename _Fn>
		static _SclTy& afunc_1st(_InIt _First, _InIt _Last, _SclTy& _Result, _Afn _Afunc, _Fn _Func,
			size_t _Inc = 1) {
			// for [_First, _Last): _Afunc(_Result, _Func(*_First))
			// _Afunc = [](auto&, auto&&), for example: [](auto& Res, auto&& X){ Res += X; } ...
			// _Func  = [](const _SclTy&), for example: exp2<_BlkTy> ...
			for ( ; _First != _Last; _First+=_Inc) {
				_Afunc(_Result, _Func(*_First));
			}
			return _Result;
		}

		template<typename _Afn, typename _Fn>
		static _SclTy& afunc_2nd(_InIt _First1, _InIt _Last1, _InIt _First2, _SclTy& _Result, _Afn _Afunc, _Fn _Func,
			size_t _Inc1 = 1, size_t _Inc2 = 1) {
			// for [_First1, _Last1): _Afunc(_Result, _Func(*_First1, *_First2))
			// _Afunc = [](auto&, auto&&),                for example: [](auto& Res, auto&& X){ Res += X; } ...
			// _Func  = [](const _SclTy&, const _SclTy&), for example: std::multiplies<_BlkTy>() ...
			for ( ; _First1 != _Last1; _First1+=_Inc1, _First2+=_Inc2) {
				_Afunc(_Result, _Func(*_First1, *_First2));
			}
			return _Result;
		}

		template<typename _Fn>
		static _SclTy& norm(_InIt _First, _InIt _Last, _SclTy L, _SclTy& _Result,
			size_t _Inc = 1) {
			afunc_1st(_First, _Last, _Result, 
				[](auto& _Res, auto&& _X) { _Res += _X; },
				[](auto&& _X) { return pow(abs(_X), L); }, 
				_Inc
			);

			_Result = pow(_Result, static_cast<_SclTy>(1) / L);
			return _Result;
		}

		/*<dot>
			<code>
				auto _Result = static_cast<_SclTy>(0);
				auto_vector_operation<_SclTy, _BlkTy>::afunc_2st( _First1, _Last1, _First2, _Result, 
					[](auto& _Res, auto&& _X){ _Res += _X; }, 
					std::multiplies<>() );
			</code>
		  </dot>*/
	};

	template<typename _SclTy, typename _BlkTy>
	struct _Vector_operation<_SclTy, _BlkTy, _Vector_accelerate_level::Fastest> {
		using _OutIt = _BlkTy*;
		using _InIt  = const _BlkTy *;

		using scalar_type  = _SclTy;
		using block_type   = _BlkTy;
		using block_traits = ::clmagic::block_traits<_BlkTy>;
		using scalar_operation = _Vector_operation<_SclTy, _BlkTy, _Vector_accelerate_level::Norm>;

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
		static _SclTy& afunc(_InIt _First, _InIt _Last, _SclTy& _Result, _Afn _Afunc) {
			if (_First != _Last) {
				auto _Temp = *_First++;
				for (; _First != _Last; ++_First) {
					_Afunc(_Temp, *_First);
				}

				auto _First2 = reinterpret_cast<_SclTy*>(&_Temp);
				scalar_operation::afunc(_First2, _First2+block_traits::size(), _Result, _Afunc);
			}
			return _Result;
		}

		template<typename _Afn, typename _Fn>
		static _SclTy& afunc_1st(_InIt _First, _InIt _Last, _SclTy& _Result, _Afn _Afunc, _Fn _Func) {
			// for [_First, _Last): _Afunc(_Result, _Func(*_First))
			// _Afunc = [](auto&, auto&&), for example: [](auto& Res, auto&& X){ Res += X; } ...
			// _Func  = [](const _BlkTy&), for example: exp2<_BlkTy> ...
			if (_First != _Last) {
				auto _Temp = _Func(*_First++);
				for ( ; _First != _Last; ++_First) {
					_Afunc(_Temp, _Func(*_First));
				}

				auto _First_temp = reinterpret_cast<_SclTy*>(&_Temp);
				scalar_operation::afunc(_First_temp, _First_temp+block_traits::size(), _Result, _Afunc);
			}
			return _Result;
		}

		template<typename _Afn, typename _Fn>
		static _SclTy& afunc_2nd(_InIt _First1, _InIt _Last1, _InIt _First2, _SclTy& _Result, _Afn _Afunc, _Fn _Func) {
			// for [_First1, _Last1): _Afunc(_Result, _Func(*_First1, *_First2))
			// _Afunc = [](auto&, auto&&),                for example: [](auto& Res, auto&& X){ Res += X; } ...
			// _Func  = [](const _BlkTy&, const _BlkTy&), for example: std::multiplies<_BlkTy>() ...
			if (_First1 != _Last1) {
				auto _Temp = _Func(*_First1++, *_First2++);
				for ( ; _First1 != _Last1; ++_First1, ++_First2) {
					_Afunc(_Temp, _Func(*_First1, *_First2));
				}

				auto _First_temp = reinterpret_cast<_SclTy*>(&_Temp);
				scalar_operation::afunc(_First_temp, _First_temp+block_traits::size(), _Result, _Afunc);
			}
			return _Result;
		}

		template<typename _Fn>
		static _SclTy& norm(_InIt _First, _InIt _Last, _SclTy L, _SclTy& _Result) {
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

			_Result = pow(_Result, static_cast<_SclTy>(1) / L);
			return _Result;
		}

		/*<dot>
			<code>
				auto _Result = static_cast<_SclTy>(0);
				auto_vector_operation<_SclTy, _BlkTy>::afunc_2st( _First1, _Last1, _First2, _Result, 
					[](auto& _Res, auto&& _X){ _Res += _X; }, 
					std::multiplies<>() );
			</code>
		  </dot>*/
	};

	template<typename _SclTy, typename _BlkTy>
	struct _Vector_operation<_SclTy, _BlkTy, _Vector_accelerate_level::Fast> {
		using _OutIt = _BlkTy*;
		using _InIt  = const _BlkTy*;

		using scalar_type  = _SclTy;
		using block_type   = _BlkTy;
		using block_traits = ::clmagic::block_traits<_BlkTy>;
		using scalar_operation = _Vector_operation<_SclTy, _BlkTy, _Vector_accelerate_level::Norm>;
		using block_operation  = _Vector_operation<_SclTy, _BlkTy, _Vector_accelerate_level::Fastest>;

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
				const _BlkTy       _Temp      = _Func(*(_First - 1));
				const scalar_type* _Last_temp = reinterpret_cast<const _SclTy*>((&_Temp) + 1);
				std::copy(_Last_temp-_Leading_size, _Last_temp, ((scalar_type*)_Dest) - _Leading_size );
			}

			for ( ; _First != _Last; ++_First, ++_Dest) {
				*_Dest = _Func(*_First);
			}

			if (_Tail_size != 0) {// vector-length is usually large, when _Tail_size != 0
				const _BlkTy       _Temp       = _Func(*_First);
				const scalar_type* _First_temp = reinterpret_cast<const _SclTy*>(&_Temp);
				std::copy(_First_temp, _First_temp+_Tail_size, (scalar_type*)_Dest);
			}

			return _Dest;// Almost useless
		}
	
		template<typename _Fn>
		static _OutIt func_2nd(_InIt _First1, _InIt _Last1, _InIt _First2, _OutIt _Dest, _Fn _Func, 
			size_t _Leading_size = 0, size_t _Tail_size = 0) {
			if (_Leading_size != 0) {
				const _BlkTy       _Temp      = _Func(*(_First1 - 1), *(_First2 - 1));
				const scalar_type* _Last_temp = reinterpret_cast<const scalar_type*>((&_Temp) + 1);
				std::copy(_Last_temp-_Leading_size, _Last_temp, ((scalar_type*)_Dest) - _Leading_size );
			}

			for (; _First1 != _Last1; ++_First1, ++_First2, ++_Dest) {
				*_Dest = _Func(*_First1, *_First2);
			}

			if (_Tail_size == 0) {// vector-length is usually large, when _Tail_size != 0
				const _BlkTy       _Temp       = _Func(*_First1, *_First2);
				const scalar_type* _First_temp = reinterpret_cast<const scalar_type*>(&_Temp);
				std::copy(_First_temp, _First_temp+_Tail_size, (scalar_type*)_Dest);
			}

			return _Dest;// Almost useless
		}
	
		/*<afunc>
			<mean>assign-function</mean>
		  </afunc>*/
		template<typename _Afn>
		static _SclTy& afunc(_InIt _First, _InIt _Last, _SclTy& _Result, _Afn _Afunc, 
			size_t _Leading_size = 0, size_t _Tail_size = 0) {
			if (_Leading_size != 0) {
				auto _Last_temp = reinterpret_cast<const _SclTy*>(_First);
				scalar_operation::afunc(_Last_temp-_Leading_size, _Last_temp, _Result, _Afunc);
			}

			if (_First != _Last) {
				auto _Temp = *_First++;
				for ( ; _First != _Last; ++_First) {
					_Afunc(_Temp, *_First);
				}
				auto _First_temp = reinterpret_cast<_SclTy*>(&_Temp);
				scalar_operation::afunc(_First_temp, _First_temp+block_traits::size(), _Result, _Afunc);
			}

			if (_Tail_size != 0) {
				auto _First_temp = reinterpret_cast<const _SclTy*>(_First);
				scalar_operation::afunc(_First_temp, _First_temp+_Tail_size, _Result, _Afunc);
			}

			return _Result;
		}

		template<typename _Afn, typename _Fn>
		static _SclTy& afunc_1st(_InIt _First, _InIt _Last, _SclTy& _Result, _Afn _Afunc, _Fn _Func, 
			size_t _Leading_size = 0, size_t _Tail_size = 0) {
			if (_Leading_size != 0) {
				const auto _Temp      = _Func(*(_First - 1));
				auto       _Last_temp = reinterpret_cast<const _SclTy*>((&_Temp) + 1);
				scalar_operation::afunc(_Last_temp-_Leading_size, _Last_temp, _Result, _Afunc);
			}

			block_operation::afunc_1st(_First, _Last, _Result, _Afunc, _Func);// impotant!

			if (_Tail_size != 0) {
				const auto _Temp       = _Func(*_Last);
				auto       _First_temp = reinterpret_cast<const _SclTy*>(&_Temp);
				scalar_operation::afunc(_First_temp, _First_temp+_Tail_size, _Result, _Afunc);
			}

			return _Result;
		}

		template<typename _Afn, typename _Fn>
		static _SclTy& afunc_2nd(_InIt _First1, _InIt _Last1, _InIt _First2, _SclTy& _Result, _Afn _Afunc, _Fn _Func,
			size_t _Leading_size = 0, size_t _Tail_size = 0) {
			if (_Leading_size != 0) {
				const auto _Temp      = _Func(*(_First1 - 1), *(_First2 - 1));
				auto       _Last_temp = reinterpret_cast<const _SclTy*>((&_Temp) + 1);
				scalar_operation::afunc(_Last_temp-_Leading_size, _Last_temp, _Result, _Afunc);
			}

			if (_First1 != _Last1) {
				auto _Temp = _Func(*_First1++, *_First2++);
				for ( ; _First1 != _Last1; ++_First1, ++_First2) {
					_Afunc(_Temp, _Func(*_First1, *_First2));
				}

				auto _First_temp = reinterpret_cast<_SclTy*>(&_Temp);
				scalar_operation::afunc(_First_temp, _First_temp+block_traits::size(), _Result, _Afunc);
			}

			if (_Tail_size != 0) {
				const auto _Temp       = _Func(*_First1, *_First2);
				auto       _First_temp = reinterpret_cast<const _SclTy*>(&_Temp);
				scalar_operation::afunc(_First_temp, _First_temp+_Tail_size, _Result, _Afunc);
			}

			return _Result;
		}
	
		template<typename _Fn>
		static _SclTy& norm(_InIt _First, _InIt _Last, _SclTy L, _SclTy& _Result, 
			size_t _Leading_size = 0, size_t _Tail_size = 0) {
			afunc_1st(_First, _Last, _Result, 
				[](auto& _Res, auto&& _X) { _Res += _X; },
				[](auto&& _X) { return pow(abs(_X), L); },
				_Leading_size, _Tail_size
			);

			_Result = pow(_Result, static_cast<_SclTy>(1) / L);
			return _Result;
		}
	};

	template<typename _SclTy, typename _BlkTy>
	struct _Vector_operation<_SclTy, _BlkTy, _Vector_accelerate_level::Auto> {
		using _OutIt = void*;
		using _InIt  = const void *;

		using size_type    = ::clmagic::vector_size<_SclTy, _BlkTy>;
		using block_traits = ::clmagic::block_traits<_BlkTy>;

		using scalar_operation  = _Vector_operation<_SclTy, _BlkTy, _Vector_accelerate_level::Norm>;
		using block_operation   = _Vector_operation<_SclTy, _BlkTy, _Vector_accelerate_level::Fastest>;
		using fastest_operation = block_operation;
		using fast_operation    = _Vector_operation<_SclTy, _BlkTy, _Vector_accelerate_level::Fast>;
		using norm_operation    = block_operation;

		template<typename _Fn>
		static _OutIt func_1st(_InIt _First, _InIt _Last, _OutIt _Dest, _Fn _Func, 
			const size_type& _Vsize) {
			if (_Vsize.can_fastest()) {
				return fastest_operation::func_1st(static_cast<const _BlkTy*>(_First), static_cast<const _BlkTy*>(_Last),
					static_cast<_BlkTy*>(_Dest), _Func);
			} else if (!_Vsize.empty()) {
				auto       _First_bk = reinterpret_cast<const _BlkTy*>( (const _SclTy*)(_First) + _Vsize.leading_size );
				const auto _Last_bk  = _First_bk + _Vsize.block_size;
				auto       _Dest_bk  = reinterpret_cast<const _BlkTy*>( (const _SclTy*)(_Dest) + _Vsize.leading_size );
				return fast_operation::func_1st(_First_bk, _Last_bk, _Dest_bk, _Func,
					_Vsize.leading_size, _Vsize.tail_size);
			}

			return norm_operation::func_1st(static_cast<const _SclTy*>(_First), static_cast<const _SclTy*>(_Last),
				static_cast<_SclTy*>(_Dest), _Func);
		}

		template<typename _Fn>
		static _OutIt func_1st(_InIt _First, _InIt _Last, _OutIt _Dest, _Fn _Func,
			size_t _Inc1 = 1, size_t _Inc2 = 1) {
			if (_Inc1 == 1 && _Inc2 == 1) {// memory-continue
				return func_1st( _First, _Last, _Dest, _Func, size_type((const _SclTy*)_First, (const _SclTy*)_Last) );
			}

			return norm_operation::func_1st((const _SclTy*)(_First), (const _SclTy*)(_Last), 
				(_SclTy*)(_Dest), _Func, _Inc1, _Inc2);
		}
	
		template<typename _Fn>
		static _OutIt func_2nd(_InIt _First1, _InIt _Last1, _InIt _First2, _OutIt _Dest, _Fn _Func,
			const size_type& _Vsize1, const size_type& _Vsize2) {
			if (_Vsize1 == _Vsize2) {
				if (_Vsize1.can_fastest()) {
					return fastest_operation::func_2nd(static_cast<const _BlkTy*>(_First1), static_cast<const _BlkTy*>(_Last1),
						static_cast<const _BlkTy*>(_First2), static_cast<_BlkTy*>(_Dest), _Func);
				} else if(!_Vsize1.empty()) {
					auto       _First1_bk = reinterpret_cast<const _BlkTy*>((const _SclTy*)(_First1) + _Vsize1.leading_size);
					const auto _Last1_bk  = _First1_bk + _Vsize1.block_size;
					auto       _First2_bk = reinterpret_cast<const _BlkTy*>((const _SclTy*)(_First2) + _Vsize1.leading_size);
					auto       _Dest_bk   = reinterpret_cast<const _BlkTy*>((const _SclTy*)(_Dest) + _Vsize1.leading_size);
					return fast_operation::func_2nd(_First1_bk, _Last1_bk, _First2_bk, _Dest_bk, _Func,
						_Vsize1.leading_size, _Vsize1.tail_size);
				}
			}

			return norm_operation::func_2nd(static_cast<const _SclTy*>(_First1), static_cast<const _SclTy*>(_Last1), 
				static_cast<const _SclTy*>(_First2), static_cast<_SclTy*>(_Dest), _Func );
		}

		template<typename _Fn>
		static _OutIt func_2nd(_InIt _First1, _InIt _Last1, _InIt _First2, _OutIt _Dest, _Fn _Func, 
			size_t _Inc1 = 1, size_t _Inc2 = 1, size_t _Inc3 = 1) {
			if (_Inc1 == 1 && _Inc2 == 1 && _Inc3 == 1) {// is_continue
				const auto _Diff = std::distance((const _SclTy*)(_First1), (const _SclTy*)(_Last1));

				return func_2nd( _First1, _Last1, _First2, _Dest, _Func, 
					size_type((const _SclTy*)_First1, (const _SclTy*)_Last1), 
					size_type((const _SclTy*)_First2, (const _SclTy*)_First2 + _Diff) );
			}

			return norm_operation::func_2nd(static_cast<const _SclTy*>(_First1), static_cast<const _SclTy*>(_Last1),
				static_cast<const _SclTy*>(_First2), static_cast<_SclTy*>(_Dest), _Func, 
				_Inc1, _Inc2, _Inc3);
		}
	
		template<typename _Afn>
		static _SclTy& afunc(_InIt _First, _InIt _Last, _SclTy& _Result, _Afn _Afunc, const size_type& _Vsize) {
			if ( _Vsize.can_fastest() ) {
				return fastest_operation::afunc((const _BlkTy*)_First, (const _BlkTy*)_Last, _Result, _Afunc);
			} else if ( !_Vsize.empty() ) {
				auto _First_bk = reinterpret_cast<const _BlkTy*>( (const _SclTy*)(_First) + _Vsize.leading_size );
				return fast_operation::afunc(_First_bk, _First_bk+_Vsize.block_size, _Result, _Afunc);
			}

			return norm_operation::afunc((const _SclTy*)_First, (const _SclTy*)_Last, _Result, _Afunc);
		}

		template<typename _Afn>
		static _SclTy& afunc(_InIt _First, _InIt _Last, _SclTy& _Result, _Afn _Afunc,
			size_t _Inc = 1) {
			if (_Inc == 1) {
				return afunc(_First, _Last, _Result, _Afunc, size_type((const _SclTy*)_First, (const _SclTy*)_Last));
			}

			return norm_operation::afunc((const _SclTy*)_First, (const _SclTy*)_Last, _Result, _Afunc);
		}

		template<typename _Afn, typename _Fn>
		static _SclTy& afunc_1st(_InIt _First, _InIt _Last, _SclTy& _Result, _Afn _Afunc, _Fn _Func,
			const size_type& _Vsize) {
			if ( _Vsize.can_fastest() ) {
				return fastest_operation::afunc_1st((const _BlkTy*)_First, (const _BlkTy*)_Last, _Result, _Afunc, _Func);
			} else if ( !_Vsize.empty() ) {
				auto _First_bk = reinterpret_cast<const _BlkTy*>( (const _SclTy*)(_First) + _Vsize.leading_size );
				return fast_operation::afunc_1st(_First_bk, _First_bk+_Vsize.block_size, _Result, _Afunc, _Func);
			}

			return norm_operation::afunc_1st((const _SclTy*)_First, (const _SclTy*)_Last, _Result, _Afunc, _Func);
		}

		template<typename _Afn, typename _Fn>
		static _SclTy& afunc_1st(_InIt _First, _InIt _Last, _SclTy& _Result, _Afn _Afunc, _Fn _Func,
			size_t _Inc = 1) {
			if (_Inc == 1) {
				return afunc_1st(_First, _Last, _Result, _Afunc, _Func, size_type((const _SclTy*)_First, (const _SclTy*)_Last));
			}

			return norm_operation::afunc_1st((const _SclTy*)_First, (const _SclTy*)_Last, _Result, _Afunc, _Func, _Inc);
		}

		template<typename _Afn, typename _Fn>
		static _OutIt afunc_2nd(_InIt _First1, _InIt _Last1, _InIt _First2, _SclTy& _Result, _Afn _Afunc, _Fn _Func,
			const size_type& _Vsize1, const size_type& _Vsize2) {
			if (_Vsize1 == _Vsize2) {
				if ( _Vsize1.can_fastest() ) {
					return fastest_operation::afunc_2nd(static_cast<const _BlkTy*>(_First1), static_cast<const _BlkTy*>(_Last1),
						static_cast<const _BlkTy*>(_First2), _Result, _Afunc, _Func);
				} else if( !_Vsize1.empty() ) {
					auto       _First1_bk = reinterpret_cast<const _BlkTy*>((const _SclTy*)(_First1) + _Vsize1.leading_size);
					const auto _Last1_bk  = _First1_bk + _Vsize1.block_size;
					auto       _First2_bk = reinterpret_cast<const _BlkTy*>((const _SclTy*)(_First2) + _Vsize1.leading_size);
					return fast_operation::afunc_2nd(_First1_bk, _Last1_bk, _First2_bk, _Result, _Afunc, _Func,
						_Vsize1.leading_size, _Vsize1.tail_size);
				}
			}

			return norm_operation::afunc_2nd(static_cast<const _SclTy*>(_First1), static_cast<const _SclTy*>(_Last1),
				static_cast<const _SclTy*>(_First2), _Result, _Afunc, _Func );
		}

		template<typename _Afn, typename _Fn>
		static _OutIt afunc_2nd(_InIt _First1, _InIt _Last1, _InIt _First2, _SclTy& _Result, _Afn _Afunc, _Fn _Func,
			size_t _Inc1 = 1, size_t _Inc2 = 1) {
			if (_Inc1 == 1 && _Inc2 == 1) {
				const auto _Diff = std::distance((const _SclTy*)(_First1), (const _SclTy*)(_Last1));

				return afunc_2nd( _First1, _Last1, _First2, _Result, _Func,
					size_type((const _SclTy*)_First1, (const _SclTy*)_Last1), 
					size_type((const _SclTy*)_First2, (const _SclTy*)_First2 + _Diff) );
			}

			return norm_operation::afunc_2nd(static_cast<const _SclTy*>(_First1), static_cast<const _SclTy*>(_Last1),
				static_cast<const _SclTy*>(_First2), _Result, _Afunc, _Func);
		}
	
		template<typename _Fn>
		static _SclTy& norm(_InIt _First, _InIt _Last, _SclTy L, _SclTy& _Result,
			size_t _Inc1 = 1, size_t _Inc2 = 1) {
			afunc_1st(_First, _Last, _Result, 
				[](auto& _Res, auto&& _X) { _Res += _X; },
				[](auto&& _X) { return pow(abs(_X), L); },
				_Inc1, _Inc2
			);

			_Result = pow(_Result, static_cast<_SclTy>(1) / L);
			return _Result;
		}

		/*<dot>
			<code>
				auto _Result = static_cast<_SclTy>(0);
				auto_vector_operation<_SclTy, _BlkTy>::afunc_2st( _First1, _Last1, _First2, _Result, 
					[](auto& _Res, auto&& _X){ _Res += _X; }, 
					std::multiplies<>() );
			</code>
		  </dot>*/
	};

	template<typename _SclTy>
	struct _Vector_operation<_SclTy, _SclTy, _Vector_accelerate_level::Auto>
		: public _Vector_operation<_SclTy, _SclTy, _Vector_accelerate_level::Norm>{
		// 
	};

	template<typename _SclTy, typename _BlkTy>
		using fastest_vector_operation = _Vector_operation<_SclTy, _BlkTy, _Vector_accelerate_level::Fastest>;
	template<typename _SclTy, typename _BlkTy>
		using fast_vector_operation = _Vector_operation<_SclTy, _BlkTy, _Vector_accelerate_level::Fast>;
	template<typename _SclTy, typename _BlkTy>
		using norm_vector_operation = _Vector_operation<_SclTy, _BlkTy, _Vector_accelerate_level::Norm>;
	template<typename _SclTy, typename _BlkTy>
		using auto_vector_operation = _Vector_operation<_SclTy, _BlkTy, _Vector_accelerate_level::Auto>;



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


	template<typename _SclTy, typename _BlkTy>
	struct stack_allocator : std::vector<_BlkTy> {
		using _Mybase       = std::vector<_BlkTy>;
		using _This_type = stack_allocator<_SclTy, _BlkTy>;

		static _This_type& data() {
			static _This_type _Static_stack;
			return _Static_stack;
		}

		static size_t calc_block_count(size_t _Count/*scalar*/) {
			return ceil(_Count, block_traits<_BlkTy>::size()) / block_traits<_BlkTy>::size();
		/*	const bool _Divide = (_Count % block_traits<_BlkTy>::size()) != 0;
			return _Count / block_traits<_BlkTy>::size() + static_cast<size_t>(_Divide);*/
		}

		static _SclTy* allocate(size_t _Count/*scalar*/) {
			return reinterpret_cast<_SclTy*>(data().seek_n(calc_block_count(_Count)));
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

		_BlkTy& top() {
			return ((*this)[_Myseek]);
		}

		_BlkTy* seek_n(size_t/*block*/ _Count) {
			const auto _Newseek = _Myseek + _Count;
			assert(_Newseek <= _Mybase::size());

			auto _Ptr = &this->top();
			_Myseek = _Newseek;
			return _Ptr;
		}

		size_t _Myseek;
	};


	// forward declare
	template<typename _SclTy, typename _BlkTy>
		struct const_subvector;
	template<typename _SclTy, typename _BlkTy>
		struct subvector;

#define SUBVECTOR_OPERATOR_1ST(LHS, FUNC)           \
size_t _Size   = (##LHS##).size();                  \
auto   _Memory = clmagic::stack_allocator<_SclTy, _BlkTy>::allocate(_Size); \
auto   _Result = subvector<_SclTy, _BlkTy>(_Memory, _Memory + _Size, 1, true); \
auto_vector_operation<_SclTy, _BlkTy>::func_1st((##LHS##).begin(), (##LHS##).end(), _Result.begin(), (##FUNC##), \
	(##LHS##).stride(), _Result.stride()); \
return std::move(_Result)

#define SUBVECTOR_OPERATOR_2ND(LHS, RHS, FUNC)      \
size_t _Size   = (##LHS##).size();                  \
auto   _Memory = clmagic::stack_allocator<_SclTy, _BlkTy>::allocate(_Size); \
auto   _Result = subvector<_SclTy, _BlkTy>(_Memory, _Memory + _Size, 1, true); \
auto_vector_operation<_SclTy, _BlkTy>::func_2nd((##LHS##).begin(), (##LHS##).end(), (##RHS##).begin(), _Result.begin(), (##FUNC##), \
	(##LHS##).stride(), (##RHS##).stride(), _Result.stride()); \
return std::move(_Result)

	template<typename _SclTy, typename _BlkTy>
	struct const_subvector {// const _SclTy
		using iterator_category = std::random_access_iterator_tag;
		using difference_SclType   = ptrdiff_t;

		using block_type       = const _BlkTy;
		using block_pointer    = block_type*;
		using scalar_type      = const _SclTy;
		using scalar_pointer   = scalar_type*;
		using scalar_reference = scalar_type&;

		using size_type        = ::clmagic::vector_size<_SclTy, _BlkTy>;
		
		using block_traits     = ::clmagic::block_traits<_BlkTy>;
		using allocator_traits = ::clmagic::stack_allocator<_SclTy, _BlkTy>;

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
		subvector<_SclTy, _BlkTy> operator-() const {
			assert(!this->empty());
			SUBVECTOR_OPERATOR_1ST(*this, std::negate<>());
		}
		subvector<_SclTy, _BlkTy> operator+(const const_subvector& _Right) const {
			assert(!this->empty());
			assert(this->size() == _Right.size());
			SUBVECTOR_OPERATOR_2ND(*this, _Right, std::plus<>() );
		}
		subvector<_SclTy, _BlkTy> operator-(const const_subvector& _Right) const {
			assert(!this->empty());
			assert(this->size() == _Right.size());
			SUBVECTOR_OPERATOR_2ND(*this, _Right, std::minus<>());
		}
		subvector<_SclTy, _BlkTy> operator*(const const_subvector& _Right) const {
			assert(!this->empty());
			assert(this->size() == _Right.size());
			SUBVECTOR_OPERATOR_2ND(*this, _Right, std::multiplies<>());
		}
		subvector<_SclTy, _BlkTy> operator/(const const_subvector& _Right) const {
			assert(!this->empty());
			assert(this->size() == _Right.size());
			SUBVECTOR_OPERATOR_2ND(*this, _Right, std::divides<>());
		}
		
		subvector<_SclTy, _BlkTy> operator+(const _SclTy& _Scalar) const {
			assert(!this->empty());
			const auto _Rhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs + _Rhs; };
			SUBVECTOR_OPERATOR_1ST(*this, _Func);
		}
		subvector<_SclTy, _BlkTy> operator-(const _SclTy& _Scalar) const {
			assert(!this->empty());
			const auto _Rhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs - _Rhs; };
			SUBVECTOR_OPERATOR_1ST(*this, _Func);
		}
		subvector<_SclTy, _BlkTy> operator*(const _SclTy& _Scalar) const {
			assert(!this->empty());
			const auto _Rhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs * _Rhs; };
			SUBVECTOR_OPERATOR_1ST(*this, _Func);
		}
		subvector<_SclTy, _BlkTy> operator/(const _SclTy& _Scalar) const {
			assert(!this->empty());
			const auto _Rhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs / _Rhs; };
			SUBVECTOR_OPERATOR_1ST(*this, _Func);
		}
		friend subvector<_SclTy, _BlkTy> operator+(const _SclTy& _Scalar, const const_subvector& _Right) {
			assert(!_Right.empty());
			const auto _Lhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Lhs](auto&& _Rhs) { return _Lhs + _Rhs; };
			SUBVECTOR_OPERATOR_1ST(*this, _Func);
		}
		friend subvector<_SclTy, _BlkTy> operator-(const _SclTy& _Scalar, const const_subvector& _Right) {
			assert(!_Right.empty());
			const auto _Lhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Lhs](auto&& _Rhs) { return _Lhs - _Rhs; };
			SUBVECTOR_OPERATOR_1ST(*this, _Func);
		}
		friend subvector<_SclTy, _BlkTy> operator*(const _SclTy& _Scalar, const const_subvector& _Right) {
			assert(!_Right.empty());
			const auto _Lhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Lhs](auto&& _Rhs) { return _Lhs * _Rhs; };
			SUBVECTOR_OPERATOR_1ST(*this, _Func);
		}
		friend subvector<_SclTy, _BlkTy> operator/(const _SclTy& _Scalar, const const_subvector& _Right) {
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
	template<typename _SclTy, typename _BlkTy> inline
	subvector<_SclTy, _BlkTy> mod(const const_subvector<_SclTy, _BlkTy>& _Left, const const_subvector<_SclTy, _BlkTy>& _Right) {
		assert(!_Left.empty());
		assert(_Left.size() == _Right.size());
		SUBVECTOR_OPERATOR_2ND(_Left, _Right, std::modulus<_SclTy>());
	}
	template<typename _SclTy, typename _BlkTy> inline
	subvector<_SclTy, _BlkTy> pow(const const_subvector<_SclTy, _BlkTy>& _Left, const const_subvector<_SclTy, _BlkTy>& _Right) {
		assert(!_Left.empty());
		assert(_Left.size() == _Right.size());
		const auto _Func = [](auto&& A, auto&& B) { return pow(A, B); };
		SUBVECTOR_OPERATOR_2ND(_Left, _Right, _Func);
	}
	
	template<typename _SclTy, typename _BlkTy> inline
	_SclTy dot(const const_subvector<_SclTy, _BlkTy>& _Left, const const_subvector<_SclTy, _BlkTy>& _Right) {
		_SclTy _Result = static_cast<_SclTy>(0);
		return auto_vector_operation<_SclTy, _BlkTy>::afunc_2nd( _Left.begin(), _Left.end(), _Right.begin(), _Result,
			[](auto& _Res, auto&& _X) { _Res += _X; }, std::multiplies<>(), 
			_Left.stride(), _Right.stride() );
	}

	// vector OP scalar
	template<typename _SclTy, typename _BlkTy> inline
	subvector<_SclTy, _BlkTy> mod(const const_subvector<_SclTy, _BlkTy>& _Left, const _SclTy& _Scalar) {
		assert(!_Left.empty());
		const auto _Rhs  = clmagic::block_traits<_SclTy, _BlkTy>::set1(_Scalar);
		const auto _Func = [&_Rhs](auto&& _Lhs) { return mod(_Lhs, _Rhs); };
		SUBVECTOR_OPERATOR_1ST(_Left, _Func);
	}
	template<typename _SclTy, typename _BlkTy> inline
	subvector<_SclTy, _BlkTy> pow(const const_subvector<_SclTy, _BlkTy>& _Left, const _SclTy& _Scalar) {
		assert(!_Left.empty());
		const auto _Rhs  = clmagic::block_traits<_SclTy, _BlkTy>::set1(_Scalar);
		const auto _Func = [&_Rhs](auto&& _Lhs) { return pow(_Lhs, _Rhs); };
		SUBVECTOR_OPERATOR_1ST(_Left, _Func);
	}
#undef SUBVECTOR_OPERATOR_1ST
#undef SUBVECTOR_OPERATOR_2ND


#define SUBVECTOR_ASSIGN_OPERATOR_1ST(LHS, DST, FUNC) \
auto_vector_operation<_SclTy, _BlkTy>::func_1st((##LHS##).begin(), (##LHS##).end(), \
	(##DST##).begin(), (##FUNC##), \
	(##LHS##).stride(), (##DST##).stride()); \
return (*this)

#define SUBVECTOR_ASSIGN_OPERATOR_2ND(LHS, RHS, DST, FUNC) \
auto_vector_operation<_SclTy, _BlkTy>::func_2nd((##LHS##).begin(), (##LHS##).end(), (##RHS##).begin(), \
	(##DST##).begin(), (##FUNC##), \
	(##LHS##).stride(), (##RHS##).stride(), (##DST##).stride()); \
return (*this)

	template<typename _SclTy, typename _BlkTy>
	struct subvector : public const_subvector<_SclTy, _BlkTy> {// default const _SclTy
		using _Mybase = const_subvector<_SclTy, _BlkTy>;
		friend _Mybase;

		using iterator_category = std::random_access_iterator_tag;
		using difference_SclType   = ptrdiff_t;

		using block_type       = _BlkTy;
		using block_pointer    = block_type*;
		using scalar_type      = _SclTy;
		using scalar_pointer   = scalar_type*;
		using scalar_reference = scalar_type&;

		using block_traits = clmagic::block_traits<_BlkTy>;

		subvector(scalar_pointer _First_arg, scalar_pointer _Last_arg)
			: _Mybase(_First_arg, _Last_arg) {}
		subvector(scalar_pointer _First, scalar_pointer _Last, size_t _Stride)
			: _Mybase(_First, _Last, _Stride) {}
		subvector(subvector<_SclTy, _BlkTy>& _Right) {
			size_t _Size   = _Right.size();
			auto   _Memory = _Mybase::allocator_traits::allocate(_Size);
			_Mybase::reset(_Memory, _Memory+_Size, 1, true);
		}
		subvector(subvector<_SclTy, _BlkTy>&& _Right) noexcept// owner move to this
			: _Mybase(std::move(_Right)) {}

		// full-clone
		subvector& operator=(const const_subvector<_SclTy, _BlkTy>& _Right) {
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
		subvector& operator=(const subvector<_SclTy, _BlkTy>& _Right) { 
			return ( (*this) = static_cast<const const_subvector<_SclTy, _BlkTy>&>(_Right) );
		}
		subvector& operator=(subvector<_SclTy, _BlkTy>&& _Right) noexcept {
			return ((*this) = static_cast<subvector<_SclTy, _BlkTy>&>(_Right));
		}

		subvector operator()(size_t _First, size_t _Last) const {
			return subvector(this->ptr(_First), this->ptr(_Last), this->stride());
		}
		subvector& operator+=(const const_subvector<_SclTy, _BlkTy>& _Right) {
			SUBVECTOR_ASSIGN_OPERATOR_2ND(*this, _Right, *this, std::plus<>());
		}
		subvector& operator-=(const const_subvector<_SclTy, _BlkTy>& _Right) {
			SUBVECTOR_ASSIGN_OPERATOR_2ND(*this, _Right, *this, std::minus<>());
		}
		subvector& operator*=(const const_subvector<_SclTy, _BlkTy>& _Right) {
			SUBVECTOR_ASSIGN_OPERATOR_2ND(*this, _Right, *this, std::multiplies<>());
		}
		subvector& operator/=(const const_subvector<_SclTy, _BlkTy>& _Right) {
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
			const auto _Rhs  = block_traits::set1(_Right);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs * _Rhs; };
			SUBVECTOR_ASSIGN_OPERATOR_1ST(*this, *this, _Func);
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
			vector_size<_SclTy, _BlkTy> _Mysize;
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

	template<typename _SclTy, typename _BlkTy> inline
	_SclTy norm(const_subvector<_SclTy, _BlkTy> _X, const _SclTy& _Level) {
		_SclTy _Result = static_cast<_SclTy>(0);
		return auto_vector_operation<_SclTy, _BlkTy>::norm(_X.begin(), _X.end(), _Level, _Result, _X.stride());
	}
	template<typename _SclTy, typename _BlkTy> inline
	_SclTy normL2_square(const_subvector<_SclTy, _BlkTy> _X) {
		return dot(_X, _X);
	}
	template<typename _SclTy, typename _BlkTy> inline
	subvector<_SclTy, _BlkTy> normalize(const const_subvector<_SclTy, _BlkTy>& _Source) {
		const auto _NormL2sq = normL2_square(_Source);
		return ( !approach_equal(_NormL2sq, static_cast<_SclTy>(1), std::numeric_limits<_SclTy>::epsilon()) ) ? 
			std::move(_Source / sqrt(_NormL2sq)): 
			_Source;
	}


	

	enum Coordinates {
		_LH_,
		_RH_
	};

#define VECTOR_COMPARE_1ST(LHS, PRED) \
vector<bool, _Size> _Result;          \
std::transform((##LHS##).begin(), (##LHS##).end(), _Result.begin(), (##PRED##)); \
return std::move(_Result)

#define VECTOR_COMPARE_2ND(LHS, RHS, PRED) \
vector<bool, _Size> _Result;               \
std::transform((##LHS##).begin(), (##LHS##).end(), (##RHS##).begin(), _Result.begin(), (##PRED##)); \
return std::move(_Result)

	/*
	----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+---- float = scalar = block
	-------------------+-------------------+-------------------+-------------------+-------------------+-------------------+ __m128 = block
	Vector is a lot of block, Its scalar is minimum block
	*/
	template<typename _SclTy, size_t _Size, typename _BlkTy = _SclTy>
	class __declspec(align(std::alignment_of_v<_BlkTy>)) vector {
		constexpr static size_t _Real_size = ceil(_Size, std::alignment_of_v<_BlkTy>);
		
		_SclTy _Mydata[_Real_size];
	
	public:
		using block_type  = _BlkTy;
		using scalar_type = _SclTy;
		using operation   = ::clmagic::fastest_vector_operation<_SclTy, _BlkTy>;// impotant!!!

		using subvector       = ::clmagic::subvector<_SclTy, _BlkTy>;
		using const_subvector = ::clmagic::const_subvector<_SclTy, _BlkTy>;
		using block_traits    = ::clmagic::block_traits<_BlkTy>;
		//using scalar_traits = clmagic::block_traits<_SclTy>;

		static constexpr size_t size() {
			return _Size; }
		static constexpr size_t tail_size() {
			return _Real_size - _Size; }
		static constexpr size_t block_size() {
			return _Real_size / block_traits::size(); }
		
		template<typename _Ty = _SclTy>
		_Ty* ptr() {
			return reinterpret_cast<_Ty*>(_Mydata);
		}
		template<typename _Ty = _SclTy>
		_Ty* ptr(size_t _Pos) {
			return reinterpret_cast<_Ty*>(_Mydata + _Pos);
		}
		template<typename _Ty = _SclTy>
		_Ty& ref() {
			return ( * ptr<_Ty>() );
		}
		template<typename _Ty = _SclTy>
		_Ty& ref(size_t _Pos) {
			return ( * ptr<_Ty>(_Pos) );
		}
		template<typename _Ty = _SclTy>
		_Ty& at(size_t _Pos) {
			return (ref<_Ty>(_Pos));
		}

		template<typename _Ty = _SclTy>
		const _Ty* ptr() const {
			return reinterpret_cast<const _Ty*>(_Mydata);
		}
		template<typename _Ty = _SclTy>
		const _Ty* ptr(size_t _Pos) const {
			return reinterpret_cast<const _Ty*>(_Mydata + _Pos);
		}
		template<typename _Ty = _SclTy>
		const _Ty& ref() const {
			return (*ptr<_Ty>());
		}
		template<typename _Ty = _SclTy>
		const _Ty& ref(size_t _Pos) const {
			return (*ptr<_Ty>(_Pos));
		}
		template<typename _Ty = _SclTy>
		const _Ty& at(size_t _Pos) const {
			return (ref<_Ty>(_Pos));
		}

		template<typename _Ty = _SclTy>
		_Ty* begin() {
			return ptr<_Ty>();
		}
		template<typename _Ty = _SclTy>
		_Ty* end() {
			return ptr<_Ty>( size() );
		}
		template<typename _Ty = _SclTy>
		const _Ty* begin() const {
			return ptr<_Ty>();
		}
		template<typename _Ty = _SclTy>
		const _Ty* end() const {
			return ptr<_Ty>(size());
		}

		_SclTy& operator[](size_t _Pos) {
			return _Mydata[_Pos]; }
		const _SclTy& operator[](size_t _Pos) const {
			return _Mydata[_Pos]; }

		template<typename _Fn>
		static void func_1st(const vector& _Left, vector& _Result, _Fn _Func) {
			auto       _First = _Left.begin<_BlkTy>();
			const auto _Last  = _Left.begin<_BlkTy>() + _Left.block_size();
			auto       _Dest  = _Result.begin<_BlkTy>();
			operation::func_1st(_First, _Last, _Dest, _Func);
		}
		template<typename _Fn>
		static void func_2nd(const vector& _Left, const vector& _Right, vector& _Result, _Fn _Func) {
			auto       _First1 = _Left.begin<_BlkTy>();
			const auto _Last1  = _Left.begin<_BlkTy>() + _Left.block_size();
			auto       _First2 = _Right.begin<_BlkTy>();
			auto       _Dest   = _Result.begin<_BlkTy>();
			operation::func_2nd(_First1, _Last1, _First2, _Dest, _Func);
		}

		// assgin(...)
		template<typename _Iter>
		void assign(_Iter _First, _Iter _Last) {
			assert( std::distance(_First, _Last) <= std::_Iter_diff_t<_Iter>(this->size()) );
			auto _Dest = std::copy(_First, _Last, this->begin());
			std::fill(_Dest, this->end() + this->tail_size(), static_cast<_SclTy>(0));
		}
		void assign(const _SclTy& _Val) {
			std::fill(this->begin(), this->end(), _Val); 
			std::fill(this->end(), this->end() + this->tail_size(), static_cast<_SclTy>(0));
		}
		void assign(std::initializer_list<_SclTy> _Ilist) {
			assert(_Ilist.size() <= this->size());
			auto _Dest = std::copy(_Ilist.begin(), _Ilist.end(), this->begin());
			std::fill(_Dest, this->end() + this->tail_size(), static_cast<_SclTy>(0));
		}
		
		constexpr vector() : _Mydata{ 0 } {}
		explicit vector(const _SclTy& _Val) {
			this->assign(_Val); 
		}
		vector(std::initializer_list<_SclTy> _Ilist) {
			this->assign(_Ilist); 
		}
		template<typename _Iter> 
		vector(_Iter _First, _Iter _Last) { 
			this->assign(_First, _Last); 
		}
		template<typename _Fn>
		vector(const vector& _Left, _Fn _Func) : _Mydata{ 0 } {
			func_1st(_Left, *this, _Func);
		}
		template<typename _Fn>
		vector(const vector& _Left, const vector& _Right, _Fn _Func) : _Mydata{ 0 } {
			func_2nd(_Left, _Right, *this, _Func);
		}
		
		vector& operator=(const vector&) = default;
		vector& operator=(std::initializer_list<_SclTy> _Ilist) {
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
			VECTOR_COMPARE_2ND(*this, _Right, std::equal_to<>());
		}
		vector<bool, _Size> operator!=(const vector& _Right) const {
			VECTOR_COMPARE_2ND(*this, _Right, std::not_equal_to<>());
		}
		vector<bool, _Size> operator< (const vector& _Right) const {
			VECTOR_COMPARE_2ND(*this, _Right, std::less<>());
		}
		vector<bool, _Size> operator> (const vector& _Right) const {
			VECTOR_COMPARE_2ND(*this, _Right, std::greater<>());
		}
		vector<bool, _Size> operator<=(const vector& _Right) const {
			VECTOR_COMPARE_2ND(*this, _Right, std::less_equal<>());
		}
		vector<bool, _Size> operator>=(const vector& _Right) const {
			VECTOR_COMPARE_2ND(*this, _Right, std::greater_equal<>());
		}
		vector<bool, _Size> operator==(const _SclTy& _Scalar) const {
			VECTOR_COMPARE_1ST(*this, [&_Scalar](auto&& _X) { return _Scalar == _X; });
		}
		vector<bool, _Size> operator!=(const _SclTy& _Scalar) const {
			VECTOR_COMPARE_1ST(*this, [&_Scalar](auto&& _X) { return _Scalar != _X; });
		}
		vector<bool, _Size> operator< (const _SclTy& _Scalar) const {
			VECTOR_COMPARE_1ST(*this, [&_Scalar](auto&& _X) { return _Scalar < _X; });
		}
		vector<bool, _Size> operator> (const _SclTy& _Scalar) const {
			VECTOR_COMPARE_1ST(*this, [&_Scalar](auto&& _X) { return _Scalar > _X; });
		}
		vector<bool, _Size> operator<=(const _SclTy& _Scalar) const {
			VECTOR_COMPARE_1ST(*this, [&_Scalar](auto&& _X) { return _Scalar <= _X; });
		}
		vector<bool, _Size> operator>=(const _SclTy& _Scalar) const {
			VECTOR_COMPARE_1ST(*this, [&_Scalar](auto&& _X) { return _Scalar >= _X; });
		}

		vector operator-() const {
			return vector(*this, std::negate<>()); 
		}
		vector operator+(const vector& _Right) const {
			return vector(*this, _Right, std::plus<>()); 
		}
		vector operator-(const vector& _Right) const {
			return vector(*this, _Right, std::minus<>()); 
		}
		vector operator*(const vector& _Right) const {
			return vector(*this, _Right, std::multiplies<>());
		}
		vector operator/(const vector& _Right) const {
			return vector(*this, _Right, std::divides<>()); 
		}
		vector operator%(const vector& _Right) const {
			return vector(*this, _Right, std::modulus<>()); 
		}
		
		vector operator+(const _SclTy& _Scalar) const {
			const auto _Rhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs + _Rhs; };
			return vector(*this, _Func);
		}
		vector operator-(const _SclTy& _Scalar) const {
			const auto _Rhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs - _Rhs; };
			return vector(*this, _Func);
		}
		vector operator*(const _SclTy& _Scalar) const {
			const auto _Rhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs * _Rhs; };
			return vector(*this, _Func);
		}
		vector operator/(const _SclTy& _Scalar) const {
			const auto _Rhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs / _Rhs; };
			return vector(*this, _Func);
		}
		vector operator%(const _SclTy& _Scalar) const {
			const auto _Rhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs % _Rhs; };
			return vector(*this, _Func);
		}
		friend vector operator+(const _SclTy& _Scalar, const vector& _Right) {
			const auto _Lhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Lhs](auto&& _Rhs) { return _Lhs + _Rhs; };
			return vector(_Right, _Func);
		}
		friend vector operator-(const _SclTy& _Scalar, const vector& _Right) {
			const auto _Lhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Lhs](auto&& _Rhs) { return _Lhs - _Rhs; };
			return vector(_Right, _Func);
		}
		friend vector operator*(const _SclTy& _Scalar, const vector& _Right) {
			const auto _Lhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Lhs](auto&& _Rhs) { return _Lhs * _Rhs; };
			return vector(_Right, _Func);
		}
		friend vector operator/(const _SclTy& _Scalar, const vector& _Right) {
			const auto _Lhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Lhs](auto&& _Rhs) { return _Lhs / _Rhs; };
			return vector(_Right, _Func);
		}
		friend vector operator%(const _SclTy& _Scalar, const vector& _Right) {
			const auto _Lhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Lhs](auto&& _Rhs) { return _Lhs % _Rhs; };
			return vector(_Right, _Func);
		}
		
		vector& operator+=(const vector& _Right) {
			func_2nd(*this, _Right, *this, std::plus<>());
			return (*this);
		}
		vector& operator-=(const vector& _Right) {
			func_2nd(*this, _Right, *this, std::minus<>());
			return (*this);
		}
		vector& operator*=(const vector& _Right) {
			func_2nd(*this, _Right, *this, std::multiplies<>());
			return (*this);
		}
		vector& operator/=(const vector& _Right) {
			func_2nd(*this, _Right, *this, std::divides<>());
			return (*this);
		}
		vector& operator%=(const vector& _Right) {
			func_2nd(*this, _Right, *this, std::modulus<>());
			return (*this);
		}
		vector& operator+=(const _SclTy& _Scalar) {
			const auto _Rhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs + _Rhs; };
			func_1st(*this, *this, _Func);
			return (*this);
		}
		vector& operator-=(const _SclTy& _Scalar) {
			const auto _Rhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs - _Rhs; };
			func_1st(*this, *this, _Func);
			return (*this);
		}
		vector& operator*=(const _SclTy& _Scalar) {
			const auto _Rhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs * _Rhs; };
			func_1st(*this, *this, _Func);
			return (*this);
		}
		vector& operator/=(const _SclTy& _Scalar) {
			const auto _Rhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs / _Rhs; };
			func_1st(*this, *this, _Func);
			return (*this);
		}
		vector& operator%=(const _SclTy& _Scalar) {
			const auto _Rhs  = block_traits::set1(_Scalar);
			const auto _Func = [&_Rhs](auto&& _Lhs) { return _Lhs % _Rhs; };
			func_1st(*this, *this, _Func);
			return (*this);
		}

		vector& normalize() {
			const auto _NormL2sq = normL2_square(*this);
			if (!approach_equal(_NormL2sq, static_cast<_SclTy>(1), std::numeric_limits<_SclTy>::epsilon())) {
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
	};
#undef VECTOR_COMPARE_1ST
#undef VECTOR_COMPARE_2ND

	template<typename _SclTy, typename _BlkTy = _SclTy>
		using vector2 = vector<_SclTy, 2, _BlkTy>;
	template<typename _SclTy, typename _BlkTy = _SclTy>
		using vector3 = vector<_SclTy, 3, _BlkTy>;
	template<typename _SclTy, typename _BlkTy = _SclTy>
		using vector4 = vector<_SclTy, 4, _BlkTy>;

	template<size_t _Size>
		using bvector = vector<bool, _Size>;

	template<typename _SclTy, size_t _Size, typename _BlkTy = _SclTy>
		using point = vector<_SclTy, _Size, _BlkTy>;
	template<typename _SclTy, typename _BlkTy = _SclTy>
		using point2 = vector<_SclTy, 2, _BlkTy>;
	template<typename _SclTy, typename _BlkTy = _SclTy>
		using point3 = vector<_SclTy, 3, _BlkTy>;
	
	using point2_size_t = point2<size_t>;


#define vectorN vector<_SclTy, _Size, _BlkTy>
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	_SclTy sum(const vectorN& _X) {// X[0] + X[1] + X[2] + ... + X[N]
		auto       _First  = _X.begin<_BlkTy>();
		const auto _Last   = _X.begin<_BlkTy>() + _X.block_size();
		_SclTy     _Result = static_cast<_SclTy>(0);
		return fastest_vector_operation<_SclTy, _BlkTy>::afunc(_First, _Last, _Result, std::plus<>());
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	_SclTy product(const vectorN& _X) {// X[0] * X[1] * X[2] * ... * X[N]
		auto       _First  = _X.begin<_BlkTy>();
		const auto _Last   = _X.begin<_BlkTy>() + _X.block_size();
		_SclTy     _Result = static_cast<_SclTy>(0);
		return fastest_vector_operation<_SclTy, _BlkTy>::afunc(_First, _Last, _Result, std::multiplies<>());
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	_SclTy dot(const vectorN& _X, const vectorN& _Y) {// X[0]*Y[0] + X[1]*Y[1] + X[2]*Y[2] + ... * X[N]*Y[N]
		auto       _First1 = _X.begin<_BlkTy>();
		const auto _Last1  = _X.begin<_BlkTy>() + _X.block_size();
		auto       _First2 = _Y.begin<_BlkTy>();
		_SclTy     _Result = static_cast<_SclTy>(0);
		return fastest_vector_operation<_SclTy, _BlkTy>::afunc_2nd(_First1, _Last1, _First2, _Result,
			[](auto& _Res, auto&& _X) { _Res += _X; }, std::multiplies<>());
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	_SclTy normL2_square(const vectorN& _X) {// X[0]² + X[1]² + X[2]² + ... * X[N]²
		return dot(_X, _X);
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	_SclTy length(const vectorN& _X) {// sqrt(dot(X, X))
		return sqrt(normL2_square(_X));
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	_SclTy norm(const vectorN& _X, const _SclTy& _Level) {// sqrt(dot(X, X))
		auto       _First  = _X.begin<_BlkTy>();
		const auto _Last   = _X.begin<_BlkTy>() + _X.block_size();
		_SclTy     _Result = static_cast<_SclTy>(0);
		return fastest_vector_operation<_SclTy, _BlkTy>::norm(_First, _Last, _Level, _Result);
	}

	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN normalize(const vectorN& _X) {
		const auto _NormL2sq = normL2_square(_X);
		return !approach_equal(_NormL2sq, static_cast<_SclTy>(1), std::numeric_limits<_SclTy>::epsilon()) ?
			_X / sqrt(_NormL2sq) : 
			_X;
	}

	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN mod(const vectorN& _Left, const vectorN& _Right) {
		return vectorN(_Left, _Right, std::modulus<>());
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN pow(const vectorN& _Left, const vectorN& _Right) {
		return vectorN(_Left, _Right,
			[](auto&& A, auto&& B) { return pow(A, B); });
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN cross3(const vectorN& _Left, const vectorN& _Right) {
		return vectorN{
			_Left[1] * _Right[2] - _Left[2] * _Right[1],
			_Left[2] * _Right[0] - _Left[0] * _Right[2],
			_Left[0] * _Right[1] - _Left[1] * _Right[0]
		};
	}

	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN abs(const vectorN& _Left) {
		return vectorN(_Left, [](auto&& bk) { return abs(bk); });
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN floor(const vectorN& _Left) {
		return vectorN(_Left, [](auto&& bk) { return floor(bk); });
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN ceil(const vectorN& _Left) {
		return vectorN(_Left, [](auto&& bk) { return ceil(bk); });
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN trunc(const vectorN& _Left) {
		return vectorN(_Left, [](auto&& bk) { return trunc(bk); });
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN round(const vectorN& _Left) {
		return vectorN(_Left, [](auto&& bk) { return round(bk); });
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN sqrt(const vectorN& _Left) {
		return vectorN(_Left, [](auto&& bk) { return sqrt(bk); });
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN cbrt(const vectorN& _Left) {
		return vectorN(_Left, [](auto&& bk) { return cbrt(bk); });
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN invsqrt(const vectorN& _Left) {
		return vectorN(_Left, [](auto&& bk) { return invsqrt(bk); });
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN invcbrt(const vectorN& _Left) {
		return vectorN(_Left, [](auto&& bk) { return invcbrt(bk); });
	}

	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN sin(const vectorN& _Left) {
		return vectorN(_Left, [](auto&& bk) { return sin(bk); });
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN cos(const vectorN& _Left) {
		return vectorN(_Left, [](auto&& bk) { return cos(bk); });
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN tan(const vectorN& _Left) {
		return vectorN(_Left, [](auto&& bk) { return tan(bk); });
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN asin(const vectorN& _Left) {
		return vectorN(_Left, [](auto&& bk) { return asin(bk); });
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN acos(const vectorN& _Left) {
		return vectorN(_Left, [](auto&& bk) { return acos(bk); });
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN atan(const vectorN& _Left) {
		return vectorN(_Left, [](auto&& bk) { return atan(bk); });
	}
	template<typename _SclTy, size_t _Size, typename _BlkTy> inline
	vectorN atan2(const vectorN& _Y, const vectorN& _X) {
		return vectorN(_Y, _X, [](auto&& bky, auto&& bkx) { return atan2(bky, bkx); } );
	}

	// compare function
	template<size_t _Size, typename _BlkTy>
	bool all_of(const vector<bool, _Size, _BlkTy>& _X) {
		for (auto _First = _X.begin(); _First != _X.end(); ++_First) {
			if (!(*_First)) {
				return false;
			}
		}
		return true;
	}
	template<size_t _Size, typename _BlkTy>
	bool any_of(const vector<bool, _Size, _BlkTy>& _X) {
		for (auto _First = _X.begin(); _First != _X.end(); ++_First) {
			if (*_First) {
				return true;
			}
		}
		return false;
	}
	template<size_t _Size, typename _BlkTy>
	bool none_of(const vector<bool, _Size, _BlkTy>& _X) {
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
	template<typename _SclTy, typename _BlkTy = _SclTy, typename _Alloc = std::allocator<_BlkTy>>
	class vector_any : public clmagic::subvector<_SclTy, _BlkTy> {
		using _Mybase = clmagic::subvector<_SclTy, _BlkTy>;
	public:
		using scalar_type         = _SclTy;
		using scalar_pointer         = _SclTy*;
		using scalar_const_pointer   = _SclTy const*;
		using scalar_reference       = _SclTy&;
		using scalar_const_reference = _SclTy const&;

		using block_type           = _BlkTy;
		using block_pointer        = _BlkTy*;
		using block_const_pointer  = _BlkTy const*;
		using block_reference       = _BlkTy&;
		using block_const_reference = _BlkTy const&;

		using subvector       = clmagic::subvector<_SclTy, _BlkTy>;
		using const_subvector = clmagic::const_subvector<_SclTy, _BlkTy>;

		using block_traits    = clmagic::block_traits<_BlkTy>;
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
		void assign(const vector_any& _Left, const _BlkTy& _Right_bk, _Fn _Func) {
			this->assign(_Left,
				[_Func, &_Right_bk](block_reference _Left_bk) { return _Func(_Left_bk, _Right_bk); });
		}

		template<typename _Fn>
		void assign(const _BlkTy& _Left_bk, const vector_any& _Right, _Fn _Func) {
			this->assign(_Right,
				[_Func, &_Left_bk](block_reference _Right_bk) { return _Func(_Left_bk, _Right_bk); });
		}

		// <Internal-function>
		template<typename _Fn>
		void _Assign(const vector_any& _Left, const _SclTy& _Scalar, _Fn _Func) {
			this->assign(_Left, block_traits::set1(_Scalar));
		}

		template<typename _Fn>
		void _Assign(const _SclTy& _Scalar, const vector_any& _Right, _Fn _Func) {
			this->assign(block_traits::set1(_Scalar), _Right);
		}
		// </Internal-function>

		template<typename _Iter>
		vector_any(_Iter _First, _Iter _Last) : vector_any() {
			this->assign(_First, _Last);
		}

		vector_any(std::initializer_list<_SclTy> _Ilist) : vector_any() {
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
		vector_any(const vector_any& _Left, const _SclTy& _Scalar, _Fn _Func) : vector_any() {// construct( this, _Func(_Left, block_tratis::set1(_Scalar)) )
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
		vector_any(const _SclTy& _Scalar, const vector_any& _Right, _Fn _Func) : vector_any() {// construct( this, _Func(block_tratis::set1(_Scalar), _Right) )
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
		vector_any operator+(const _SclTy& _Scalar) const {
			return std::move(vector_any(*this, _Scalar, std::plus<block_type>()));
		}
		vector_any operator-(const _SclTy& _Scalar) const {
			return std::move(vector_any(*this, _Scalar, std::minus<block_type>()));
		}
		vector_any operator*(const _SclTy& _Scalar) const {
			return std::move(vector_any(*this, _Scalar, std::multiplies<block_type>()));
		}
		vector_any operator/(const _SclTy& _Scalar) const {
			return std::move(vector_any(*this, _Scalar, std::divides<block_type>()));
		}
		vector_any operator%(const _SclTy& _Scalar) const {
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
		vector_any& operator+=(const _SclTy& _Scalar) {
			this->_Assign(*this, _Scalar, std::plus<block_type>());
			return (*this);
		}
		vector_any& operator-=(const _SclTy& _Scalar) {
			this->_Assign(*this, _Scalar, std::minus<block_type>());
			return (*this);
		}
		vector_any& operator*=(const _SclTy& _Scalar) {
			this->_Assign(*this, _Scalar, std::multiplies<block_type>());
			return (*this);
		}
		vector_any& operator/=(const _SclTy& _Scalar) {
			this->_Assign(*this, _Scalar, std::divides<block_type>());
			return (*this);
		}
		vector_any& operator%=(const _SclTy& _Scalar) {
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
				_Myvec._Mysize = vector_size<_SclTy, _BlkTy>(_Myvec._Myfirst, _Myvec._Mylast);// correct vector-size
			} else {
				_Reallocate(_Calculate_growth(this->capacity() + 1));
				emplace_back(std::forward<_Valty...>(_Val...));
			}
		}

		void push_back(const _SclTy& _Val) {
			this->emplace_back(_Val);
		}

		void push_back(_SclTy&& _Val) {
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
					std::cout << "[vector_any<_SclTy, _BlkTy> memory exception] ->[~vector_any()]" << std::endl;
					//throw std::exception("[vector_any<_SclTy, _BlkTy> memory exception] ->[~vector_any()]");
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

		scalar_pointer _Construct_range(scalar_pointer _First, scalar_const_pointer _Last, const _SclTy& _Val) {
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

		/* learning from C++Stantard-library-vector<_SclTy,...> */
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

		/* learning from C++Stantard-library-vector<_SclTy,...> */
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

		/* learning from C++Stantard-library-vector<_SclTy,...> */
		void _Resize(size_t _Newsize/*scalar*/, const _SclTy& _Val) {
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
		_BlkTy* _Mydata;
		size_t  _Mycapacity;
	};



	template<typename _SclTy>
		constexpr bool is_vector_v = false;
	template<typename _SclTy, size_t _Size, typename _BlkTy>
		constexpr bool is_vector_v< vector<_SclTy, _Size, _BlkTy> > = true;
	template<typename _SclTy, typename _BlkTy>
		constexpr bool is_vector_v< vector_any<_SclTy, _BlkTy> > = true;

	template<typename _SclTy, size_t _Size, typename _BlkTy>
		constexpr bool is_support_scalar_operator< vector<_SclTy, _Size, _BlkTy> > = true;
	template<typename _SclTy, typename _BlkTy>
		constexpr bool is_support_scalar_operator< vector_any<_SclTy, _BlkTy> > = true;




	/*
	@_unit_vector<T>: as vector3<T>
	@_Traits: length = 1.0
	*/
	template<typename _SclTy, size_t _Size, typename _BlkTy = _SclTy>
	struct unit_vector : public vector<_SclTy, _Size, _BlkTy> {
		using _Mybase     = vector<_SclTy, _Size, _BlkTy>;
		using scalar_type = _SclTy;
		using vector_SclType = unit_vector<_SclTy, _Size, _BlkTy>;

		unit_vector() = default;

		unit_vector(std::initializer_list<_SclTy> _Ilist, bool _Unitized = false)
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

	template<typename _SclTy, typename _BlkTy = _SclTy>
		using unit_vector3 = unit_vector<_SclTy, 3, _BlkTy>;








	/*template<typename _SclTy, size_t _Size, typename _Vtag> inline
	auto distance(const vector<_SclTy, _Size, _Vtag>& _A, const vector<_SclTy, _Size, _Vtag>& _B) 
		-> decltype(length(_B - _A)) {
		return length(_B - _A);
	}*/

	//template<size_t N, typename T> inline
	//	T radians(_in(vector<N, T>) _A, _in(vector<N, T>) _B) 
	//	{	// return dot(A,B)÷|A|÷|B|
	//	return (dot(_A, _B) / _A.length() / _B.length());
	//	}

	//template<typename _SclTy, size_t _Size, typename _Vtag> inline
	//unit_vectorN faceforward(const unit_vectorN& _Normal, const vectorN& _I, const vectorN& _Nref) {// dot(_I,_Nref) < 0 ? N : -N
	//	return (dot(_I, _Nref) < static_cast<_SclTy>(0) ? _Normal : -_Normal);
	//}

	//template<typename _SclTy, size_t _Size, typename _Vtag> inline
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

	//template<typename _SclTy, size_t _Size, typename _Vtag> inline
	//vectorN proj(const vectorN& _Vector, const unit_vectorN& _Proj) {// return dot(V,N)*N
	//	return (dot(_Vector, _Proj) * _Proj);
	//}

	//template<typename _SclTy, typename _Vtag> inline
	//vector3<_SclTy, _Vtag> proj(const vector3<_SclTy, _Vtag>& _Vector, const unit_vector3<_SclTy, _Vtag>& _Rt, 
	//	const unit_vector3<_SclTy, _Vtag>& _Up, const unit_vector3<_SclTy, _Vtag>& _Fwd) {// return [dot3(V,r), dot(V,u), dot(V,f)]
	//	return vector3<_SclTy, _Vtag>{ dot(_Vector, _Rt), dot(_Vector, _Up), dot(_Vector, _Fwd) };
	//}

	//template<typename _SclTy, size_t _Size, typename _Vtag> inline
	//vectorN reflect(const vectorN& _Direction, const unit_vectorN& _Normal) {
	//	return (static_cast<_SclTy>(2) * proj(_Direction, _Normal) - _Direction);
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